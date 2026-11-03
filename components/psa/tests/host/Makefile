# Host unit tests for the pure-C parts of the SPM.
# make -C components/psa/tests/host test [MBEDTLS_V=3|4]
# MBEDTLS_V=3: SPE core and client headers of mbedtls 3.6, in build3/
# MBEDTLS_V=4: the same of TF-PSA-Crypto 1.2, in build4/

CC := clang
CFLAGS := -Wall -Wextra -Werror -std=c99 -DSPM_HOST \
          -fsanitize=address,undefined -fno-omit-frame-pointer \
          -I../../include -I../../spm
SPM := ../../spm
HAL_INC := ../../../../bsp/hal/inc
HAL_SEC_INC := ../../../../bsp/hal/sec/inc
MBEDTLS_V ?= 2
SPE_APP := ../../../../apps/security/ipro7_psa_spe
CLIENT := ../../client
ifeq ($(MBEDTLS_V),4)
BUILD := build4
TFPSA := ../../../crypto/tfpsa/tf-psa-crypto
SPE_CFG_H := tfpsa_config.h
CLIENT_CFG_H := psa_client_tfpsa_config.h
CFG_DEF := TF_PSA_CRYPTO_CONFIG_FILE
LIB_INC := -I$(TFPSA)/include -I$(TFPSA)/drivers/builtin/include
# the sources of components/crypto/tfpsa/CMakeLists.txt
LIB_DIRS := $(TFPSA)/core $(TFPSA)/drivers/builtin/src $(TFPSA)/extras \
            $(TFPSA)/platform $(TFPSA)/utilities
LIB_PRIV := $(LIB_DIRS:%=-I%) -I$(TFPSA)/dispatch \
            -DLIBTESTDRIVER1_TF_PSA_CRYPTO_MARKER=0
MBEDTLS_LIB := $(filter-out psa_its_file, \
                 $(basename $(notdir $(wildcard $(LIB_DIRS:%=%/*.c)))))
LIB_EXTRA := $(BUILD)/mbedtls/hw_entropy.o
else ifeq ($(MBEDTLS_V),3)
BUILD := build3
MBEDTLS := ../../../crypto/mbedtls3/mbedtls
SPE_CFG_H := mbedtls3_config.h
CLIENT_CFG_H := psa_client_config3.h
CFG_DEF := MBEDTLS_CONFIG_FILE
LIB_INC := -I$(MBEDTLS)/include
LIB_DIRS := $(MBEDTLS)/library
LIB_PRIV := -I$(MBEDTLS)/library
# the sources of components/crypto/mbedtls3/CMakeLists.txt
MBEDTLS_LIB := $(filter-out ssl_% x509% mps_% debug pkcs7 entropy_poll \
                 net_sockets timing, \
                 $(basename $(notdir $(wildcard $(MBEDTLS)/library/*.c))))
else
BUILD := build
MBEDTLS := ../../../crypto/mbedtls/mbedtls
SPE_CFG_H := mbedtls_app_config.h
CLIENT_CFG_H := psa_client_config.h
CFG_DEF := MBEDTLS_CONFIG_FILE
LIB_INC := -I$(MBEDTLS)/include
LIB_DIRS := $(MBEDTLS)/library
LIB_PRIV := -I$(MBEDTLS)/library
# mbedtls PSA core for test_svc_crypto, built with the SPE config: the
# sources of components/crypto/mbedtls/CMakeLists.txt that hold code
# under apps/security/ipro7_psa_spe/mbedtls_app_config.h
MBEDTLS_LIB := aes asn1parse asn1write bignum ccm cipher cipher_wrap cmac \
               constant_time ctr_drbg ecdh ecdsa ecp ecp_curves entropy \
               gcm hkdf hmac_drbg md oid platform platform_util sha1 \
               sha256 sha512 psa_crypto psa_crypto_aead psa_crypto_cipher \
               psa_crypto_client psa_crypto_driver_wrappers psa_crypto_ecp \
               psa_crypto_hash psa_crypto_mac psa_crypto_rsa psa_crypto_se \
               psa_crypto_slot_management psa_crypto_storage
endif
SPE_CFG := -D$(CFG_DEF)='"$(SPE_CFG_H)"' -I$(SPE_APP) $(LIB_INC)
# client-mode mbedtls config of the NSPE client library
CLIENT_CFG := -D$(CFG_DEF)='"$(CLIENT_CFG_H)"' -I$(CLIENT) $(LIB_INC)
CLIENT_SRCS := $(CLIENT)/psa_crypto_client.c \
               $(CLIENT)/psa_crypto_client_sym.c \
               $(CLIENT)/psa_crypto_client_asym.c \
               $(CLIENT)/psa_its_client.c $(CLIENT)/psa_attest_client.c \
               $(CLIENT)/psa_platform_client.c $(CLIENT)/psa_fwu_client.c
MBEDTLS_OBJS := $(MBEDTLS_LIB:%=$(BUILD)/mbedtls/%.o) $(LIB_EXTRA)
MBEDTLS_A := $(BUILD)/libmbedhost.a
MBEDTLS_CFLAGS := -std=c99 -g -O1 -w -fsanitize=address,undefined \
                  -fno-omit-frame-pointer $(SPE_CFG) $(LIB_PRIV) \
                  -I../../include
ADAPTER := $(SPM)/its_mbedtls_adapter.c
# tinycbor encoder and parser for eat.c and the token checks
TINYCBOR := ../../../3rdparty/tinycbor/tinycbor/src
CBOR_LIB := cborencoder cborencoder_close_container_checked cborparser
CBOR_OBJS := $(CBOR_LIB:%=$(BUILD)/tinycbor/%.o)
CBOR_CFG := -isystem $(TINYCBOR)
ATTEST := $(SPM)/svc_attest.c $(SPM)/svc_platform.c $(SPM)/eat.c
ATTEST_H := $(SPM)/svc_attest.h $(SPM)/svc_platform.h $(SPM)/eat.h \
            ../../include/psa_ipro/attest_rpc.h \
            ../../include/psa_ipro/platform_rpc.h \
            ../../include/psa/initial_attestation.h
# svc_platform / svc_attest tests on the attest_test.c harness
ATTEST_TESTS := $(BUILD)/test_svc_platform $(BUILD)/test_svc_attest
ITS_STORE := $(SPM)/its_store.c its_ram.c
# littlefs for its_lfs.c on the lfs_ram.c flash (lfs_xip_init rules)
LFS := ../../../fs/littlefs
LFS_OBJS := $(BUILD)/lfs/lfs.o $(BUILD)/lfs/lfs_util.o
LFS_CONF := $(BUILD)/stub_lfs/generated/autoconf.h
LFS_CFG := -I$(BUILD)/stub_lfs -I$(LFS)/littlefs -I$(LFS)/port
ITS_STORE_H := $(SPM)/its_store.h its_ram.h
# huk.c per config: 1 IN_RAM+DEV, 2 IN_RAM, 3 IN_RAM+KAT+DEV, 4 DEV,
# 5 KAT, 6 = 3 with the HUK in slot 2; eFuse and SEC_ENG come from
# stub/ and test_huk.c
HUK_TESTS := $(foreach n,1 2 3 4 5 6,$(BUILD)/test_huk_$(n))
HUK_CFG_1 := -DCONFIG_PSA_HUK_IN_RAM -DCONFIG_PSA_DEV_HUK
HUK_CFG_2 := -DCONFIG_PSA_HUK_IN_RAM
HUK_CFG_3 := -DCONFIG_PSA_HUK_IN_RAM -DCONFIG_PSA_HUK_KAT -DCONFIG_PSA_DEV_HUK
HUK_CFG_4 := -DCONFIG_PSA_DEV_HUK
HUK_CFG_5 := -DCONFIG_PSA_HUK_KAT
HUK_CFG_6 := $(HUK_CFG_3) -DCONFIG_PSA_HUK_EFUSE_SLOT=2
# kdf.c over huk.c on one test HUK: RAM path and SEC_ENG KEY_SEL 1
HUK_KDF_TESTS := $(BUILD)/test_kdf_huk_ram $(BUILD)/test_kdf_huk_engine
HUK_KDF_ram := -DCONFIG_PSA_HUK_IN_RAM -DHUK_PATH='"RAM path"'
HUK_KDF_engine := -DHUK_PATH='"SEC_ENG path"' -DCONFIG_PSA_HUK_LOCK
# huk_prov.c over huk.c (KEY_SEL 1, lock on) and the stub/ fakes
HUKP_CFG := -Istub -I$(HAL_INC) -I$(HAL_SEC_INC) -DCONFIG_PSA_HUK_KEY_SEL=1 -DCONFIG_PSA_HUK_LOCK
HUKP_SRCS := test_huk_prov.c $(SPM)/huk_prov.c $(SPM)/huk.c \
             stub/fake_efuse.c stub/fake_trng.c
# configurations huk_prov.c must refuse to build (R1)
HUKP_BAD := -DCONFIG_PSA_HUK_IN_RAM -DCONFIG_PSA_DEV_HUK \
            -DCONFIG_PSA_HUK_KAT -UCONFIG_PSA_HUK_LOCK \
            -DCONFIG_PSA_HUK_KEY_SEL=3
# huk_lock with the lock on (production config) and off
HUK_LOCK_TESTS := $(BUILD)/test_huklock_on $(BUILD)/test_huklock_off
HUK_LOCK_on := -DCONFIG_PSA_HUK_LOCK -DLOCK_CFG='"LOCK DEV"'
HUK_LOCK_off := -DLOCK_CFG='"DEV"'
SVC_CRYPTO := $(wildcard $(SPM)/svc_crypto*.c) $(SPM)/spm_heap.c \
              $(ITS_STORE) $(ADAPTER)
SVC_CRYPTO_H := $(SPM)/svc_crypto.h $(SPM)/svc_crypto_priv.h \
                $(SPM)/spm_heap.h ../../include/psa_ipro/crypto_rpc.h \
                $(ITS_STORE_H)
# NMSIS riscv_encoding.h (PMP, CSR and mcfg_info bits) for spm_pmp.c
NMSIS := ../../../../bsp/drivers/ipro7_soc/risc-v/Core/Include
# U-mode FreeRTOS port: nspe_runtime.c helpers for test_nspe_virq
NSPE_PORT := ../../../os/freertos/Source/portable_ipro7_nspe
RV_CC := /opt/toolchain/riscv_ipro7/bin/riscv64-unknown-elf-gcc
RV_FLAGS := -march=rv32imafc -mabi=ilp32f -fsyntax-only -Wall -Werror \
            -I../../include
TESTS := $(BUILD)/test_spm_range $(BUILD)/test_spm_virq \
         $(BUILD)/test_spm_harden \
         $(BUILD)/test_spm_pmp $(BUILD)/test_spm_pmp_usb \
         $(BUILD)/test_spm_csr $(BUILD)/test_spm_ldst \
         $(BUILD)/test_spm_tzc $(BUILD)/test_spm_tzc_cci \
         $(BUILD)/test_spm_services $(BUILD)/test_spm_heap \
         $(BUILD)/test_spm_stack \
         $(BUILD)/test_crypto_rpc_client $(BUILD)/test_crypto_rpc_server \
         $(BUILD)/test_svc_crypto $(BUILD)/test_kdf $(BUILD)/test_its_store \
         $(BUILD)/test_its_meta $(BUILD)/test_its_adapter \
         $(BUILD)/test_svc_its $(BUILD)/test_its_lfs $(BUILD)/test_eat \
         $(ATTEST_TESTS) \
         $(HUK_TESTS) $(HUK_KDF_TESTS) $(HUK_LOCK_TESTS) \
         $(BUILD)/test_nspe_virq $(BUILD)/test_nspe_trap \
         $(BUILD)/test_client_marshal $(BUILD)/test_svc_fwu \
         $(BUILD)/test_huk_prov

test: $(TESTS)
	@for t in $(TESTS); do ./$$t || exit 1; done
	@c=$$(./$(BUILD)/test_crypto_rpc_client | grep '^layout'); \
	 s=$$(./$(BUILD)/test_crypto_rpc_server | grep '^layout'); \
	 echo "client $$c"; echo "server $$s"; \
	 if [ -n "$$c" ] && [ "$$c" = "$$s" ]; then \
	     echo "PASS 1/1 crypto_rpc layout client == server"; \
	 else \
	     echo "FAIL 0/1 crypto_rpc layout client != server"; exit 1; \
	 fi
	@if $(CC) $(CFLAGS) $(SPE_CFG) -Istub -I$(HAL_INC) -I$(HAL_SEC_INC) -fsyntax-only \
	     -DCONFIG_PSA_HUK_KEY_SEL=0 $(SPM)/huk.c 2>/dev/null; then \
	     echo "FAIL 0/1 huk.c builds KEY_SEL 0 without IN_RAM"; exit 1; \
	 else \
	     echo "PASS 1/1 huk.c refuses KEY_SEL 0 without IN_RAM"; \
	 fi
	@if $(CC) $(CFLAGS) $(SPE_CFG) -Istub -I$(HAL_INC) -I$(HAL_SEC_INC) -fsyntax-only \
	     -DCONFIG_PSA_HUK_KEY_SEL=1 -DCONFIG_PSA_HUK_EFUSE_SLOT=3 \
	     $(SPM)/huk.c 2>/dev/null; then \
	     echo "FAIL 0/1 huk.c builds HUK slot 3"; exit 1; \
	 elif $(CC) $(CFLAGS) $(SPE_CFG) $(HUKP_CFG) -fsyntax-only \
	     -DCONFIG_PSA_HUK_EFUSE_SLOT=2 $(SPM)/huk_prov.c 2>&1 | \
	     grep -q "burns slot 4"; then \
	     echo "PASS 1/1 huk.c refuses slot 3, huk_prov.c slot 2"; \
	 else \
	     echo "FAIL 0/1 huk_prov.c builds HUK slot 2"; exit 1; \
	 fi
	@for f in $(HUKP_BAD); do \
	     if ! $(CC) $(CFLAGS) $(SPE_CFG) $(HUKP_CFG) -fsyntax-only $$f \
	         $(SPM)/huk_prov.c 2>&1 | grep -q "HUK provisioning needs"; then \
	         echo "FAIL 0/1 huk_prov.c: no #error with $$f"; exit 1; \
	     fi; \
	 done; echo "PASS 1/1 huk_prov.c refuses $(words $(HUKP_BAD)) configs"
	@if [ -x $(RV_CC) ]; then \
	     $(RV_CC) $(RV_FLAGS) $(CLIENT_CFG) -DRPC_CLIENT test_crypto_rpc.c && \
	     $(RV_CC) $(RV_FLAGS) $(SPE_CFG) -DRPC_SERVER test_crypto_rpc.c && \
	     echo "PASS 1/1 crypto_rpc rv32 static asserts, both modes" || \
	     { echo "FAIL 0/1 crypto_rpc rv32 static asserts"; exit 1; }; \
	 else \
	     echo "SKIP crypto_rpc rv32 check: no $(RV_CC)"; \
	 fi

$(BUILD)/test_spm_range: test_spm_range.c $(SPM)/spm_range.c \
                         $(SPM)/spm_copy.c check.h | $(BUILD)
	$(CC) $(CFLAGS) -o $@ $(filter %.c,$^)

$(BUILD)/test_spm_virq: test_spm_virq.c $(SPM)/spm_virq.c check.h | $(BUILD)
	$(CC) $(CFLAGS) -o $@ $(filter %.c,$^)

$(BUILD)/test_spm_harden: test_spm_harden.c $(SPM)/spm_harden.c \
                          $(SPM)/spm_harden.h check.h | $(BUILD)
	$(CC) $(CFLAGS) -o $@ $(filter %.c,$^)

# spm_pmp.c on recording CSR stubs, spm_periph.c on stub/system_soc.h,
# without and with the NSPE DMA0 / USB windows
PMP_DEPS := test_spm_pmp.c $(SPM)/spm_pmp.c $(SPM)/spm_pmp.h \
            $(SPM)/spm_periph.c $(SPM)/spm_periph.h stub/system_soc.h \
            check.h
$(BUILD)/test_spm_pmp: $(PMP_DEPS) | $(BUILD)
	$(CC) $(CFLAGS) -Istub -I$(NMSIS) -o $@ $(filter %.c,$^)

$(BUILD)/test_spm_pmp_usb: $(PMP_DEPS) | $(BUILD)
	$(CC) $(CFLAGS) -Istub -I$(NMSIS) -DCONFIG_PSA_NSPE_DMA_USB \
	    -o $@ $(filter %.c,$^)

# spm_ldst.h: the lw/sw decoder of the spm_trap.c register grants
$(BUILD)/test_spm_ldst: test_spm_ldst.c $(SPM)/spm_ldst.h check.h | $(BUILD)
	$(CC) $(CFLAGS) -o $@ $(filter %.c,$^)

# spm_tzc.c on a TZC_SEC register model, CCI as ID0 and as ID1
TZC_DEPS := test_spm_tzc.c $(SPM)/spm_tzc.c $(SPM)/spm_tzc.h \
            $(SPM)/spm_periph.c $(SPM)/spm_periph.h stub/system_soc.h \
            check.h $(HAL_SEC_INC)/hal_sec_common_v2.h
$(BUILD)/test_spm_tzc: $(TZC_DEPS) | $(BUILD)
	$(CC) $(CFLAGS) -Istub -I$(HAL_INC) -I$(HAL_SEC_INC) -o $@ $(filter %.c,$^)

$(BUILD)/test_spm_tzc_cci: $(TZC_DEPS) | $(BUILD)
	$(CC) $(CFLAGS) -Istub -I$(HAL_INC) -I$(HAL_SEC_INC) -DCONFIG_PSA_TZC_CCI_ID1 -o $@ $(filter %.c,$^)

# spm_csr.h: the Zicsr decoder of the spm_trap.c CSR emulator
$(BUILD)/test_spm_csr: test_spm_csr.c $(SPM)/spm_csr.h check.h | $(BUILD)
	$(CC) $(CFLAGS) -o $@ $(filter %.c,$^)

$(BUILD)/test_nspe_virq: test_nspe_virq.c $(NSPE_PORT)/nspe_runtime.c \
                         $(NSPE_PORT)/nspe_runtime.h \
                         ../../include/psa_ipro/ecall.h check.h | $(BUILD)
	$(CC) $(CFLAGS) -I$(NSPE_PORT) -o $@ $(filter %.c,$^)

# port.c on the stub_rtos/ kernel headers
$(BUILD)/test_nspe_trap: test_nspe_trap.c $(NSPE_PORT)/port.c \
                         $(NSPE_PORT)/nspe_runtime.c \
                         $(NSPE_PORT)/nspe_runtime.h \
                         ../../include/psa_ipro/ecall.h \
                         stub_rtos/FreeRTOS.h stub_rtos/task.h \
                         check.h | $(BUILD)
	$(CC) $(CFLAGS) -Istub_rtos -I$(NSPE_PORT) -o $@ $(filter %.c,$^)

$(BUILD)/test_spm_services: test_spm_services.c $(SPM)/spm_services.c \
                            $(SPM)/spm_range.c $(SPM)/spm.h \
                            $(SPM)/svc_crypto.h $(SPM)/svc_its.h \
                            $(SPM)/svc_attest.h $(SPM)/svc_platform.h \
                            $(SPM)/svc_fwu.h $(SPM)/huk.h check.h | $(BUILD)
	$(CC) $(CFLAGS) -o $@ $(filter %.c,$^)

$(BUILD)/test_spm_heap: test_spm_heap.c $(SPM)/spm_heap.c check.h | $(BUILD)
	$(CC) $(CFLAGS) -o $@ $(filter %.c,$^)

$(BUILD)/test_spm_stack: test_spm_stack.c $(SPM)/spm_stack.c \
                         $(SPM)/spm_stack.h check.h | $(BUILD)
	$(CC) $(CFLAGS) -o $@ $(filter %.c,$^)

$(BUILD)/test_crypto_rpc_client: test_crypto_rpc.c \
                                 $(CLIENT)/$(CLIENT_CFG_H) \
                                 ../../include/psa_ipro/crypto_rpc.h \
                                 check.h | $(BUILD)
	$(CC) $(CFLAGS) $(CLIENT_CFG) -DRPC_CLIENT -o $@ test_crypto_rpc.c

# client libs on a recording psa_call stub (client_stub.c)
$(BUILD)/test_client_marshal: test_client_marshal.c test_client_ids.c \
                              client_stub.c client_stub.h check.h \
                              $(CLIENT_SRCS) $(wildcard $(CLIENT)/*.h) \
                              ../../include/psa_ipro/crypto_rpc.h \
                              ../../include/psa_ipro/its_rpc.h \
                              ../../include/psa_ipro/attest_rpc.h \
                              ../../include/psa_ipro/platform_api.h \
                              ../../include/psa_ipro/fwu_api.h \
                              ../../include/psa_ipro/fwu_rpc.h \
                              | $(BUILD)
	$(CC) $(CFLAGS) $(CLIENT_CFG) -o $@ $(filter %.c,$^)

$(BUILD)/test_crypto_rpc_server: test_crypto_rpc.c \
                                 $(SPE_APP)/$(SPE_CFG_H) \
                                 ../../include/psa_ipro/crypto_rpc.h \
                                 check.h | $(BUILD)
	$(CC) $(CFLAGS) $(SPE_CFG) -DRPC_SERVER -o $@ test_crypto_rpc.c

$(BUILD)/test_svc_crypto: test_svc_crypto.c test_svc_crypto_family.c \
                          test_svc_crypto_api12.c \
                          svc_test.c svc_test.h check.h $(SVC_CRYPTO) \
                          $(SVC_CRYPTO_H) $(MBEDTLS_A) | $(BUILD)
	$(CC) $(CFLAGS) $(SPE_CFG) -o $@ $(filter %.c %.a,$^)

$(BUILD)/test_kdf: test_kdf.c $(SPM)/kdf.c $(SPM)/kdf.h $(SPM)/huk.h \
                   check.h $(MBEDTLS_A) | $(BUILD)
	$(CC) $(CFLAGS) $(SPE_CFG) -o $@ $(filter %.c %.a,$^)

$(BUILD)/test_its_store: test_its_store.c $(ITS_STORE) $(ITS_STORE_H) \
                         check.h $(MBEDTLS_A) | $(BUILD)
	$(CC) $(CFLAGS) $(SPE_CFG) -o $@ $(filter %.c %.a,$^)

$(BUILD)/test_its_meta: test_its_meta.c $(ITS_STORE) $(ITS_STORE_H) \
                        check.h $(MBEDTLS_A) | $(BUILD)
	$(CC) $(CFLAGS) $(SPE_CFG) -o $@ $(filter %.c %.a,$^)

$(BUILD)/test_huk_%: test_huk.c $(SPM)/huk.c $(SPM)/huk.h stub/drv_ef_ctrl.h \
                     $(HAL_SEC_INC)/hal_sec_aes_v2.h check.h $(MBEDTLS_A) | $(BUILD)
	$(CC) $(CFLAGS) $(SPE_CFG) -Istub -I$(HAL_INC) -I$(HAL_SEC_INC) -DCONFIG_PSA_HUK_KEY_SEL=2 \
	    $(HUK_CFG_$*) -DHUK_CFG=$* -o $@ $(filter %.c %.a,$^)

$(BUILD)/test_kdf_huk_%: test_kdf_huk.c $(SPM)/huk.c $(SPM)/huk.h \
                         $(SPM)/kdf.c $(SPM)/kdf.h stub/drv_ef_ctrl.h \
                         $(HAL_SEC_INC)/hal_sec_aes_v2.h check.h $(MBEDTLS_A) | $(BUILD)
	$(CC) $(CFLAGS) $(SPE_CFG) -Istub -I$(HAL_INC) -I$(HAL_SEC_INC) -DCONFIG_PSA_HUK_KEY_SEL=1 \
	    $(HUK_KDF_$*) -o $@ $(filter %.c %.a,$^)

$(BUILD)/test_huklock_%: test_huk_lock.c $(SPM)/huk.c $(SPM)/huk.h \
                         $(SPM)/kdf.c $(SPM)/kdf.h stub/drv_ef_ctrl.h \
                         $(HAL_SEC_INC)/hal_sec_aes_v2.h check.h $(MBEDTLS_A) | $(BUILD)
	$(CC) $(CFLAGS) $(SPE_CFG) -Istub -I$(HAL_INC) -I$(HAL_SEC_INC) -DCONFIG_PSA_HUK_KEY_SEL=1 \
	    -DCONFIG_PSA_DEV_HUK $(HUK_LOCK_$*) -o $@ $(filter %.c %.a,$^)

$(BUILD)/test_huk_prov: $(HUKP_SRCS) $(SPM)/huk_prov.h $(SPM)/huk.h \
                       stub/fake_hukp.h stub/drv_ef_ctrl.h \
                       $(HAL_SEC_INC)/hal_sec_aes_v2.h check.h $(MBEDTLS_A) | $(BUILD)
	$(CC) $(CFLAGS) $(SPE_CFG) $(HUKP_CFG) -o $@ $(filter %.c %.a,$^)

$(BUILD)/test_its_adapter: test_its_adapter.c $(ITS_STORE) $(ITS_STORE_H) \
                           $(ADAPTER) check.h $(MBEDTLS_A) | $(BUILD)
	$(CC) $(CFLAGS) $(SPE_CFG) -o $@ $(filter %.c %.a,$^)

$(BUILD)/test_svc_its: test_svc_its.c $(SPM)/svc_its.c $(SPM)/svc_its.h \
                       $(SPM)/its_lfs.h ../../include/psa_ipro/its_rpc.h \
                       $(ITS_STORE) $(ITS_STORE_H) check.h $(MBEDTLS_A) \
                       | $(BUILD)
	$(CC) $(CFLAGS) $(SPE_CFG) -o $@ $(filter %.c %.a,$^)

$(BUILD)/test_its_lfs: test_its_lfs.c lfs_ram.c lfs_ram.h $(SPM)/its_lfs.c \
                       $(SPM)/its_lfs.h $(SPM)/spm_harden.h \
                       $(SPM)/svc_its.c $(SPM)/svc_its.h \
                       $(SPM)/its_store.c $(SPM)/its_store.h \
                       $(LFS_CONF) check.h $(LFS_OBJS) $(MBEDTLS_A) \
                       | $(BUILD)
	$(CC) $(CFLAGS) $(SPE_CFG) $(LFS_CFG) -o $@ $(filter %.c %.o %.a,$^)

# the SPE littlefs option lfs_port.h reads from generated/autoconf.h
$(LFS_CONF):
	@mkdir -p $(@D)
	@echo '#define CONFIG_LITTLEFS_USE_PARTITION_TABLE 1' > $@

$(BUILD)/lfs/%.o: $(LFS)/littlefs/%.c | $(BUILD)/lfs
	@$(CC) -std=c99 -g -O1 -w -fsanitize=address,undefined \
	    -fno-omit-frame-pointer -I$(LFS)/littlefs -c -o $@ $<

# svc_fwu.c on stub_pt/ flash calls; hal_ptmgr.h for the table types
FWU_CFG := -Istub_pt -I../../../../bsp/hal/inc -I../../../utils/include
$(BUILD)/test_svc_fwu: test_svc_fwu.c fwu_fixture.h $(SPM)/svc_fwu.c \
                       $(SPM)/svc_fwu.h ../../include/psa_ipro/fwu_rpc.h \
                       ../../../utils/src/utils_crc.c $(wildcard stub_pt/*.h) \
                       check.h | $(BUILD)
	$(CC) $(CFLAGS) $(FWU_CFG) -o $@ $(filter %.c,$^)

$(BUILD)/test_eat: test_eat.c $(SPM)/eat.c $(SPM)/eat.h check.h \
                   $(ITS_STORE) $(ITS_STORE_H) $(ADAPTER) \
                   $(CBOR_OBJS) $(MBEDTLS_A) | $(BUILD)
	$(CC) $(CFLAGS) $(SPE_CFG) $(CBOR_CFG) -o $@ $(filter %.c %.o %.a,$^)

$(ATTEST_TESTS): $(BUILD)/%: %.c attest_test.c attest_test.h $(ATTEST) \
                 $(ATTEST_H) $(ITS_STORE) $(ITS_STORE_H) $(ADAPTER) \
                 check.h $(CBOR_OBJS) $(MBEDTLS_A) | $(BUILD)
	$(CC) $(CFLAGS) $(SPE_CFG) $(CBOR_CFG) -o $@ $(filter %.c %.o %.a,$^)

$(MBEDTLS_A): $(MBEDTLS_OBJS)
	@rm -f $@ && ar rcs $@ $^

define LIB_RULE
$$(BUILD)/mbedtls/%.o: $(1)/%.c $$(SPE_APP)/$$(SPE_CFG_H) | $$(BUILD)/mbedtls
	@$$(CC) $$(MBEDTLS_CFLAGS) -c -o $$@ $$<
endef
$(foreach d,$(LIB_DIRS),$(eval $(call LIB_RULE,$(d))))

$(BUILD)/mbedtls/hw_entropy.o: hw_entropy.c hw_poll.h | $(BUILD)/mbedtls
	@$(CC) $(MBEDTLS_CFLAGS) -c -o $@ $<

$(BUILD)/tinycbor/%.o: $(TINYCBOR)/%.c | $(BUILD)/tinycbor
	@$(CC) -std=c99 -g -O1 -w -fsanitize=address,undefined \
	    -fno-omit-frame-pointer -I$(TINYCBOR) -c -o $@ $<

$(BUILD) $(BUILD)/mbedtls $(BUILD)/tinycbor $(BUILD)/lfs:
	mkdir -p $@

clean:
	rm -rf build build3 build4

.PHONY: test clean
