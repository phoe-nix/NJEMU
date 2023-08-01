----------------------------------------------------------------------------------------

※关于编译源码

编译环境PSPSDK 0.11.2 + MSYS。

※此源代码解压后，需要编辑Makefile才能正常编译。

编译前打开Makefile编辑各个项目。
设置编译目标，开头为#表示编译无效，删除#表示有效
详细见下面。


·编译设置

BUILD_CPS1 = 1      编译CPS1PSP。

BUILD_CPS2 = 1      编译CPS2PSP。

BUILD_MVS = 1       编译MVSPSP。

BUILD_NCDZ = 1      编译NCDZPSP。

LARGE_MEMORY = 1           编译PSP-2000 + CFW3.71 M33以上系统的用户模式。

KERNEL_MODE = 1        编译FW1.5核心。

ADHOC = 1              启用AdHoc联机对战功能。(NCDZPSP无效)

SAVE_STATE = 1         启用即时存档/读档功能。

COMMAND_LIST = 1       启用操作指令(command)。

UI_32BPP = 1           使用用户界面的32位颜色。可以使用壁纸，可能会出现内存不足现象。

RELEASE = 1            REREASE = 1为有效，RELEASE = 0为无效。
                       使用代码中包含#if RELEASE ～ #endif。
                       无效情况表示可以运行盗版游戏。

·版本的设置

VERSION_MAJOR = 2      大规模更新时的版本号。

VERSION_MINOR = 2      小规模更新时的版本号。
                       还有，偶数为稳定版，奇数为开发版。
                       因此ver.1.0的下个发布版本号为ver.1.2。

VERSION_BUILD = 0      小BUG修正做成的版本号。

----------------------------------------------------------------------------------------

※关于SystemButtons.prx

  核心模式下通过执行PRX读取系统按键专用的线程。
  编译方法是移动systembutton_prx目录make。

  最后，编译结束时，复制systembutton.prx到EBOOT所在的目录。

----------------------------------------------------------------------------------------
