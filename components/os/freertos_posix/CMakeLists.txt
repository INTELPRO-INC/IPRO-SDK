__generate_library(1)

################# Add global include #################
__component_add_private_include_dirs(include/FreeRTOS_POSIX)
__component_add_include_dirs(include)
#######################################################

############## Add current dir source files ###########
__component_add_sources(src/FreeRTOS_POSIX_clock.c)
__component_add_sources(src/FreeRTOS_POSIX_pthread_barrier.c)
__component_add_sources(src/FreeRTOS_POSIX_pthread_cond.c)
__component_add_sources(src/FreeRTOS_POSIX_sched.c)
__component_add_sources(src/FreeRTOS_POSIX_timer.c)
__component_add_sources(src/FreeRTOS_POSIX_utils.c)
__component_add_sources(src/FreeRTOS_POSIX_mqueue.c)
__component_add_sources(src/FreeRTOS_POSIX_pthread.c)
__component_add_sources(src/FreeRTOS_POSIX_pthread_mutex.c)
__component_add_sources(src/FreeRTOS_POSIX_semaphore.c)
__component_add_sources(src/FreeRTOS_POSIX_unistd.c)
__component_add_sources(src/os_hal.c)

############ Add global compile option ################
#add components denpend on this component
