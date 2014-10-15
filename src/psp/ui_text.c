/******************************************************************************

	ui_text.c

	��`�����󥿥ե��`���ƥ����ȹ���

******************************************************************************/

#include "psp.h"
#include <psputility_sysparam.h>


const char *ui_text[UI_TEXT_MAX];

static const char *text_ENGLISH[UI_TEXT_MAX] =
{
		"\0",
		"\n",

		/* psp/filer.c */
		"Please wait...",
		"Could not open zipname.%s",
#ifdef ADHOC
		"Please turn on the WLAN switch.",
		"Failed to load AdHoc modules.",
#endif

		/* psp/sound.c */
		"Could not reserve audio channel for sound.",
		"Could not start sound thread.",

		/* psp/ui.c */
		"Warning: Battery is low (%d%%). Please charge battery!",

		FONT_CIRCLE " to launch, " FONT_CROSS " to cancel",
		FONT_CIRCLE " to confirm, " FONT_CROSS " to cancel",
#ifdef SAVE_STATE
		FONT_CIRCLE " return to game, " FONT_CROSS " return to menu",
#endif
		"Press any button.",
		"Start emulation.",
#ifdef ADHOC
		"Start emulation. (AdHoc)",
#endif
		"Exit emulation.",
		"Reset emulation.",
		"Need to restart emulation.",
#if (EMU_SYSTEM != NCDZ)
		"THIS GAME DOESN'T WORK.",
		"You won't be able to make it work correctly.",
		"Don't bother.",
#endif
		"Do you make this directory the startup directory?",
#ifdef PSP_SLIM
		"This program requires PSP-2000 + FW 3.71 M33 or later.",
#endif
#ifdef SAVE_STATE
		"Start save state.",
		"Start load state.",
#endif
#if defined(SAVE_STATE) || defined(COMMAND_LIST)
		"Complete.",
#endif
#ifdef SAVE_STATE
		"Delete state file.",
#endif
#if (EMU_SYSTEM == NCDZ)
		"MP3 files not found. CDDA cannot play.",
		"IPL.TXT not found.",
		"Boot NEO��GEO CDZ BIOS.",
		"NEO��GEO CDZ BIOS (neocd.bin) not found.",
		"Invalid NEO��GEO CDZ BIOS found.",
		"Cannot launch game.",
#endif

		"Help - %s",
		"Not use",
		"Press any button to return to menu.",
		"File browser",
		"Scroll",
		"Scroll 1 page",
		"Launch game",
		"Launch game (AdHoc)",
		"Exit emulator",
		"Show BIOS select menu",
		"Open color settings menu",
		"Show this help",
		"Boot BIOS",
		"Set selected directory as startup",
		"Change value",
		"Select",
		"Select slot",
		"Change function",
		"Execute function",
		"Select item",
		"Select item / Change value",
		"RGB value +5",
		"Return to file browser",
		"RGB value -5",
		"Restore all values to default",
		"Scroll / Select item",
		"Open / Close item menu",
		"Return to main menu / game",

		/* psp/ui_menu.c */
		"On",
		"Off",
		"Yes",
		"No",
		"Enable",
		"Disable",
		"Return to main menu",

		"Game configuration menu",
		"Raster Effects",
		"Stretch Screen",
#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
		"360x270 (4:3)",
		"384x270 (24:17)",
		"466x272 (12:7)",
		"480x270 (16:9)",
		"Rotate Screen",
#else
		"320x224 (4:3)",
		"360x270 (4:3)",
		"366x270 (19:14)",
		"420x270 (14:9)",
		"480x270 (16:9)",
#endif
		"Video Sync",
		"Auto Frameskip",
		"Frameskip",
		"level 1",
		"level 2",
		"level 3",
		"level 4",
		"level 5",
		"level 6",
		"level 7",
		"level 8",
		"level 9",
		"level 10",
		"level 11",
		"Show FPS",
		"60fps Limit",
		"Enable Sound",
		"Sample Rate",
		"11025Hz",
		"22050Hz",
		"44100Hz",
		"Sound Volume",
		"0%",
		"10%",
		"20%",
		"30%",
		"40%",
		"50%",
		"60%",
		"70%",
		"80%",
		"90%",
		"100%",
		"Controller",
		"Player 1",
		"Player 2",
#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
		"Player 3",
		"Player 4",
#endif
		"PSP clock",
		"222MHz",
		"266MHz",
		"300MHz",
		"333MHz",
#if (EMU_SYSTEM == MVS)
		"Default",
#endif
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"Machine Region",
		"Japan",
		"USA",
		"Europe",
#endif
#if (EMU_SYSTEM == MVS)
		"Machine Mode",
		"Console (AES)",
		"Arcade (MVS)",
#endif
#if (EMU_SYSTEM == NCDZ)
		"Emulate Load Screen",
		"CD-ROM Speed Limit",
		"Enable CDDA",
		"CDDA Volume",
#endif

		"Key configuration menu",
		"Not use",
		FONT_UPARROW,
		FONT_DOWNARROW,
		FONT_LEFTARROW,
		FONT_RIGHTARROW,
		FONT_CIRCLE,
		FONT_CROSS,
		FONT_SQUARE,
		FONT_TRIANGLE,
		"L TRIGGER",
		"R TRIGGER",
		"START",
		"SELECT",
		"Low",
		"Normal",
		"High",
#if (EMU_SYSTEM == CPS2)
		"Player1 Start",
		"Player2 Start",
#endif
		"%d frame",
		"%d frames",
		FONT_UPARROW,
		FONT_DOWNARROW,
		FONT_LEFTARROW,
		FONT_RIGHTARROW,
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"Button A",
		"Button B",
		"Button C",
		"Button D",
#else
		"Button 1",
		"Button 2",
		"Button 3",
		"Button 4",
		"Button 5",
		"Button 6",
#endif
		"Start",
#if (EMU_SYSTEM == NCDZ)
		"Select",
#else
		"Coin",
		"Service Coin",
#endif
#if (EMU_SYSTEM == MVS)
		"Test Switch",
#elif (EMU_SYSTEM != NCDZ)
		"Service Switch",
#endif
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"Autofire A",
		"Autofire B",
		"Autofire C",
		"Autofire D",
#else
		"Autofire 1",
		"Autofire 2",
		"Autofire 3",
		"Autofire 4",
		"Autofire 5",
		"Autofire 6",
#endif
		"Autofire Interval",
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"Hotkey A+B",
		"Hotkey A+C",
		"Hotkey A+D",
		"Hotkey B+C",
		"Hotkey B+D",
		"Hotkey C+D",
		"Hotkey A+B+C",
		"Hotkey A+B+D",
		"Hotkey A+C+D",
		"Hotkey B+C+D",
		"Hotkey A+B+C+D",
#else
		"Hotkey 1+2",
		"Hotkey 1+3",
		"Hotkey 1+4",
		"Hotkey 2+3",
		"Hotkey 2+4",
		"Hotkey 3+4",
		"Hotkey 1+2+3",
		"Hotkey 1+2+4",
		"Hotkey 1+3+4",
		"Hotkey 2+3+4",
		"Hotkey 1+2+3+4",
		"Hotkey 4+5+6",
#endif
#if (EMU_SYSTEM != NCDZ)
		"Analog Sensitivity",
#endif
#if (EMU_SYSTEM == CPS1)
		"Dial (Left)",
		"Dial (Right)",
		"Pause",
#endif
#if (EMU_SYSTEM == CPS2)
		"Paddle (Left)",
		"Paddle (Right)",
#endif
#if (EMU_SYSTEM == MVS)
		"Big",
		"Small",
		"Double Up",
		"Start/Collect",
		"Payout",
		"Cancel",
		"Bet",
		"Bet/Cancel All",
		"Operator Menu",
		"Clear Credit",
		"Hopper Out",
#endif
		"Save Screenshot",
		"Switch Player",
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"Button Layout",
		"Type 1 (NEOGEO PAD)",
		"Type 2 (MVS)",
		"Type 3 (PlayStation)",
		"User Define",
#endif

		"DIP switch settings menu",
		"This game has no DIP switches.",

		"Save/Load State",
		"Slot %d:",
		"Empty",
		"Load",
		"Delete",
		"Save",
		"Play Date",
		"Save Time",
		"Version",
		"Faild to delete file \"%s\".",

		"Main menu",
		"Game configuration",
		"Change game settings.",
		"Key configuration",
		"Change buttons and autofire/hotkey settings.",
#if (EMU_SYSTEM != NCDZ)
		"DIP switch settings",
		"Change hardware DIP switch settings.",
#endif
		"Reset emulation",
		"Reset " SYSTEM_NAME " emulation.",
		"Return to file browser",
		"Stop emulation and return to file browser.",
		"Return to game",
		"Return to game emulation.",
		"Exit emulator",
		"Exit emulation and return to PSP menu.",
#ifdef COMMAND_LIST
		"Show command list",
		"Show command list. (MAME Plus! format \"command.dat\")",
#endif

#if PSP_VIDEO_32BPP
		"Color settings menu",
		"Background image type",
		"Background image blightness",
		"Title bar text",
		"Selected text",
		"Normal text",
		"Information message text",
		"Warning message text",
		"File select bar (start)",
		"File select bar (end)",
		"Title bar/Message box",
		"Title bar/ Message box frame",
		"Background",
		"Red",
		"Green",
		"Blue",
		"Default image",
		"User's image",
		"logo.png only",
		"File select bar",
#endif

		"Command list",

		/* psp/mp3.c */
#if (EMU_SYSTEM == NCDZ)
		"Counld not reopen mp3 file \"%s\".",
		"MP3 decode error.",
		"Could not reserve audio channel for MP3.",
		"Could not start MP3 thread.",
#endif

		/* psp/adhoc.c*/
#ifdef ADHOC
		"lobby",
		"server",
		"crient",
		"Waiting for another PSP to join.\n",
		"Connecting to the %s.",
		"Connected.",
		"Disconnecting from the %s.",
		"Disconnected.",
		"failed.",
		"Select a server to connect to, or " FONT_TRIANGLE " to return.\n",
		"Waiting for %s to accept the connection.\n",
		"To cancel press " FONT_CROSS ".\n",
		"%s has requested a connection.\n",
		"To accept the connection press " FONT_CIRCLE ", to cancel press " FONT_CROSS ".\n",
		"Wainting for synchronization.",
#endif

		/* psp/png.c */
		"Could not allocate memory for PNG.",
		"Could not enecode PNG image.",
#if PSP_VIDEO_32BPP || (EMU_SYSTEM == NCDZ)
		"Could not decode PNG image.",
#endif
		"%d bit color PNG image not supported.",

		/* emumain.c */
		"Fatal error",
		"Snapshot saved as \"%s_%02d.png\".",

#if USE_CACHE
		/* common/cache.c */
		"Could not open cache file.\n",
		"Memory not enough.\n",
		"Could not allocate cache memory.\n",
		"%dKB cache allocated.\n",
		"Cache load error!!!\n",
		"Loading cache information data...\n",
		"Unsupported version of cache file \"V%c%c\".\n",
		"Current required version is \"" CACHE_VERSION "\".\n",
		"Please rebuild cache file.\n",
#if (EMU_SYSTEM == CPS2)
		"Could not open sprite block %03x\n",
#elif (EMU_SYSTEM == MVS)
		"PCM cache enabled.\n",
#endif
#endif

		/* common/loadrom.c */
		"Press any button.\n",
		"Could not allocate %s memory.\n",
		"CRC32 not correct. \"%s\"\n",
		"File not found. \"%s\"\n",

#ifdef SAVE_STATE
		/* common/state.c */
		"Could not allocate state buffer.",
		"Could not create file \"%s.sv%d\"",
		"Could not open file \"%s.sv%d\"",
		"Saving \"%s.sv%d\"",
		"Loading \"%s.sv%d\"",
#if (EMU_SYSTEM == MVS)
		"Could not reload BIOS. Press any button to exit.",
#elif (EMU_SYSTEM == NCDZ)
		"Could not compress state data.",
		"Could not uncompress state data.",
#endif
#endif

#ifdef COMMAND_LIST
		/* common/cmdlist.c */
		"Command list for this game not found.",
		"Command list - %s",
		"%d/%d items",
		"COMMAND.DAT size reduction",
		"This processing removes the command list of the games not being\n",
		"supported by this emulator from COMMAND.DAT.\n",
		"If you want to reduce the size of command.dat, press " FONT_CIRCLE " button.\n",
		"Otherwise, press " FONT_CROSS " button to return to file browser.\n",
		"Checking COMMAND.DAT format...\n",
		"ERROR: Unknown format.\n",
		"ERROR: Empty file.\n",
		"ERROR: Could not allocate memory.\n",
		"ERROR: Could not rename file.\n",
		"ERROR: Could create output file.\n",
		"Copying \"%s\"...\n",
		"Original size:%dbytes, Result:%dbytes (-%.1f%%)\n",
#endif

		/* emulation core */
		"Done.\n",
		"Exit emulation",
		"Please wait.\n",

		/* inptport.c */
		"Controller: Player 1",
		"Controller: Player %d",
#ifdef ADHOC
		"Lost sync.\n",
		"Paused by %s",
		"Return to Game",
		"Disconnect",
#endif

		/* memintrf.c */
		"Loading \"%s\"\n",
		"Load ROM",
#if (EMU_SYSTEM != NCDZ)
		"Checking ROM info...\n",
		"This game not supported.\n",
		"ROM not found. (zip file name incorrect)\n",
		"Driver for \"%s\" not found.\n",
		"ROM set \"%s\" (parent: %s)\n",
		"ROM set \"%s\"\n",
#endif

#if (EMU_SYSTEM == CPS1)

		/* memintrf.c */
		"rominfo.cps1 not found.\n",
		"Could not allocate memory. (0x8000 bytes)",

#elif (EMU_SYSTEM == CPS2)

		/* cps2crpt.c */
		"Decrypting %d%%\r",
		"Decrypting 100%%\n",

		/* memintrf.c */
		"rominfo.cps2 not found.\n",

#ifdef PSP_SLIM
		/* vidhrdw.c */
		"Decoding GFX...\n",
#endif

#elif (EMU_SYSTEM == MVS)

		/* biosmenu.c */
		"BIOS select menu",
		"BIOS not found.",
		"All NVRAM files are removed.\n",

		/* memintrf.c */
		"rominfo.mvs not found.\n",
		"Loading \"%s (%s)\"\n",
		"Decrypting ROM...\n",
		"Loading decrypted GFX2 ROM...\n",
		"Loading decrypted SOUND1 ROM...\n",
		"Could not allocate memory for decrypt ROM.\n",
		"Could not allocate memory for sprite data.\n",
		"Try to use sprite cache...\n",
		"Checking BIOS...\n",

#elif (EMU_SYSTEM == NCDZ)

		/* ncdz.c */
		"Error while processing IPL.TXT.",
		"Reset caused by watchdog counter.",

		/* cdrom.c */
		"Could not open file \"%s\".",
		"Could not insert CD-ROM state data.",

		/* vidhrdw.c */
		"CD-ROM speed limit: Off",
		"CD-ROM speed limit: On",

		/* memintrf.c */
		"Checking game ID...\n",
#endif
		"Cheat menu",
		"Cheat",
		"Select cheat",
		"Cheat for this game not found.",
/*
		"Memory free",
		"Mem free",*/
		NULL
};
static const char *text_JAPANESE[UI_TEXT_MAX] =
{
		"\0",
		"\n",

		/* psp/filer.c */
		"���Ф餯����������������",
		"zipname.%s�򥪩`�ץ�Ǥ��ޤ���",
#ifdef ADHOC
		"WLAN�����å���ON�ˤ��Ƥ���������",
		"AdHoc�⥸��`����`�ɳ����ޤ���Ǥ�����",
#endif

		/* psp/sound.c */
		"���`�ǥ�������ͥ��_���Ǥ��ޤ���",
		"������ɥ���åɤ��_ʼ�Ǥ��ޤ���",

		/* psp/ui.c */
		"����: �Хåƥ�`�β�����������ޤ���(%d%%)����늤��Ƥ���������",

		FONT_CIRCLE " ����      " FONT_CROSS " ��ֹ",
		FONT_CIRCLE " �A��      " FONT_CROSS " ��ֹ",
#ifdef SAVE_STATE
		FONT_CIRCLE " ���`��ˑ���   " FONT_CROSS " ��˥�`�ˑ���",
#endif
		"�Τ��ܥ����Ѻ���Ƥ���������",
		"���ߥ��`�������_ʼ���ޤ���",
#ifdef ADHOC
		"���ߥ��`�������_ʼ���ޤ���(AdHoc)",
#endif
		"���ߥ��`������K�ˤ��ޤ���",
		"���ߥ��`������ꥻ�åȤ��ޤ���",
		"���ߥ��`���������Ӥ��ޤ���",
#if (EMU_SYSTEM != NCDZ)
		"���Υ��`��τ������ޤ���",
		"���Υ��`��ϬF״�ǤϤɤ���äƤ������˄������ޤ��󤬡�",
		"�ݤˤ��ʤ��Ǥ���������",
#endif
		"���Υǥ��쥯�ȥ����ڥǥ��쥯�ȥ���O�����ޤ���",
#ifdef PSP_SLIM
		"���Υץ�����PSP-2000 + FW 3.71 M33�Խ����äǤ���",
#endif
#ifdef SAVE_STATE
		"���Ʃ`�ȥ��`�֤��_ʼ���ޤ���",
		"���Ʃ`�ȥ�`�ɤ��_ʼ���ޤ���",
#endif
#if defined(SAVE_STATE) || defined(COMMAND_LIST)
		"���ˤ��ޤ�����",
#endif
#ifdef SAVE_STATE
		"���Ʃ`�ȥǩ`�����������ޤ���",
#endif
#if (EMU_SYSTEM == NCDZ)
		"MP3�ե����뤬�o���顢CDDA����������ޤ���",
		"IPL.TXT������ޤ���",
		"NEO��GEO CDZ��BIOS�����Ӥ��ޤ���",
		"NEO��GEO CDZ��BIOS(neocd.bin)������ޤ���",
		"NEO��GEO CDZ��BIOS���o���Ǥ���",
		"���`������ӤǤ��ޤ���",
#endif

		"�إ�� - %s",
		"ʹ�ä��ޤ���",
		"�Τ��ܥ����Ѻ���ȥ�˥�`�ˑ���ޤ�",
		"�ե�����֥饦��",
		"���`������Ƅ�",
		"���`�����1�ک`�����Ƅ�",
		"���`�������",
		"���`������� (AdHoc)",
		"���ߥ��`����K��",
		"BIOS�x�k��˥�`���ʾ",
		"����`�O����˥�`���_��",
		"���Υإ�פ��_��",
		"BIOS������",
		"�x�k�ФΥǥ��쥯�ȥ�����ӥǥ��쥯�ȥ���O��",
		"������",
		"�x�k",
		"����åȤ��x�k",
		"�I�����",
		"�I���g��",
		"�Ŀ���x�k",
		"�Ŀ���x�k/���Ή��",
		"RGB�΂���+5",
		"�ե�����֥饦���ˑ���",
		"RGB�΂���-5",
		"ȫ�Ƥ΂���˜ʤˑ���",
		"�ꥹ�Ȥ򥹥���`��/�Ŀ���x�k",
		"�Ŀ��˥�`���_��/�]����",
		"���`��/�ᥤ���˥�`�ˑ���",

		/* psp/ui_menu.c */
		"����",
		"����",
		"�Ϥ�",
		"������",
		"�Є�",
		"�o��",
		"�ᥤ���˥�`�ˑ���",
		"���`���O����˥�`",
		"�饹�����ե�����",
		"���撈��",
#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
		"360x270 (4:3)",
		"384x270 (24:17)",
		"466x272 (12:7)",
		"480x270 (16:9)",
		"������ܞ",
#else
		"320x224 (4:3)",
		"360x270 (4:3)",
		"366x270 (19:14)",
		"420x270 (14:9)",
		"480x270 (16:9)",
#endif
		"VBLANKͬ��",
		"�Ԅӥե�`�ॹ���å�",
		"�ե�`�ॹ���å�",
		"��٥�1",
		"��٥�2",
		"��٥�3",
		"��٥�4",
		"��٥�5",
		"��٥�6",
		"��٥�7",
		"��٥�8",
		"��٥�9",
		"��٥�10",
		"��٥�11",
		"FPS��ʾ",
		"60fps�ե�`������",
		"�����������",
		"����ץ��`��",
		"11025Hz",
		"22050Hz",
		"44100Hz",
		"�����������",
		"0%",
		"10%",
		"20%",
		"30%",
		"40%",
		"50%",
		"60%",
		"70%",
		"80%",
		"90%",
		"100%",
		"����ȥ�`��",
		"�ץ쥤��`1",
		"�ץ쥤��`2",
#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
		"�ץ쥤��`3",
		"�ץ쥤��`4",
#endif
		"PSP CPU����å�",
		"222MHz",
		"266MHz",
		"300MHz",
		"333MHz",
#if (EMU_SYSTEM == MVS)
		"BIOS�˜�",
#endif
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"�����ƥ�����O��",
		"�ձ�",
		"����ꥫ",
		"��`��å�",
#endif
#if (EMU_SYSTEM == MVS)
		"�����ƥ��`���O��",
		"��ͥ�� (AES)",
		"���`���`�� (MVS)",
#endif
#if (EMU_SYSTEM == NCDZ)
		"��`�ɻ��楨�ߥ��`�����",
		"CD-ROM�ٶ�����",
		"CDDA����",
		"CDDA����",
#endif

		"�����ܥ����O����˥�`",
		"ʹ�ä��ʤ�",
		FONT_UPARROW,
		FONT_DOWNARROW,
		FONT_LEFTARROW,
		FONT_RIGHTARROW,
		FONT_CIRCLE,
		FONT_CROSS,
		FONT_SQUARE,
		FONT_TRIANGLE,
		"L�ȥꥬ",
		"R�ȥꥬ",
		"START",
		"SELECT",
		"��",
		"�˜�",
		"��",
#if (EMU_SYSTEM == CPS2)
		"�ץ쥤��`1 �����`��",
		"�ץ쥤��`2 �����`��",
#endif
		"%d�ե�`��",
		"%d�ե�`��",
		FONT_UPARROW,
		FONT_DOWNARROW,
		FONT_LEFTARROW,
		FONT_RIGHTARROW,
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"�ܥ���A",
		"�ܥ���B",
		"�ܥ���C",
		"�ܥ���D",
#else
		"�ܥ���1",
		"�ܥ���2",
		"�ܥ���3",
		"�ܥ���4",
		"�ܥ���5",
		"�ܥ���6",
#endif
		"�����`��",
#if (EMU_SYSTEM == NCDZ)
		"���쥯��",
#else
		"������",
		"���`�ӥ�������",
#endif
#if (EMU_SYSTEM == MVS)
		"�ƥ��ȥ����å�",
#elif (EMU_SYSTEM != NCDZ)
		"���`�ӥ������å�",
#endif
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"�ܥ���A �B��",
		"�ܥ���B �B��",
		"�ܥ���C �B��",
		"�ܥ���D �B��",
#else
		"�ܥ���1 �B��",
		"�ܥ���2 �B��",
		"�ܥ���3 �B��",
		"�ܥ���4 �B��",
		"�ܥ���5 �B��",
		"�ܥ���6 �B��",
#endif
		"�B���g��",
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"AB ͬ�rѺ��",
		"AC ͬ�rѺ��",
		"AD ͬ�rѺ��",
		"BC ͬ�rѺ��",
		"BD ͬ�rѺ��",
		"CD ͬ�rѺ��",
		"ABC ͬ�rѺ��",
		"ABD ͬ�rѺ��",
		"ACD ͬ�rѺ��",
		"BCD ͬ�rѺ��",
		"ABCD ͬ�rѺ��",
#else
		"1+2ͬ�rѺ��",
		"1+3ͬ�rѺ��",
		"1+4ͬ�rѺ��",
		"2+3ͬ�rѺ��",
		"2+4ͬ�rѺ��",
		"3+4ͬ�rѺ��",
		"1+2+3ͬ�rѺ��",
		"1+2+4ͬ�rѺ��",
		"1+3+4ͬ�rѺ��",
		"2+3+4ͬ�rѺ��",
		"1+2+3+4ͬ�rѺ��",
		"4+5+6ͬ�rѺ��",
#endif
#if (EMU_SYSTEM != NCDZ)
		"���ʥ��ж�",
#endif
#if (EMU_SYSTEM == CPS1)
		"�������� (���ܞ)",
		"�������� (�һ�ܞ)",
		"�ݩ`��",
#endif
#if (EMU_SYSTEM == CPS2)
		"�ѥɥ� (���ܞ)",
		"�ѥɥ� (�һ�ܞ)",
#endif
#if (EMU_SYSTEM == MVS)
		"�ӥå�",
		"����`��",
		"���֥륢�å�",
		"�����`��/��Ӌ",
		"�B������",
		"����󥻥�",
		"�٥å�",
		"ȫ�Ƥ˥٥å�/ȫ�ƥ���󥻥�",
		"���ڥ�`����˥�`",
		"���쥸�åȤ򥯥ꥢ",
		"�ۥåѩ`ȡ�����",
#endif
		"������`�󥷥�åȱ���",
		"�ץ쥤��`�Ф��椨",
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"�ܥ�������",
		"������1 (NEO��GEO�ѥå�)",
		"������2 (MVS)",
		"������3 (PlayStation)",
		"��`�����x",
#endif

		"�ǥ��åץ����å��O����˥�`",
		"���Υ��`��ˤϥǥ��åץ����å��Ϥ���ޤ���",

		"���`��/��`�ɥ��Ʃ`��",
		"����å�%d:",
		"�ǩ`���ʤ�",
		"��`��",
		"����",
		"���`��",
		"�ץ쥤�ո�",
		"���`�֕r��",
		"�Щ`�����",
		"�ե�����\"%s\"�������Ǥ��ޤ���Ǥ�����",

		"�ᥤ���˥�`",
		"���`���O��",
		"���`��΄����O���������ޤ���",
		"�����ܥ����O��",
		"�����ܥ�����B��/�ۥåȥ��`���O���򤷤ޤ���",
#if (EMU_SYSTEM != NCDZ)
		"�ǥ��åץ����å��O��",
		"�ǥ��åץ����å����O���������ޤ���",
#endif
		"���ߥ��`������ꥻ�å�",
		SYSTEM_NAME "�Υ��ߥ��`������ꥻ�åȤ��ޤ���",
		"�ե�����֥饦���ˑ���",
		"���ߥ��`������K�ˤ����ե�����֥饦���ˑ���ޤ���",
		"���`��ˑ���",
		"���`�໭��ˑ���ޤ���",
		"���ߥ��`����K��",
		"���Υ��եȥ�������K�ˤ���PSP�Υ�˥�`�ˑ���ޤ���",
#ifdef COMMAND_LIST
		"���ޥ�ɥꥹ�ȱ�ʾ",
		"���Υ��`��Υ��ޥ��һ�E���ʾ���ޤ���",
#endif

#if PSP_VIDEO_32BPP
		"����`�O����˥�`",
		"��������ηN�",
		"������������뤵",
		"�����ȥ�Щ`������",
		"�x�k���줿����",
		"�˜ʤ�����",
		"����å��`��������",
		"�����å��`��������",
		"�ե������x�k�Щ`(�_ʼ)",
		"�ե������x�k�Щ`(�K��)",
		"�����ȥ�Щ`/��å��`���ܥå���",
		"�����ȥ�Щ`/��å��`���ܥå����Ζ�",
		"����ɫ(��������δʹ�Õr)",
		"���x��",
		"�v�x��",
		"���x��",
		"�˜ʤλ���",
		"��`���λ���",
		"logo.png�Τ�",
		"�ե������x�k�Щ`",
#endif

		"���ޥ�ɥꥹ��",

		/* psp/mp3.c */
#if (EMU_SYSTEM == NCDZ)
		"MP3�ե�����\"%s\"���٥��`�ץ�Ǥ��ޤ���",
		"MP3�ǥ��`�ɥ���`���k�����ޤ�����",
		"MP3�����å��`�ǥ�������ͥ��_���Ǥ��ޤ���",
		"MP3����åɤ��_ʼ�Ǥ��ޤ���",
#endif

		/* psp/adhoc.c*/
#ifdef ADHOC
		"��ө`",
		"���`�Щ`",
		"���饤�����",
		"����PSP�νӾA����C�С�\n",
		"%s�˽ӾA��...",
		"�ӾA���ޤ�����",
		"%s�����ж���...",
		"�жϤ��ޤ�����",
		"�ӾAʧ��",
		"�ӾA���륵�`�Щ`���x�k��" FONT_TRIANGLE "��Ѻ������ֹ���ޤ���\n",
		"%s�νӾA�S�ɤ���äƤ��ޤ���\n",
		FONT_CROSS "��Ѻ������ֹ���ޤ���\n",
		"%s���ӾA�S�ɤ�Ҫ�󤷤Ƥ��ޤ���\n",
		FONT_CIRCLE "�ǽӾA���S�ɡ�" FONT_CROSS "�ǽӾA��ܷ񤷤ޤ���\n",
		"ͬ�ڤ���äƤ��ޤ���",
#endif

		/* psp/png.c */
		"PNG�����äΥ����_���Ǥ��ޤ���",
		"PNG��������ɤǤ��ޤ���",
#if PSP_VIDEO_32BPP || (EMU_SYSTEM == NCDZ)
		"PNG�����չ�_�Ǥ��ޤ���",
#endif
		"%dbit����`��PNG����ˤό��ꤷ�Ƥ��ޤ���",

		/* emumain.c */
		"�����Ĥʥ���`",
		"������`�󥷥�åȤ�\"%s_%02d.png\"�˱��椷�ޤ�����",

#if USE_CACHE
		/* common/cache.c */
		"����å���ե�����򥪩`�ץ�Ǥ��ޤ���\n",
		"���꤬���ޤ���\n",
		"����å�������_���Ǥ��ޤ���\n",
		"%dKB�Υ���å�������_�����ޤ�����\n",
		"����å����`���Ф˥���`���k�����ޤ�����\n",
		"����å������ǩ`�����`����...\n",
		"���ݩ`�Ȥ���Ƥ��ʤ��Щ`�����\"V%c%c\"�Υ���å���ե�����Ǥ���\n",
		"�F��Ҫ�󤵤�륭��å���ե�����ΥЩ`������\"" CACHE_VERSION "\"�Ǥ���\n",
		"����å���ե���������ɤ��ʤ����Ƥ���������\n",
#if (EMU_SYSTEM == CPS2)
		"���ץ饤�ȥ֥�å�%03x�򥪩`�ץ�Ǥ��ޤ���\n",
#elif (EMU_SYSTEM == MVS)
		"PCM����å����ʹ�ä��ޤ���\n",
#endif
#endif

		/* common/loadrom.c */
		"�Τ��ܥ����Ѻ���Ƥ���������\n",
		"�����_���Ǥ��ޤ���(REGION_%s)\n",
		"CRC32������������ޤ���\"%s\"\n",
		"�ե����뤬Ҋ�Ĥ���ޤ���\"%s\"\n",

#ifdef SAVE_STATE
		/* common/state.c */
		"���Ʃ`���äΥХåե���_���Ǥ��ޤ���",
		"�ե�����\"%s.sv%d\"�����ɤǤ��ޤ���",
		"�ե�����\"%s.sv%d\"�򥪩`�ץ�Ǥ��ޤ���",
		"\"%s.sv%d\"�򱣴���",
		"\"%s.sv%d\"���`����",
#if (EMU_SYSTEM == MVS)
		"BIOS���`�ɤǤ��ޤ��󡣺Τ��ܥ����Ѻ���ȽK�ˤ��ޤ���",
#elif (EMU_SYSTEM == NCDZ)
		"���Ʃ`�ȥǩ`����R�s�Ǥ��ޤ���",
		"���Ʃ`�ȥǩ`����չ�_�Ǥ��ޤ���",
#endif
#endif

#ifdef COMMAND_LIST
		/* common/cmdlist.c */
		"���Υ��`���äΥ��ޥ�ɥꥹ�Ȥ�����ޤ���",
		"���ޥ�ɥꥹ�� - %s",
		"%d/%d �Ŀ",
		"COMMAND.DAT �������sС�I��",
		"���Υ��ߥ��`���ǌ��ꤷ�Ƥ��ʤ����`��Υ��ޥ�ɥꥹ�Ȥ���������\n",
		"COMMAND.DAT�Υե����륵������sС���ޤ���\n",
		"�I����Ф����Ϥϡ�" FONT_CIRCLE "�ܥ����Ѻ���Ƥ���������\n",
		FONT_CROSS "�ܥ����Ѻ���ȄI�����ֹ���ƥե�����֥饦���ˑ���ޤ���\n",
		"COMMAND.DAT ��ʖ���...\n",
		"����`: δ�����\"COMMAND.DAT\"���ե��������ʽ�����ʤ�ޤ���\n",
		"����`: �դΥե�����Ǥ���\n",
		"����`: �����_���Ǥ��ޤ���\n",
		"����`: �ե�������ͩ`��Ǥ��ޤ���\n",
		"����`: �����ե���������ɤǤ��ޤ���\n",
		"\"%s\" �Υ��ޥ�ɤ򥳥ԩ`��...\n",
		"������ %dbytes �� %dbytes (%.1f%%�p)\n",
#endif

		/* emulation core */
		"���ˤ��ޤ�����\n",
		"���ߥ��`�����νK��",
		"���Ф餯����������������\n",

		/* inptport.c */
		"����ȥ�`��`: �ץ쥤��`1",
		"����ȥ�`��`: �ץ쥤��`%d",
#ifdef ADHOC
		"ͬ�ڤ�ʧ���ޤ�����\n",
		"%s�ˤ��ݩ`��",
		"���`������_",
		"�жϤ��ƽK��",
#endif

		/* memintrf.c */
		"��`���� \"%s\"\n",
		"ROM�Υ�`��",
#if (EMU_SYSTEM != NCDZ)
		"ROM��������å���...\n",
		"���Υ��`��ϥ��ݩ`�Ȥ���Ƥ��ޤ���\n",
		"ROM��Ҋ�Ĥ���ޤ���(zip�ե����������������ʤ�)\n",
		"\"%s\"�Υɥ饤�Ф�����ޤ���\n",
		"ROM���å���\"%s\" (�H���å���: %s)\n",
		"ROM���å���\"%s\"\n",
#endif

#if (EMU_SYSTEM == CPS1)

		/* memintrf.c */
		"rominfo.cps1��Ҋ�Ĥ���ޤ���\n",
		"�����_���Ǥ��ޤ���(0x8000�Х���)",

#elif (EMU_SYSTEM == CPS2)

		/* cps2crpt.c */
		"�ͺńI���g���� %d%%\r",
		"�ͺńI���g���� 100%%\n",

		/* memintrf.c */
		"rominfo.cps2��Ҋ�Ĥ���ޤ���\n",

#ifdef PSP_SLIM
		/* vidhrdw.c */
		"����ե��å��ǩ`����չ�_��...\n",
#endif

#elif (EMU_SYSTEM == MVS)

		/* biosmenu.c */
		"BIOS�x�k��˥�`",
		"BIOS��Ҋ�Ĥ���ޤ���",
		"NVRAM�ե������ȫ����������ޤ�����\n",

		/* memintrf.c */
		"rominfo.mvs��Ҋ�Ĥ���ޤ���\n",
		"��`���� \"%s (%s)\"\n",
		"�ͺńI���g����...\n",
		"�ͺŜg��GFX2 ROM���`����...\n",
		"�ͺŜg��SOUND1 ROM���`����...\n",
		"ROM�ͺ��äΥ����_���Ǥ��ޤ���\n",
		"���ץ饤�ȥǩ`���äΥ����_���Ǥ��ޤ���\n",
		"���ץ饤�ȥ���å����ʹ�ä�ԇ�ߤޤ���\n",
		"BIOS������å���...\n",

#elif (EMU_SYSTEM == NCDZ)

		/* ncdz.c */
		"IPL.TXT�I���Ф˥���`���k�����ޤ�����",
		"�����å��ɥå������󥿤ˤ��ꥻ�åȤ��k�����ޤ�����",

		/* cdrom.c */
		"�ե�����\"%s\"�򥪩`�ץ�Ǥ��ޤ���",
		"CD-ROM�Υ��Ʃ`�ȥǩ`����׷�ӤǤ��ޤ���",

		/* vidhrdw.c */
		"CD-ROM�ٶ�����: �o��",
		"CD-ROM�ٶ�����: �Є�",

		/* memintrf.c */
		"���`��ID������å���...\n",
#endif
		"���`�ȥ�˥�`",
		"���`��",
		"���`�Ȥ��x�k���ޤ���",
		"���Υ��`���äΥ��`�Ȥ�����ޤ���",
/*
		"������",
		"�������O���������ޤ���",*/
		NULL
};
#include "ui_text_spanish.c"
static const char *text_CHINESE_SIMPLIFIED[UI_TEXT_MAX] =
{
		"\0",
		"\n",

		/* psp/filer.c */
		"���Ե�...",
		"�޷���zipname.%s",
#ifdef ADHOC
		"���WLAN����",
		"����ģ���ȡʧ��.",
#endif

		/* psp/sound.c */
		"�޷�ȷ����Ƶ",
		"�޷�������Ƶ�߳�",

		/* psp/ui.c */
		"����:��������(%d%%)������!",

		FONT_CIRCLE " ȷ��  " FONT_CROSS " ȡ��",
		FONT_CIRCLE " ȷ��  " FONT_CROSS " ȡ��",
#ifdef SAVE_STATE
		FONT_CIRCLE " ������Ϸ  " FONT_CROSS " ���ز˵�",
#endif
		"�밴�����",
		"��ʼģ��",
#ifdef ADHOC
		"��ʼģ�� (����)",
#endif
		"�˳�ģ����",
		"����ģ����",
		"��Ҫ����ģ����",
#if (EMU_SYSTEM != NCDZ)
		"����Ϸ�޷�����",
		"�޷���������",
		"������",
#endif
		"�Ƿ񽫴��ļ�������Ϊ��ʼĿ¼?",
#ifdef PSP_SLIM
		"�������ҪPSP-2000�����ϵĻ���+3.71 M33�����ϵͳ",
#endif
#ifdef SAVE_STATE
		"��ʼ��ʱ�浵",
		"��ʼ��ʱ����",
#endif
#if defined(SAVE_STATE) || defined(COMMAND_LIST)
		"���",
#endif
#ifdef SAVE_STATE
		"ɾ���浵�ļ�",
#endif
#if (EMU_SYSTEM == NCDZ)
		"û���ҵ�MP3�ļ����޷�����CDDA",
		"û���ҵ�IPL.TXT",
		"����NEO��GEO CDZ BIOS",
		"û���ҵ�NEO��GEO CDZ BIOS (neocd.bin)",
		"��Ч��NEO��GEO CDZ BIOS",
		"�޷�������Ϸ",
#endif

		"���� - %s",
		"δʹ��",
		"�밴��������ز˵�",
		"��ϷĿ¼",
		"�ƶ�",
		"��ҳ",
		"������Ϸ",
		"������Ϸ (����)",
		"�˳�ģ����",
		"��ʾBIOSѡ��˵�",
		"����ɫ����˵�",
		"��ʾ����",
		"����BIOS",
		"����Ϊ��ʼĿ¼",
		"������ֵ",
		"ѡ��",
		"ѡ��浵",
		"�л�����",
		"ִ��",
		"ѡ����Ŀ",
		"ѡ����Ŀ/������ֵ",
		"RGB��ֵ+5",
		"������ϷĿ¼",
		"RGB��ֵ-5",
		"�������ݻָ�Ĭ��",
		"�ƶ�/ѡ����Ŀ",
		"��/�ر���Ŀ�˵�",
		"�������˵�/��Ϸ",

		/* psp/ui_menu.c */
		"��",
		"��",
		"��",
		"��",
		"��",
		"��",
		"�������˵�",
		"��Ϸ���ò˵�",
		"��դЧ��",
		"��������",
#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
		"360x270 (4:3)",
		"384x270 (24:17)",
		"466x272 (12:7)",
		"480x270 (16:9)",
		"������ת",
#else
		"320x224 (4:3)",
		"360x270 (4:3)",
		"366x270 (19:14)",
		"420x270 (14:9)",
		"480x270 (16:9)",
#endif
		"��ֱͬ��",
		"�Զ���֡",
		"��֡����",
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		"10",
		"11",
		"��ʾ֡��",
		"����60֡",
		"��������",
		"������",
		"11025Hz",
		"22050Hz",
		"44100Hz",
		"����",
		"0%",
		"10%",
		"20%",
		"30%",
		"40%",
		"50%",
		"60%",
		"70%",
		"80%",
		"90%",
		"100%",
		"������",
		"���1",
		"���2",
#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
		"���3",
		"���4",
#endif
		"PSPƵ��",
		"222MHz",
		"266MHz",
		"300MHz",
		"333MHz",
#if (EMU_SYSTEM == MVS)
		"Ĭ��",
#endif
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"��������",
		"�հ�",
		"����",
		"ŷ��",
#endif
#if (EMU_SYSTEM == MVS)
		"����ģʽ",
		"���û� (AES)",
		"�ֻ� (MVS)",
#endif
#if (EMU_SYSTEM == NCDZ)
		"ģ���ȡ����",
		"CD-ROM�ٶ�����",
		"����CDDA",
		"CDDA����",
#endif

		"�������ò˵�",
		"δʹ��",
		FONT_UPARROW,
		FONT_DOWNARROW,
		FONT_LEFTARROW,
		FONT_RIGHTARROW,
		FONT_CIRCLE,
		FONT_CROSS,
		FONT_SQUARE,
		FONT_TRIANGLE,
		"L",
		"R",
		"START",
		"SELECT",
		"��",
		"��",
		"��",
#if (EMU_SYSTEM == CPS2)
		"���1 ��ʼ",
		"���2 ��ʼ",
#endif
		"%d",
		"%d",
		FONT_UPARROW,
		FONT_DOWNARROW,
		FONT_LEFTARROW,
		FONT_RIGHTARROW,
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"����A",
		"����B",
		"����C",
		"����D",
#else
		"����1",
		"����2",
		"����3",
		"����4",
		"����5",
		"����6",
#endif
		"��ʼ",
#if (EMU_SYSTEM == NCDZ)
		"ѡ��",
#else
		"Ͷ��",
		"����Ͷ��",
#endif
#if (EMU_SYSTEM == MVS)
		"���Կ���",
#elif (EMU_SYSTEM != NCDZ)
		"���Կ���",
#endif
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"����A",
		"����B",
		"����C",
		"����D",
#else
		"����1",
		"����2",
		"����3",
		"����4",
		"����5",
		"����6",
#endif
		"�������",
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"�ȼ�A+B",
		"�ȼ�A+C",
		"�ȼ�A+D",
		"�ȼ�B+C",
		"�ȼ�B+D",
		"�ȼ�C+D",
		"�ȼ�A+B+C",
		"�ȼ�A+B+D",
		"�ȼ�A+C+D",
		"�ȼ�B+C+D",
		"�ȼ�A+B+C+D",
#else
		"�ȼ�1+2",
		"�ȼ�1+3",
		"�ȼ�1+4",
		"�ȼ�2+3",
		"�ȼ�2+4",
		"�ȼ�3+4",
		"�ȼ�1+2+3",
		"�ȼ�1+2+4",
		"�ȼ�1+3+4",
		"�ȼ�2+3+4",
		"�ȼ�1+2+3+4",
		"�ȼ�4+5+6",
#endif
#if (EMU_SYSTEM != NCDZ)
		"ҡ��������",
#endif
#if (EMU_SYSTEM == CPS1)
		"Dial (��ת)",
		"Dial (��ת)",
		"Pause",
#endif
#if (EMU_SYSTEM == CPS2)
		"Paddle (��ת)",
		"Paddle (��ת)",
#endif
#if (EMU_SYSTEM == MVS)
		"Big",
		"Small",
		"Double Up",
		"Start/Collect",
		"Payout",
		"Cancel",
		"Bet",
		"Bet/Cancel All",
		"Operator Menu",
		"Clear Credit",
		"Hopper Out",
#endif
		"�����ͼ",
		"�л����",
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"����ģʽ",
		"����1 (NEOGEO����)",
		"����2 (�ֻ�)",
		"����3 (PlayStation)",
		"�Զ�",
#endif

		"DIP�������ò˵�",
		"����Ϸû��DIP����",

		"��ʱ�浵/����",
		"�浵%d:",
		"��",
		"��ȡ",
		"ɾ��",
		"����",
		"��������",
		"����ʱ��",
		"�汾",
		"ɾ��\"%s\"ʧ��",

		"���˵�",
		"��Ϸ����",
		"������Ϸ������",
		"��������",
		"���İ���������/�ȼ�������",
#if (EMU_SYSTEM != NCDZ)
		"DIP��������",
		"����Ӳ���ϵ�DIP��������",
#endif
		"����ģ����",
		"����"SYSTEM_NAME"ģ����",
		"������ϷĿ¼",
		"ֹͣģ������������ϷĿ¼",
		"������Ϸ",
		"����ģ����������Ϸ",
		"�˳�ģ����",
		"�˳�ģ����������PSP�˵�",
#ifdef COMMAND_LIST
		"��ʾ����˵��",
		"��ʾ����˵�� (MAME Plus��ʽ��\"command.dat\")",
#endif

#if PSP_VIDEO_32BPP
		"��ɫ����˵�",
		"����ͼ����",
		"����ͼ����",
		"���������",
		"ѡ������",
		"��ͨ����",
		"��Ϣ����",
		"��������",
		"�ļ�ѡ��� (ͷ)",
		"�ļ�ѡ��� (β)",
		"�����/��Ϣ��",
		"�������/��Ϣ���",
		"����",
		"��",
		"��",
		"��",
		"Ĭ��ͼ��",
		"�û�ͼ��",
		"logo.png",
		"�ļ�ѡ���",
#endif

		"����˵��",

		/* psp/mp3.c */
#if (EMU_SYSTEM == NCDZ)
		"�޷����´�\"%s\"",
		"MP3�������",
		"�޷�ȷ��MP3��Ƶ",
		"�޷�����MP3�߳�",
#endif

		/* psp/adhoc.c*/
#ifdef ADHOC
		"����",
		"������",
		"�ͻ���",
		"�ȴ�����PSP����\n",
		"��������%s",
		"������",
		"�Ѵ�%s�Ͽ�����",
		"�Ͽ�����",
		"ʧ��",
		"ѡ��һ�����������ӣ���"FONT_TRIANGLE"������\n",
		"���ڵȴ�%sͬ������\n",
		"��"FONT_CROSS"��ȡ��\n",
		"%s����������\n",
		"��"FONT_CIRCLE"��ͬ�����ӣ���"FONT_CROSS"��ȡ��\n",
		"�ȴ�ͬ����",
#endif

		/* psp/png.c */
		"�޷�ΪPNG�����ڴ�",
		"�޷��R��PNGͼ��",
#if PSP_VIDEO_32BPP || (EMU_SYSTEM == NCDZ)
		"�޷���RPNGͼ��",
#endif
		"��֧��%dλ��ɫ��PNGͼ",

		/* emumain.c */
		"���ش���",
		"��ͼ����Ϊ\"%s_%02d.png\"",

#if USE_CACHE
		/* common/cache.c */
		"�޷��򿪻����ļ�\n",
		"�ڴ治��\n",
		"�޷����仺��\n",
		"�ѷ���%dKB����\n",
		"�����ȡ����!!!\n",
		"���ڶ�ȡ������Ϣ����\n",
		"��֧��\"V%c%c\"��Ļ����ļ�\n",
		"Ŀǰ��Ҫ\""CACHE_VERSION"\"��\n",
		"������ת�������ļ�\n",
#if (EMU_SYSTEM == CPS2)
		"�޷���ͼ���%03x\n",
#elif (EMU_SYSTEM == MVS)
		"PCM�����ѿ���\n",
#endif
#endif

		/* common/loadrom.c */
		"�밴�����\n",
		"�޷�����%s�ڴ�\n",
		"CRC32����ȷ\"%s\"\n",
		"û���ҵ�\"%s\"\n",

#ifdef SAVE_STATE
		/* common/state.c */
		"�޷����仺����",
		"�޷�����\"%s.sv%d\"",
		"�޷���\"%s.sv%d\"",
		"���ڱ���\"%s.sv%d\"",
		"���ڶ�ȡ\"%s.sv%d\"",
#if (EMU_SYSTEM == MVS)
		"�޷����¶�ȡBIOS���밴������˳�",
#elif (EMU_SYSTEM == NCDZ)
		"�޷��R����ʱ�浵����",
		"�޷���R��ʱ�浵����",
#endif
#endif

#ifdef COMMAND_LIST
		/* common/cmdlist.c */
		"û�з��ִ���Ϸ�Ĳ���˵��",
		"����˵�� - %s",
		"%d/%d",
		"����COMMAND.DAT����",
		"�˲�����ɾ��COMMAND.DAT��û��ʹ�õ�����Ϸ����˵��\n",
		"��ģ����֧�ֵ���Ϸ����˵�����ᱻɾ��\n",
		"���������command.dat��������밴"FONT_CIRCLE"��\n",
		"���������밴"FONT_CROSS"��������ϷĿ¼\n",
		"���ڼ��COMMAND.DAT��ʽ\n",
		"����: δ֪��ʽ\n",
		"����: ���ļ�\n",
		"����: �޷������ڴ�\n",
		"����: �޷�������\n",
		"����: �޷���������ļ�\n",
		"���ڸ���\"%s\"\n",
		"ԭʼ����:%d�ֽ�, ������:%d�ֽ� (-%.1f%%)\n",
#endif

		/* emulation core */
		"���\n",
		"�˳�ģ����",
		"���Ե�\n",

		/* inptport.c */
		"������: ���1",
		"������: ���%d",
#ifdef ADHOC
		"ͬ��ʧ��\n",
		"%s����ͣ",
		"������Ϸ",
		"�Ͽ�����",
#endif

		/* memintrf.c */
		"���ڶ�ȡ\"%s\"\n",
		"��ȡ��ϷROM",
#if (EMU_SYSTEM != NCDZ)
		"���ڼ��ROM��Ϣ\n",
		"��֧�ִ���Ϸ\n",
		"û���ҵ�ROM (zip�ļ�������ȷ)\n",
		"û���ҵ�\"%s\"������\n",
		"ROM����\"%s\" (��ROM��: %s)\n",
		"ROM����\"%s\"\n",
#endif

#if (EMU_SYSTEM == CPS1)

		/* memintrf.c */
		"û���ҵ�rominfo.cps1\n",
		"�޷������ڴ� (0x8000 bytes)",

#elif (EMU_SYSTEM == CPS2)

		/* cps2crpt.c */
		"���ڽ��ܣ�����%d%%\r",
		"���ڽ��ܣ�����100%%\n",

		/* memintrf.c */
		"û���ҵ�rominfo.cps2\n",

#ifdef PSP_SLIM
		/* vidhrdw.c */
		"����չ��ͼ������\n",
#endif

#elif (EMU_SYSTEM == MVS)

		/* biosmenu.c */
		"BIOSѡ��˵�",
		"û���ҵ�BIOS",
		"����NVRAM�ļ��ѱ�ɾ��\n",

		/* memintrf.c */
		"û���ҵ�rominfo.mvs\n",
		"���ڶ�ȡ\"%s (%s)\"\n",
		"���ڽ���ROM\n",
		"���ڶ�ȡGFX2����ROM\n",
		"���ڶ�ȡSOUND1����ROM\n",
		"�޷�Ϊ����ROM�����ڴ�\n",
		"�޷�Ϊͼ�����ݷ����ڴ�\n",
		"����ʹ��ͼ�񻺴�\n",
		"���ڼ��BIOS\n",

#elif (EMU_SYSTEM == NCDZ)

		/* ncdz.c */
		"����IPL.TXT����",
		"����ԭ��:���ʧ��",

		/* cdrom.c */
		"�޷���\"%s\"",
		"�޷�����CD-ROM����",

		/* vidhrdw.c */
		"CD-ROM�ٶ�����:��",
		"CD-ROM�ٶ�����:��",

		/* memintrf.c */
		"���ڼ����ϷID\n",
#endif
		"����ָ�˵�",
		"����ָ",
		"ѡ�����ָ����",
		"û�з��ִ���Ϸ�Ľ���ָ",
/*
		"�ڴ��ͷ�",
		"�����ڴ��ͷŵ�����",*/
		NULL
};
static const char *text_CHINESE_TRADITIONAL[UI_TEXT_MAX] =
{
		"\0",
		"\n",

		/* psp/filer.c */
		"Ո�Ե�...",
		"�o�����_zipname.%s",
#ifdef ADHOC
		"Ո���_WLAN�_�P",
		"�Cģ�K�xȡʧ��.",
#endif

		/* psp/sound.c */
		"�o���_�J���l",
		"�o���������l����",

		/* psp/ui.c */
		"����:�������(%d%%)��Ո���!",

		FONT_CIRCLE " �_��  " FONT_CROSS " ȡ��",
		FONT_CIRCLE " �_��  " FONT_CROSS " ȡ��",
#ifdef SAVE_STATE
		FONT_CIRCLE " �����[��  " FONT_CROSS " ���زˆ�",
#endif
		"Ո�������I",
		"�_ʼģ�M",
#ifdef ADHOC
		"�_ʼģ�M (�C)",
#endif
		"�˳�ģ�M��",
		"�؆�ģ�M��",
		"��Ҫ�؆�ģ�M��",
#if (EMU_SYSTEM != NCDZ)
		"���[��o���\\��",
		"�o�������\\��",
		"Ո����",
#endif
		"�Ƿ񌢴��ļ��A�O�Þ��_ʼĿ�?",
#ifdef PSP_SLIM
		"��ܛ����ҪPSP-2000�����ϵęC��+3.71 M33�����ϵ�y",
#endif
#ifdef SAVE_STATE
		"�_ʼ���r��n",
		"�_ʼ���r�x�n",
#endif
#if defined(SAVE_STATE) || defined(COMMAND_LIST)
		"���",
#endif
#ifdef SAVE_STATE
		"�h����n�ļ�",
#endif
#if (EMU_SYSTEM == NCDZ)
		"�]���ҵ�MP3�ļ����o������CDDA",
		"�]���ҵ�IPL.TXT",
		"����NEO��GEO CDZ BIOS",
		"�]���ҵ�NEO��GEO CDZ BIOS (neocd.bin)",
		"�oЧ��NEO��GEO CDZ BIOS",
		"�o���\\���[��",
#endif

		"���� - %s",
		"δʹ��",
		"Ո�������I���زˆ�",
		"�[��Ŀ�",
		"�Ƅ�",
		"���",
		"�\\���[��",
		"�\\���[�� (�C)",
		"�˳�ģ�M��",
		"�@ʾBIOS�x��ˆ�",
		"���_�ɫ����ˆ�",
		"�@ʾ����",
		"����BIOS",
		"�O�Þ��_ʼĿ�",
		"���Ĕ�ֵ",
		"�x��",
		"�x���n",
		"�ГQ",
		"����",
		"�x���Ŀ",
		"�x���Ŀ/���Ĕ�ֵ",
		"RGB��ֵ+5",
		"�����[��Ŀ�",
		"RGB��ֵ-5",
		"���Д����֏�Ĭ�J",
		"�Ƅ�/�x���Ŀ",
		"���_/�P�]�Ŀ�ˆ�",
		"�������ˆ�/�[��",

		/* psp/ui_menu.c */
		"�_",
		"�P",
		"��",
		"��",
		"�_",
		"�P",
		"�������ˆ�",
		"�[���O�òˆ�",
		"���Ч��",
		"����s��",
#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
		"360x270 (4:3)",
		"384x270 (24:17)",
		"466x272 (12:7)",
		"480x270 (16:9)",
		"�������D",
#else
		"320x224 (4:3)",
		"360x270 (4:3)",
		"366x270 (19:14)",
		"420x270 (14:9)",
		"480x270 (16:9)",
#endif
		"��ֱͬ��",
		"�Ԅ�����",
		"�������e",
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		"10",
		"11",
		"�@ʾ����",
		"����60��",
		"�_����",
		"�����",
		"11025Hz",
		"22050Hz",
		"44100Hz",
		"����",
		"0%",
		"10%",
		"20%",
		"30%",
		"40%",
		"50%",
		"60%",
		"70%",
		"80%",
		"90%",
		"100%",
		"������",
		"���1",
		"���2",
#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
		"���3",
		"���4",
#endif
		"PSP�l��",
		"222MHz",
		"266MHz",
		"300MHz",
		"333MHz",
#if (EMU_SYSTEM == MVS)
		"Ĭ�J",
#endif
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"�C���^��",
		"�հ�",
		"����",
		"�W��",
#endif
#if (EMU_SYSTEM == MVS)
		"�C��ģʽ",
		"���ÙC (AES)",
		"�֙C (MVS)",
#endif
#if (EMU_SYSTEM == NCDZ)
		"ģ�M�xȡ����",
		"CD-ROM�ٶ�����",
		"����CDDA",
		"CDDA����",
#endif

		"���I�O�òˆ�",
		"δʹ��",
		FONT_UPARROW,
		FONT_DOWNARROW,
		FONT_LEFTARROW,
		FONT_RIGHTARROW,
		FONT_CIRCLE,
		FONT_CROSS,
		FONT_SQUARE,
		FONT_TRIANGLE,
		"L",
		"R",
		"START",
		"SELECT",
		"��",
		"��",
		"��",
#if (EMU_SYSTEM == CPS2)
		"���1 �_ʼ",
		"���2 �_ʼ",
#endif
		"%d",
		"%d",
		FONT_UPARROW,
		FONT_DOWNARROW,
		FONT_LEFTARROW,
		FONT_RIGHTARROW,
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"���IA",
		"���IB",
		"���IC",
		"���ID",
#else
		"���I1",
		"���I2",
		"���I3",
		"���I4",
		"���I5",
		"���I6",
#endif
		"�_ʼ",
#if (EMU_SYSTEM == NCDZ)
		"�x��",
#else
		"Ͷ��",
		"�{ԇͶ��",
#endif
#if (EMU_SYSTEM == MVS)
		"�yԇ�_�P",
#elif (EMU_SYSTEM != NCDZ)
		"�{ԇ�_�P",
#endif
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"�B�lA",
		"�B�lB",
		"�B�lC",
		"�B�lD",
#else
		"�B�l1",
		"�B�l2",
		"�B�l3",
		"�B�l4",
		"�B�l5",
		"�B�l6",
#endif
		"�B�l�g��",
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"���IA+B",
		"���IA+C",
		"���IA+D",
		"���IB+C",
		"���IB+D",
		"���IC+D",
		"���IA+B+C",
		"���IA+B+D",
		"���IA+C+D",
		"���IB+C+D",
		"���IA+B+C+D",
#else
		"���I1+2",
		"���I1+3",
		"���I1+4",
		"���I2+3",
		"���I2+4",
		"���I3+4",
		"���I1+2+3",
		"���I1+2+4",
		"���I1+3+4",
		"���I2+3+4",
		"���I1+2+3+4",
		"���I4+5+6",
#endif
#if (EMU_SYSTEM != NCDZ)
		"�u�U�`����",
#endif
#if (EMU_SYSTEM == CPS1)
		"Dial (���D)",
		"Dial (���D)",
		"Pause",
#endif
#if (EMU_SYSTEM == CPS2)
		"Paddle (���D)",
		"Paddle (���D)",
#endif
#if (EMU_SYSTEM == MVS)
		"Big",
		"Small",
		"Double Up",
		"Start/Collect",
		"Payout",
		"Cancel",
		"Bet",
		"Bet/Cancel All",
		"Operator Menu",
		"Clear Credit",
		"Hopper Out",
#endif
		"����؈D",
		"�ГQ���",
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"���Iģʽ",
		"���1 (NEOGEO���I)",
		"���2 (�֙C)",
		"���3 (PlayStation)",
		"�Զ�",
#endif

		"DIP�_�P�O�òˆ�",
		"���[��]��DIP�_�P",

		"���r��n/�x�n",
		"��n%d:",
		"��",
		"�xȡ",
		"�h��",
		"����",
		"�[������",
		"����r�g",
		"�汾",
		"�h��\"%s\"ʧ��",

		"���ˆ�",
		"�[���O��",
		"�����[����O��",
		"���I�O��",
		"���İ��I���B�l/���I���O��",
#if (EMU_SYSTEM != NCDZ)
		"DIP�_�P�O��",
		"����Ӳ���ϵ�DIP�_�P�O��",
#endif
		"�؆�ģ�M��",
		"�؆�"SYSTEM_NAME"ģ�M��",
		"�����[��Ŀ�",
		"ֹͣģ�M���K�����[��Ŀ�",
		"�����[��",
		"����ģ�M���^�m�[��",
		"�˳�ģ�M��",
		"�˳�ģ�M���K����PSP�ˆ�",
#ifdef COMMAND_LIST
		"�@ʾ�����f��",
		"�@ʾ�����f�� (MAME Plus��ʽ��\"command.dat\")",
#endif

#if PSP_VIDEO_32BPP
		"�ɫ����ˆ�",
		"�����D���",
		"�����D����",
		"���}������",
		"�x������",
		"��ͨ����",
		"��Ϣ����",
		"��������",
		"�ļ��x��� (�^)",
		"�ļ��x��� (β)",
		"���}��/��Ϣ��",
		"���}���/��Ϣ���",
		"����",
		"�t",
		"�G",
		"�{",
		"Ĭ�J�D��",
		"�Ñ�D��",
		"logo.png",
		"�ļ��x���",
#endif

		"�����f��",

		/* psp/mp3.c */
#if (EMU_SYSTEM == NCDZ)
		"�o�����´��_\"%s\"",
		"MP3��a�e�`",
		"�o���_�JMP3���l",
		"�o������MP3����",
#endif

		/* psp/adhoc.c*/
#ifdef ADHOC
		"��d",
		"������",
		"�͑���",
		"�ȴ�����PSP����\n",
		"�����B��%s",
		"���B��",
		"�я�%s���_�B��",
		"���_�B��",
		"ʧ��",
		"�x��һ���������B�ӣ���"FONT_TRIANGLE"�I����\n",
		"���ڵȴ�%sͬ���B��\n",
		"��"FONT_CROSS"�Iȡ��\n",
		"%sՈ���B����\n",
		"��"FONT_CIRCLE"�Iͬ���B�ӣ���"FONT_CROSS"�Iȡ��\n",
		"�ȴ�ͬ����",
#endif

		/* psp/png.c */
		"�o����PNG����ȴ�",
		"�o�����sPNG�D��",
#if PSP_VIDEO_32BPP || (EMU_SYSTEM == NCDZ)
		"�o���≺PNG�D��",
#endif
		"��֧��%dλ�ɫ��PNG�D",

		/* emumain.c */
		"�����e�`",
		"�؈D�����\"%s_%02d.png\"",

#if USE_CACHE
		/* common/cache.c */
		"�o�����_�����ļ�\n",
		"�ȴ治��\n",
		"�o�����侏��\n",
		"�ѷ���%dKB����\n",
		"�����xȡ�e�`!!!\n",
		"�����xȡ������Ϣ����\n",
		"��֧��\"V%c%c\"��ľ����ļ�\n",
		"Ŀǰ��Ҫ\""CACHE_VERSION"\"��\n",
		"Ո�����D�Q�����ļ�\n",
#if (EMU_SYSTEM == CPS2)
		"�o�����_�D��K%03x\n",
#elif (EMU_SYSTEM == MVS)
		"PCM�������_��\n",
#endif
#endif

		/* common/loadrom.c */
		"Ո�������I\n",
		"�o������%s�ȴ�\n",
		"CRC32�����_\"%s\"\n",
		"�]���ҵ�\"%s\"\n",

#ifdef SAVE_STATE
		/* common/state.c */
		"�o�����侏�n�^",
		"�o������\"%s.sv%d\"",
		"�o�����_\"%s.sv%d\"",
		"���ڱ���\"%s.sv%d\"",
		"�����xȡ\"%s.sv%d\"",
#if (EMU_SYSTEM == MVS)
		"�o�������xȡBIOS��Ո�������I�˳�",
#elif (EMU_SYSTEM == NCDZ)
		"�o�����s���r��n����",
		"�o���≺���r��n����",
#endif
#endif

#ifdef COMMAND_LIST
		/* common/cmdlist.c */
		"�]�аl�F���[��Ĳ����f��",
		"�����f�� - %s",
		"%d/%d",
		"�s�pCOMMAND.DAT����",
		"�˲������h��COMMAND.DAT�Л]��ʹ�õ����[������f��\n",
		"��ģ�M��֧�ֵ��[������f���������h��\n",
		"�����s�pcommand.dat���w�e��Ո��"FONT_CIRCLE"�I\n",
		"����s�pՈ��"FONT_CROSS"�I�����[��Ŀ�\n",
		"���ڙz��COMMAND.DAT��ʽ\n",
		"�e�`: δ֪��ʽ\n",
		"�e�`: ���ļ�\n",
		"�e�`: �o������ȴ�\n",
		"�e�`: �o��������\n",
		"�e�`: �o������ݔ���ļ�\n",
		"�����}�u\"%s\"\n",
		"ԭʼ����:%d�ֹ�, �s�p��:%d�ֹ� (-%.1f%%)\n",
#endif

		/* emulation core */
		"���\n",
		"�˳�ģ�M��",
		"Ո�Ե�\n",

		/* inptport.c */
		"������: ���1",
		"������: ���%d",
#ifdef ADHOC
		"ͬ��ʧ��\n",
		"%s�ѕ�ͣ",
		"�����[��",
		"���_�B��",
#endif

		/* memintrf.c */
		"�����xȡ\"%s\"\n",
		"�xȡ�[��ROM",
#if (EMU_SYSTEM != NCDZ)
		"���ڙz��ROM��Ϣ\n",
		"��֧�ִ��[��\n",
		"�]���ҵ�ROM (zip�ļ��������_)\n",
		"�]���ҵ�\"%s\"����\n",
		"ROM���Q\"%s\" (��ROM��: %s)\n",
		"ROM���Q\"%s\"\n",
#endif

#if (EMU_SYSTEM == CPS1)

		/* memintrf.c */
		"�]���ҵ�rominfo.cps1\n",
		"�o������ȴ� (0x8000 bytes)",

#elif (EMU_SYSTEM == CPS2)

		/* cps2crpt.c */
		"���ڽ��ܣ��M��%d%%\r",
		"���ڽ��ܣ��M��100%%\n",

		/* memintrf.c */
		"�]���ҵ�rominfo.cps2\n",

#ifdef PSP_SLIM
		/* vidhrdw.c */
		"����չ�_�D�񔵓�\n",
#endif

#elif (EMU_SYSTEM == MVS)

		/* biosmenu.c */
		"BIOS�x��ˆ�",
		"�]���ҵ�BIOS",
		"����NVRAM�ļ��ѱ��h��\n",

		/* memintrf.c */
		"�]���ҵ�rominfo.mvs\n",
		"�����xȡ\"%s (%s)\"\n",
		"���ڽ���ROM\n",
		"�����xȡGFX2����ROM\n",
		"�����xȡSOUND1����ROM\n",
		"�o�������ROM����ȴ�\n",
		"�o����D�񔵓�����ȴ�\n",
		"�Lԇʹ�ÈD�񾏴�\n",
		"���ڙz��BIOS\n",

#elif (EMU_SYSTEM == NCDZ)

		/* ncdz.c */
		"̎��IPL.TXT���e",
		"����ԭ��:�z�yʧ��",

		/* cdrom.c */
		"�o�����_\"%s\"",
		"�o������CD-ROM����",

		/* vidhrdw.c */
		"CD-ROM�ٶ�����:�P",
		"CD-ROM�ٶ�����:�_",

		/* memintrf.c */
		"���ڙz���[��ID\n",
#endif
		"����ָ�ˆ�",
		"����ָ",
		"�x�����ָ���a",
		"�]�аl�F���[��Ľ���ָ",
/*
		"�ȴ�ጷ�",
		"���ăȴ�ጷŵ��O��",*/
		NULL
};

//			JAPANESE			0
//			ENGLISH				1
//			FRENCH				2
//			SPANISH				3
//			GERMAN				4
//			ITALIAN				5
//			DUTCH				6
//			PORTUGUESE			7
//			RUSSIAN				8
//			KOREAN				9
//			CHINESE_TRADITIONAL	10
//			CHINESE_SIMPLIFIED	11
static int lang = 0;


void ui_text_init(void)
{
	int i;

	sceUtilityGetSystemParamInt(PSP_SYSTEMPARAM_ID_INT_LANGUAGE, &lang);
    switch (lang)
	{
	case PSP_SYSTEMPARAM_LANGUAGE_CHINESE_SIMPLIFIED:
		lang = LANG_CHINESE_SIMPLIFIED;
		for (i = 0; i < UI_TEXT_MAX; i++)
			ui_text[i] = text_CHINESE_SIMPLIFIED[i];
		break;

	case PSP_SYSTEMPARAM_LANGUAGE_CHINESE_TRADITIONAL:
		lang = LANG_CHINESE_TRADITIONAL;
		for (i = 0; i < UI_TEXT_MAX; i++)
			ui_text[i] = text_CHINESE_TRADITIONAL[i];
		break;

	case PSP_SYSTEMPARAM_LANGUAGE_JAPANESE:
		lang = LANG_JAPANESE;
		for (i = 0; i < UI_TEXT_MAX; i++)
			ui_text[i] = text_JAPANESE[i];
		break;

	case PSP_SYSTEMPARAM_LANGUAGE_SPANISH:
		lang = LANG_SPANISH;
		for (i = 0; i < UI_TEXT_MAX; i++)
			ui_text[i] = text_SPANISH[i];
		break;

	default:
		lang = LANG_ENGLISH;
		for (i = 0; i < UI_TEXT_MAX; i++)
			ui_text[i] = text_ENGLISH[i];
		break;
	}
}

int ui_text_get_language(void)
{
	return lang;
}
