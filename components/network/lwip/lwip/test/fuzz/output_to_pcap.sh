#!/bin/bash
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


if [ -z "$1" ]
then
	echo "This script will make pcap files from the afl-fuzz crash/hang files"
	echo "It needs hexdump and text2pcap"
	echo "Please give output directory as argument"
	exit 2
fi

for i in `ls $1/crashes/id*`
do
	PCAPNAME=`echo $i | grep pcap`
	if [ -z "$PCAPNAME" ]; then
		hexdump -C $i > $1/$$.tmp
		text2pcap $1/$$.tmp ${i}.pcap
	fi
done
for i in `ls $1/hangs/id*`
do
	PCAPNAME=`echo $i | grep pcap`
	if [ -z "$PCAPNAME" ]; then
		hexdump -C $i > $1/$$.tmp
		text2pcap $1/$$.tmp ${i}.pcap
	fi
done
rm -f $1/$$.tmp

echo
echo "Created pcap files:"
ls $1/*/*.pcap
