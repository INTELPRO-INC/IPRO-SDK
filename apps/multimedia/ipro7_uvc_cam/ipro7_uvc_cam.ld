/*
 * IPRO7 UVC Camera Application Linker Script
 *
 * App-local script: it takes precedence over the generated template
 * bsp/board/linker/ipro7/ipro7_common.ld.in, so sections added to that
 * template have to be carried over here by hand.
 *
 * Memory Layout:
 *   ocram      : 0x11010000 - 0x11020000 (64KB, sram1) - Retained during LP mode
 *   ocram_noret: 0x11020000 - 0x11030000 (64KB, sram4@blai) - NOT retained
 *
 * With cr_blai_en=1 (NPU active): sram4 remapped to 0x11020000, 128KB total
 * With cr_blai_en=0 (NPU off):    sram2/3/4 at natural addr, 256KB total
 *   After LP cycles, clear cr_blai_en to get 192KB from 0x11020000.
 *
 * Usage for non-retained buffers:
 *   ATTR_OCRAM_NORET uint8_t my_buffer[1024];
 */

OUTPUT_ARCH( "riscv" )

ENTRY( _start )

/* Force inclusion of LP firmware symbols from library */
EXTERN( hal_lp_wakeup_entry )
EXTERN( hal_lp_init )

__EM_SIZE = 0K;

MEMORY
{
  /* FW partition. Default 2M OTA layout (partition_cfg_2M.toml) bounds FW to
   * 0x3000..0x12A000 (LENGTH 0x127000). Keep this LENGTH in sync with the FW
   * size0 of the partition table you flash. */
  flash      (rxa!w) : ORIGIN = 0x13000000, LENGTH = 0x127000
  psram      (rwxa)  : ORIGIN = 0x1e000000, LENGTH = 8M

  /* OCRAM - Retained during low power mode (64KB = sram1) */
  ocram      (wxa!r) : ORIGIN = 0x11010000, LENGTH = 64K

  /* OCRAM No-Retention (64KB = sram4 remapped via cr_blai_en) */
  ocram_noret (wxa!r) : ORIGIN = 0x11020000, LENGTH = 64K

  hbnram     (wxa!r) : ORIGIN = 0x20090000, LENGTH = 4K
}

REGION_ALIAS("ROM", flash)
REGION_ALIAS("RAM", ocram)

SECTIONS
{
  /* To provide symbol __STACK_SIZE, __HEAP_SIZE */
  PROVIDE(__STACK_SIZE = 4K);
  PROVIDE(__HEAP_SIZE = 2K);
  __TOT_STACK_SIZE = __STACK_SIZE;

  .init           :
  {
    /* vector table locate at ROM */
    *(.text.vtable)
    KEEP (*(SORT_NONE(.text.init)))
    . = ALIGN(4);
  } >ROM

  /* Code section located at ROM */
  .text           :
  {
    *(.text.unlikely .text.unlikely.*)
    *(.text.startup .text.startup.*)
    *(.text .text.*)
    *(.gnu.linkonce.t.*)

    *(EXCLUDE_FILE
     ( *hal_mtimer*.o*
       *_udivdi3.o*
       *lib_a-memset*.o*) .text*)

    /* readonly data placed in ROM */
    . = ALIGN(8);
    *(.srodata.cst16)
    *(.srodata.cst8)
    *(.srodata.cst4)
    *(.srodata.cst2)
    *(.srodata .srodata.*)
    *(.rdata)
    *(.rodata .rodata.*)
    *(.gnu.linkonce.r.*)
    /* below sections are used for rt-thread */
    . = ALIGN(4);
    __rt_init_start = .;
    KEEP(*(SORT(.rti_fn*)))
    __rt_init_end = .;
    . = ALIGN(4);
    __fsymtab_start = .;
    KEEP(*(FSymTab))
    __fsymtab_end = .;
    . = ALIGN(4);
    __vsymtab_start = .;
    KEEP(*(VSymTab))
    __vsymtab_end = .;

	/* static cli cmds */
    . = ALIGN(4);
    _static_cli_cmds_start = .;
    KEEP(*(.static_cli_cmds))
    *(.static_cli_cmds)
    _static_cli_cmds_end = .;

    /* section information for usb desc */
    . = ALIGN(4);
    _usb_desc_start = .;
    KEEP(*(usb_desc))
    . = ALIGN(4);
    _usb_desc_end = .;

    /* camera driver */
    . = ALIGN(4);
    _cam_desc_start = .;
    KEEP(*(.camera_desc))
    *(.camera_desc)
    _cam_desc_end = .;

    /* isp param */
    . = ALIGN(4);
    _img_param_desc_start = .;
    KEEP(*(.img_param_desc))
    *(.img_param_desc)
    _img_param_desc_end = .;

  } >ROM

  .fini           :
  {
    KEEP (*(SORT_NONE(.fini)))
  } >ROM

  .preinit_array  :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >ROM

  .init_array     :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))
    KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >ROM

  .fini_array     :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))
    KEEP (*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } >ROM

  .ctors          :
  {
    /* gcc uses crtbegin.o to find the start of
     * the constructors, so we make sure it is
     * first.  Because this is a wildcard, it
     * doesn't matter if the user does not
     * actually link against crtbegin.o; the
     * linker won't look for a file to match a
     * wildcard.  The wildcard also means that it
     * doesn't matter which directory crtbegin.o
     * is in.
     */
    KEEP (*crtbegin.o(.ctors))
    KEEP (*crtbegin?.o(.ctors))
    /* We don't want to include the .ctor section from
     * the crtend.o file until after the sorted ctors.
     * The .ctor section from the crtend file contains the
     * end of ctors marker and it must be last
     */
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
  } >ROM

  .dtors          :
  {
    KEEP (*crtbegin.o(.dtors))
    KEEP (*crtbegin?.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
  } >ROM

  /* TCM code section - Essential LP wakeup code stays in fast OCRAM */
  .tcmcode : ALIGN(8)
  {
    /* Essential LP wakeup code - must stay in fast OCRAM */
    *hal_lp.c.o(.tcm_code.*)
    *hal_lp.c.o(.tcm_const.*)
    *hal_lp_entry.S.o(.tcm_code.*)
  } >RAM AT>ROM

  /* Writable TCM data in its own section, as in the shared template
   * (TCMCODE_SECTION in bsp/board/linker/generate_linker_script.cmake);
   * section_copy.c copies it via _tcmdata*. Not page-aligned on purpose:
   * that costs up to 4 KB of OCRAM. */
  .tcmdata : ALIGN(8)
  {
    /* TCM data - DMA descriptors, buffers requiring fast OCRAM access */
    *(.tcm_data*)
    . = ALIGN(8);
  } >RAM AT>ROM

  PROVIDE( _tcmtext_lma = LOADADDR(.tcmcode) );
  PROVIDE( _tcmtext = ADDR(.tcmcode) );
  PROVIDE( _tcmtext_end = ADDR(.tcmcode) + SIZEOF(.tcmcode) );
  PROVIDE( _tcmdata_lma = LOADADDR(.tcmdata) );
  PROVIDE( _tcmdata = ADDR(.tcmdata) );
  PROVIDE( _tcmdata_end = ADDR(.tcmdata) + SIZEOF(.tcmdata) );

  /* em_tcm_ret section - merged into ocram (sram1, 64KB) */
  .em_tcm_ret : ALIGN(8)
  {
    PROVIDE( __em_tcm_ret_start = . );
    /* LP main, Flash, UART, clock code - less critical for wakeup latency */
    *hal_lpfw_main.c.o(.tcm_code.*)
    *hal_lpfw_main.c.o(.tcm_const.*)
    *hal_flash.c.o(.tcm_code.*)
    *hal_flash.c.o(.tcm_const.*)
    *lp_uart.c.o(.tcm_code.*)
    *lp_uart.c.o(.tcm_const.*)
    *(.sclock_rlt_code.*)
    *(.sclock_rlt_const.*)
    /* All other tcm code */
    *(EXCLUDE_FILE(*hal_lp.c.o *hal_lp_entry.S.o) .tcm_code.*)
    *(EXCLUDE_FILE(*hal_lp.c.o *hal_lp_entry.S.o) .tcm_const.*)
    . = ALIGN(8);
    PROVIDE( __em_tcm_ret_end = . );
  } >RAM AT>ROM

  PROVIDE( _em_tcm_ret_lma = LOADADDR(.em_tcm_ret) );
  PROVIDE( _em_tcm_ret = ADDR(.em_tcm_ret) );

  /*
   * HBN Code Section - Low Power Firmware
   *
   * This section contains code that runs from HBN RAM after PDS wakeup.
   * VMA = hbnram (HBN_RAM_BASE, 0x20090000 for IPRO7) - where code executes
   * LMA = ROM (flash) - where code is stored
   *
   * The code is copied from flash to HBN RAM by hal_lp_load_hbn_code()
   * before entering low power mode.
   *
   * KEEP() is used to prevent garbage collection of LP firmware code.
   */
  .hbn_code : ALIGN(4)
  {
    __hbn_code_start = .;
    KEEP(*(.hbn_code))
    KEEP(*(.hbn_code.*))
    KEEP(*(.hbn_data))
    KEEP(*(.hbn_data.*))
    . = ALIGN(4);
    __hbn_code_end = .;
  } >hbnram AT>ROM

  PROVIDE( __hbn_code_load_addr = LOADADDR(.hbn_code) );

  /*
   * Retained HBN RAM diagnostics (hal_pm.c), placed as in the shared template
   * bsp/board/linker/ipro7/ipro7_common.ld.in. Without this rule .hbn_noinit
   * is an orphan that ld drops into OCRAM inside the .data copy range, so the
   * boot-time data copy overwrites what it was meant to keep. HBN RAM is
   * partitioned by hal_hbnram_mgnt.h: the LP parameter block starts at +0x400,
   * the app-owned window is +0x8e4..+0xeff (first word = bootloader magic), and
   * +0xf00 is the iot2lp parameter block.
   */
  .hbn_noinit ORIGIN(hbnram) + 0x8e8 (NOLOAD) : ALIGN(4)
  {
    __hbn_noinit_start = .;
    KEEP(*(.hbn_noinit))
    KEEP(*(.hbn_noinit.*))
    . = ALIGN(4);
    __hbn_noinit_end = .;
  } >hbnram

  ASSERT(__hbn_code_end <= ORIGIN(hbnram) + 0x400,
         "HBN LP firmware exceeds fixed 1KB region")
  ASSERT(__hbn_noinit_end <= ORIGIN(hbnram) + 0xf00,
         "HBN noinit exceeds IPRO7 reserved region")

  /* OCRAM data section - initialized data that must stay in fast OCRAM */
  .data_ocram : ALIGN(8)
  {
    PROVIDE( _data_ocram_lma = LOADADDR(.data_ocram) );
    PROVIDE( _data_ocram_vma = ADDR(.data_ocram) );
    *(.ocram_data)
    *(.ocram_data.*)
    . = ALIGN(8);
    PROVIDE( _edata_ocram = . );
  } >RAM AT>ROM

  /* OCRAM bss section - uninitialized data in fast OCRAM */
  .bss_ocram (NOLOAD) : ALIGN(8)
  {
    PROVIDE( _bss_ocram_start = . );
    *(.ocram_bss)
    *(.ocram_bss.*)
    . = ALIGN(8);
    PROVIDE( _bss_ocram_end = . );
  } >RAM

  PROVIDE( _ilm_lma = LOADADDR(.text) );
  PROVIDE( _ilm = ADDR(.text) );
  PROVIDE( _eilm = . );
  PROVIDE( _text_lma = LOADADDR(.text) );
  PROVIDE( _text = ADDR(.text) );
  PROVIDE (_etext = .);
  PROVIDE (__etext = .);
  PROVIDE (etext = .);

  .data            : ALIGN(8)
  {
    KEEP(*(.data.ctest*))
    *(.data .data.*)
    *(.gnu.linkonce.d.*)
    . = ALIGN(8);
    PROVIDE( __global_pointer$ = . + 0x800 );
    *(.sdata .sdata.* .sdata*)
    *(.gnu.linkonce.s.*)
    /* Fix undefined symbol: __eh_frame_start/__eh_frame_hdr_start/__eh_frame_end/__eh_frame_hdr_end */
    PROVIDE_HIDDEN (__eh_frame_hdr_start = .);
    *(.eh_frame_hdr)
    PROVIDE_HIDDEN (__eh_frame_hdr_end = .);
    PROVIDE_HIDDEN (__eh_frame_start = .);
    *(.eh_frame)
    PROVIDE_HIDDEN (__eh_frame_end = .);
    . = ALIGN(8);

    *(.vectable)

    *(._k_queue.static.*)
    *(._k_sem.static.*)
    *(._k_mutex.static.*)
    _bt_gatt_service_static_list_start = .;
    KEEP(*(SORT_BY_NAME("._bt_gatt_service_static.static.*")))
    _bt_gatt_service_static_list_end = .;
    _bt_l2cap_fixed_chan_list_start = .;
    KEEP(*(SORT_BY_NAME("._bt_l2cap_fixed_chan.static.*")))
    _bt_l2cap_fixed_chan_list_end = .;
    . = ALIGN(8);
    *(.decode_jpeg_data)
  } >RAM AT>ROM

  .tdata           : ALIGN(8)
  {
    PROVIDE( __tls_base = . );
    *(.tdata .tdata.* .gnu.linkonce.td.*)
  } >RAM AT>ROM

  PROVIDE( _data_lma = LOADADDR(.data) );
  PROVIDE( _data = ADDR(.data) );
  PROVIDE( _edata = . );
  PROVIDE( edata = . );

  PROVIDE( _fbss = . );
  PROVIDE( __bss_start = . );

  .tbss (NOLOAD)   : ALIGN(8)
  {
    *(.tbss .tbss.* .gnu.linkonce.tb.*)
    *(.tcommon)
    PROVIDE( __tls_end = . );
  } >RAM

  .tbss_space (NOLOAD) : ALIGN(8)
  {
    . = . + SIZEOF(.tbss);
  } >RAM

  .bss (NOLOAD)   : ALIGN(8)
  {
    *(.sbss*)
    *(.gnu.linkonce.sb.*)
    *(.bss .bss.*)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    . = ALIGN(8);
  } >RAM

  PROVIDE( _end = . );
  PROVIDE( end = . );

  /* No-init data section - not cleared during startup */
  /* Used for data that must survive BSS clear (e.g., boot2 parameters) */
  .noinit_data (NOLOAD) : ALIGN(8)
  {
    __noinit_data_start = .;
    *(.noinit_data)
    *(.noinit_data.*)
    . = ALIGN(8);
    __noinit_data_end = .;
  } >RAM

  /* Non-cacheable address offset: 0x21xxxxxx - 0x11xxxxxx = 0x10000000 */
  __NOCACHE_ADDR_OFFSET = 0x10000000;

  /* Record end of cacheable region for nocache section placement */
  __ocram_cacheable_end = .;

  /* Non-cacheable RAM section (initialized data) */
  /* VMA = current position + offset */
  .nocache_ram . + __NOCACHE_ADDR_OFFSET (NOLOAD) : ALIGN(8)
  {
    __nocache_ram_start = .;
    *(.nocache_ram)
    *(.nocache_ram.*)
    *(.noncacheable)
    *(.noncacheable.*)
    . = ALIGN(8);
    __nocache_ram_end = .;
  }

  /* Non-cacheable RAM section (uninitialized, no need to clear) */
  /* Continue from previous nocache section (already in non-cacheable space) */
  .nocache_noinit_ram __nocache_ram_end (NOLOAD) : ALIGN(8)
  {
    __nocache_noinit_start = .;
    *(.nocache_noinit_ram)
    *(.nocache_noinit_ram.*)
    . = ALIGN(8);
    __nocache_noinit_end = .;
  }

  /* Calculate total nocache size (in cacheable address space units) */
  __nocache_total_size = (__nocache_noinit_end - __NOCACHE_ADDR_OFFSET) - __ocram_cacheable_end;

  /* Calculate where stack should start (after nocache region in physical memory) */
  __stack_start_addr = __ocram_cacheable_end + __nocache_total_size;


  /* PSRAM code section - executable code in PSRAM */
  .text_psram : ALIGN(8)
  {
    PROVIDE( _text_psram_lma = LOADADDR(.text_psram) );
    PROVIDE( _text_psram_vma = ADDR(.text_psram) );
    *(.psram_code)
    *(.psram_code.*)
    . = ALIGN(8);
    PROVIDE( _etext_psram = . );
  } >psram AT>ROM


  /* PSRAM data section - initialized data in PSRAM */
  .data_psram : ALIGN(8)
  {
    PROVIDE( _data_psram_lma = LOADADDR(.data_psram) );
    PROVIDE( _data_psram_vma = ADDR(.data_psram) );
    *(.psram_data)
    *(.psram_data.*)
    . = ALIGN(8);
    PROVIDE( _edata_psram = . );
  } >psram AT>ROM


  /* PSRAM bss section - uninitialized data in PSRAM */
  .bss_psram (NOLOAD) : ALIGN(8)
  {
    PROVIDE( _bss_psram_start = . );
    *(.psram_bss)
    *(.psram_bss.*)
    . = ALIGN(8);
    PROVIDE( _bss_psram_end = . );
  } >psram


  /*
   * OCRAM No-Retention Section
   *
   * This section is for buffers that do NOT need to be retained during
   * low power mode (PDS/HBN). Contents will be lost after LP wakeup.
   *
   * Use cases:
   *   - Temporary image processing buffers
   *   - DMA transfer buffers
   *   - Audio/video frame buffers
   *   - Any data that will be regenerated after wakeup
   *
   * Usage:
   *   ATTR_OCRAM_NORET uint8_t frame_buffer[4096];
   *   ATTR_OCRAM_NORET_NOINIT uint8_t dma_buffer[1024];  // No zero-init
   */
  .ocram_noret (NOLOAD) : ALIGN(8)
  {
    PROVIDE( __ocram_noret_start = . );
    KEEP(*(.ocram_noret))
    KEEP(*(.ocram_noret.*))
    . = ALIGN(8);
  } >ocram_noret

  .ocram_noret_noinit (NOLOAD) : ALIGN(8)
  {
    KEEP(*(.ocram_noret_noinit))
    KEEP(*(.ocram_noret_noinit.*))
    . = ALIGN(8);
    PROVIDE( __ocram_noret_end = . );
  } >ocram_noret

  PROVIDE( __ocram_noret_size = __ocram_noret_end - __ocram_noret_start );

  /* Heap in ocram_noret for dynamic allocation of non-retained buffers */
  PROVIDE( __heap_ocram_noret_start = __ocram_noret_end );
  PROVIDE( __heap_ocram_noret_size = ORIGIN(ocram_noret) + LENGTH(ocram_noret) - __heap_ocram_noret_start );
  PROVIDE( __heap_ocram_noret_end = __heap_ocram_noret_start + __heap_ocram_noret_size );


  /* Stack section - explicitly set address to avoid overlap with nocache region */
  .stack __stack_start_addr (NOLOAD) :
  {
    . = ALIGN(16);
    PROVIDE( __StackLimit = . );
    PROVIDE( __StackBottom = . );
    . += __TOT_STACK_SIZE;
    . = ALIGN(16);
    PROVIDE( __StackTop = . );
    PROVIDE( _sp = . );
  } >RAM


  /* NVDS disabled - app uses LittleFS for storage */
  /* Provide stub symbols for compatibility */
  PROVIDE( __nvds_start = 0 );
  PROVIDE( __nvds_end = 0 );
  PROVIDE( __nvds_size = 0 );

  . = ALIGN(16);
  PROVIDE( __heap_start = . );
  PROVIDE( __heap_size = ORIGIN(RAM) + LENGTH(RAM) - __heap_start );
  PROVIDE( __heap_end = __heap_start + __heap_size );

  /* Non-cacheable RAM information */
  PROVIDE( __nocache_ram_addr = __nocache_ram_start );
  PROVIDE( __nocache_ram_size = __nocache_total_size );

  PROVIDE( __heap_psram_start = _bss_psram_end );
  PROVIDE( __heap_psram_size = ORIGIN(psram) + LENGTH(psram) - __heap_psram_start );
  PROVIDE( __heap_psram_end = __heap_psram_start + __heap_psram_size );

  /* cli */
  PROVIDE( _ld_static_cli_cmds_start = _static_cli_cmds_start );
  PROVIDE( _ld_static_cli_cmds_end   = _static_cli_cmds_end );

  /* cam desc */
  PROVIDE( cam_desc_start = _cam_desc_start );
  PROVIDE( cam_desc_end   = _cam_desc_end );

  /* img param */
  PROVIDE(_ld_img_param_desc_start = _img_param_desc_start);
  PROVIDE(_ld_img_param_desc_end   = _img_param_desc_end);


  /* ram information */
  PROVIDE( _ld_ram_size0 = LENGTH(flash) );
  PROVIDE( _ld_ram_addr0 = ORIGIN(flash) );
  PROVIDE( _ld_ram_size1 = LENGTH(ocram) );
  PROVIDE( _ld_ram_addr1 = ORIGIN(ocram) );
  PROVIDE( _ld_ram_size2 = LENGTH(hbnram) );
  PROVIDE( _ld_ram_addr2 = ORIGIN(hbnram) );
  PROVIDE( _ld_ram_size3 = SIZEOF(.stack) );
  PROVIDE( _ld_ram_addr3 = ADDR(.stack) );

  /* ocram_noret information */
  PROVIDE( _ld_ocram_noret_addr = ORIGIN(ocram_noret) );
  PROVIDE( _ld_ocram_noret_size = LENGTH(ocram_noret) );
  PROVIDE( _ld_ocram_noret_used = __ocram_noret_size );


  /* NVDS information */
  PROVIDE( _ld_nvds_addr = __nvds_start );
  PROVIDE( _ld_nvds_size = __nvds_size );


  /* EM size */
  PROVIDE(__LD_CONFIG_EM_SEL = __EM_SIZE);
}
