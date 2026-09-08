__generate_library()

## These include paths would be exported to project level
__component_add_include_dirs(.)
__component_add_include_dirs(mbedtls/include)
__component_add_include_dirs(port)

# When Matter is enabled CHIP brings its own mbedtls with a Matter-specific
# config (ipro7-chip-mbedtls-config.h has ECJPAKE/SPAKE2+/etc). Building
# ipro_sdk's mbedtls in parallel produces multiple-definition link errors
# against libCHIP.a's bundled mbedtls. Header paths above are still
# exposed so SDK-side code that #includes <mbedtls/...> still compiles —
# the actual symbols come from libCHIP.a at link time.
if(CONFIG_MATTER_ENABLE)
    set(_matter_mbedtls_stub "${CMAKE_CURRENT_BINARY_DIR}/mbedtls_skipped_stub.c")
    if(NOT EXISTS "${_matter_mbedtls_stub}")
        file(WRITE "${_matter_mbedtls_stub}"
            "/* CONFIG_MATTER_ENABLE=y — SDK mbedtls compilation skipped.\n"
            " * Real mbedtls symbols come from libCHIP.a's bundled copy. */\n"
            "int ipro7_sdk_mbedtls_skipped_marker = 0;\n")
    endif()
    __component_add_sources(${_matter_mbedtls_stub})
    return()
endif()

# Add hardware acceleration include path based on platform
if (CONFIG_IPRO7)
  __component_add_include_dirs(port/hw_acc_ipro7)
else()
  __component_add_include_dirs(port/hw_acc)
endif()

## not be exported to project level
__component_add_private_include_dirs(mbedtls/library)

__component_add_compile_definitions(-DCONFIG_MBEDTLS)
__component_add_compile_definitions(-DMBEDTLS_CONFIG_FILE=\"mbedtls_sample_config.h\")
__component_add_compile_definitions(-DIPRO_CRYPT_HARDWARE)

# AES: Use hardware implementation if available, otherwise software
if (NOT CONFIG_MBEDTLS_AES_USE_HW)
  __component_add_sources(mbedtls/library/aes.c)
endif()
__component_add_sources(mbedtls/library/aesni.c)
__component_add_sources(mbedtls/library/arc4.c)
__component_add_sources(mbedtls/library/aria.c)
__component_add_sources(mbedtls/library/asn1parse.c)
__component_add_sources(mbedtls/library/asn1write.c)
__component_add_sources(mbedtls/library/base64.c)
__component_add_sources(mbedtls/library/blowfish.c)
__component_add_sources(mbedtls/library/camellia.c)
__component_add_sources(mbedtls/library/ccm.c)
__component_add_sources(mbedtls/library/certs.c)
__component_add_sources(mbedtls/library/chacha20.c)
__component_add_sources(mbedtls/library/chachapoly.c)
__component_add_sources(mbedtls/library/cipher.c)
__component_add_sources(mbedtls/library/cipher_wrap.c)
__component_add_sources(mbedtls/library/cmac.c)
__component_add_sources(mbedtls/library/constant_time.c)
__component_add_sources(mbedtls/library/ctr_drbg.c)
__component_add_sources(mbedtls/library/debug.c)
__component_add_sources(mbedtls/library/des.c)
__component_add_sources(mbedtls/library/dhm.c)
__component_add_sources(mbedtls/library/ecdh.c)
__component_add_sources(mbedtls/library/ecdsa.c)
__component_add_sources(mbedtls/library/ecjpake.c)
__component_add_sources(mbedtls/library/ecp.c)
__component_add_sources(mbedtls/library/ecp_curves.c)
__component_add_sources(mbedtls/library/entropy.c)
# __component_add_sources(mbedtls/library/entropy_poll.c)  # Unix/Windows only, use port/entropy_poll_alt.c
__component_add_sources(mbedtls/library/error.c)
__component_add_sources(mbedtls/library/gcm.c)
__component_add_sources(mbedtls/library/havege.c)
__component_add_sources(mbedtls/library/hkdf.c)
__component_add_sources(mbedtls/library/hmac_drbg.c)
__component_add_sources(mbedtls/library/md2.c)
__component_add_sources(mbedtls/library/md4.c)
__component_add_sources(mbedtls/library/md5.c)
__component_add_sources(mbedtls/library/md.c)
__component_add_sources(mbedtls/library/memory_buffer_alloc.c)
__component_add_sources(mbedtls/library/mps_reader.c)
__component_add_sources(mbedtls/library/mps_trace.c)
__component_add_sources(mbedtls/library/nist_kw.c)
__component_add_sources(mbedtls/library/oid.c)
__component_add_sources(mbedtls/library/padlock.c)
__component_add_sources(mbedtls/library/pem.c)
__component_add_sources(mbedtls/library/pk.c)
__component_add_sources(mbedtls/library/pkparse.c)
__component_add_sources(mbedtls/library/pkcs11.c)
__component_add_sources(mbedtls/library/pkcs12.c)
__component_add_sources(mbedtls/library/pkcs5.c)
__component_add_sources(mbedtls/library/pk_wrap.c)
__component_add_sources(mbedtls/library/pkwrite.c)
__component_add_sources(mbedtls/library/platform.c)
__component_add_sources(mbedtls/library/platform_util.c)
__component_add_sources(mbedtls/library/poly1305.c)
__component_add_sources(mbedtls/library/ripemd160.c)
__component_add_sources(mbedtls/library/rsa.c)
__component_add_sources(mbedtls/library/rsa_internal.c)

# SHA: Use hardware if available, else software
if (NOT CONFIG_MBEDTLS_SHA1_USE_HW)
  __component_add_sources(mbedtls/library/sha1.c)
endif()
if (NOT CONFIG_MBEDTLS_SHA256_USE_HW)
  __component_add_sources(mbedtls/library/sha256.c)
endif()
if (NOT CONFIG_MBEDTLS_SHA512_USE_HW)
  __component_add_sources(mbedtls/library/sha512.c)
endif()

__component_add_sources(mbedtls/library/ssl_cache.c)
__component_add_sources(mbedtls/library/ssl_ciphersuites.c)
__component_add_sources(mbedtls/library/ssl_cli.c)
__component_add_sources(mbedtls/library/ssl_cookie.c)
__component_add_sources(mbedtls/library/ssl_msg.c)
__component_add_sources(mbedtls/library/ssl_srv.c)
__component_add_sources(mbedtls/library/ssl_ticket.c)
__component_add_sources(mbedtls/library/ssl_tls13_keys.c)
__component_add_sources(mbedtls/library/ssl_tls.c)
__component_add_sources(mbedtls/library/threading.c)
# __component_add_sources(mbedtls/library/timing.c)  # Unix/Windows only, not needed for embedded
__component_add_sources(mbedtls/library/version.c)
__component_add_sources(mbedtls/library/version_features.c)
__component_add_sources(mbedtls/library/x509.c)
__component_add_sources(mbedtls/library/x509_create.c)
__component_add_sources(mbedtls/library/x509_crl.c)
# x509_crt.c is safe when MBEDTLS_FS_IO is undef (no dirent.h dependency)
__component_add_sources(mbedtls/library/x509_crt.c)
__component_add_sources(mbedtls/library/x509_csr.c)
__component_add_sources(mbedtls/library/x509write_crt.c)
__component_add_sources(mbedtls/library/x509write_csr.c)
__component_add_sources(mbedtls/library/xtea.c)

if(CONFIG_LWIP)
  __component_add_compile_definitions(-DMBEDTLS_NET_C)
  __component_add_sources(port/net_sockets.c)
endif()

__component_add_sources(port/entropy_poll_alt.c)

set(MBEDTLS_USE_PKA 0)

# Determine hardware acceleration port directory
# IPRO7 uses Sec_Eng_* API, IPRO6LE uses device driver framework
if (CONFIG_IPRO7)
  set(HW_ACC_DIR "port/hw_acc_ipro7")
  __component_add_include_dirs(port/hw_acc_ipro7)
else()
  set(HW_ACC_DIR "port/hw_acc")
endif()

# AES HW
if (CONFIG_MBEDTLS_AES_USE_HW)
  __component_add_compile_definitions(-DCONFIG_MBEDTLS_AES_USE_HW)
  __component_add_sources(${HW_ACC_DIR}/aes_alt.c)
endif()

# SHA1 HW
if (CONFIG_MBEDTLS_SHA1_USE_HW)
  __component_add_compile_definitions(-DCONFIG_MBEDTLS_SHA1_USE_HW)
  __component_add_sources(${HW_ACC_DIR}/sha1_alt.c)
endif()

# SHA256 HW
if (CONFIG_MBEDTLS_SHA256_USE_HW)
  __component_add_compile_definitions(-DCONFIG_MBEDTLS_SHA256_USE_HW)
  __component_add_sources(${HW_ACC_DIR}/sha256_alt.c)
endif()

# SHA512 HW
if (CONFIG_MBEDTLS_SHA512_USE_HW)
  __component_add_compile_definitions(-DCONFIG_MBEDTLS_SHA512_USE_HW)
  __component_add_sources(${HW_ACC_DIR}/sha512_alt.c)
endif()

# GCM HW
if (CONFIG_MBEDTLS_GCM_USE_HW)
  __component_add_compile_definitions(-DCONFIG_MBEDTLS_GCM_USE_HW)
  __component_add_sources(${HW_ACC_DIR}/gcm_alt.c)
endif()

# ECDSA HW
if (CONFIG_MBEDTLS_ECDSA_USE_HW)
  __component_add_compile_definitions(-DCONFIG_MBEDTLS_ECDSA_USE_HW)
  __component_add_compile_definitions(-DMBEDTLS_ECDSA_SIGN_ALT)
  __component_add_compile_definitions(-DMBEDTLS_ECDSA_VERIFY_ALT)
  __component_add_sources(${HW_ACC_DIR}/ecdsa_alt.c)
  __component_add_sources(${HW_ACC_DIR}/ecdsa_self_test_simple.c)
endif()

# ECDH HW
if (CONFIG_MBEDTLS_ECDH_USE_HW)
  __component_add_compile_definitions(-DCONFIG_MBEDTLS_ECDH_USE_HW)
  __component_add_compile_definitions(-DMBEDTLS_ECDH_GEN_PUBLIC_ALT)
  __component_add_compile_definitions(-DMBEDTLS_ECDH_COMPUTE_SHARED_ALT)
  __component_add_sources(${HW_ACC_DIR}/ecdh_alt.c)
endif()

# bignum HW
if (CONFIG_MBEDTLS_BIGNUM_USE_HW)
  if (CONFIG_IPRO7)
    # IPRO7 uses sec_dsa_mexp_binary() for HW modular exponentiation
    __component_add_sources(port/bignum.c)
    __component_add_sources(${HW_ACC_DIR}/bignum_alt.c)
  else()
    set(MBEDTLS_USE_PKA 1)
    __component_add_sources(port/bignum.c)
    __component_add_sources(${HW_ACC_DIR}/bignum_alt.c)
  endif()
else()
  __component_add_sources(mbedtls/library/bignum.c)
endif()

# ECC HW
if (CONFIG_MBEDTLS_ECC_USE_HW)
  if (CONFIG_IPRO7)
    # ECC/PKA not yet implemented for IPRO7, use software fallback
    message(WARNING "ECC hardware acceleration not yet implemented for IPRO7, using software")
  else()
    set(MBEDTLS_USE_PKA 1)
    __component_add_compile_definitions(-DCONFIG_MBEDTLS_ECC_USE_HW)
    __component_add_sources(${HW_ACC_DIR}/ecp_alt.c)
    __component_add_sources(${HW_ACC_DIR}/ecp_curves_alt.c)
  endif()
endif()

if(MBEDTLS_USE_PKA)
  __component_add_sources(${HW_ACC_DIR}/bignum_ext.c)
endif()

if (CONFIG_MBEDTLS_SELF_TEST)
    __component_add_sources(port/mbedtls_self_test.c)
    # MBEDTLS_SELF_TEST already defined in mbedtls_sample_config.h
    __component_add_compile_definitions(-DCONFIG_MBEDTLS_SELF_TEST)
endif()

__component_add_sources(port/sec_mutex.c)