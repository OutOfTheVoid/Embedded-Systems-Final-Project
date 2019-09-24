openocd -f ../tools/openocd/tcl/board/ek-tm4c123gxl.cfg -s ../tools/openocd/tcl &
arm-none-eabi-gdb --command=debug_init.gdb
killall openocd
