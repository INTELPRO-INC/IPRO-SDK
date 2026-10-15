__component_add_subdirectory_ifdef(CONFIG_COMPONENTS_FREERTOS_ENABLE freertos)
__component_add_subdirectory_ifdef(CONFIG_COMPONENTS_FREERTOS_POSIX_ENABLE freertos_posix)
# Zephyr builds come in through zephyr/module.yml (west), not this tree.

# IPRO OSAL - OS Abstraction Layer (works with FreeRTOS)
__component_add_subdirectory_ifdef(CONFIG_IPRO_OSAL ipro_osal)


