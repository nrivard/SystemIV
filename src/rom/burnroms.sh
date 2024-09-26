#!/bin/sh

read -p "Place ODD ROM in your programmer and press ENTER key to continue"
minipro --device "AM29F040@PLCC32" -s --write SixtyN8k.odd.rom
read -p "Place EVEN ROM in your programmer and press ENTER key to continue"
minipro --device "AM29F040@PLCC32" -s --write SixtyN8k.even.rom
