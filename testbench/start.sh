#!/bin/bash

if [ $1 ]; then
    declare test_name=$1
fi

declare testbench_path=~/Programmierung/Aktuelle_Projekte/_not_from_me/vice-emu-code/testprogs/testbench
declare emu64_path=~/Programmierung/Aktuelle_Projekte/qtcreator/emu64/build/install_linux/bin

cd ${testbench_path}
EMUDIR=${emu64_path} ./testbench.sh emu64 ${test_name} --verbose
