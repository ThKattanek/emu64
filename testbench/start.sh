#!/bin/bash

if [ $1 ]; then
    declare test_name=$1
fi

declare testbench_path=/home/thorsten/Programmierung/Aktuelle_Projekte/vice-emu-code/testprogs/testbench
declare emu64_path=/home/thorsten/Programmierung/Aktuelle_Projekte/emu64/build/install_linux/bin

cd ${testbench_path}
EMUDIR=${emu64_path} ./testbench.sh emu64 ${test_name}
