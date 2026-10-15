# IPRO7 UVC Camera Application - Top Level Kconfig
#
# Note: Requires srctree to be set to SDK root (handled by Makefile)

mainmenu "IPRO7 UVC Camera Configuration"

# Include SDK-wide configuration
source "bsp/Kconfig"
source "components/Kconfig"

# Application-specific configuration
source "apps/multimedia/ipro7_uvc_cam/Kconfig"
