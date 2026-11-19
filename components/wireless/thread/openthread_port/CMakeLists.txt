__generate_library()

__component_add_include_dirs(include)
__component_add_include_dirs(.)
if(NOT IPRO_MATTER_PACKAGE_ACTIVE OR IPRO_MATTER_BR_PACKAGE_ACTIVE)
__component_add_include_dirs(../openthread/include)
__component_add_include_dirs(../openthread/src)             # for "include/common/code_utils.hpp"
__component_add_include_dirs(../openthread/src/include)     # for "common/code_utils.hpp"
__component_add_include_dirs(../openthread/src/core)
__component_add_include_dirs(../openthread/examples/platforms)
if(NOT IPRO_MATTER_PACKAGE_ACTIVE)
    __component_add_include_dirs(../../crypto/mbedtls/mbedtls/include)
    __component_add_include_dirs(../../crypto/mbedtls/port)
endif()
endif()

# IEEE 802.15.4 driver includes
__component_add_include_dirs(../../ieee802154/include)

__component_add_private_include_dirs(src)

__component_add_sources(
    src/ot_stub.c              # Platform utilities (assert/log/CLI helpers)
    src/alarm.c                # Timer/alarm implementation (FreeRTOS)
    src/random.c               # Random number generation (TRNG)
    src/settings.c             # Settings/NVS layer (EasyFlash)
    src/ot_freertos.c          # FreeRTOS system integration (event handling + main init)
    src/lp_sleep.c             # PDS31 tickless-idle (no-op stub when CONFIG_THREAD_LP_SLEEP=n)
)

# These files reach into FTD/MTD-only C++ internals and cannot be compiled
# against the deliberately minimal radio-only OpenThread instance.
if(CONFIG_THREAD_WIFI_BORDER_ROUTER)
    __component_add_sources(src/ot_memory.c src/ot_cli_line.c)
elseif(CONFIG_NEWLIBC_CONSOLE_GUARD)
    # CLI output emitted as whole lines (with the console guard: never mixed
    # with other tasks' lines, e.g. the radio_trace stream)
    __component_add_sources(src/ot_cli_line.c)
endif()

# platform PDS through ipro_pm (the radio as a power-management participant)
if(CONFIG_IPRO_PM_THREAD AND CONFIG_IPRO_PM AND NOT CONFIG_THREAD_RADIO_SPINEL)
    __component_add_sources(src/ot_pm.c)
endif()

if(CONFIG_THREAD_RADIO_SPINEL)
    __component_add_sources(src/spinel_uart.cpp src/ot_radio_spinel.cpp src/ot_rcp_health.c)
    target_compile_options(openthread_port PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>)
else()
    __component_add_sources(src/ot_radio_ieee802154.c src/ot_radio_csl.c)
endif()

if(NOT CONFIG_OPENTHREAD_RADIO AND NOT CONFIG_THREAD_RADIO_SPINEL
   AND NOT IPRO_MATTER_PACKAGE_ACTIVE)
    __component_add_sources(
        src/ot_fastpoll_shim.cpp
        src/sed_fastpoll.c
    )
endif()

# C++ files in this component need OT's role + platform defines because they
# include OT internal headers (instance.hpp etc.). Match what OT itself sets
# on its openthread-{ftd,mtd,radio} targets so types compile consistently.
# Also disable exceptions so we link cleanly with OT's -fno-exceptions cores.
if(CONFIG_OPENTHREAD_FTD)
    __component_add_compile_definitions(-DOPENTHREAD_FTD=1 -DOPENTHREAD_MTD=0 -DOPENTHREAD_RADIO=0)
elseif(CONFIG_OPENTHREAD_MTD)
    __component_add_compile_definitions(-DOPENTHREAD_FTD=0 -DOPENTHREAD_MTD=1 -DOPENTHREAD_RADIO=0)
elseif(CONFIG_OPENTHREAD_RADIO)
    __component_add_compile_definitions(-DOPENTHREAD_FTD=0 -DOPENTHREAD_MTD=0 -DOPENTHREAD_RADIO=1)
endif()

# OT internal headers reference these platform-class defines via ot-config.
# Mirror them here so openthread_port's C++ shim builds without depending on
# ot-config (which is created later, when add_subdirectory(openthread) runs).
__component_add_compile_definitions(-DOPENTHREAD_PLATFORM_POSIX=0 -DOPENTHREAD_PLATFORM_NEXUS=0)

target_compile_options(openthread_port PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions -fno-rtti -Wno-shadow>
)

# ot_fastpoll_shim.cpp reaches into OT C++ internals (Mle / Mac /
# KeyManager / SubMac) via AsCoreType() casts, so its class layout MUST
# match the OT core it links against. The two builds use DIFFERENT OT
# core flags, so the shim must follow suit:
#
#   - Matter build: OT core is compiled by CHIP with -std=gnu++17
#     -fno-short-enums. Match that.
#   - Standalone SDK build: OT core (libopenthread-mtd) is compiled by
#     the SDK with -fshort-enums (compiler_flags.cmake, global). Forcing
#     -fno-short-enums here makes enum members a different SIZE than in
#     the core → every member past the first enum is at the wrong offset
#     → mle.GetRloc16() / KeyManager / SubMac::mCurrKey read garbage and
#     fast-poll builds a bad Data Request. Match the core: -fshort-enums.
#
# Per-source COMPILE_OPTIONS land AFTER target/dir/global flags, and gcc
# takes the LAST -std=/-f...-enums it sees, so these win.
if(IPRO_MATTER_PACKAGE_ACTIVE)
    # No private-layout-dependent fast-poll code is built in this profile.
elseif(CONFIG_MATTER_ENABLE)
    # CHIP builds libopenthread-mtd with -std=gnu++17 -fno-short-enums.
    set_source_files_properties(src/ot_fastpoll_shim.cpp PROPERTIES
        COMPILE_OPTIONS "-std=gnu++17;-fno-short-enums")
else()
    # Standalone SDK builds libopenthread-mtd with -std=c++11 -fshort-enums
    # (see openthread-mtd.dir/flags.make). The shim casts otInstance* to the
    # OT C++ Instance and indexes members via Get<Type>()/AsCoreType, so its
    # class layout MUST match the core. Layout = f(std, enum-size, AND the
    # OPENTHREAD_CONFIG_* feature macros that add/remove Instance members).
    #
    # OpenThread's own CMake injects several layout-affecting -D macros onto
    # the openthread-mtd target that the openthread_port component does NOT
    # inherit. Notably OPENTHREAD_CONFIG_THREAD_VERSION (version-gated
    # members) and OPENTHREAD_CONFIG_PING_SENDER_ENABLE (adds a PingSender
    # member to Instance, shifting every Get<Type>() offset after it).
    # Without these the shim builds a DIFFERENT Instance layout → Get<>()
    # returns wrong-offset pointers → StopPolling()/IsChild() fault and hang
    # the OT task inside sed_fastpoll_enable(). Mirror the core's macro set
    # EXACTLY (diffed from compile_commands.json: openthread-mtd vs shim).
    set_source_files_properties(src/ot_fastpoll_shim.cpp PROPERTIES
        COMPILE_OPTIONS "-std=c++11;-fshort-enums;-DOPENTHREAD_CONFIG_THREAD_VERSION=OT_THREAD_VERSION_1_4;-DOPENTHREAD_CONFIG_PING_SENDER_ENABLE=1;-DOPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS=0;-DOPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS_MANAGEMENT=0;-DOPENTHREAD_CONFIG_NCP_HDLC_ENABLE=1;-DOPENTHREAD_CONFIG_NCP_INFRA_IF_ENABLE=0;-DOPENTHREAD_CONFIG_NCP_DNSSD_ENABLE=0;-DOPENTHREAD_CONFIG_NCP_CLI_STREAM_ENABLE=0")
endif()

# Project / platform config files.
#
# Both SDK Thread apps and Matter apps build OT from THIS tree (CHIP's
# build sets openthread_root=this dir via args.gni). To keep OT class
# layout consistent between openthread_port (incl. the ot_fastpoll_shim
# that reaches into Mle / KeyManager via `&AsCoreType(otInst)` casts)
# and CHIP's pre-built OT objects, both sides MUST compile with the
# same project config header.
#
# CHIP uses ipro7-openthread-config.h (in src/platform/ipro7). When
# CONFIG_MATTER_ENABLE is set we follow suit; standalone SDK Thread
# apps keep openthread-core-ipro7-config.h (Kconfig-driven).
if(IPRO_MATTER_BR_PACKAGE_ACTIVE)
    __component_add_compile_definitions(-DOPENTHREAD_PROJECT_CORE_CONFIG_FILE="openthread-core-ipro7-config.h")
    __component_add_compile_definitions(-DOPENTHREAD_PLATFORM_CORE_CONFIG_FILE="openthread-core-ipro7-config.h")
    __component_add_sources(src/matter_br_package_contract.c)
    message(STATUS "[ot_port] BR package: retain SDK core/short-enum ABI")
elseif(IPRO_MATTER_PACKAGE_ACTIVE)
    __component_add_compile_definitions(-DOPENTHREAD_PROJECT_CORE_CONFIG_FILE="ipro/OpenThreadConfig.h")
    __component_add_sources(src/matter_package_contract.c)
    # Per-source options follow the SDK interface's -fshort-enums, so they
    # actually win. Do not change the enum ABI of BLE/vendor consumers.
    get_target_property(_matter_ot_sources openthread_port SOURCES)
    set_source_files_properties(${_matter_ot_sources} PROPERTIES COMPILE_OPTIONS "-fno-short-enums")
    message(STATUS "[ot_port] matched package headers/core; no private fast-poll shim")
elseif(CONFIG_MATTER_ENABLE)
    # Unreachable since 2026-09-16: components/matter refuses to configure
    # without a package, and a package sets IPRO_MATTER_PACKAGE_ACTIVE above.
    message(FATAL_ERROR "[ot_port] CONFIG_MATTER_ENABLE without a Matter package; see components/matter/CMakeLists.txt")
else()
    __component_add_compile_definitions(-DOPENTHREAD_PROJECT_CORE_CONFIG_FILE="openthread-core-ipro7-config.h")
    __component_add_compile_definitions(-DOPENTHREAD_PLATFORM_CORE_CONFIG_FILE="openthread-core-ipro7-config.h")
endif()

# Add compile definitions based on configuration
__component_add_compile_definitions(-DOT_FREERTOS_ENABLE=1)

# Note: OPENTHREAD_FTD, OPENTHREAD_MTD, and OPENTHREAD_CONFIG_THREAD_VERSION
# are set by the main OpenThread CMakeLists.txt based on OT_FTD/OT_MTD options
# Don't redefine them here to avoid warnings

if(CONFIG_OPENTHREAD_RADIO)
    __component_add_compile_definitions(-DOPENTHREAD_RADIO=1)
    __component_add_sources(src/ot_rcp_uart.c)
endif()

if(CONFIG_OPENTHREAD_CLI)
    __component_add_compile_definitions(-DOPENTHREAD_CONFIG_CLI_ENABLE=1)
endif()

if(CONFIG_OPENTHREAD_COMMISSIONER)
    __component_add_compile_definitions(-DOPENTHREAD_CONFIG_COMMISSIONER_ENABLE=1)
endif()

if(CONFIG_OPENTHREAD_JOINER)
    __component_add_compile_definitions(-DOPENTHREAD_CONFIG_JOINER_ENABLE=1)
endif()

if(CONFIG_OPENTHREAD_DHCP6_CLIENT)
    __component_add_compile_definitions(-DOPENTHREAD_CONFIG_DHCP6_CLIENT_ENABLE=1)
endif()

if(CONFIG_OPENTHREAD_DHCP6_SERVER)
    __component_add_compile_definitions(-DOPENTHREAD_CONFIG_DHCP6_SERVER_ENABLE=1)
endif()

if(CONFIG_OPENTHREAD_DNS_CLIENT)
    __component_add_compile_definitions(-DOPENTHREAD_CONFIG_DNS_CLIENT_ENABLE=1)
endif()
