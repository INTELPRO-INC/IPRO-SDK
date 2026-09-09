# IPRO6 Demo Application - Top Level Kconfig
#
# This file includes both SDK-wide configuration and application-specific configuration.
# When running menuconfig in the application directory, this is the entry point.
#
# Note: Requires srctree to be set to SDK root (handled by Makefile)

mainmenu "IPRO6 Demo Configuration"

# Include SDK-wide configuration (paths relative to srctree = SDK root)
source "bsp/Kconfig"
source "components/Kconfig"

# Application-specific configuration (relative to srctree)
source "apps/platform/ipro6_demo/Kconfig"
