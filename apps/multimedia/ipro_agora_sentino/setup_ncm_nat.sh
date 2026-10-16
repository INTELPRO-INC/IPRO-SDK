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

# Setup NCM NAT for IPRO7 device
# Run this AFTER device boots and en16 becomes active

IFACE="en16"
GW_IP="192.168.137.1"
NET="192.168.137.0/24"
WAN="en0"  # WiFi interface for internet

echo "Waiting for $IFACE to become active..."
for i in $(seq 1 30); do
    STATUS=$(ifconfig $IFACE 2>/dev/null | grep "status:" | awk '{print $2}')
    if [ "$STATUS" = "active" ]; then
        echo "$IFACE is active!"
        break
    fi
    sleep 1
done

if [ "$STATUS" != "active" ]; then
    echo "ERROR: $IFACE not active after 30s"
    exit 1
fi

echo "Setting $IFACE IP to $GW_IP..."
sudo ifconfig $IFACE $GW_IP netmask 255.255.255.0 up

echo "Enabling IP forwarding..."
sudo sysctl -w net.inet.ip.forwarding=1

echo "Setting up NAT on $WAN..."
echo "nat on $WAN from $NET to any -> ($WAN)" | sudo pfctl -f - -e 2>/dev/null

echo ""
echo "Done! Verify:"
ifconfig $IFACE | grep "inet "
sudo pfctl -s nat 2>/dev/null
echo ""
echo "Device should get static IP 192.168.137.2 after DHCP timeout"
