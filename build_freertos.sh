#!/bin/bash
#======================================================================
# Copyright (c) 2026 IntelPro Technology Co., Ltd.
# All rights reserved.
#
# This software and associated documentation files (the "Software") are
# proprietary and confidential to IntelPro Technology Co., Ltd..
#
# Access to and use of this Software is limited exclusively to authorized 
# customers and partners of IntelPro Technology Co., Ltd. who have entered into a 
# valid license or non-disclosure agreement.
#
# Unauthorized copying, modification, distribution, or reverse engineering 
# of this Software, via any medium, is strictly prohibited without 
# prior written permission from IntelPro Technology Co., Ltd..
#
# SPDX-License-Identifier: Proprietary
#======================================================================


# build_freertos.sh - Unified CLI for building, flashing, monitoring, and testing FreeRTOS projects.
#
# Usage: ./build_freertos.sh [actions...] [options] <project>
#
# Actions are composable and executed in order:
#   build, clean, flash, monitor, size, test
#
# Examples:
#   ./build_freertos.sh ipro_ble_remote                        # build (default)
#   ./build_freertos.sh clean build bluetooth/ble/ipro_ble_remote  # clean + rebuild
#   ./build_freertos.sh build flash monitor ipro_ble_remote    # full workflow
#   ./build_freertos.sh test ipro_unit_test                    # automated test
#   ./build_freertos.sh -l                                     # list projects

set -euo pipefail

# --- Defaults ---
SDK_BASE="$(cd "$(dirname "$0")" && pwd)"
SERIAL_PORT="/dev/cu.usbserial-2120"
TIMEOUT=30
WAIT_PATTERN=""
JOBS=""
VERBOSE=0
DEFCONFIG=""
ACTIONS=()
PROJECT_INPUT=""
FLASH_TOOL="$SDK_BASE/tools/ipro_iot_tool_lite/ipro_iot_tool_lite"
SERIAL_MONITOR="$SDK_BASE/tools/serial_monitor.py"

# --- Color support ---
if [ -t 1 ]; then
    RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[0;33m'; CYAN='\033[0;36m'; BOLD='\033[1m'; RESET='\033[0m'
else
    RED=''; GREEN=''; YELLOW=''; CYAN=''; BOLD=''; RESET=''
fi

ok()   { echo -e "${GREEN}✓${RESET} $*"; }
fail() { echo -e "${RED}✗${RESET} $*" >&2; }
warn() { echo -e "${YELLOW}⚠${RESET} $*" >&2; }
info() { echo -e "${CYAN}→${RESET} $*"; }

elapsed() {
    local secs=$1
    if (( secs >= 60 )); then
        printf "%dm%ds" $((secs/60)) $((secs%60))
    else
        printf "%ds" "$secs"
    fi
}

# --- Help ---
show_help() {
    cat <<'EOF'
Usage: ./build_freertos.sh [actions...] [options] <project>

Actions (combinable, executed in order):
  build      Build project (default if no action given)
  clean      Clean build directory
  flash      Flash binary to board
  monitor    Open serial monitor
  size       Show ELF size + memory region usage
  test       Build + flash + monitor with auto test pattern

Options:
  -p, --port PORT      Serial port (default: /dev/cu.usbserial-2120)
  -t, --timeout SEC    Monitor/test timeout (default: 30)
  -w, --wait-for PAT   Wait-for pattern (regex) for monitor
  -d, --defconfig FILE Use alternate defconfig (builds in build_<name>/)
  -j, --jobs N         Parallel make jobs
  -v, --verbose        Show full build output
  -l, --list           List available projects
  -h, --help           Show help

Examples:
  ./build_freertos.sh ipro_ble_remote                        # build (default)
  ./build_freertos.sh clean build bluetooth/ble/ipro_ble_remote  # clean + rebuild
  ./build_freertos.sh build flash monitor ipro_ble_remote    # full workflow
  ./build_freertos.sh test ipro_unit_test                    # automated test
  ./build_freertos.sh size bluetooth/ble/ipro_ble_remote         # ELF size report
  ./build_freertos.sh -d defconfig_xxx ipro_blecontroller_test  # build with defconfig
  ./build_freertos.sh -l                                     # list all projects
EOF
    exit 0
}

# --- Project Discovery ---
# Find all project directories (contain CMakeLists.txt) under apps/
discover_projects() {
    find "$SDK_BASE/apps" -name CMakeLists.txt -not -path "*/build/*" | while read -r f; do
        dirname "$f"
    done | sort
}

resolve_project() {
    local input="$1"
    local candidates=()

    # If explicit path given (contains /), try direct resolution first
    if [[ "$input" == *"/"* ]]; then
        local direct="$SDK_BASE/apps/$input"
        if [ -d "$direct" ] && [ -f "$direct/CMakeLists.txt" ]; then
            echo "$direct"
            return 0
        fi
    fi

    # Search by basename match
    local search_name
    search_name="$(basename "$input")"

    while IFS= read -r proj_dir; do
        if [ "$(basename "$proj_dir")" = "$search_name" ]; then
            candidates+=("$proj_dir")
        fi
    done < <(discover_projects)

    if [ ${#candidates[@]} -eq 1 ]; then
        echo "${candidates[0]}"
        return 0
    elif [ ${#candidates[@]} -gt 1 ]; then
        fail "Multiple projects match '$input':"
        for c in "${candidates[@]}"; do
            echo "  - ${c#"$SDK_BASE/apps/"}" >&2
        done
        echo "Use the full path to disambiguate." >&2
        return 1
    fi

    # No exact match — try partial/fuzzy
    local partials=()
    while IFS= read -r proj_dir; do
        local base
        base="$(basename "$proj_dir")"
        if [[ "$base" == *"$search_name"* ]]; then
            partials+=("$proj_dir")
        fi
    done < <(discover_projects)

    if [ ${#partials[@]} -gt 0 ]; then
        fail "Project '$input' not found. Did you mean:"
        for c in "${partials[@]}"; do
            echo "  - ${c#"$SDK_BASE/apps/"}" >&2
        done
    else
        fail "Project '$input' not found."
        echo "Use -l to list available projects." >&2
    fi
    return 1
}

list_projects() {
    local last_category=""
    while IFS= read -r proj_dir; do
        local rel="${proj_dir#"$SDK_BASE/apps/"}"
        local name
        name="$(basename "$proj_dir")"
        local category
        category="$(dirname "$rel")"

        if [ "$category" = "." ]; then
            category="(top-level)"
        fi

        if [ "$category" != "$last_category" ]; then
            [ -n "$last_category" ] && echo ""
            echo -e "${BOLD}${category}${RESET}"
            last_category="$category"
        fi

        # Try to extract a description from .config first line comment
        local desc=""
        if [ -f "$proj_dir/.config" ]; then
            desc="$(head -1 "$proj_dir/.config" 2>/dev/null | sed -n 's/^# *//p')"
        fi

        if [ -n "$desc" ]; then
            printf "  %-35s %s\n" "$name" "$desc"
        else
            printf "  %s\n" "$name"
        fi
    done < <(discover_projects)
}

# --- Get chip suffix and binary paths ---
get_chip_suffix() {
    local project_dir="$1"
    local config
    if [ -n "$DEFCONFIG" ]; then
        config="$project_dir/$DEFCONFIG"
    else
        config="$project_dir/.config"
    fi
    if [ -f "$config" ]; then
        local chip
        chip="$(grep -m1 '^CONFIG_CHIP=' "$config" 2>/dev/null | sed 's/CONFIG_CHIP="*\([^"]*\)"*/\1/' || true)"
        if [ -n "$chip" ]; then
            echo "$chip"
            return
        fi
        # Fallback: detect from CONFIG_IPRO7
        if grep -q '^CONFIG_IPRO7=y' "$config" 2>/dev/null; then
            echo "IPRO7"
            return
        fi
    fi
    echo "IPRO7"
}

get_build_dir() {
    local project_dir="$1"
    if [ -n "$DEFCONFIG" ]; then
        local dc_name
        dc_name="$(basename "$DEFCONFIG" | sed 's/^defconfig_//')"
        echo "$project_dir/build_${dc_name}"
    else
        echo "$project_dir/build"
    fi
}

get_binary_paths() {
    local project_dir="$1"
    local name
    name="$(basename "$project_dir")"
    local chip
    chip="$(get_chip_suffix "$project_dir")"
    local build_out
    build_out="$(get_build_dir "$project_dir")/build_out"

    ELF_PATH="$build_out/${name}_${chip}.elf"
    BIN_PATH="$build_out/${name}_${chip}.bin"
}

get_cross_compile() {
    local project_dir="$1"
    local config
    if [ -n "$DEFCONFIG" ]; then
        config="$project_dir/$DEFCONFIG"
    else
        config="$project_dir/.config"
    fi
    # Default RISC-V toolchain
    local prefix="/opt/toolchain/riscv_ipro7/bin/riscv64-unknown-elf-"
    if [ -f "$config" ]; then
        local cpu_id
        cpu_id="$(grep -m1 '^CONFIG_CPU_ID=' "$config" 2>/dev/null | sed 's/CONFIG_CPU_ID="*\([^"]*\)"*/\1/' || true)"
        if [ -n "$cpu_id" ]; then
            prefix="/opt/toolchain/riscv_ipro7/bin/riscv64-unknown-elf-"
        fi
    fi
    echo "$prefix"
}

# --- Actions ---
action_build() {
    local project_dir="$1"
    local name
    name="$(basename "$project_dir")"
    info "Building ${BOLD}$name${RESET}..."

    local start=$SECONDS
    local make_args=("-C" "$project_dir")
    [ -n "$DEFCONFIG" ] && make_args+=("DEFCONFIG=$DEFCONFIG")
    [ -n "$JOBS" ] && make_args+=("-j$JOBS")

    local build_dir
    build_dir="$(get_build_dir "$project_dir")"
    local log_file="$build_dir/build.log"
    mkdir -p "$build_dir"

    if [ "$VERBOSE" -eq 1 ]; then
        make "${make_args[@]}" 2>&1 | tee "$log_file"
        local rc=${PIPESTATUS[0]}
        local dur=$(( SECONDS - start ))
        if [ $rc -eq 0 ]; then
            get_binary_paths "$project_dir"
            ok "Build succeeded ($(elapsed $dur))"
            if [ -f "$BIN_PATH" ]; then
                local size_kb
                size_kb=$(( $(stat -f%z "$BIN_PATH" 2>/dev/null || stat -c%s "$BIN_PATH" 2>/dev/null || echo 0) / 1024 ))
                echo "  Binary: $BIN_PATH (${size_kb} KB)"
            fi
        else
            fail "Build failed ($(elapsed $dur))"
            return 1
        fi
    else
        if make "${make_args[@]}" > "$log_file" 2>&1; then
            local dur=$(( SECONDS - start ))
            get_binary_paths "$project_dir"
            ok "Build succeeded ($(elapsed $dur))"
            if [ -f "$BIN_PATH" ]; then
                local size_kb
                size_kb=$(( $(stat -f%z "$BIN_PATH" 2>/dev/null || stat -c%s "$BIN_PATH" 2>/dev/null || echo 0) / 1024 ))
                echo "  Binary: $BIN_PATH (${size_kb} KB)"
            fi
            echo "  Log: $log_file"
        else
            local dur=$(( SECONDS - start ))
            fail "Build failed ($(elapsed $dur))"
            echo "--- Last 30 lines ---"
            tail -30 "$log_file"
            echo ""
            echo "  Full log: $log_file"
            return 1
        fi
    fi
}

action_clean() {
    local project_dir="$1"
    local name
    name="$(basename "$project_dir")"
    info "Cleaning ${BOLD}$name${RESET}..."
    local clean_args=("-C" "$project_dir" "clean")
    [ -n "$DEFCONFIG" ] && clean_args+=("DEFCONFIG=$DEFCONFIG")
    make "${clean_args[@]}"
    ok "Clean complete"
}

action_flash() {
    local project_dir="$1"
    get_binary_paths "$project_dir"

    if [ ! -f "$BIN_PATH" ]; then
        fail "Binary not found: $BIN_PATH"
        echo "  Run 'build' first."
        return 1
    fi

    if [ ! -x "$FLASH_TOOL" ]; then
        fail "Flash tool not found: $FLASH_TOOL"
        return 1
    fi

    info "Entering download mode..."
    python3 "$SERIAL_MONITOR" "$SERIAL_PORT" --send reboot_dl --timeout 2 2>/dev/null || true
    sleep 2

    info "Flashing ${BOLD}$(basename "$BIN_PATH")${RESET}..."
    local start=$SECONDS
    if "$FLASH_TOOL" --chipname ipro7 --port "$SERIAL_PORT" --firmware "$BIN_PATH" --baudrate 921600 --flash-pin 0x02; then
        local dur=$(( SECONDS - start ))
        ok "Flash succeeded ($(elapsed $dur))"
    else
        local dur=$(( SECONDS - start ))
        fail "Flash failed ($(elapsed $dur))"
        return 1
    fi
}

action_monitor() {
    local project_dir="$1"
    info "Opening serial monitor on ${BOLD}$SERIAL_PORT${RESET}..."

    local mon_args=("$SERIAL_PORT" "--timeout" "$TIMEOUT" "--send" "reboot")
    if [ -n "$WAIT_PATTERN" ]; then
        mon_args+=("--wait-for" "$WAIT_PATTERN")
    fi

    python3 "$SERIAL_MONITOR" "${mon_args[@]}"
}

action_size() {
    local project_dir="$1"
    get_binary_paths "$project_dir"

    if [ ! -f "$ELF_PATH" ]; then
        fail "ELF not found: $ELF_PATH"
        echo "  Run 'build' first."
        return 1
    fi

    local cross
    cross="$(get_cross_compile "$project_dir")"

    info "Size report for ${BOLD}$(basename "$ELF_PATH")${RESET}"
    echo ""
    "${cross}size" "$ELF_PATH"
    echo ""

    # Show section breakdown
    "${cross}size" -A "$ELF_PATH" | head -30
}

action_test() {
    local project_dir="$1"

    # test = build + flash + monitor with test pattern
    action_build "$project_dir"
    action_flash "$project_dir"

    info "Running tests (timeout: ${TIMEOUT}s)..."
    local mon_args=("$SERIAL_PORT" "--timeout" "$TIMEOUT" "--send" "reboot" "--wait-for" "ALL TESTS PASSED|SOME TESTS FAILED")

    local output
    output="$(python3 "$SERIAL_MONITOR" "${mon_args[@]}" 2>&1)" || true
    echo "$output"

    if echo "$output" | grep -q "ALL TESTS PASSED"; then
        ok "All tests passed"
        return 0
    elif echo "$output" | grep -q "SOME TESTS FAILED"; then
        fail "Some tests failed"
        return 1
    else
        warn "Test timed out (no result pattern seen)"
        return 2
    fi
}

# --- Argument Parsing ---
parse_args() {
    while [ $# -gt 0 ]; do
        case "$1" in
            -h|--help)    show_help ;;
            -l|--list)    list_projects; exit 0 ;;
            -p|--port)    SERIAL_PORT="$2"; shift ;;
            -t|--timeout) TIMEOUT="$2"; shift ;;
            -w|--wait-for) WAIT_PATTERN="$2"; shift ;;
            -d|--defconfig) DEFCONFIG="$2"; shift ;;
            -j|--jobs)    JOBS="$2"; shift ;;
            -v|--verbose) VERBOSE=1 ;;
            build|clean|flash|monitor|size|test)
                ACTIONS+=("$1") ;;
            -*)
                fail "Unknown option: $1"
                echo "Use -h for help."
                exit 1 ;;
            *)
                if [ -z "$PROJECT_INPUT" ]; then
                    PROJECT_INPUT="$1"
                else
                    fail "Unexpected argument: $1"
                    exit 1
                fi ;;
        esac
        shift
    done
}

# --- Main ---
main() {
    parse_args "$@"

    # -l and -h are handled in parse_args; if we get here we need a project
    if [ -z "$PROJECT_INPUT" ]; then
        fail "No project specified."
        echo "Use -h for help, or -l to list projects."
        exit 1
    fi

    # Resolve project
    local project_dir
    project_dir="$(resolve_project "$PROJECT_INPUT")" || exit 1

    local project_name
    project_name="$(basename "$project_dir")"
    local project_rel="${project_dir#"$SDK_BASE/apps/"}"

    # Default action is build
    if [ ${#ACTIONS[@]} -eq 0 ]; then
        ACTIONS=("build")
    fi

    echo -e "${BOLD}Project:${RESET} $project_name  ${CYAN}($project_rel)${RESET}"
    [ -n "$DEFCONFIG" ] && echo -e "${BOLD}Defconfig:${RESET} $DEFCONFIG"
    echo ""

    # Execute actions in order
    local results=()
    local overall=0

    for action in "${ACTIONS[@]}"; do
        local rc=0
        case "$action" in
            build)   action_build "$project_dir"   || rc=$? ;;
            clean)   action_clean "$project_dir"   || rc=$? ;;
            flash)   action_flash "$project_dir"   || rc=$? ;;
            monitor) action_monitor "$project_dir" || rc=$? ;;
            size)    action_size "$project_dir"     || rc=$? ;;
            test)    action_test "$project_dir"     || rc=$? ;;
        esac

        if [ $rc -ne 0 ]; then
            results+=("${RED}✗ $action${RESET} (exit $rc)")
            overall=$rc
            fail "Action '$action' failed — stopping."
            break
        else
            results+=("${GREEN}✓ $action${RESET}")
        fi
    done

    # Summary (only if multiple actions)
    if [ ${#results[@]} -gt 1 ]; then
        echo ""
        echo -e "${BOLD}Summary:${RESET}"
        for r in "${results[@]}"; do
            echo -e "  $r"
        done
    fi

    exit $overall
}

main "$@"
