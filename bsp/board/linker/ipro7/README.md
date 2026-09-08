# IPRO7 Linker Templates

This directory contains linker script templates for IPRO7 chip.

## Available Templates

### 1. ipro7_common.ld.in (Default)
Standard linker template for IPRO7 chip. Used by default if no `LINKER_TEMPLATE` is specified in config files.

## Adding Alternate Templates

You can create alternate templates for specific use cases:

```bash
# Example: Create a specialized template
cp ipro7_common.ld.in ipro7_secure.ld.in
# Edit ipro7_secure.ld.in with secure boot sections...
```

Then specify it in your config file (e.g., `configs/ipro7_flash.cmake`):

```cmake
# Use alternate template
set(LINKER_TEMPLATE "ipro7_secure.ld.in")
```

## Template Selection

The template selection follows this logic:
1. If `LINKER_TEMPLATE` is defined in config file → use specified template
2. Otherwise → use `ipro7_common.ld.in` (default)

All templates must be placed in this `ipro7/` directory.

## Variables Available in Templates

Templates can use these variables (defined in config files):
- `@FLASH_ORIGIN@` - Flash memory start address
- `@FLASH_SIZE@` - Flash memory size
- `@OCRAM_ORIGIN@` - OCRAM start address
- `@OCRAM_SIZE@` - OCRAM size
- `@PSRAM_ORIGIN@` - PSRAM start address (if enabled)
- `@PSRAM_SIZE@` - PSRAM size (if enabled)
- `@RAM_REGION@` - RAM region alias (ocram or psram)
- `@STACK_SIZE@` - Stack size
- `@HEAP_SIZE@` - Heap size
- `@EM_SIZE@` - EM size
- `@HBNRAM_ORIGIN@` - HBNRAM start address
- `@HBNRAM_SIZE@` - HBNRAM size
- `@PSRAM_MEMORY_REGION@` - PSRAM memory region definition
- `@NVDS_SECTION@` - NVDS section (if enabled)
- `@NVDS_PROVIDES@` - NVDS symbol provides

See `generate_linker_script.cmake` for complete list.
