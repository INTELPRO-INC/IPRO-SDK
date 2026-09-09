OUTPUT_ARCH( "riscv" )

/* ipro6_if_wifi dedicated linker script (2026-07-24).
 *
 * Forked from the generated ipro6_evb_flash.ld (bsp/board/linker/ipro6/
 * ipro6_common.ld.in) via CONFIG_LINKER_SCRIPT so WiFi-bridge memory tuning
 * (SDIO nocache pools, ram_wifi TX/RX buffer layout) no longer shares a
 * template with ipro6_demo.
 *
 * Differences from the generated script (ipro7-style defensive layout):
 *   - __min_heap_size + link-time ASSERTs: the auto layout places heap last
 *     (cacheable | nocache | stack | heap-gets-the-rest), so any .noncacheable
 *     growth silently shrinks the heap. On 2026-07-23 a 37.6KB nocache addition
 *     starved the heap below what stack_wifi needs -> boot-time wedge with no
 *     diagnostic. Now the link FAILS instead.
 *   - heap size guarded against underflow (MAX-style clamp, see ipro7
 *     ipro7_common.ld.in heap computation notes). */
ENTRY( _start )

/*
 * RAM Boot Mode (FPGA debug):
 * When DATA_LMA_REGION is empty, LMA = VMA for data sections so OpenOCD
 * can load them directly to RAM without startup code copying.
 * Flash boot: DATA_LMA_REGION = "AT > flash"
 * RAM boot:   DATA_LMA_REGION = "" (empty)
 */

/* Reserve the EM carve when either BT controller stack is linked:
 * btblecontroller uses btble_controller_init, the btdm stack uses
 * btdm_controller_init. board_wram_em_sel_init() derives em_sel from the
 * resulting ram_wifi span, so this must match the linked stack. */
__EM_SIZE = (DEFINED(btble_controller_init) || DEFINED(btdm_controller_init)) ? 32K : 0K;
__RFTLV_SIZE_OFFSET = 1K;
__RFTLV_SIZE_HOLE = 2K;
__RFTLV_HEAD1_H = (0x46525049); /* IPRF */
__RFTLV_HEAD1_L = (0x41524150); /* PARA */

MEMORY
{
  flash       (rxai!w) : ORIGIN = 0x13000000, LENGTH = 2M
  ram_tcm     (wxa)    : ORIGIN = 0x11000000, LENGTH = 320K
  ram_wifi    (wxa)    : ORIGIN = 0x21050000, LENGTH = 160K - __EM_SIZE
  hbnram      (wxa)    : ORIGIN = 0x20090000, LENGTH = 4K
}
REGION_ALIAS("RAM", ram_tcm)


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
    /* CLI-only A/B controls/census; keep them in XIP instead of spending
     * scarce cached OCRAM with the per-frame txl_agg hot path below. */
    *libmacsw.a:txl_agg.c.o(.text.txl_agg_pool_set
                            .text.txl_agg_pool_phys
                            .text.txl_agg_hold_armed_get
                            .text.txl_agg_hold_skip_count_get)
    /* Timeout-only recovery is not part of the healthy TX datapath. Keep it
     * in XIP so completion ownership checks stay in cached OCRAM. */
    *libmacsw.a:txl_cntrl.c.o(.text.txl_cntrl_recover_stale)
    /* Descriptor placement runs once per fhost packet and is not part of the
     * per-MPDU MAC hot path. Keep it in XIP to preserve the TCM heap floor. */
    *libmacsw.a:export_tx.c.o(.text.tx_desc_align_for_fhost)
    /* [PERF 07-24] hot per-frame WiFi/RTOS objects run from cached OCRAM
     * (relocated into .data below) instead of 80MHz XIP flash — keep this
     * EXCLUDE list in sync with the glob list inside .data. */
    *(EXCLUDE_FILE(*fhost_tx.c.o *txl_cntrl.c.o *txu_cntrl.c.o
                   *txl_cfm.c.o *macif_fhost.c.o *ke_event.c.o
                   *trcver_sdio_ipro6.c.o *net_wifi_transceiver.c.o
                   *net_al.c.o *libfreertos.a:queue.c.o *liblwip.a:pbuf.c.o
                   *txl_agg.c.o *libmacsw.a:rc.c.o *export_tx.c.o
                   *tx_swdesc.c.o
                   *libmacsw.a:co_list.c.o *rxu_cntrl.c.o
                   *rxl_cntrl.c.o *fhost_rx.c.o *rxl_hwdesc.c.o
                   *libmacsw.a:me_utils.c.o *libmacsw.a:ke_task.c.o) .text)
    *(EXCLUDE_FILE(*fhost_tx.c.o *txl_cntrl.c.o *txu_cntrl.c.o
                   *txl_cfm.c.o *macif_fhost.c.o *ke_event.c.o
                   *trcver_sdio_ipro6.c.o *net_wifi_transceiver.c.o
                   *net_al.c.o *libfreertos.a:queue.c.o *liblwip.a:pbuf.c.o
                   *txl_agg.c.o *libmacsw.a:rc.c.o *export_tx.c.o
                   *tx_swdesc.c.o
                   *libmacsw.a:co_list.c.o *rxu_cntrl.c.o
                   *rxl_cntrl.c.o *fhost_rx.c.o *rxl_hwdesc.c.o
                   *libmacsw.a:me_utils.c.o *libmacsw.a:ke_task.c.o) .text.*)
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
  } > ram_tcm AT > flash

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

    /* [PERF 07-24] per-frame hot code relocated from XIP flash to cached
     * OCRAM. Placed INSIDE .data (not .text.ram) because only the .data
     * output section gets the startup LMA->VMA copy (see CRITICAL comment
     * above; startup_ipro6.S has no loop for _text_load/_text_run).
     * Round-1 set = 6 objects ~29.2KB (txl_agg/queue.c/export_tx stay in
     * flash until round 2 funding). Funding: SMID dnld pool 10->9 (+6.1K),
     * ab_trace 2048->512 (+12K), upld cbuf 12->10 (+4K), heap slack; the
     * heap-floor ASSERT below still guards the balance. */
    . = ALIGN(8);
    *libipro_fhost.a:fhost_tx.c.o(.text .text.*)
    *libmacsw.a:txl_cntrl.c.o(.text .text.*)
    *libmacsw.a:txu_cntrl.c.o(.text .text.*)
    *libmacsw.a:txl_cfm.c.o(.text .text.*)
    *libmacsw.a:macif_fhost.c.o(.text .text.*)
    *libmacsw.a:ke_event.c.o(.text .text.*)
    /* Round-2 (07-24): worker inject front-half + queue hop + pbuf ops.
     * Funding: Tmr Svc stack 16K->4K, idle 4K->2K, main_loop 4K->2K,
     * ab_trace 256->128 (ps high-water evidence in main.c comments). */
    *libsdiowifi.a:trcver_sdio_ipro6.c.o(.text .text.*)
    *libsdiowifi.a:net_wifi_transceiver.c.o(.text .text.*)
    *libwifi6_lwip_adapter.a:net_al.c.o(.text .text.*)
    *libfreertos.a:queue.c.o(.text .text.*)
    *liblwip.a:pbuf.c.o(.text .text.*)
    /* Round-3/4 (07-24): A-MPDU build/close + rc (l1 54us bucket) + swdesc +
     * desc-init. bam.c swapped back to flash to fund rc.c (umac bucket small).
     * Funding: zcprobe staging retired (15.5K nocache reclaimed). */
    *libmacsw.a:txl_agg.c.o(.text .text.*)
    *libmacsw.a:rc.c.o(.text .text.*)
    *libmacsw.a:export_tx.c.o(.text .text.*)
    *libmacsw.a:tx_swdesc.c.o(.text .text.*)
    *libmacsw.a:co_list.c.o(.text .text.*)
    /* P4 round-1 (07-24): DL umac per-frame core, FUNCTION-level globs
     * (-ffunction-sections is global, cmake/compiler_flags.cmake). Only
     * ~2.5KB funded: heap cushion 1856B + rxprof ring 32->8 (672B).
     * rxu_cntrl.c.o is EXCLUDE_FILE'd from the flash .text globs above so
     * these sections first-match HERE; the cold remainder of the object is
     * caught by .xip_text_cold (flash) after this section. */
    *libmacsw.a:rxu_cntrl.c.o(.text.rxu_cntrl_evt
                              .text.rxu_cntrl_frame_handle
                              .text.rxu_cntrl_reord_modulo*)
    /* P4 round-2 (07-24): rest of the per-frame DL chain. Funding:
     * MBEDTLS_AES_ROM_TABLES (8KB .bss -> flash .rodata). rxl_hwdesc.c.o
     * is whole-object (all per-frame); rxl_cntrl/fhost_rx/rxu reord are
     * function-level with cold catch in .xip_text_cold. */
    *libmacsw.a:rxu_cntrl.c.o(.text.rxu_cntrl_reord_fwd
                              .text.rxu_cntrl_reord_flush
                              .text.rxu_msdu_upload_and_indicate)
    *libmacsw.a:rxl_cntrl.c.o(.text.rxl_cntrl_evt
                              .text.rxl_dma_evt
                              .text.rxl_dma_int_handler
                              .text.rxl_internal_dma_int_handler)
    *libipro_fhost.a:fhost_rx.c.o(.text.fhost_rx_task
                                  .text.fhost_rx_buf_push
                                  .text.fhost_rx_buf_send*
                                  .text.fhost_rx_buf_lease
                                  .text.fhost_rx_buf_dispatch)
    *libmacsw.a:rxl_hwdesc.c.o(.text .text.*)
    /* [CFM2 07-24] per-MPDU flash calls in the cfm path: me_tx_cfm_amsdu is
     * called unconditionally per MPDU from txu_cntrl_cfm, ke_state_get per
     * BA-QoS MPDU from bam_tx_cfm. ~170B; cold rest in .xip_text_cold. */
    *libmacsw.a:me_utils.c.o(.text.me_tx_cfm_amsdu*)
    *libmacsw.a:ke_task.c.o(.text.ke_state_get*)
    . = ALIGN(8);

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
    _bt_gatt_service_static_list_start = .;
    KEEP(*(SORT_BY_NAME("._bt_gatt_service_static.static.*")))
    _bt_gatt_service_static_list_end = .;
    _bt_l2cap_fixed_chan_list_start = .;
    KEEP(*(SORT_BY_NAME("._bt_l2cap_fixed_chan.static.*")))
    _bt_l2cap_fixed_chan_list_end = .;
    _bt_l2cap_br_fixed_chan_list_start = .;
    KEEP(*(SORT_BY_NAME("._bt_l2cap_br_fixed_chan.static.*")))
    _bt_l2cap_br_fixed_chan_list_end = .;

    /* For BFLB Coredump */
    _coredump_binary_id_start = . - ADDR(.data) + LOADADDR(.data);
    KEEP(*(.coredump_binary_id))

    . = ALIGN(4);
    *(.tcm_code.*)
    *(.tcm_const.*)
    /* ATTR_DTCM_SECTION (.tcm_data) — initialized data that lives in DTCM/OCRAM
     * but must be copied from its flash LMA at startup. Without these globs the
     * section becomes an orphan whose initializer is never copied, so structs
     * like pdsCfgLevel31 read back all-zeros at runtime (PDS never starts). */
    *(.tcm_data)
    *(.tcm_data.*)
    *(.sclock_rlt_code.*)
    *(.sclock_rlt_const.*)

    *libbl6_freertos.a:*.o(.text*)
    *libbl6_freertos.a:*.o(.rodata*)

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
  } > ram_tcm AT > flash

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

  /* BSS section in ram_tcm */
  PROVIDE( _fbss = . );
  PROVIDE( __bss_start = . );

  .bss (NOLOAD) : ALIGN(8)
  {
    *(.sbss*)
    *(.gnu.linkonce.sb.*)
    *(.bss .bss.*)
    /* FreeRTOS PRIVILEGED_DATA et al. land here (mpu_wrappers.h); IPRO6 has
     * no retained-OCRAM split, so fold them into regular zero-init BSS to
     * keep them out of orphan PROGBITS placement after .data. */
    *(.ocram_bss)
    *(.ocram_bss.*)
    *(.gnu.linkonce.b.*)
    *(COMMON)
    . = ALIGN(8);
  } > ram_tcm

  PROVIDE( __bss_end = . );
  PROVIDE( _end = . );
  PROVIDE( end = . );

  /* Non-cacheable address offset: 0x21xxxxxx - 0x11xxxxxx = 0x10000000 */
  __NOCACHE_ADDR_OFFSET = 0x10000000;

  /* Record end of cacheable region for nocache section placement */
  __ocram_cacheable_end = .;

  /* Non-cacheable RAM section (NOLOAD, initialized at runtime)
   * VMA = cacheable end + offset → maps to 0x21xxxxxx address space
   * Physical OCRAM is shared with ram_tcm, accessed via non-cacheable alias */
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

  /* Stack after nocache region in physical OCRAM */
  __stack_start_addr = __ocram_cacheable_end + __nocache_total_size;

  /* Layout self-checks (ipro7-style): everything below heap must fit with a
   * usable heap left over. __min_heap_size is the floor for the ram_tcm heap
   * that feeds FreeRTOS/newlib; bench-calibrated for this app on 2026-07-23:
   * ~65KB heap = stack_wifi wedges at boot, ~89KB = verified working under
   * full traffic. Override per-build with --defsym=__min_heap_size=N.
   *
   * [mem-32K 2026-07-29] 80K -> 97K. The campaign returned 15,872 B to this
   * heap (EVFS + the peripheral demo + the printf buffer + rm_env + the DNS
   * table + the cfm-latency map + the timer task stack), taking it from 84,320
   * to 102,320 B. Raising the floor with it is the whole point: an unguarded
   * win gets eaten back by ordinary static growth within months and nobody
   * notices until something wedges at boot. 97K leaves 2,992 B spendable, a
   * little MORE than the 2,400 B this floor left before, so the tripwire is no
   * tighter than the one it replaces. */
  __min_heap_size = DEFINED(__min_heap_size) ? __min_heap_size : 97K;
  ASSERT(__stack_start_addr + __stack_size <= ORIGIN(ram_tcm) + LENGTH(ram_tcm),
         "ipro6_if_wifi.ld: cacheable+nocache+stack overflow ram_tcm")

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

  /* NVDS section disabled - CONFIG_ENABLE_NVDS not set */

  /* Heap in ram_tcm - after stack. Underflow-guarded (ipro7 style): if the
   * heap start ever passes the region end the size clamps to 0 instead of
   * wrapping to a huge unsigned value, and the ASSERT below turns the
   * too-small heap into a link error instead of a silent boot-time wedge. */
  PROVIDE( _heap_start = . );
  PROVIDE( __heap_start = . );
  PROVIDE( _heap_size = (ORIGIN(ram_tcm) + LENGTH(ram_tcm) > _heap_start)
                        ? ORIGIN(ram_tcm) + LENGTH(ram_tcm) - _heap_start : 0 );
  PROVIDE( __heap_size = _heap_size );
  PROVIDE( __heap_end = _heap_start + _heap_size );
  ASSERT(_heap_size >= __min_heap_size,
         "ipro6_if_wifi.ld: ram_tcm heap below __min_heap_size (80K default) - nocache/.bss growth ate the heap; shrink pools or raise the floor consciously")

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
    /* txl_agg.c owns the block-packed A-THD/BAR arena. Place its SHAREDRAM
     * input immediately after the 16 KB RX buffer, where `.` is already 1 KB
     * aligned, so the arena's load-bearing aligned(1024) costs no padding.
     * The generic SHAREDRAM match below consumes every other object. */
    *libmacsw.a:txl_agg.c.o(SHAREDRAM)
    *(SHAREDRAMIPC)
    *(SHAREDRAM)
    /* end of wifi ram (ram_wifi LENGTH already excludes the __EM_SIZE carve) */
    _eshram = ORIGIN(ram_wifi) + LENGTH(ram_wifi);
    *ipc_shared.o(COMMON)
    *sdu_shared.o(COMMON)
    *hal_desc.o(COMMON)
    *txl_buffer_shared.o(COMMON)
    *txl_frame_shared.o(COMMON)
    *scan_shared.o(COMMON)
    *scanu_shared.o(COMMON)
    *mfp_bip.o(COMMON)
    *me_mic.o(COMMON)
    /* smid_zc_slot gets its own section so this bracket covers exactly that
     * array (.lwip_mem.sdiowifi is shared with sdiowifi_tx_buf.c and would
     * not). Bracketing is what makes the 4 KB guard below independent of where
     * the array lands, of whether anything is placed after it, and of whether
     * this build links an SDIO transceiver at all. Must precede the generic
     * .lwip_mem* glob — ld assigns each input section to the FIRST match. */
    __smid_zc_slot_start = .;
    *(.lwip_mem.smid_zc_slot)
    __smid_zc_slot_end = .;
    *(.lwip_mem*)
    . = ALIGN(16);
    PROVIDE( __wifi_bss_end = . );
  } > ram_wifi

  PROVIDE( _heap_wifi_start = . );
  PROVIDE( _heap_wifi_size = ORIGIN(ram_wifi) + LENGTH(ram_wifi) - _heap_wifi_start );

  /* Not a heap — the three `_heap_wifi` consumers are all compiled out of this
   * build (sdiowifi_mgmr.c:209 behind CONFIG_SDIO_HIGH_PERFORMANCE, hard-0 at
   * sdiowifi_tx_buf.h:8; :230 behind `#if 0`; tx_buffer_copy.c:88 behind
   * FHOST_BUFFER_COPY, defined nowhere). The name is historical; the floor is
   * a plain growth tripwire again.
   *
   * It was briefly a hard layout freeze (23872, one commit back) while the
   * 07-28/29 bench deaths were unexplained. ROOT CAUSE FOUND AND FIXED
   * 2026-07-29: the MAC TX DMA fails a descriptor fetch that straddles a 4 KB
   * page. Each smid_zc_slot carries its MAC descriptors in-line (THD +108,
   * policy +180, tx_pbd +244..264); with the array only 32-byte aligned, 5 of
   * the 64 reachable page phases put a boundary inside that block, and those
   * layouts died 2/4..36/36 while boundary-clear layouts ran clean — moving
   * the boundary 32 B off the tx_pbd flipped 36/36-dead into clean in the
   * same window. `smid_zc_slot` is now aligned(4096) (trcver_sdio_ipro6.c),
   * slot size divides the page, so no descriptor can ever straddle and the
   * tail is ordinary reclaimable space again.
   * Residual: ~4-5% of legs die with the same underrun signature on EVERY
   * layout, aligned or not — that is a separate, still-open issue.
   *
   * 12K floor as before 07-29: enough to catch a runaway static-array change
   * at link time while leaving ~8 KB of the current 20,480 B tail spendable.
   * Override with -Wl,--defsym=__min_wifi_heap_size=N.
   *
   * [mem-32K 2026-07-29] 12K -> 29K. FHOST_RX_BUF_CNT 26 -> 18 and the lwIP
   * memp trim returned 17,760 B here, taking the tail from 20,256 to 38,016 B.
   * 29K keeps 8,320 B spendable - the same ~8 KB the 12K floor was chosen to
   * leave - so this raises the tripwire without turning it back into the hard
   * layout freeze that was tried and backed out one commit ago.
   *
   * [P1 07-29] 29K -> 13K, because the campaign's slack was spent as intended.
   * SMID_ZC_SLOT_CNT 24 -> 32 buys uplink in-flight window with 16,384 B of this
   * tail, taking it 38,016 -> 21,632 B. The floor follows the spend exactly
   * (29,696 - 16,384 = 13,312 = 13K), so the spendable margin stays 8,320 B -
   * the same ~8 KB every previous floor was chosen to leave. Keeping 29K here
   * would fail the link; setting it to 21K would leave 128 B and recreate the
   * hard freeze noted above. The tripwire still catches a runaway static array;
   * it just no longer pretends the slot pool is untouchable. */
  __min_wifi_heap_size = DEFINED(__min_wifi_heap_size) ? __min_wifi_heap_size : 13K;
  ASSERT(_heap_wifi_size >= __min_wifi_heap_size,
         "ram_wifi heap/tail below __min_wifi_heap_size: a static ram_wifi section grew past the budget. Trim it or raise the floor deliberately.")

  /* The alignment IS the load-bearing guarantee now, so verify what the array
   * ACTUALLY got, using the bracket placed around its own input section above.
   *
   * Two earlier forms were wrong and are recorded so they are not retried:
   *   - `(__wifi_bss_end - 49152) & 4095` derived the base from the section end
   *     on the assumption that this array is the last object in .wifibss. That
   *     goes stale the moment anything is placed after it, and it FAILED
   *     OUTRIGHT in the USB build, which links no SDIO transceiver at all.
   *   - `!DEFINED(smid_zc_slot) || ...` does not work: ld evaluates both sides
   *     of || and still errors on the undefined symbol. Papering over that with
   *     PROVIDE(smid_zc_slot = 0) is WORSE than the bug — the guard then passes
   *     vacuously (0 & 4095 == 0) whenever the symbol goes missing, e.g. if the
   *     array is made static again. A guard that can silently stop guarding is
   *     not a guard.
   *
   * Empty bracket == this build has no such array (USB): nothing to check.
   * Non-empty: check the END, because a start marker is recorded before ld pads
   * to the input section's alignment. End-aligned implies base-aligned while the
   * array spans whole pages, which trcver_sdio_ipro6.c pins with a
   * _Static_assert on sizeof. Independent of ordering, of staticness, and of
   * the defconfig. */
  ASSERT(__smid_zc_slot_end == __smid_zc_slot_start ||
         (__smid_zc_slot_end & 4095) == 0,
         "smid_zc_slot lost its 4 KB alignment. A MAC TX descriptor straddling a 4 KB page halts the TX DMA (measured 36/36 dead legs). Restore aligned(4096) on the array in trcver_sdio_ipro6.c.")

  /* [PERF P4 07-24] cold remainder of function-level-relocated objects.
   * These objects are EXCLUDE_FILE'd from the flash .text globs so their
   * hot .text.<fn> sections can first-match inside .data (cached OCRAM);
   * every section NOT claimed there lands here, back in XIP flash.
   * Placement rules (both mandatory):
   *   1. AFTER .data in script order — ld assigns each input section to the
   *      FIRST matching output rule, and the hot-function globs live in .data.
   *   2. AFTER every '.'-dependent statement — a flash-VMA section here moves
   *      the location counter into flash space, and empty NOLOAD sections
   *      (.boot2) are discarded, so a PROVIDE(sym = .) between RAM sections
   *      would inherit the FLASH address (broke __bss_start on first try:
   *      startup zeroed flash, dead before UART). Everything below this point
   *      is pure symbol arithmetic, so the counter position no longer leaks. */
  .xip_text_cold :
  {
    *libmacsw.a:rxu_cntrl.c.o(.text .text.*)
    *libmacsw.a:rxl_cntrl.c.o(.text .text.*)
    *libipro_fhost.a:fhost_rx.c.o(.text .text.*)
    *libmacsw.a:me_utils.c.o(.text .text.*)
    *libmacsw.a:ke_task.c.o(.text .text.*)
    . = ALIGN(4);
  } > flash

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

  PROVIDE(__LD_CONFIG_EM_SEL = __EM_SIZE);



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
