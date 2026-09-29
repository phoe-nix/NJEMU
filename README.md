NJEmu 2.3.5 is based on NJEmu 2.3.1.


----------------------------------------------

how to use:


O: OK

X: Cancel

Help: select

HOME(PS): emulator menu

On PSV or PPSSPP,HOME(PS) button can't use,you can delete "SystemButtons.prx"

SELECT+START: emulator menu

----------------------------------------------


CPS1PSP

Capcom System 1 Emulator



CPS2PSP

Capcom System 2 Emulator



MVSPSP

Neo-Geo MVS/AES Emulator



NCDZPSP

neogeo Emulator


---------------------------------------------

NJEmu 2.3.5 change log


romset use MAME0.152

font use simhei(CHARSET:GBK)

Japanese command list must use GBK charset,you can use notepad++ convert

fix png format bug

change help to select

change bios menu to R trigger

support multi language

added command hotkey

gamelist to 512

support cheat

added some romset(hack/bootleg)

---------------------------------------------

CPS1PSP

fix dipswitch

fix Mercs can't use player3

added some hack roms button3

fix Warriors of Fate (bootleg)

fix Huo Feng Huang(Chinese bootleg of Sangokushi II) sound

---------------------------------------------

CPS2PSP

---------------------------------------------

MVSPSP

fix dip menu

fix Jockey Grand Prix

fix King of Gladiator (KOF'97 bootleg)

support 128MB crom cache

support unibios 1.0-3.0 and neogit bios

support m1 decrypt

fix 000-lo.lo length

---------------------------------------------

NCDZPSP

fix 000-lo.lo length(fix sleep mode)

---------------------------------------------

Thanks to

cheat code: davex

hbl/multi language support: 173210

mame team

and NJ's emulator

----------------------------------------------

Building:

Needs the current PSPSDK toolchain (https://github.com/pspdev/pspdev), with psp-config on PATH.
Pick the emulator with BUILD_CPS1PSP, BUILD_CPS2PSP, BUILD_MVSPSP or BUILD_NCDZPSP, e.g.

    make BUILD_CPS2PSP= BUILD_MVSPSP=1

The EBOOT.PBP ends up in SLIM/MVS/ (3XX/ for PSP-1000 builds). Copy it together with the files
from data/ (SystemButtons.prx, zipname and rominfo) to a folder under ms0:/PSP/GAME/.
SystemButtons.prx is built separately, with make in systembuttons_prx/.
