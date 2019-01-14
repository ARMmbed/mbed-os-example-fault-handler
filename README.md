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
```
Mbed-OS exception handler test

Forcing exception: 3

++ MbedOS Fault Handler ++

FaultType: HardFault

Context:
R0   : 0000AAA3
R1   : 00000208
R2   : 00004B8C
R3   : 00004D69
R4   : 20000840
R5   : 00000003
R6   : 00000000
R7   : 00000000
R8   : 00000000
R9   : 00000000
R10  : 00000000
R11  : 00000000
R12  : 000084A9
SP   : 20001F10
LR   : 00002C59
PC   : 00004B4E
xPSR : 81000000
PSP  : 20001EA8
MSP  : 2002FFD8
CPUID: 410FC241
HFSR : 40000000
MMFSR: 00000000
BFSR : 00000000
UFSR : 00000100
DFSR : 00000008
AFSR : 00000000
Mode : Thread
Priv : Privileged
Stack: PSP

-- MbedOS Fault Handler --


++ MbedOS Error Info ++
Error Status: 0x80FF013D Code: 317 Module: 255
Error Message: Fault exception
Location: 0x5DEB
Error Value: 0x4B4E
Current Thread: main  Id: 0x20001F28 Entry: 0x6321 StackSize: 0x1000 StackMem: 0x20000F28 SP: 0x2002FF70
For more info, visit: https://armmbed.github.io/mbedos-error/?error=0x80FF013D
-- MbedOS Error Info -
== The system has been rebooted due to a fatal error. ==

Rebooted and Halted...test completed
```
To investigate more on this hardfault copy and save this crash information to a text file and run the crash_log_parser.py tool as below.

## Running the Crash Log Parser
crash_log_parser.py <Path to Crash log> <Path to Elf/Axf file of the build> <Path to Map file of the build>
For example:
crashlogparser.py crash.log C:\MyProject\BUILD\k64f\arm\mbed-os-hf-handler.elf C:\MyProject\BUILD\k64f\arm\mbed-os-hf-handler.map

An example output from running crash_log_parser is shown below.

```
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
```

## Finding the exact instruction where the application crashed

You can find the exact instruction that crashed the board by looking at the debug symbols in the `elf` file. For example, here is an error that was thrown after an unaligned read (`generate_bus_fault_unaligned_access()` function in `main.cpp`):

```
Crash Info:
        Crash location = generate_bus_fault_unaligned_access() [0x000017E8] (based on PC value)
        Caller location = exception_generator(_ExceptType) [0x000018A9] (based on LR value)
        Stack Pointer at the time of crash = [20002E88]
        Target and Fault Info:
                Processor Arch: ARM-V7M or above
                Processor Variant: C24
                Forced exception, a fault with configurable priority has been escalated to HardFault
                Unaligned access error has occurred
```

To see the location, extract the symbols from the `elf` file:

```
$ arm-none-eabi-objdump -S BUILD/K64F/GCC_ARM/mbed-os-example-fault-handler.elf > symbols.txt
```

This gives a human-readable text file. Load symbols.txt in a text editor, and look for the last four digits of the crash location in lower case, with a `:` at the end.

```
000017d8 <_Z35generate_bus_fault_unaligned_accessv>:
    SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
    17d8:	4a05      	ldr	r2, [pc, #20]	; (17f0 <_Z35generate_bus_fault_unaligned_accessv+0x18>)
    printf("\nval= %X", val);
    17da:	4806      	ldr	r0, [pc, #24]	; (17f4 <_Z35generate_bus_fault_unaligned_accessv+0x1c>)
    SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
    17dc:	6953      	ldr	r3, [r2, #20]
    17de:	f043 0308 	orr.w	r3, r3, #8
    17e2:	6153      	str	r3, [r2, #20]
    printf("\nval= %X", val);
    17e4:	f64a 23a3 	movw	r3, #43683	; 0xaaa3
    17e8:	6819      	ldr	r1, [r3, #0]                            <---- FAULT LOCATION
    17ea:	f005 bb11 	b.w	6e10 <printf>
    17ee:	bf00      	nop
    17f0:	e000ed00 	.word	0xe000ed00
```

As you can see the error happened when the load register (`ldr`) call was made with the value of register 3 (`r3`). As you also have access to the register values in the initial crash report (on the board) we can also see exactly which address we were trying to read from:

```
FaultType: HardFault

Context:
R0   : 0000C772
R1   : 00000000
R2   : E000ED00
R3   : 0000AAA3                 <----- This address
R4   : 00000003
R5   : 00000000
```

Which matches with the address we're trying to read in `generate_bus_fault_unaligned_access`.
