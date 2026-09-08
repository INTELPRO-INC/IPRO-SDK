# Linker Script Generation - Quick Start

## Overview

Linker scripts are now automatically generated based on configuration. No manual editing of `.ld` files needed.

## Configuration Options

### 1. Memory Configuration (PSRAM)

#### PSRAM + OCRAM (Recommended)
When `CONFIG_USE_PSRAM=y` in `.config`:
- PSRAM region available (16MB)
- OCRAM region available (256KB)
- RAM points to PSRAM for larger applications

#### OCRAM Only
When `CONFIG_USE_PSRAM` is not set:
- Only OCRAM region available (256KB)
- RAM points to OCRAM
- Smaller memory footprint

### 2. NVDS Section Configuration

NVDS (Non-Volatile Data Storage) section can be enabled/disabled:

**Default behavior**:
- **All builds** (RAM and Flash): NVDS enabled (1MB reserved)

**Override in `.config`**:
```bash
# Use default (NVDS enabled)
# CONFIG_ENABLE_NVDS is not set

# Explicitly disable NVDS (save 1MB RAM)
CONFIG_ENABLE_NVDS=n
```

## How to Configure

Edit your application's `.config`:

```bash
# Memory configuration
CONFIG_USE_PSRAM=y        # Enable PSRAM + OCRAM
# CONFIG_USE_PSRAM is not set  # OCRAM only

# NVDS configuration (optional - uses defaults if not set)
CONFIG_ENABLE_NVDS=y      # Enable NVDS section
# CONFIG_ENABLE_NVDS=n    # Disable NVDS section
```

### 3. Using NVDS Symbols in C Code

When NVDS is enabled, use `CONFIG_LINKER_NVDS_ENABLED` macro:

```c
#if defined(CONFIG_LINKER_NVDS_ENABLED) && (CONFIG_LINKER_NVDS_ENABLED == 1)
    extern uint8_t __nvds_start[];
    extern uint8_t __nvds_size[];
    // Access NVDS region
#else
    // NVDS disabled
#endif
```

**Important**: Do NOT use `CONFIG_ENABLE_NVDS` from `.config` in C code. It may be a string (`"n"` or `"y"`). Always use `CONFIG_LINKER_NVDS_ENABLED` (integer 0 or 1).

## How to Configure

Then rebuild:
```bash
cd apps/platform/ipro7_demo
rm -rf build
make
```

## Verify Configuration

Check CMake output during build:
```
-- Generated linker script: .../build/linker/ipro7_evb_ram.ld
--   - Board: ipro7_evb
--   - Build Type: ram
--   - Config: chip-generic (ipro7)  ← Shows fallback mechanism
--   - RAM Region: psram
--   - NVDS Enabled: ON
```

Inspect generated linker script:
```bash
cat apps/platform/ipro7_demo/build/linker/ipro7_evb_ram.ld
```

## Adding New Chip

1. Create config files: `bsp/board/linker/configs/<chip>_<type>.cmake`
2. Define memory layout (Flash, PSRAM, OCRAM, etc.)
3. **All boards** using that chip automatically use it

Example for IPRO8:
```bash
# Create two files:
bsp/board/linker/configs/ipro8_flash.cmake
bsp/board/linker/configs/ipro8_ram.cmake
```

Now all IPRO8 boards (EVB, FPGA, future boards) share these configurations.

See `README.md` for details.

## Backward Compatibility

To use static linker scripts (old behavior):
```cmake
set(CONFIG_USE_GENERATED_LINKER_SCRIPT OFF)
```

## File Structure

```
bsp/board/linker/
├── ipro_common.ld.in              # Template
├── generate_linker_script.cmake   # Generation logic with fallback
├── configs/                        # Chip-generic configurations
│   ├── ipro7_flash.cmake          # All IPRO7 boards
│   ├── ipro7_ram.cmake            # All IPRO7 boards
│   └── ipro6le_flash.cmake          # All IPRO6LE boards
├── README.md                       # Detailed documentation
└── USAGE.md                        # This file
```

**Note**: EVB and FPGA boards share the same configurations when memory layout is identical.

## Generated Output

Linker scripts are generated to:
```
<app>/build/linker/<board>_<type>.ld
```

Example:
```
apps/platform/ipro7_demo/build/linker/ipro7_evb_ram.ld
```

## Troubleshooting

**Problem**: Build fails with "Configuration file not found"
**Solution**: Create the configuration file in `bsp/board/linker/configs/`

**Problem**: Wrong RAM region selected
**Solution**: Check `CONFIG_USE_PSRAM` setting in `.config`

**Problem**: Need custom memory layout
**Solution**: Edit the board's config file in `bsp/board/linker/configs/`
