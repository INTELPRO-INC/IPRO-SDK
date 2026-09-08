# Linker Script Generation Architecture

This directory contains the infrastructure for generating linker scripts based on board and build type configurations.

## Architecture Overview

```
bsp/board/linker/
├── ipro6le/                           # IPRO6LE chip templates
│   ├── ipro6le_common.ld.in          # Default IPRO6LE template
│   └── README.md                    # IPRO6LE template documentation
├── ipro7/                           # IPRO7 chip templates
│   ├── ipro7_common.ld.in          # Default IPRO7 template
│   └── README.md                    # IPRO7 template documentation
├── generate_linker_script.cmake     # CMake generation function with fallback
├── configs/                         # Chip-generic configurations
│   ├── ipro7_flash.cmake           # IPRO7 Flash (all boards)
│   ├── ipro7_ram.cmake             # IPRO7 RAM (all boards)
│   └── ipro6le_flash.cmake           # IPRO6LE Flash (all boards)
└── README.md                        # This file
```

## Key Features

### 1. Chip-Specific Template Architecture

Each chip has its own template directory (e.g., `ipro6le/`, `ipro7/`) containing:
- Default template: `{chip}_common.ld.in`
- Optional alternate templates for special configurations
- README documenting chip-specific template usage

Templates are automatically selected based on the chip name extracted from the board name (e.g., `ipro7_evb` → uses `ipro7/` templates).

### 2. Template Selection

Config files can optionally specify which template to use:

```cmake
# In configs/ipro7_flash.cmake
set(LINKER_TEMPLATE "ipro7_secure.ld.in")  # Use alternate template
```

If not specified, defaults to `{chip}_common.ld.in`.

### 3. Configuration-Driven with Fallback

Configuration files are organized by chip, not by board. The system uses a smart fallback mechanism:

1. **First try**: Board-specific config (e.g., `ipro7_evb_ram.cmake`)
2. **Fallback**: Chip-generic config (e.g., `ipro7_ram.cmake`)

This eliminates duplicate configurations when boards share the same memory layout.

Each configuration file defines:
- Memory regions (Flash, PSRAM, OCRAM, HBNRAM)
- Memory layout (origins and sizes)
- Stack/Heap sizes
- NVDS section configuration
- Optional: `LINKER_TEMPLATE` to use alternate template

### 4. Dynamic Configuration

The architecture supports runtime configuration of memory layout and features:

#### PSRAM Configuration

Two memory configurations:

#### Configuration 1: PSRAM + OCRAM (when `CONFIG_USE_PSRAM=ON`)
```
MEMORY
{
  flash     (rxa!w) : ORIGIN = 0x1e000000, LENGTH = 4M
  psram     (wxa!r) : ORIGIN = 0x1e400000, LENGTH = 16M
  ocram     (wxa!r) : ORIGIN = 0x11010000, LENGTH = 256K
  hbnram    (wxa!r) : ORIGIN = 0x20010000, LENGTH = 4K
}
REGION_ALIAS("RAM", psram)  # RAM points to PSRAM
```

#### Configuration 2: OCRAM only (when `CONFIG_USE_PSRAM=OFF` or not set)
```
MEMORY
{
  flash     (rxa!w) : ORIGIN = 0x1e000000, LENGTH = 4M
  /* PSRAM disabled - CONFIG_USE_PSRAM not set */
  ocram     (wxa!r) : ORIGIN = 0x11010000, LENGTH = 256K
  hbnram    (wxa!r) : ORIGIN = 0x20010000, LENGTH = 4K
}
REGION_ALIAS("RAM", ocram)  # RAM points to OCRAM
```

#### NVDS Section Configuration

NVDS (Non-Volatile Data Storage) section can be enabled/disabled via `CONFIG_ENABLE_NVDS`:

**Enabled** (default for all builds):
```ld
.nvds (NOLOAD) : ALIGN(4K)
{
  PROVIDE( __nvds_start = . );
  . += 1M;  /* 1MB reserved */
  PROVIDE( __nvds_end = . );
} >RAM
```

**Disabled** (`CONFIG_ENABLE_NVDS=n`):
```ld
/* NVDS section disabled - CONFIG_ENABLE_NVDS not set */
```

**Configuration priority**:
1. `CONFIG_ENABLE_NVDS=n` in `.config` → Force disable
2. `CONFIG_ENABLE_NVDS=y` in `.config` → Force enable (same as default)
3. `# CONFIG_ENABLE_NVDS is not set` (Kconfig style) → Use default (ON)
4. Variable not defined → Use board config default (`ENABLE_NVDS=ON`)

**Default**: NVDS enabled for **all builds** (RAM and Flash).

#### Using NVDS in C Code

The build system provides `CONFIG_LINKER_NVDS_ENABLED` macro for conditional compilation:

```c
#if defined(CONFIG_LINKER_NVDS_ENABLED) && (CONFIG_LINKER_NVDS_ENABLED == 1)
    // NVDS is enabled - symbols are available
    extern uint8_t __nvds_start[];
    extern uint8_t __nvds_size[];
    printf("NVDS region: 0x%08lx\n", (unsigned long)__nvds_start);
#else
    // NVDS is disabled - symbols not available
    printf("NVDS: disabled\n");
#endif
```

**Important**: Do NOT use `CONFIG_ENABLE_NVDS` from `.config` directly in C code, as it may be defined as a string (`"n"` or `"y"`) which causes preprocessor errors. Always use `CONFIG_LINKER_NVDS_ENABLED` instead, which is guaranteed to be an integer (0 or 1).

## Usage

### Automatic Generation (Default)

Linker scripts are automatically generated during the CMake configuration phase:

```bash
cd apps/platform/ipro7_demo
make
# Generated script: build/linker/ipro7_evb_ram.ld
```

### Manual Generation

You can manually generate a linker script using CMake:

```cmake
include(${CMAKE_SOURCE_DIR}/bsp/board/linker/generate_linker_script.cmake)

generate_linker_script(
    BOARD ipro7_evb
    BUILD_TYPE ram
    OUTPUT_DIR ${CMAKE_BINARY_DIR}/linker
)

# Result in: GENERATED_LINKER_SCRIPT variable
```

### Disable Auto-Generation

To use static linker scripts instead (backward compatibility):

```cmake
set(CONFIG_USE_GENERATED_LINKER_SCRIPT OFF)
```

## Adding New Configurations

### 1. For a New Chip

**Step 1**: Create chip template directory and default template

```bash
mkdir bsp/board/linker/ipro8/
cp bsp/board/linker/ipro7/ipro7_common.ld.in bsp/board/linker/ipro8/ipro8_common.ld.in
# Edit ipro8_common.ld.in for IPRO8-specific sections...
```

**Step 2**: Create chip-generic configuration files: `configs/<chip>_<build_type>.cmake`

Example for IPRO8:
- `configs/ipro8_flash.cmake`
- `configs/ipro8_ram.cmake`

These will apply to **all boards** using that chip (EVB, FPGA, etc.).

**Step 3** (Optional): Add README to document chip-specific templates

```bash
# Document any IPRO8-specific template features
vim bsp/board/linker/ipro8/README.md
```

### 2. For Board-Specific Overrides (Optional)

If a specific board needs different memory layout:

Create: `configs/<chip>_<board>_<build_type>.cmake`

Example: `configs/ipro8_custom_flash.cmake`

```cmake
# IPRO8 EVB Flash Configuration

set(FLASH_ORIGIN "0x13000000")
set(FLASH_SIZE   "8M")
set(PSRAM_ORIGIN "0x1e400000")
set(PSRAM_SIZE   "32M")
set(OCRAM_ORIGIN "0x11010000")
set(OCRAM_SIZE   "512K")
set(HBNRAM_ORIGIN "0x20010000")
set(HBNRAM_SIZE  "4K")

set(RAM_REGION   "psram")
set(STACK_SIZE   "8K")
set(HEAP_SIZE    "4K")
set(EM_SIZE      "32K")

# NVDS configuration
set(ENABLE_NVDS  OFF)
set(NVDS_SECTION "")
set(NVDS_PROVIDES "")
```

### 3. For Alternate Templates (Same Chip)

If you need a specialized linker script for specific use cases (e.g., secure boot, low power):

**Step 1**: Create alternate template in chip directory

```bash
cd bsp/board/linker/ipro7/
cp ipro7_common.ld.in ipro7_secure.ld.in
# Edit ipro7_secure.ld.in with secure boot sections...
```

**Step 2**: Specify template in config file

```cmake
# In configs/ipro7_flash.cmake (or a board-specific config)
set(LINKER_TEMPLATE "ipro7_secure.ld.in")  # Use alternate template

# Rest of configuration...
set(FLASH_ORIGIN "0x13000000")
# ...
```

**Example use cases**:
- `ipro7_secure.ld.in` - Secure boot with signed sections
- `ipro7_lowpower.ld.in` - Low-power mode with retention memory
- `ipro7_debug.ld.in` - Debug build with extra sections

### 4. Configuration Lookup Process

The generation system automatically searches for configurations:

```
For board "ipro7_evb" with build type "ram":

1. Try: configs/ipro7_evb_ram.cmake  (board-specific)
   ↓ Not found
2. Try: configs/ipro7_ram.cmake      (chip-generic)
   ✓ Found! Use this.
```

**Result**: All IPRO7 boards (EVB, FPGA, etc.) share the same configuration unless overridden.

### 4. No CMake Changes Needed

The system automatically detects board and build type from Kconfig variables.

## Configuration Variables

### Required Variables

| Variable | Description | Example |
|----------|-------------|---------|
| `FLASH_ORIGIN` | Flash memory start address | `0x1e000000` |
| `FLASH_SIZE` | Flash memory size | `4M` |
| `PSRAM_ORIGIN` | PSRAM start address | `0x1e400000` |
| `PSRAM_SIZE` | PSRAM size | `16M` |
| `OCRAM_ORIGIN` | OCRAM start address | `0x11010000` |
| `OCRAM_SIZE` | OCRAM size | `256K` |
| `HBNRAM_ORIGIN` | Hibernate RAM start address | `0x20010000` |
| `HBNRAM_SIZE` | Hibernate RAM size | `4K` |
| `RAM_REGION` | Default RAM region | `psram` or `ocram` |
| `STACK_SIZE` | Stack size | `4K` |
| `HEAP_SIZE` | Heap size | `2K` |
| `EM_SIZE` | EM size | `16K` |

### Optional Variables

| Variable | Description | Default |
|----------|-------------|---------|
| `ENABLE_NVDS` | Enable NVDS section in board config | `ON` (all builds) |
| `NVDS_SIZE` | NVDS section size | `1M` |

**Note**: NVDS section and symbols are auto-generated. No need to define `NVDS_SECTION` or `NVDS_PROVIDES` in config files.

## Build Output

Generated linker scripts are placed in:
```
<BUILD_DIR>/linker/<board>_<build_type>.ld
```

Example:
```
apps/platform/ipro7_demo/build/linker/ipro7_evb_ram.ld
```

## CMake Integration

The generation is integrated into the board CMakeLists.txt:

```cmake
# bsp/board/ipro7/CMakeLists.txt

if(DEFINED CONFIG_USE_GENERATED_LINKER_SCRIPT OR NOT DEFINED CONFIG_USE_GENERATED_LINKER_SCRIPT)
    include(${CMAKE_SOURCE_DIR}/bsp/board/linker/generate_linker_script.cmake)

    generate_linker_script(
        BOARD ${BOARD_NAME}
        BUILD_TYPE ${BUILD_TYPE}
        OUTPUT_DIR ${CMAKE_BINARY_DIR}/linker
    )

    set(LINKER_PATH "${GENERATED_LINKER_SCRIPT}")
    message(STATUS "Using generated linker script: ${LINKER_PATH}")
else()
    # Use static linker script (backward compatibility)
    set(LINKER_FILE "ipro7_${BUILD_TYPE}.ld")
    set(LINKER_PATH "${CMAKE_CURRENT_SOURCE_DIR}/${BOARD_DIR}/${LINKER_FILE}")
    message(STATUS "Using static linker script: ${LINKER_PATH}")
endif()

__set_linker_script("${LINKER_PATH}")
```

## Verification

To verify the generated linker script:

1. Build the project:
   ```bash
   cd apps/platform/ipro7_demo
   make
   ```

2. Check CMake output:
   ```
   -- Generated linker script: .../build/linker/ipro7_evb_ram.ld
   --   - Board: ipro7_evb
   --   - Build Type: ram
   --   - Flash Origin: 0x1e000000
   --   - RAM Region: ocram (or psram)
   --   - NVDS Enabled: ON
   ```

3. Inspect generated file:
   ```bash
   cat apps/platform/ipro7_demo/build/linker/ipro7_evb_ram.ld
   ```

## Troubleshooting

### Configuration File Not Found

**Error:**
```
Configuration file not found: .../configs/ipro7_evb_ram.cmake
```

**Solution:**
Create the missing configuration file in `bsp/board/linker/configs/`.

### Wrong RAM Region

**Issue:** PSRAM expected but OCRAM used (or vice versa)

**Solution:**
Check `CONFIG_USE_PSRAM` setting:
- If PSRAM needed: Ensure `CONFIG_USE_PSRAM=y` in `.config`
- If OCRAM only: Ensure `CONFIG_USE_PSRAM` is not set or `=n`

### Static Linker Script Used

**Issue:** System uses static `.ld` files instead of generated ones

**Solution:**
Remove or comment out:
```cmake
set(CONFIG_USE_GENERATED_LINKER_SCRIPT OFF)
```

## Design Philosophy

This architecture follows the "good taste" principle:
- **Single source of truth**: One template, chip-generic configurations
- **No special cases**: Configuration-driven with smart fallback
- **Eliminate duplication**: FPGA and EVB share configs when memory layout is identical
- **Simple data structures**: Plain CMake variables, no complex logic
- **Explicit over implicit**: Clear configuration files, minimal magic

### Why Chip-Generic?

**Before** (5 config files):
```
ipro7_evb_flash.cmake    }
ipro7_fpga_flash.cmake   } Identical content!
ipro7_evb_ram.cmake      }
ipro7_fpga_ram.cmake     } Identical content!
ipro6le_evb_flash.cmake
```

**After** (3 config files):
```
ipro7_flash.cmake  → Applies to EVB, FPGA, and future boards
ipro7_ram.cmake    → Applies to EVB, FPGA, and future boards
ipro6le_flash.cmake  → Applies to all IPRO6LE boards
```

**Benefit**: Less code, easier maintenance, same flexibility.

## Migration from Static Linker Scripts

The system maintains backward compatibility. To migrate:

1. **Phase 1**: Keep both systems (current state)
   - Generated scripts used by default
   - Static scripts available as fallback

2. **Phase 2**: Test all boards with generated scripts
   - Verify memory layout matches expectations
   - Check PSRAM/OCRAM configurations

3. **Phase 3**: Deprecate static scripts
   - Move static `.ld` files to `backup/` directory
   - Update documentation

## Future Enhancements

Possible improvements:
- Support for additional memory regions (DTCM, ITCM)
- Per-section memory placement configuration
- Automatic memory usage optimization
- Linker script validation tools
