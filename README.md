# Example application to demonstrate Mbed-OS fault handler

This document provide steps required to manually trigger an exception in Mbed-OS and see the exception working and steps to analyze the crash dump.

## Import the example application

From the command-line, import the example:

```
mbed import mbed-os-example-fault-handler
cd mbed-os-example-fault-handler
```

### Now compile

Invoke `mbed compile`, and specify the name of your platform and your favorite toolchain (`GCC_ARM`, `ARM`, `IAR`). For example, for the ARM Compiler 5:

```
mbed compile -m K64F -t ARM
```

Your PC may take a few minutes to compile your code. At the end, you see the following result:

```
[snip]
+----------------------------+-------+-------+------+
| Module                     | .text | .data | .bss |
+----------------------------+-------+-------+------+
| Misc                       | 13939 |    24 | 1372 |
| core/hal                   | 16993 |    96 |  296 |
| core/rtos                  |  7384 |    92 | 4204 |
| features/FEATURE_IPV4      |    80 |     0 |  176 |
| frameworks/greentea-client |  1830 |    60 |   44 |
| frameworks/utest           |  2392 |   512 |  292 |
| Subtotals                  | 42618 |   784 | 6384 |
+----------------------------+-------+-------+------+
Allocated Heap: unknown
Allocated Stack: unknown
Total Static RAM memory (data + bss): 7168 bytes
Total RAM memory (data + bss + heap + stack): 7168 bytes
Total Flash memory (text + data + misc): 43402 bytes
Image: .\.build\K64F\ARM\mbed-os-example-fault-handler.bin
```

### Program your board

1. Connect your mbed device to the computer over USB.
1. Copy the binary file(mbed-os-example-fault-handler.bin) to the mbed device.
1. Press the reset button to start the program.

An exception will be triggered and you will see following crash information in STDOUT.
The crash information contains register context at the time exception and current threads in the system.
The information printed out to STDOUT will be similar to below. Registers captured depends on specific
Cortex-M core you are using. For example, if your target is using Cortex-M0, some registers like 
MMFSR, BFSR, UFSR may not be available and will not appear in the crash log.

++ MbedOS Fault Handler ++

FaultType: HardFault

Context:
R0   : 0000AAA3
R1   : 20002070
R2   : 00009558
R3   : 00412A02
R4   : E000ED14
R5   : 00000000
R6   : 00000000
R7   : 00000000
R8   : 00000000
R9   : 00000000
R10  : 00000000
R11  : 00000000
R12  : 0000BCE5
SP   : 20002070
LR   : 00009E75
PC   : 00009512
xPSR : 01000000
PSP  : 20002008
MSP  : 2002FFD8
CPUID: 410FC241
HFSR : 40000000
MMFSR: 00000000
BFSR : 00000000
UFSR : 00000100
DFSR : 00000008
AFSR : 00000000
SHCSR: 00000000

Thread Info:
Current:
State: 00000002 EntryFn: 0000ADF5 Stack Size: 00001000 Mem: 20001070 SP: 20002030
Next:
State: 00000002 EntryFn: 0000ADF5 Stack Size: 00001000 Mem: 20001070 SP: 20002030
Wait Threads:
State: 00000083 EntryFn: 0000AA1D Stack Size: 00000300 Mem: 20000548 SP: 200007D8
Delay Threads:
Idle Thread:
State: 00000001 EntryFn: 00009F59 Stack Size: 00000200 Mem: 20000348 SP: 20000508

-- MbedOS Fault Handler --

To generate more information copy and save this crash information to a text file and run the crash_log_parser.py tool as below.
NOTE: Make sure you copy the section with text "MbedOS Fault Handler" as the this tool looks for that header.

## Running the Crash Log Parser
crash_log_parser.py <Path to Crash log> <Path to Elf/Axf file of the build> <Path to Map file of the build>
For example:
crashlogparse.py crash.log C:\MyProject\BUILD\k64f\arm\mbed-os-hf-handler.elf C:\MyProject\BUILD\k64f\arm\mbed-os-hf-handler.map

An example output from running crash_log_parser is shown below.

Parsed Crash Info:
        Crash location = zero_div_test() [0000693E]
        Caller location = $Super$$main [00009E99]
        Stack Pointer at the time of crash = [20001CC0]
        Target/Fault Info:
                Processor Arch: ARM-V7M or above
                Processor Variant: C24
                Forced exception, a fault with configurable priority has been escalated to HardFault
                Divide by zero error has occurred

Done parsing...
