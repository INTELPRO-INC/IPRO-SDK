OUTPUT_ARCH( "riscv" )

/* ipro6_fmac_fw: the I6DL stub's stub.ld (generated ipro6_evb_ram.ld).
 * OCRAM 0x11000000: code, rodata, data, MSP stack, then heap region 0;
 * load address = run address, the loader writes it there and jumps to
 * _start. WRAM: MAC shared RAM, .bss, heap region 1; the top 4 KB of
 * WRAM is the loader handoff. */
ENTRY( _start )

/*
 * RAM Boot Mode (FPGA debug):
 * When DATA_LMA_REGION is empty, LMA = VMA for data sections so OpenOCD
 * can load them directly to RAM without startup code copying.
 * Flash boot: DATA_LMA_REGION = "AT > flash"
 * RAM boot:   DATA_LMA_REGION = "" (empty)
 */

__RFTLV_SIZE_OFFSET = 1K;
__RFTLV_SIZE_HOLE = 2K;
__RFTLV_HEAD1_H = (0x46525049); /* IPRF */
__RFTLV_HEAD1_L = (0x41524150); /* PARA */

MEMORY
{
  flash       (rxai!w) : ORIGIN = 0x11000000, LENGTH = 320K
  ram_wifi    (wxa)    : ORIGIN = 0x21050000, LENGTH = 156K
  hbnram      (wxa)    : ORIGIN = 0x20090000, LENGTH = 4K
}
REGION_ALIAS("ram_tcm", flash)
REGION_ALIAS("RAM", flash)


SECTIONS
{
  __stack_size = DEFINED(__stack_size) ? __stack_size : 2K;
  BOOT2_PT_ADDR = 0x63027c00;

  /* ========================================
   * FLASH Sections (XIP - Execute In Place)
   * ======================================== */

  /* IPRO7 startup expects .text.vtable and .text.init sections */
  .init           :
  {
    *(.text.vtable)
    *(.text.entry)
    KEEP (*(SORT_NONE(.text.init)))
    KEEP (*(SORT_NONE(.init)))
  } > flash

  /* value for rftlv */
  .rftlv.tool :
  {
      . = ORIGIN(flash) + __RFTLV_SIZE_OFFSET;
      PROVIDE( _ld_symbol_rftlv_address = . );
      LONG(__RFTLV_HEAD1_H);
      LONG(__RFTLV_HEAD1_L);
      . = ORIGIN(flash) + __RFTLV_SIZE_OFFSET + __RFTLV_SIZE_HOLE;
  } > flash

  .text           :
  {
    PROVIDE( _text_lma = LOADADDR(.text) );
    PROVIDE( _text = ADDR(.text) );

    *(.text.unlikely .text.unlikely.*)
    *(.text.startup .text.startup.*)
    *(.text .text.*)
    *(.gnu.linkonce.t.*)

    . = ALIGN(4);
    PROVIDE( _etext = . );
  } > flash

  .rodata         :
  {
    *(.rdata)
    *(.rodata .rodata.*)

    . = ALIGN(4);
    _version_info_section_start = .;
    KEEP(*(.version.*))
    _version_info_section_end = .;

    *(.sdata2.*)

    /* camera driver */
    . = ALIGN(4);
    _cam_desc_start = .;
    KEEP(*(.camera_desc))
    *(.camera_desc)
    _cam_desc_end = .;

    /* static cli cmds */
    . = ALIGN(4);
    _static_cli_cmds_start = .;
    KEEP(*(.static_cli_cmds))
    *(.static_cli_cmds)
    _static_cli_cmds_end = .;

    /* RT-Thread FinSH shell symbol tables */
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

    /* CI cmds */
    . = ALIGN(4);
    KEEP(*(.ctest))

    /* Symbol static fw debug attribute entry */
    . = ALIGN(4);
    _static_fw_dbg_entry_start = .;
    KEEP(SORT(*)(.static_fw_dbg_entry*))
    *(.static_fw_dbg_entry)
    _static_fw_dbg_entry_end = .;
    /* Symbol static fw debug count attribute entry */
    . = ALIGN(4);
    _static_fw_dbg_count_start = .;
    KEEP(SORT(*)(.static_fw_dbg_count*))
    *(.static_fw_dbg_count)
    _static_fw_dbg_count_end = .;

    /* static fw attribute entry */
    . = ALIGN(4);
    _static_fw_cfg_entry_start = .;
    KEEP(*(.wifi.cfg.entry))
    _static_fw_cfg_entry_end = .;

    /* static blog code1 */
    . = ALIGN(4);
    _static_blogcomponent_code_start = .;
    KEEP(SORT(*)(.static_blogcomponent_code*))
    *(.static_blogcomponent_code*)
    _static_blogcomponent_code_end = .;

    /* static blog code2 */
    . = ALIGN(4);
    _static_blogfile_code_start = .;
    KEEP(SORT(*)(.static_blogfile_code*))
    *(.static_blogfile_code*)
    _static_blogfile_code_end = .;

    /* static blog code3 */
    . = ALIGN(4);
    _static_blogpri_code_start = .;
    KEEP(SORT(*)(.static_blogpri_code*))
    *(.static_blogpri_code*)
    _static_blogpri_code_end = .;

    *(.gnu.linkonce.r.*)

    /*Framework table section, use ALIGN here to avoid fill section*/
    . = ALIGN(4);
    _rom_framework_audio_device_start = .;
    KEEP(*(.framework.audio_device))
    _rom_framework_audio_device_end = .;

    /*Jpeg Decode Data*/
    . = ALIGN(8);
    *(.decode_jpeg_data)

  } > flash

  .preinit_array :
  {
    . = ALIGN(4);
    __preinit_array_start = .;
    KEEP (*(.preinit_array))
    __preinit_array_end = .;
  } > flash

  .init_array :
  {
    . = ALIGN(4);
    __init_array_start = .;
    KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*)))
    KEEP (*(.init_array))
    __init_array_end = .;
  } > flash

  /* FPGA RAM as Heap disabled */

  /* ========================================
   * RAM Sections (Load from Flash to RAM)
   * All AT>flash sections must be contiguous in LMA
   * ======================================== */

  /* HBN RAM section - for low power code */
  .hbn : ALIGN(4)
  {
    PROVIDE( _hbn_load = LOADADDR(.hbn) );
    PROVIDE( _hbn_run = ADDR(.hbn) );
    PROVIDE( _hbn_run_end = ADDR(.hbn) + SIZEOF(.hbn));
    PROVIDE( __hbn_code_start = . );
    PROVIDE( __hbn_code_load_addr = LOADADDR(.hbn) );

    /* KEEP: the LP wakeup entry is reached only via a runtime address
     * (the PDS wake vector), never a symbol reference from retained code,
     * so --gc-sections would otherwise discard it and leave .hbn empty. */
    KEEP(*(.hbn_code))
    KEEP(*(.hbn_code.*))
    KEEP(*(.hbn_data))
    KEEP(*(.hbn_data.*))

    . = ALIGN(4);
    PROVIDE( __hbn_code_end = . );
  } >hbnram AT > flash

  /* RAM code section (if any code needs to run from RAM) */
  .text.ram  :
  {
    PROVIDE( _text_load = LOADADDR(.text.ram) );
    PROVIDE( _text_run = ADDR(.text.ram) );
    PROVIDE( _text_run_end = ADDR(.text.ram) + SIZEOF(.text.ram));
  } > ram_tcm 

  /* Data section - all initialized data in ram_tcm as a single output section.
   *
   * CRITICAL: All sub-sections that need flash-to-RAM copy MUST be inside this
   * single output section. Using separate output sections with ALIGN() creates
   * different VMA and LMA gaps (GNU LD advances VMA and LMA region counters
   * independently), which breaks the startup code's linear memcpy from
   * _data_lma to _data through _edata.
   *
   * Within a single output section, ". = ALIGN(N)" advances both VMA and LMA
   * by the same amount, keeping the copy correct.
   */
  .data : ALIGN(8)
  {
    PROVIDE( _data_load = LOADADDR(.data) );
    PROVIDE( _data_run = ADDR(.data) );
    PROVIDE( _data_lma = LOADADDR(.data) );
    PROVIDE( _data = ADDR(.data) );


    *(.data .data.*)
    *(.gnu.linkonce.d.*)

    *(.sdata .sdata.*)
    *(.gnu.linkonce.s.*)

    . = ALIGN(8);
    *(.srodata.cst16)
    *(.srodata.cst8)
    *(.srodata.cst4)
    *(.srodata.cst2)
    *(.srodata .srodata.*)

    . = ALIGN(8);
    *(._k_queue.static.*)
    *(._k_sem.static.*)
    *(._k_mutex.static.*)
    _net_buf_pool_list_start = .;
    /* The legacy dual-mode host uses this historical name as the base of the
     * contiguous static pool table.  Keep it an alias of the canonical list
     * start so BLE-only users retain the existing section layout. */
    PROVIDE(_ld_net_buf_pool_list = .);
    KEEP(*(SORT_BY_NAME("._net_buf_pool.static.*")))
    _net_buf_pool_list_end = .;
    _bt_conn_cb_list_start = .;
    KEEP(*(SORT_BY_NAME("._bt_conn_cb.static.*")))
    _bt_conn_cb_list_end = .;
    _bt_gatt_service_static_list_start = .;
    KEEP(*(SORT_BY_NAME("._bt_gatt_service_static.static.*")))
    _bt_gatt_service_static_list_end = .;
    _bt_l2cap_fixed_chan_list_start = .;
    KEEP(*(SORT_BY_NAME("._bt_l2cap_fixed_chan.static.*")))
    _bt_l2cap_fixed_chan_list_end = .;
    _bt_l2cap_br_fixed_chan_list_start = .;
    KEEP(*(SORT_BY_NAME("._bt_l2cap_br_fixed_chan.static.*")))
    _bt_l2cap_br_fixed_chan_list_end = .;
    _settings_handler_static_list_start = .;
    KEEP(*(SORT_BY_NAME("._settings_handler_static.static.*")))
    _settings_handler_static_list_end = .;
    _bt_sco_conn_cb_list_start = .;
    KEEP(*(SORT_BY_NAME("._bt_sco_conn_cb.static.*")))
    _bt_sco_conn_cb_list_end = .;
    _bt_sco_hci_cb_list_start = .;
    KEEP(*(SORT_BY_NAME("._bt_sco_hci_cb.static.*")))
    _bt_sco_hci_cb_list_end = .;

    /* For coredump */
    _coredump_binary_id_start = . - ADDR(.data) + LOADADDR(.data);
    KEEP(*(.coredump_binary_id))

    . = ALIGN(4);
    *(.tcm_code .tcm_code.*)
    *(.tcm_const.*)
    /* ATTR_DTCM_SECTION (.tcm_data) — initialized data that lives in DTCM/OCRAM
     * but must be copied from its flash LMA at startup. Without these globs the
     * section becomes an orphan whose initializer is never copied, so structs
     * like pdsCfgLevel31 read back all-zeros at runtime (PDS never starts). */
    *(.tcm_data)
    *(.tcm_data.*)
    *(.sclock_rlt_code.*)
    *(.sclock_rlt_const.*)

    /* OCRAM data - HAL drivers (ATTR_OCRAM_SECTION) and FreeRTOS port vars */
    . = ALIGN(4);
    *(.ocram_data)
    *(.ocram_data.*)

    /* Vector table - ECLIC requires 512-byte alignment.
     * ALIGN inside the section advances both VMA and LMA equally. */
    . = ALIGN(512);
    PROVIDE( __vectable_start = . );
    *(.vectable)
    . = ALIGN(4);
    PROVIDE( __vectable_end = . );

    /* PSRAM data (routed to OCRAM when PSRAM disabled) */
    *(.psram_data)
    *(.psram_data.*)

    /* Global pointer area */
    . = ALIGN(4);
    PROVIDE( __global_pointer_head$ =  . );
    PROVIDE( __global_pointer$ = . + 0x7F0 );
    . = . + 0x498;

    . = ALIGN(8);
    PROVIDE( _data_run_end = . );
    PROVIDE( _edata = . );
    PROVIDE( edata = . );
  } > ram_tcm 

  /* ========================================
   * NOLOAD Sections (BSS, Stack, etc.)
   * ======================================== */

  /* Boot2 section in ram_tcm */
  .boot2 (NOLOAD) :
  {
      PROVIDE ( __boot2_pt_addr_start = . );
      *(.bss.boot2_partition_table)
      PROVIDE ( __boot2_pt_addr_end   = . );

      PROVIDE ( __boot2_flashCfg_start = . );
      *(.bss.boot2_flashCfg)
      PROVIDE ( __boot2_flashCfg_end = . );
  } > ram_tcm


  /* Record end of cacheable region */
  __ocram_cacheable_end = .;

  /* DMA buffers sit in WRAM .bss (0x2105xxxx, uncached) */
  __stack_start_addr = __ocram_cacheable_end;

  .stack __stack_start_addr (NOLOAD) :
  {
    PROVIDE ( _sp_base = . );
    PROVIDE ( __StackLimit = . );
    PROVIDE ( __StackBottom = . );
    . = ALIGN(16);
    . = . + __stack_size;
    PROVIDE( _sp_main = . );
    PROVIDE( _sp = . );
    PROVIDE( __StackTop = . );
    __freertos_irq_stack_top = .;
  } > ram_tcm


  /* Buffers that only the CPU fills (macsw RX pool), not zeroed */
  .fmac_ocram (NOLOAD) : ALIGN(32)
  {
    *(.fmac_ocram)
    *(.fmac_ocram.*)
  } > ram_tcm

  /* NVDS section disabled - CONFIG_ENABLE_NVDS not set */
  PROVIDE( __nvds_start = 0 );
  PROVIDE( __nvds_end = 0 );
  PROVIDE( __nvds_size = 0 );


  /* Heap in ram_tcm - after stack */
  PROVIDE( _heap_start = . );
  PROVIDE( __heap_start = . );
  PROVIDE( _heap_size = ORIGIN(ram_tcm) + LENGTH(ram_tcm) - _heap_start );
  PROVIDE( __heap_size = _heap_size );
  PROVIDE( __heap_end = _heap_start + _heap_size );

  /* WiFi BSS section in ram_wifi */
  .wifibss (NOLOAD) :
  {
    PROVIDE( __wifi_bss_start = ADDR(.wifibss) );
    _sshram = . ;
    /* ★ MAC RX buffer 1 goes FIRST, 16 KB-aligned, and that is a correctness
     * requirement rather than tidiness. The MAC truncates a descriptor write at a 32 KB
     * boundary: the fields below the boundary land, the first word at or after it is
     * never written and keeps whatever the ring already held, and the RX SW then follows
     * that word as a payload pointer. Measured 2026-08-10 (RC_P27_0810.md P28/P29: the
     * buffer moved 1024 B and the fault did not move; the buffer moved off the 32 KB
     * boundary and the fault rate went to zero over 40 boots).
     *
     * A 16 KB buffer contains an INTERIOR 32 KB boundary only when it is not 16 KB
     * aligned, so this placement removes the fault by construction. Done by placement and
     * not by aligned(16384) on the array because every one of these regions has a 16 KB
     * aligned ORIGIN: first here costs ZERO, whereas the attribute pads up to 16 KB and
     * overflows defconfig_ipro6_sdio (measured 166,784 of 163,840).
     *
     * Keep it first. Anything placed ahead of it makes this ALIGN pad for real, and the
     * tight config has ~13 KB of ram_wifi headroom, so the link fails rather than
     * silently shrinking the budget. Its own input section because rxl_hwdesc.c's
     * SHAREDRAM is one chunk with RX buffer 2 emitted ahead of buffer 1. */
    . = ALIGN(16384);
    *(SHAREDRAM_RXBUF)
    /* macif TX slots of the softAP image (CONFIG_IPRO6_FMAC_AP), 2 KB
     * aligned; the STA image has no such input section */
    *(.macif_tx_slots)
    *(SHAREDRAMIPC)
    *(SHAREDRAM)
    /* em_sel span: all 160 KB of WRAM incl. the handoff, no EM */
    _eshram = ABSOLUTE(0x21078000);
    *ipc_shared.o(COMMON)
    *sdu_shared.o(COMMON)
    *hal_desc.o(COMMON)
    *txl_buffer_shared.o(COMMON)
    *txl_frame_shared.o(COMMON)
    *scan_shared.o(COMMON)
    *scanu_shared.o(COMMON)
    *mfp_bip.o(COMMON)
    *me_mic.o(COMMON)
    *(.lwip_mem*)
    . = ALIGN(16);
    PROVIDE( __wifi_bss_end = . );
  } > ram_wifi

  /* BSS in WRAM after the MAC shared RAM, below the handoff block */
  PROVIDE( _fbss = . );
  PROVIDE( __bss_start = . );

  .bss (NOLOAD) : ALIGN(8)
  {
    *(.sbss*)
    *(.gnu.linkonce.sb.*)
    *(.bss .bss.*)
    /* BR BLE builds opt into per-pool sections. MLD membership records are
     * CPU metadata; moving just this pool leaves WRAM room for 16 KiB EM
     * without relocating packet storage or reducing listener capacity. */
    *(.lwip_mem.memp_memory_MLD6_GROUP_base)
    /* FreeRTOS PRIVILEGED_DATA et al. land here (mpu_wrappers.h); IPRO6 has
     * no retained-OCRAM split, so fold them into regular zero-init BSS to
     * keep them out of orphan PROGBITS placement after .data. */
    *(.ocram_bss)
    *(.ocram_bss.*)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    . = ALIGN(32);
    __nocache_ram_start = .;
    *(.nocache_ram)
    *(.nocache_ram.*)
    *(.noncacheable)
    *(.noncacheable.*)
    *(.nocache_noinit_ram)
    *(.nocache_noinit_ram.*)
    . = ALIGN(8);
    __nocache_ram_end = .;
  } > ram_wifi
  __nocache_total_size = __nocache_ram_end - __nocache_ram_start;

  PROVIDE( __bss_end = . );
  PROVIDE( _end = . );
  PROVIDE( end = . );

  PROVIDE( _heap_wifi_start = . );
  PROVIDE( _heap_wifi_size = ORIGIN(ram_wifi) + LENGTH(ram_wifi) - _heap_wifi_start );

  /* ========================================
   * Symbol Definitions
   * ======================================== */

  /*SYMOBOL used in code*/
  PROVIDE( cam_desc_start = _cam_desc_start );
  PROVIDE( cam_desc_end   = _cam_desc_end );
  PROVIDE( _ld_static_cli_cmds_start = _static_cli_cmds_start );
  PROVIDE( _ld_static_cli_cmds_end   = _static_cli_cmds_end );

  /* Symbol used in code */
  PROVIDE( _ld_static_fw_dbg_entry_start = _static_fw_dbg_entry_start );
  PROVIDE( _ld_static_fw_dbg_entry_end   = _static_fw_dbg_entry_end );

  /* Symbol used in code */
  PROVIDE( _ld_static_fw_dbg_count_start = _static_fw_dbg_count_start );
  PROVIDE( _ld_static_fw_dbg_count_end   = _static_fw_dbg_count_end );

  /*BOOT2 sections*/
  PROVIDE ( __boot2_pt_addr_src = BOOT2_PT_ADDR );

  /*CFG FW used in code*/
  PROVIDE( _ld_static_cfg_entry_start = _static_fw_cfg_entry_start );
  PROVIDE( _ld_static_cfg_entry_end   = _static_fw_cfg_entry_end );

  /* blog */
  PROVIDE( _ld_static_blogcomponent_code_start = _static_blogcomponent_code_start );
  PROVIDE( _ld_static_blogcomponent_code_end   = _static_blogcomponent_code_end );
  PROVIDE( _ld_static_blogfile_code_start = _static_blogfile_code_start );
  PROVIDE( _ld_static_blogfile_code_end   = _static_blogfile_code_end );
  PROVIDE( _ld_static_blogpri_code_start = _static_blogpri_code_start );
  PROVIDE( _ld_static_blogpri_code_end   = _static_blogpri_code_end );

  /* RAM information */
  PROVIDE( _ld_ram_size0 = LENGTH(flash) );
  PROVIDE( _ld_ram_addr0 = ORIGIN(flash) );
  PROVIDE( _ld_ram_size1 = LENGTH(ram_tcm) );
  PROVIDE( _ld_ram_addr1 = ORIGIN(ram_tcm) );
  PROVIDE( _ld_ram_size2 = LENGTH(ram_wifi) );
  PROVIDE( _ld_ram_addr2 = ORIGIN(ram_wifi) );

  PROVIDE(__LD_CONFIG_EM_SEL = 0);


  /* NVDS information */
  PROVIDE( _ld_nvds_addr = __nvds_start );
  PROVIDE( _ld_nvds_size = __nvds_size );


  /* ========================================
   * IPRO7 Startup Code Compatibility Symbols
   * ======================================== */

  /* Thread Local Storage - not used, provide dummy */
  PROVIDE( __tls_base = 0 );

  /* IPRO6 does not have PSRAM - symbols removed */
  /* psram_init.c is conditionally compiled only when CONFIG_USE_PSRAM=y */

  /* Non-cacheable RAM information */
  PROVIDE( __nocache_ram_addr = __nocache_ram_start );
  PROVIDE( __nocache_ram_size = __nocache_total_size );

}
