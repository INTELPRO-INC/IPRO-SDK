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


for i in server-client server server-revoked user ocsp; do
    rm -f $i.csr $i.key $i.pem
done

rm -f openssl.cnf.tmp
if [ -d demoCA ]; then
    rm -r demoCA
fi
rm -f ca.pem logo.asn1 logo.der server.der ocsp-server-cache.der
rm -f my-openssl.cnf my-openssl-root.cnf
#rm -r rootCA
