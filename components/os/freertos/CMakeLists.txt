__generate_library(1)

################# Add private include (higher priority) #################
__component_add_private_include_dirs(Source/include)
__component_add_private_include_dirs(.)
################# Add global include for external components #################
__component_add_include_dirs(.)
__component_add_include_dirs(Source/include)
#######################################################

############## Add current dir source files ###########
__component_add_sources(Source/croutine.c)
__component_add_sources(Source/event_groups.c)
__component_add_sources(Source/list.c)
__component_add_sources(Source/queue.c)
__component_add_sources(Source/tasks.c)
__component_add_sources(Source/stream_buffer.c)
__component_add_sources(Source/timers.c)
if(CONFIG_IPRO7 OR CONFIG_IPRO6)
# IPRO6 uses same CPU as IPRO7, so use IPRO7's portable layer
__component_add_private_include_dirs(Source/portable_ipro7)
__component_add_include_dirs(Source/portable_ipro7)

__component_add_sources(Source/portable_ipro7/MemMang/heap_5.c)
__component_add_sources(Source/portable_ipro7/port.c)
__component_add_sources(Source/portable_ipro7/GCC/portasm.S)
# Pass HW stack protection flag to assembly (FreeRTOSConfig.h not included in .S files)
if(CONFIG_HW_STACK_PROTECTION)
    __component_add_private_compile_options(-DconfigHW_STACK_PROTECTION=1)
endif()
elseif(${CONFIG_IPRO6LE})
__component_add_private_include_dirs(Source/portable_ipro6le)
__component_add_private_include_dirs(Source/portable_ipro6le/GCC/RISC-V)
__component_add_private_include_dirs(Source/portable_ipro6le/GCC/RISC-V/chip_specific_extensions/RV32F_float_abi_single)
__component_add_include_dirs(Source/portable_ipro6le)
__component_add_include_dirs(Source/portable_ipro6le/GCC/RISC-V)
__component_add_include_dirs(Source/portable_ipro6le/GCC/RISC-V/chip_specific_extensions/RV32F_float_abi_single)
__component_add_private_compile_options(-DportasmHANDLE_INTERRUPT=interrupt_entry)
__component_add_sources(Source/portable_ipro6le/GCC/RISC-V/port.c)
__component_add_sources(Source/portable_ipro6le/GCC/RISC-V/portASM.S)
__component_add_sources(Source/portable_ipro6le/GCC/RISC-V/portStatTrap.c)
__component_add_sources(Source/portable_ipro6le/MemMang/heap_5.c)
endif()

################# Default FreeRTOS app hooks #################
__component_add_sources(freertos_app_hooks.c)

############ Add global compile option ################
#add components denpend on this component
