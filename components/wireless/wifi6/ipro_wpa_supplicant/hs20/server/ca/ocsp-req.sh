#!/bin/sh
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


for i in *.pem; do
    echo "===[ $i ]==================="
    openssl ocsp -text -CAfile ca.pem -verify_other demoCA/cacert.pem -trust_other -issuer demoCA/cacert.pem -cert $i -url http://localhost:8888/

#    openssl ocsp -text -CAfile rootCA/cacert.pem -issuer demoCA/cacert.pem -cert $i -url http://localhost:8888/

#    openssl ocsp -text -CAfile rootCA/cacert.pem -verify_other demoCA/cacert.pem -trust_other -issuer demoCA/cacert.pem -cert $i -url http://localhost:8888/
#    openssl ocsp -text -CAfile rootCA/cacert.pem -VAfile ca.pem -trust_other -issuer demoCA/cacert.pem -cert $i -url http://localhost:8888/
done
