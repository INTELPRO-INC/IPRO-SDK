# IPRO6 BTDM Demo Application Entry Point
# ========================================
# This file is the entry point for menuconfig

mainmenu "IPRO6 BTDM+PHYRF Test Configuration"

# Include SDK root Kconfig
source "Kconfig"

# Include application-specific Kconfig
source "apps/platform/ipro6_btdm_phyrf/Kconfig"
