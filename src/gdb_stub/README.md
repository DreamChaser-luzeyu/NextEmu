# GDB Stub 

### Frequently Used Commands
```bash
# Show GDB RSP Logs
set debug remote 1
# Connect to remote target 127.0.0.1:5678
target remote 127.0.0.1:5678
# Specify symbol file
add-symbol-file path/to/file [text section begin addr]
# Show source
list
# Show all registers
info registers
# Show register pc
info registers pc
# Show assembly
disp/10i $pc
# Set pc to $pc+4
set var $pc=$pc+4
# Set sizeof(int) mem[$pc] to 0
set {int}$pc=0
# Set sizeof(long long) mem[$pc] to 0
set {long long}$pc=0
# Show value(g means in 8 bytes) of address 0x80000000
x/xg 0x80000000
# Insert breakpoint at line 10 (Run `list` to show source line num)
b 10
# Show 10 assembly instructions at symbol sbi_init
x/10i sbi_init
```