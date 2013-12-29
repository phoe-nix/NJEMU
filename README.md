20131229  
CPS2 ROMSET同步至MAME151  

20131227  
CPS1 ROMSET同步至MAME152  

20131223  
CPS  
添加sfz系列的热键1+2+3和4+5+6  
MVS  
调整KOF2001仿PS2版的ROMINFO

20131220  
MVS  
修正rominfo  
更新转换工具，转换过程无报错  
修正ZIPNAME  
添加KOF97天下布武085、KOF96SP、更新KOF96AE2.0  
neogeo的ROMS已同步至MAME150  

20131207  
(测试，百度网盘暂时未提供对应MAME150的ROM)  
MVS更新neocrypt.c，同步MAME150和PLUS的ROM版本 

20131204	
修复neo git BIOS按键  
修复command缩减问题 
优化字库	

20131203	
修复帮助菜单错乱	
添加BIOS帮助菜单	
修复菜单音量刷新	
保存金手指选择光标	
修复颜色菜单管理亮度	
修复KOF2003PCB	
添加KOF2003 仿PS2版	
优化neocrypt.c	

20131130	
MVS	
添加GAME_HACK文字颜色	
添加ROM支持:	
sthoopcd,街头篮球 (Neo CD 转换),GAME_HACK	
lasthpcd,最后的希望 (Neo CD转换),GAME_HACK	

20131125	
MVS	
fr2ch 偶像麻将CD转换版同步至MAME，修复后期关卡花屏问题

20131120	
CPS	
修复热键无法保存的问题

MVS	
更新neocrypt.c，同步MAME的源码(M1解密代码未移植成功)	
更新转换工具，去除多余转换的ROM	
添加ROM支持:	
kof96cn,拳皇 '96 (中文版)	
kof97cn,拳皇 '97 (中文版)	
kof97oro,拳皇 '97 Plus 2003 (盗版)	
kof97pla,拳皇 '97 Plus 2003 (盗版/hack) 更名	
kog,拳皇 '97 KOG (盗版) 解锁，修复死机	
kof99ae,拳皇 '99 Anniversary Edition 公测版20130113  
mslugxc1,合金弹头X 多载具版  

20131005
修复缩减command死机

20130919	
CPS1修复画面闪屏	
MVS调整中文游戏列表，wh2jh改为wh2j	

20130916	
CPS1内置dinohunt补丁，可直接运行原版ROM	

20130915	
翻译转换工具	
MVS更新转换工具，避免重复转换添加的子ROM	
kof99n改为kof99k	

20130913	
MVS添加KOF97仿PS版，更新转换工具	
MVS添加并支持KOF98AE，感谢OrochiZ改的代码	

20130912	
MVS添加KOF2000和KOF2002的仿PS2版	
MVS支持128M的CROM，可正常模拟KOF2002仿PS2版	
MVS修复KOF2000n无法使用家用机模式	

20130910	
MVS添加BAN BAN BUSTER和Treasure of the Caribbean	

20130908	
MVS修复亚版家用机BIOS花屏，添加X键从BIOS选择菜单返回游戏列表	
MVS修复按键设置丢失问题	
MVS更改分辨率	
菜单优化	

20130831	
MVS修复jockeygp,赌马大亨无法运行的问题	
MVS添加几个子ROM，屏蔽neocup98,ssideki3,kog	

20130829	
删除主菜单HOME键退出功能	
优化主菜单刷新显示	

20130828	
修正主菜单无法刷新菜单文本的BUG	
更新源码地址，代码整理后再上传	
MVS重读ROM时自动关闭金手指，避免无法正常进入游戏	
MVS修正000-lo.lo的读取长度	
ncdz修正待机后死机问题	
ncdz修正000-lo.lo的CRC识别问题	

20130817	
MVS补全BIOS，正确命名BIOS	
修正更改街机和家用机模式无法重读ROM的BUG	

20130815	
翻译CPS1和MVS中DIP(固定简体中文)	
CPS1和MVS更改DIP开关后无需重读ROM	
CPS1修复willow的DIP开关死机BUG	
CPS1补全msword的生命DIP开关	
CPS1修复失落世界无法控制2P的BUG	
NEOGEO同步MAME的BIOS	
NEOGEO修正投币槽数量的BUG	

20130813	
CPS1修复DIP开关中投币孔和PLAY MODE的BUG	

20130812添加主菜单按HOME退出的功能	
CPS1修正部分游戏可选到玩家4的BUG	
CPS1修正mercs无法使用玩家3的BUG	
CPS1添加三国2和恐龙HACK版的3键	
CPS1添加dinohunt	
MVS添加ironclad和部分游戏的子ROM，详细见ZIPNAMECH1.MVS(PSP语言选择简体中文)	

20130728	
优化金手指功能，删除剩余内存显示功能	
添加CPS1三国志2中文版，双截龙和KOF2000的CRC修正	

20130420	
添加CPS1和CPS2的热键功能	

20130416	
CPS1中，添加L+R+START=同时按P1和P2的START的功能	
修正CPS1中DIP SWITCH的Enemy's attack frequency更改后无法生效的BUG	

20130411	
加入DAVEX的金手指代码(联机无效)，格式为FBA用的CHEAT格式	

20130331	
按键设置中添加command list快捷键，取消28号的快捷键R+SELECT	

20130328	
添加CPS和MVS中操作说明“command list”快捷键：R+SELECT，方便查看。NCDZ已有。	
添加“返回游戏目录”确认提示，避免误操	
看了源代码发现有快捷键	
（L+R+SELECT=服务开关，可以调难度；L+R+START=1P+2P的START）	

20130327	
MVS更新：ZIP名字识别长度支持128个字符，方便自己添加HACK ROM	

20121227
MVS更新UNIBIOS 3.0	
添加多种分辨率	
支持简体中文、繁体中文、日文、英文菜单	

字库使用黑体制作，使用GBK编码，command和ZIPNAME中的编码必须用GBK(ansi)

----Licence----
Codes added by phoe-nix is licenced under GPLv3.
