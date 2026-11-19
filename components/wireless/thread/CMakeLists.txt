# Thread Components CMakeLists.txt

# Raw per-app .config builds do not resolve Kconfig dependencies.
if(CONFIG_THREAD_BR_AUTOMATIC_BACKBONE AND NOT CONFIG_THREAD_WIFI_BORDER_ROUTER)
    message(FATAL_ERROR "Automatic BR backbone requires CONFIG_THREAD_WIFI_BORDER_ROUTER")
endif()
if((CONFIG_THREAD_BR_NAT64 OR CONFIG_THREAD_BR_UPSTREAM_DNS)
   AND NOT CONFIG_THREAD_WIFI_BORDER_ROUTER)
    message(FATAL_ERROR "BR NAT64/upstream DNS requires CONFIG_THREAD_WIFI_BORDER_ROUTER")
endif()
if(CONFIG_THREAD_BR_DNS64 AND NOT CONFIG_THREAD_BR_UPSTREAM_DNS)
    message(FATAL_ERROR "BR DNS64 requires CONFIG_THREAD_BR_UPSTREAM_DNS")
endif()
if(CONFIG_THREAD_BR_NAT64 AND NOT CONFIG_THREAD_BR_UPSTREAM_DNS)
    message(FATAL_ERROR "BR NAT64 prefix discovery requires CONFIG_THREAD_BR_UPSTREAM_DNS")
endif()
if(CONFIG_THREAD_BR_NAT64)
    if(NOT DEFINED CONFIG_THREAD_BR_NAT64_MAX_MAPPINGS)
        set(CONFIG_THREAD_BR_NAT64_MAX_MAPPINGS 32)
    endif()
    if(NOT "${CONFIG_THREAD_BR_NAT64_MAX_MAPPINGS}" MATCHES "^[1-9][0-9]*$"
       OR CONFIG_THREAD_BR_NAT64_MAX_MAPPINGS GREATER 128)
        message(FATAL_ERROR "BR NAT64 mapping limit must be an integer in 1..128")
    endif()
    __component_add_compile_definitions(BR_NAT64_SESSIONS=${CONFIG_THREAD_BR_NAT64_MAX_MAPPINGS})
endif()

# Low-level HDLC sources do not include the platform core config header.
# Propagate this layout-affecting test flag to every Thread translation unit
# before creating either the port or OpenThread targets.
if(CONFIG_THREAD_RCP_DEVICE_UART_DIAGNOSTICS)
    add_compile_definitions(CONFIG_THREAD_RCP_DEVICE_UART_DIAGNOSTICS=1)
endif()

# Add OpenThread port layer first (provides platform implementations)
add_subdirectory(openthread_port)

# Configure and add OpenThread stack.
# When Matter is enabled, CHIP brings its own copy of openthread-ftd/mtd via
# libCHIP.a; building ipro_sdk's copy in parallel produces multiple-definition
# link errors. The openthread_port above is still built — it provides the
# IEEE 802.15.4 radio + alarm + settings glue that CHIP's OT instance calls
# into via otPlatRadio* / otPlatAlarm* / otPlatSettings*.
if((CONFIG_OPENTHREAD_FTD OR CONFIG_OPENTHREAD_MTD OR CONFIG_OPENTHREAD_RADIO)
   AND (NOT CONFIG_MATTER_ENABLE OR IPRO_MATTER_BR_PACKAGE_ACTIVE))
    # Set OpenThread build options before including its CMakeLists.txt
    set(OT_PLATFORM "external" CACHE STRING "OpenThread platform" FORCE)
    set(OT_BUILD_EXECUTABLES OFF CACHE BOOL "Build OpenThread executables" FORCE)
    set(OT_COVERAGE OFF CACHE BOOL "Enable coverage" FORCE)

    # Restore cached radio state after an external RCP reset/timeout instead
    # of taking the POSIX-style process exit path on this embedded host.
    if(CONFIG_THREAD_RADIO_SPINEL)
        set(OT_RCP_RESTORATION_MAX_COUNT 3 CACHE STRING "Bounded RCP restoration attempts" FORCE)
    else()
        set(OT_RCP_RESTORATION_MAX_COUNT 0 CACHE STRING "No external RCP to restore" FORCE)
    endif()
    
    # Disable all applications - we build our own
    set(OT_APP_CLI OFF CACHE BOOL "Build CLI app" FORCE)
    set(OT_APP_NCP OFF CACHE BOOL "Build NCP app" FORCE)
    set(OT_APP_RCP OFF CACHE BOOL "Build RCP app" FORCE)
    
    # Disable CTest and all test building
    set(BUILD_TESTING OFF CACHE BOOL "Disable CTest" FORCE)
    
    # Use SDK's external mbedTLS library (required by other components)
    set(OT_MBEDTLS OFF CACHE BOOL "Don't use OpenThread built-in mbedTLS" FORCE)
    set(OT_EXTERNAL_MBEDTLS "mbedtls" CACHE STRING "Use SDK mbedtls" FORCE)
    set(OT_BUILTIN_MBEDTLS_MANAGEMENT OFF CACHE BOOL "Disable OpenThread mbedTLS management" FORCE)
    
    # Disable features we don't need for embedded
    set(OT_DOC OFF CACHE BOOL "Build documentation" FORCE)
    set(OT_TESTS OFF CACHE BOOL "Build tests" FORCE)
    
    # Configure device type
    if(CONFIG_OPENTHREAD_FTD)
        set(OT_FTD ON CACHE BOOL "Enable FTD" FORCE)
        set(OT_MTD OFF CACHE BOOL "Disable MTD" FORCE)
        set(OT_RCP OFF CACHE BOOL "Disable RCP" FORCE)
    elseif(CONFIG_OPENTHREAD_MTD)
        set(OT_FTD OFF CACHE BOOL "Disable FTD" FORCE)
        set(OT_MTD ON CACHE BOOL "Enable MTD" FORCE)
        set(OT_RCP OFF CACHE BOOL "Disable RCP" FORCE)
    elseif(CONFIG_OPENTHREAD_RADIO)
        set(OT_FTD OFF CACHE BOOL "Disable FTD" FORCE)
        set(OT_MTD OFF CACHE BOOL "Disable MTD" FORCE)
        set(OT_RCP ON CACHE BOOL "Enable RCP" FORCE)
    endif()
    
    # Configure features based on Kconfig
    if(CONFIG_OPENTHREAD_COMMISSIONER)
        set(OT_COMMISSIONER ON CACHE BOOL "Enable Commissioner" FORCE)
    endif()
    
    if(CONFIG_OPENTHREAD_JOINER)
        set(OT_JOINER ON CACHE BOOL "Enable Joiner" FORCE)
    endif()
    
    if(CONFIG_OPENTHREAD_DHCP6_CLIENT)
        set(OT_DHCP6_CLIENT ON CACHE BOOL "Enable DHCPv6 Client" FORCE)
    endif()
    
    if(CONFIG_OPENTHREAD_DHCP6_SERVER)
        set(OT_DHCP6_SERVER ON CACHE BOOL "Enable DHCPv6 Server" FORCE)
    endif()
    
    if(CONFIG_OPENTHREAD_DNS_CLIENT)
        set(OT_DNS_CLIENT ON CACHE BOOL "Enable DNS Client" FORCE)
    else()
        set(OT_DNS_CLIENT OFF CACHE BOOL "Enable DNS Client" FORCE)
    endif()
    
    if(CONFIG_OPENTHREAD_BORDER_ROUTER)
        set(OT_BORDER_ROUTER ON CACHE BOOL "Enable Border Router" FORCE)
    endif()

    if(CONFIG_THREAD_WIFI_BORDER_ROUTER)
        set(OT_BACKBONE_ROUTER ON CACHE BOOL "Enable Backbone Router" FORCE)
        set(OT_BACKBONE_ROUTER_MULTICAST_ROUTING ON CACHE BOOL "Enable BBR multicast listener registration" FORCE)
        set(OT_BACKBONE_ROUTER_DUA_NDPROXYING ON CACHE BOOL "Enable BBR DUA proxy" FORCE)
        set(OT_BORDER_ROUTER ON CACHE BOOL "Enable Border Router" FORCE)
        set(OT_BORDER_ROUTING ON CACHE BOOL "Enable Border Routing Manager" FORCE)
        set(OT_BORDER_ROUTING_DHCP6_PD ON CACHE BOOL "Enable delegated OMR prefix policy" FORCE)
        set(OT_BORDER_ROUTING_DHCP6_PD_CLIENT ON CACHE BOOL "Enable native DHCPv6 PD client" FORCE)
        set(OT_MDNS ON CACHE BOOL "Enable built-in multicast DNS" FORCE)
        set(OT_DNSSD_SERVER ON CACHE BOOL "Enable DNS-SD server" FORCE)
        set(OT_DNSSD_DISCOVERY_PROXY ON CACHE BOOL "Proxy Thread queries into LAN mDNS" FORCE)
        set(OT_SRP_SERVER ON CACHE BOOL "Enable SRP server" FORCE)
        set(OT_SRP_ADV_PROXY ON CACHE BOOL "Proxy SRP registrations into mDNS" FORCE)
        set(OT_PLATFORM_UDP ON CACHE BOOL "Enable app-supplied platform UDP" FORCE)
        set(OT_BORDER_AGENT ON CACHE BOOL "Enable Border Agent" FORCE)
        set(OT_BORDER_AGENT_MESHCOP_SERVICE ON CACHE BOOL "Publish MeshCoP service" FORCE)
    else()
        set(OT_BACKBONE_ROUTER OFF CACHE BOOL "Enable Backbone Router" FORCE)
        set(OT_BORDER_ROUTING_DHCP6_PD OFF CACHE BOOL "Enable delegated OMR prefix policy" FORCE)
        set(OT_BORDER_ROUTING_DHCP6_PD_CLIENT OFF CACHE BOOL "Enable native DHCPv6 PD client" FORCE)
        set(OT_BACKBONE_ROUTER_MULTICAST_ROUTING OFF CACHE BOOL "Enable BBR multicast listener registration" FORCE)
        set(OT_BACKBONE_ROUTER_DUA_NDPROXYING OFF CACHE BOOL "Enable BBR DUA proxy" FORCE)
        set(OT_MDNS OFF CACHE BOOL "Enable built-in multicast DNS" FORCE)
        set(OT_DNSSD_SERVER OFF CACHE BOOL "Enable DNS-SD server" FORCE)
        set(OT_DNSSD_DISCOVERY_PROXY OFF CACHE BOOL "Proxy Thread queries into LAN mDNS" FORCE)
        set(OT_SRP_SERVER OFF CACHE BOOL "Enable SRP server" FORCE)
        set(OT_SRP_ADV_PROXY OFF CACHE BOOL "Proxy SRP registrations into mDNS" FORCE)
        set(OT_PLATFORM_UDP OFF CACHE BOOL "Enable app-supplied platform UDP" FORCE)
        set(OT_BORDER_AGENT_MESHCOP_SERVICE OFF CACHE BOOL "Publish MeshCoP service" FORCE)
        if(CONFIG_OPENTHREAD_BORDER_AGENT_ENABLE)
            set(OT_BORDER_AGENT ON CACHE BOOL "Enable Border Agent" FORCE)
        else()
            set(OT_BORDER_AGENT OFF CACHE BOOL "Enable Border Agent" FORCE)
        endif()
    endif()

    # Set both states so reconfiguration cannot retain an enabled OT cache.
    foreach(option OT_NAT64_TRANSLATOR OT_NAT64_BORDER_ROUTING)
        if(CONFIG_THREAD_BR_NAT64)
            set(${option} ON CACHE STRING "BR NAT64" FORCE)
        else()
            set(${option} OFF CACHE STRING "BR NAT64" FORCE)
        endif()
    endforeach()
    if(CONFIG_THREAD_BR_UPSTREAM_DNS)
        set(OT_DNS_UPSTREAM_QUERY ON CACHE STRING "BR upstream DNS" FORCE)
    else()
        set(OT_DNS_UPSTREAM_QUERY OFF CACHE STRING "BR upstream DNS" FORCE)
    endif()

    if(CONFIG_OPENTHREAD_DUA_ENABLE)
        set(OT_DUA ON CACHE BOOL "Enable Domain Unicast Address client" FORCE)
    else()
        set(OT_DUA OFF CACHE BOOL "Enable Domain Unicast Address client" FORCE)
    endif()
    if(CONFIG_OPENTHREAD_MLR_ENABLE)
        set(OT_MLR ON CACHE BOOL "Enable multicast listener registration client" FORCE)
    else()
        set(OT_MLR OFF CACHE BOOL "Enable multicast listener registration client" FORCE)
    endif()
    if(CONFIG_OPENTHREAD_SRP_CLIENT_ENABLE)
        set(OT_SRP_CLIENT ON CACHE BOOL "Enable SRP client" FORCE)
    else()
        set(OT_SRP_CLIENT OFF CACHE BOOL "Enable SRP client" FORCE)
    endif()
    if(CONFIG_OPENTHREAD_ECDSA_ENABLE)
        set(OT_ECDSA ON CACHE BOOL "Enable ECDSA" FORCE)
    else()
        set(OT_ECDSA OFF CACHE BOOL "Enable ECDSA" FORCE)
    endif()

    # `ot ping` CLI command. OT_PING_SENDER defaults to OT_APP_CLI which
    # we force OFF above (we build our own CLI integration), so the OT
    # tree leaves PING_SENDER off unless we set it explicitly here.
    if(CONFIG_OPENTHREAD_PING_SENDER)
        set(OT_PING_SENDER ON CACHE BOOL "Enable ping sender" FORCE)
    endif()

    # Network Diagnostic CLIENT (otThreadSendDiagnosticGet) — lets a node query
    # a REMOTE device's diagnostics. The core default keys it to BORDER_ROUTING
    # (off here), so enable it explicitly when the app asks (fall gateway remote
    # device-diag). The diagnostic SERVER is always built; only the client opts in.
    if(CONFIG_OPENTHREAD_TMF_NETDIAG_CLIENT)
        set(OT_NETDIAG_CLIENT ON CACHE BOOL "Enable Network Diagnostic client" FORCE)
    endif()

    # Include OpenThread's own CMake build system
    add_subdirectory(openthread)
    if(CONFIG_THREAD_BR_UPSTREAM_DNS)
        # The platform transport admits four clients; its separate discovery
        # slot is not an OT upstream transaction.
        target_compile_definitions(ot-config INTERFACE
            OPENTHREAD_CONFIG_DNS_UPSTREAM_QUERY_MAX_CONCURRENT=4)
    endif()
    if(CONFIG_THREAD_BR_NAT64)
        target_compile_definitions(ot-config INTERFACE
            OPENTHREAD_CONFIG_NAT64_MAX_MAPPINGS=${CONFIG_THREAD_BR_NAT64_MAX_MAPPINGS})
    endif()

    # HDLC Decoder embeds enum state in objects owned by SDK-built NCP/port
    # code. Keep its ABI identical on both RCP and host builds.
    foreach(wire_lib openthread-hdlc openthread-spinel-rcp openthread-spinel-ncp)
        target_compile_options(${wire_lib} PRIVATE -fshort-enums)
    endforeach()

    if(CONFIG_THREAD_RADIO_SPINEL)
        # SDK uses short enums and Kconfig-driven frame layouts. The upstream
        # host libraries otherwise build without those flags and corrupt the
        # shared SpinelDriver/RadioSpinel objects across translation units.
        set_property(TARGET openthread-radio-spinel PROPERTY COMPILE_DEFINITIONS
            $<TARGET_PROPERTY:openthread-ftd,COMPILE_DEFINITIONS>)
        target_include_directories(openthread-radio-spinel PRIVATE
            $<TARGET_PROPERTY:openthread-ftd,INCLUDE_DIRECTORIES>)
        foreach(host_lib openthread-radio-spinel openthread-spinel-ncp openthread-hdlc)
            target_compile_options(${host_lib} PRIVATE
                $<TARGET_PROPERTY:openthread-ftd,COMPILE_OPTIONS>)
        endforeach()
        target_link_libraries(openthread_port PUBLIC ot-config ot-config-ftd
            openthread-radio-spinel openthread-spinel-ncp openthread-hdlc)
        set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS openthread_port)
    endif()
    
    # Add mbedTLS include directories and configuration to OpenThread's ot-config interface target
    # This makes mbedTLS headers and Thread-specific config available to all OpenThread targets
    if(TARGET ot-config)
        if(IPRO_MATTER_BR_PACKAGE_ACTIVE)
            target_link_libraries(ot-config INTERFACE ipro_matter_crypto)
        else()
            target_include_directories(ot-config INTERFACE
                ${CMAKE_SOURCE_DIR}/components/crypto/mbedtls
                ${CMAKE_SOURCE_DIR}/components/crypto/mbedtls/mbedtls/include
                ${CMAKE_SOURCE_DIR}/components/crypto/mbedtls/port
            )
            target_compile_definitions(ot-config INTERFACE
                MBEDTLS_CONFIG_FILE="mbedtls_sample_config.h"
            )
        endif()
        target_compile_definitions(ot-config INTERFACE
            OPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS=0
            OPENTHREAD_CONFIG_ENABLE_BUILTIN_MBEDTLS_MANAGEMENT=0
        )
    endif()
    
    # Link mbedtls component library to OpenThread targets
    # Note: mbedtls component is built by the crypto/mbedtls component
    if(CONFIG_OPENTHREAD_FTD)
        if(TARGET openthread-ftd AND TARGET mbedtls)
            target_link_libraries(openthread-ftd PUBLIC mbedtls)
        endif()
    elseif(CONFIG_OPENTHREAD_MTD)
        if(TARGET openthread-mtd AND TARGET mbedtls)
            target_link_libraries(openthread-mtd PUBLIC mbedtls)
        endif()
    elseif(CONFIG_OPENTHREAD_RADIO)
        if(TARGET openthread-rcp AND TARGET mbedtls)
            target_link_libraries(openthread-rcp PUBLIC mbedtls)
        endif()
    endif()
    
    # Link the OpenThread library with our port layer
    if(CONFIG_OPENTHREAD_FTD)
        target_link_libraries(openthread-ftd PUBLIC openthread_port)
        # Register OpenThread FTD library for application linking
        set_property(GLOBAL APPEND PROPERTY SDK_LIBS openthread-ftd)
        if(CONFIG_OPENTHREAD_CLI)
            set_property(GLOBAL APPEND PROPERTY SDK_LIBS openthread-cli-ftd)
        endif()
    elseif(CONFIG_OPENTHREAD_MTD)
        target_link_libraries(openthread-mtd PUBLIC openthread_port)
        # Register OpenThread MTD library for application linking
        set_property(GLOBAL APPEND PROPERTY SDK_LIBS openthread-mtd)
        if(CONFIG_OPENTHREAD_CLI)
            set_property(GLOBAL APPEND PROPERTY SDK_LIBS openthread-cli-mtd)
        endif()
    elseif(CONFIG_OPENTHREAD_RADIO)
        target_link_libraries(openthread-rcp PUBLIC openthread_port)
        set_property(GLOBAL APPEND PROPERTY SDK_LIBS openthread-rcp)
        # The radio core calls platform hooks from a later static archive.
        # Keep every port hook available despite one-pass archive ordering.
        set_property(GLOBAL APPEND PROPERTY SDK_WHOLE_ARCHIVE_LIBS openthread_port)
    endif()
endif()
