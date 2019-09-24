# Specify remote target
target extended-remote :3333

file build/a.out

# Reset to known state
monitor reset halt
load
monitor reset init

# Set a breakpoint at main().
break main

# Run to the breakpoint.
continue