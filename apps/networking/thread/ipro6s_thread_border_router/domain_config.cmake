#======================================================================
# Copyright (c) 2026 IntelPro Technology Co., Ltd.
# All rights reserved.
#
# This software and associated documentation files (the "Software") are
# proprietary and confidential to IntelPro Technology Co., Ltd..
#
# Access to and use of this Software is limited exclusively to authorized 
# customers and partners of IntelPro Technology Co., Ltd. who have entered into a 
# valid license or non-disclosure agreement.
#
# Unauthorized copying, modification, distribution, or reverse engineering 
# of this Software, via any medium, is strictly prohibited without 
# prior written permission from IntelPro Technology Co., Ltd..
#
# SPDX-License-Identifier: Proprietary
#======================================================================

# Deployment-owned /64. Empty leaves prefix policy to the network operator.
set(IPRO_THREAD_BR_DOMAIN_PREFIX "" CACHE STRING "Explicit BR Domain Prefix (/64); empty disables deployment")
if(NOT "${IPRO_THREAD_BR_DOMAIN_PREFIX}" STREQUAL "")
    # Lexical admission only: the OT parser and runtime collision checks perform
    # semantic validation. Restrict characters before emitting a C string macro.
    if(NOT "${IPRO_THREAD_BR_DOMAIN_PREFIX}" MATCHES "^[0-9a-fA-F:]+/64$")
        message(FATAL_ERROR "Invalid Domain Prefix build input")
    endif()
    if(NOT IPRO_MATTER_BR_PACKAGE_ACTIVE AND NOT CONFIG_THREAD_BR_AUTOMATIC_BACKBONE)
        message(FATAL_ERROR "Domain Prefix deployment requires an automatic BR owner")
    endif()
endif()
