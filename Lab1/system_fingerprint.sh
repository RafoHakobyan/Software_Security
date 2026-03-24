#!/bin/bash

echo "System Fingerprint"
echo "--------"

cpu=$(lscpu | grep 'Model name')

mac=$(ip link | grep ether )

echo "${cpu}${mac}" | sha256sum

