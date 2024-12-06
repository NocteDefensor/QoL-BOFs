## Cobalt Strike "Where Am I?" Beacon Object File
Cobalt Strike Beacon Object File (BOF) that uses handwritten shellcode to return the process Environment strings without touching any DLL's.

This idea was inspired by Matt Eidelberg's DEF CON 29 talk [Operation Bypass Catch My Payload If You Can](https://youtu.be/JXKNdWUs77w).
+ In this talk, Matt shows how EDR heuristics can detect Cobalt Strike beacons based on their behavior. 
+ Matt uses an example where after the beacon compromises the endpoint, the first thing it does is run the `whoami.exe` local binary.
+ This behavior of the host beacon process spawning a new `whoami.exe` process, triggers the EDR and the beacon is burned!
+ I've been doing allot of Windows Internals studying, and this video made a lightbulb go off. 
+ I thought "Why not just get the `whoami.exe` info from the process? It's already right there in the beacon processes memory!" 

So that's what I did! I created a Beacon Object File that grabs the information we'd want, right there from the beacon process memory! 

Since the goal was to make it ninja/OPSEC safe, I figured why not just do it dynamically with Assembly? About halfway through creation, I bit the bullet and burned the extra time to make it into a blog post as well, so here it all is!

This is the walkthrough blog post on how I created this Cobalt Strike Beacon Object File from start to finish:
+ [Creating the WhereAmI Cobalt Strike BOF](https://0xboku.com/2021/08/19/Bof-WhereAmI.html) 

I discovered that TrustedSec had already created a BOF for this, and of course they did because they are awesome! If you'd like to view their original work you can find it here: [trustedsec/CS-Situational-Awareness-BOF/env](https://github.com/trustedsec/CS-Situational-Awareness-BOF/blob/master/src/SA/env/entry.c)

## Using the WhereAmI BOF from the Cobalt Strike Console
![](/images/beaconLoop.png)

![](/images/greatSuccess.png)

## BOF Flow to get the Environment Variables Dynamically in Memory
Below is the high-level flow & WinDBG commands to map our path from the Thread Environment Block (TEB) to the Environment strings we will ultimately display in our Cobalt Strike interactive beacon console. 

`TEB (GS Register)` --> `PEB` --> `ProcessParameters` --> `Environment Block Address` & `Environment Size`

```bash
# TEB Address
0:000> !teb
TEB at 00000000002ae000
# PEB Address from TEB
0:000> dt !_TEB 2ae000
   +0x060 ProcessEnvironmentBlock : 0x00000000`002ad000 _PEB
# ProcessParamters Address from PEB
0:000> dt !_PEB 2ad000
   +0x020 ProcessParameters : 0x00000000`007423b0 _RTL_USER_PROCESS_PARAMETERS
# Environment Address & Size from ProcessParameters
0:000> dt !_RTL_USER_PROCESS_PARAMETERS 7423b0
   +0x080 Environment      : 0x00000000`00741130 Void
   +0x3f0 EnvironmentSize  : 0x124e
```

## Using WinDBG to Parse the PEB and View Environment Strings
![](/images/bangPeb.png)

+ We can see that `!peb` command parses out the PEB structure and displays to us the Loader (Ldr) information, the address & resolved strings of the ProcessParameters struct, as well as the Environment information we are targeting.

## Compile
```bash
cd ./whereami/
make
```

## References/Resources
+ Matt Eidelberg's DEF CON 29 talk [Operation Bypass Catch My Payload If You Can](https://youtu.be/JXKNdWUs77w)
##### Sektor7 Courses - Conquer Malware Dev (best courses ever)
+ https://institute.sektor7.net/
##### Raphael Mudge - Beacon Object Files - Luser Demo
+ https://www.youtube.com/watch?v=gfYswA_Ronw
##### Cobalt Strike - Beacon Object Files
+ https://www.cobaltstrike.com/help-beacon-object-files
##### BOF Code References
###### trustedsec/CS-Situational-Awareness-BOF
+ https://github.com/trustedsec/CS-Situational-Awareness-BOF
###### anthemtotheego/InlineExecute-Assembly
+ https://github.com/anthemtotheego/InlineExecute-Assembly/blob/main/inlineExecuteAssembly/inlineExecute-Assembly.cna
###### ajpc500/BOFs
+ https://github.com/ajpc500/BOFs/
##### Implementing ASM in C Code with GCC
+ https://outflank.nl/blog/2020/12/26/direct-syscalls-in-beacon-object-files/
+ https://www.cs.uaf.edu/2011/fall/cs301/lecture/10_12_asm_c.html
+ http://gcc.gnu.org/onlinedocs/gcc-4.0.2/gcc/Extended-Asm.html#Extended-Asm
##### Learn Assembly
+ Pentester Academy - [x86_64 Assembly Language and Shellcoding on Linux](https://www.pentesteracademy.com/course?id=7)
+ Offensive Security - [Windows User Mode Exploit Development](https://www.offensive-security.com/exp301-osed/)
 
