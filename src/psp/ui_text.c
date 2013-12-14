/******************************************************************************

	ui_text.c

	ユーザインタフェーステキスト管理

******************************************************************************/

#include "psp.h"
#include <psputility_sysparam.h>


const char *ui_text[UI_TEXT_MAX];

static const char *text[4][UI_TEXT_MAX] =
{
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
		"Boot NEO·GEO CDZ BIOS.",
		"NEO·GEO CDZ BIOS (neocd.bin) not found.",
		"Invalid NEO·GEO CDZ BIOS found.",
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
		"348x224 (14:9)",
		"398x224 (16:9)",
		"360x270 (4:3)",
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
	},
	{
		"\0",
		"\n",

		/* psp/filer.c */
		"しばらくお待ちください。",
		"zipname.%sをオープンできません。",
#ifdef ADHOC
		"WLANスイッチをONにしてください。",
		"AdHocモジュールをロード出来ませんでした。",
#endif

		/* psp/sound.c */
		"オーディオチャネルを確保できません。",
		"サウンドスレッドを開始できません。",

		/* psp/ui.c */
		"警告: バッテリーの残容量がありません(%d%%)。充電してください！",

		FONT_CIRCLE " 起動      " FONT_CROSS " 中止",
		FONT_CIRCLE " 続行      " FONT_CROSS " 中止",
#ifdef SAVE_STATE
		FONT_CIRCLE " ゲームに戻る   " FONT_CROSS " メニューに戻る",
#endif
		"何かボタンを押してください。",
		"エミュレーションを開始します。",
#ifdef ADHOC
		"エミュレーションを開始します。(AdHoc)",
#endif
		"エミュレーションを終了します。",
		"エミュレーションをリセットします。",
		"エミュレータを再起動します。",
#if (EMU_SYSTEM != NCDZ)
		"このゲームは動作しません。",
		"このゲームは現状ではどうやっても正常に動作しませんが、",
		"気にしないでください。",
#endif
		"このディレクトリを初期ディレクトリに設定します。",
#ifdef PSP_SLIM
		"このプログラムはPSP-2000 + FW 3.71 M33以降専用です。",
#endif
#ifdef SAVE_STATE
		"ステートセーブを開始します。",
		"ステートロードを開始します。",
#endif
#if defined(SAVE_STATE) || defined(COMMAND_LIST)
		"完了しました。",
#endif
#ifdef SAVE_STATE
		"ステートデータを削除します。",
#endif
#if (EMU_SYSTEM == NCDZ)
		"MP3ファイルが無い為、CDDAは再生されません。",
		"IPL.TXTがありません。",
		"NEO·GEO CDZのBIOSを起動します。",
		"NEO·GEO CDZのBIOS(neocd.bin)がありません。",
		"NEO·GEO CDZのBIOSが無効です。",
		"ゲームを起動できません。",
#endif

		"ヘルプ - %s",
		"使用しません",
		"何かボタンを押すとメニューに戻ります",
		"ファイルブラウザ",
		"カーソルを移動",
		"カーソルを1ページ分移動",
		"ゲームを起動",
		"ゲームを起動 (AdHoc)",
		"エミュレータを終了",
		"BIOS選択メニューを表示",
		"カラー設定メニューを開く",
		"このヘルプを開く",
		"BIOSを起動",
		"選択中のディレクトリを起動ディレクトリに設定",
		"値を変更",
		"選択",
		"スロットの選択",
		"処理を変更",
		"処理を実行",
		"項目の選択",
		"項目の選択/値の変更",
		"RGBの値を+5",
		"ファイルブラウザに戻る",
		"RGBの値を-5",
		"全ての値を標準に戻す",
		"リストをスクロール/項目を選択",
		"項目メニューを開く/閉じる",
		"ゲーム/メインメニューに戻る",

		/* psp/ui_menu.c */
		"オン",
		"オフ",
		"はい",
		"いいえ",
		"有効",
		"無効",
		"メインメニューに戻る",
		"ゲーム設定メニュー",
		"ラスタエフェクト",
		"画面拡大",
#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
		"360x270 (4:3)",
		"384x270 (24:17)",
		"466x272 (12:7)",
		"480x270 (16:9)",
		"画面を回転",
#else
		"348x224 (14:9)",
		"398x224 (16:9)",
		"360x270 (4:3)",
		"420x270 (14:9)",
		"480x270 (16:9)",
#endif
		"VBLANK同期",
		"自動フレームスキップ",
		"フレームスキップ",
		"レベル1",
		"レベル2",
		"レベル3",
		"レベル4",
		"レベル5",
		"レベル6",
		"レベル7",
		"レベル8",
		"レベル9",
		"レベル10",
		"レベル11",
		"FPS表示",
		"60fpsフレーム制限",
		"サウンド再生",
		"サンプルレート",
		"11025Hz",
		"22050Hz",
		"44100Hz",
		"サウンド音量",
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
		"コントローラ",
		"プレイヤー1",
		"プレイヤー2",
#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
		"プレイヤー3",
		"プレイヤー4",
#endif
		"PSP CPUクロック",
		"222MHz",
		"266MHz",
		"300MHz",
		"333MHz",
#if (EMU_SYSTEM == MVS)
		"BIOS標準",
#endif
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"システム地域設定",
		"日本",
		"アメリカ",
		"ヨーロッパ",
#endif
#if (EMU_SYSTEM == MVS)
		"システムモード設定",
		"家庭用 (AES)",
		"アーケード (MVS)",
#endif
#if (EMU_SYSTEM == NCDZ)
		"ロード画面エミュレーション",
		"CD-ROM速度制限",
		"CDDA再生",
		"CDDA音量",
#endif

		"入力ボタン設定メニュー",
		"使用しない",
		FONT_UPARROW,
		FONT_DOWNARROW,
		FONT_LEFTARROW,
		FONT_RIGHTARROW,
		FONT_CIRCLE,
		FONT_CROSS,
		FONT_SQUARE,
		FONT_TRIANGLE,
		"Lトリガ",
		"Rトリガ",
		"START",
		"SELECT",
		"低",
		"標準",
		"高",
#if (EMU_SYSTEM == CPS2)
		"プレイヤー1 スタート",
		"プレイヤー2 スタート",
#endif
		"%dフレーム",
		"%dフレーム",
		FONT_UPARROW,
		FONT_DOWNARROW,
		FONT_LEFTARROW,
		FONT_RIGHTARROW,
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"ボタンA",
		"ボタンB",
		"ボタンC",
		"ボタンD",
#else
		"ボタン1",
		"ボタン2",
		"ボタン3",
		"ボタン4",
		"ボタン5",
		"ボタン6",
#endif
		"スタート",
#if (EMU_SYSTEM == NCDZ)
		"セレクト",
#else
		"コイン",
		"サービスコイン",
#endif
#if (EMU_SYSTEM == MVS)
		"テストスイッチ",
#elif (EMU_SYSTEM != NCDZ)
		"サービススイッチ",
#endif
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"ボタンA 連射",
		"ボタンB 連射",
		"ボタンC 連射",
		"ボタンD 連射",
#else
		"ボタン1 連射",
		"ボタン2 連射",
		"ボタン3 連射",
		"ボタン4 連射",
		"ボタン5 連射",
		"ボタン6 連射",
#endif
		"連射間隔",
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"AB 同時押し",
		"AC 同時押し",
		"AD 同時押し",
		"BC 同時押し",
		"BD 同時押し",
		"CD 同時押し",
		"ABC 同時押し",
		"ABD 同時押し",
		"ACD 同時押し",
		"BCD 同時押し",
		"ABCD 同時押し",
#else
		"1+2同時押し",
		"1+3同時押し",
		"1+4同時押し",
		"2+3同時押し",
		"2+4同時押し",
		"3+4同時押し",
		"1+2+3同時押し",
		"1+2+4同時押し",
		"1+3+4同時押し",
		"2+3+4同時押し",
		"1+2+3+4同時押し",
#endif
#if (EMU_SYSTEM != NCDZ)
		"アナログ感度",
#endif
#if (EMU_SYSTEM == CPS1)
		"ダイアル (左回転)",
		"ダイアル (右回転)",
		"ポーズ",
#endif
#if (EMU_SYSTEM == CPS2)
		"パドル (左回転)",
		"パドル (右回転)",
#endif
#if (EMU_SYSTEM == MVS)
		"ビッグ",
		"スモール",
		"ダブルアップ",
		"スタート/集計",
		"払い戻し",
		"キャンセル",
		"ベット",
		"全てにベット/全てキャンセル",
		"オペレータメニュー",
		"クレジットをクリア",
		"ホッパー取り出し",
#endif
		"スクリーンショット保存",
		"プレイヤー切り替え",
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"ボタン配置",
		"タイプ1 (NEO·GEOパッド)",
		"タイプ2 (MVS)",
		"タイプ3 (PlayStation)",
		"ユーザ定義",
#endif

		"ディップスイッチ設定メニュー",
		"このゲームにはディップスイッチはありません。",

		"セーブ/ロードステート",
		"スロット%d:",
		"データなし",
		"ロード",
		"削除",
		"セーブ",
		"プレイ日付",
		"セーブ時刻",
		"バージョン",
		"ファイル\"%s\"を削除できませんでした。",

		"メインメニュー",
		"ゲーム設定",
		"ゲームの動作設定を変更します。",
		"入力ボタン設定",
		"入力ボタンと連射/ホットキーの設定をします。",
#if (EMU_SYSTEM != NCDZ)
		"ディップスイッチ設定",
		"ディップスイッチの設定を変更します。",
#endif
		"エミュレーションをリセット",
		SYSTEM_NAME "のエミュレーションをリセットします。",
		"ファイルブラウザに戻る",
		"エミュレーションを終了し、ファイルブラウザに戻ります。",
		"ゲームに戻る",
		"ゲーム画面に戻ります。",
		"エミュレータを終了",
		"このソフトウェアを終了し、PSPのメニューに戻ります。",
#ifdef COMMAND_LIST
		"コマンドリスト表示",
		"このゲームのコマンド一覧を表示します。",
#endif

#if PSP_VIDEO_32BPP
		"カラー設定メニュー",
		"背景画像の種類",
		"背景画像の明るさ",
		"タイトルバーの文字",
		"選択された文字",
		"標準の文字",
		"情報メッセージの文字",
		"警告メッセージの文字",
		"ファイル選択バー(開始)",
		"ファイル選択バー(終了)",
		"タイトルバー/メッセージボックス",
		"タイトルバー/メッセージボックスの枠",
		"背景色(背景画像未使用時)",
		"赤輝度",
		"緑輝度",
		"青輝度",
		"標準の画像",
		"ユーザの画像",
		"logo.pngのみ",
		"ファイル選択バー",
#endif

		"コマンドリスト",

		/* psp/mp3.c */
#if (EMU_SYSTEM == NCDZ)
		"MP3ファイル\"%s\"を再オープンできません。",
		"MP3デコードエラーが発生しました。",
		"MP3再生用オーディオチャネルを確保できません。",
		"MP3スレッドを開始できません。",
#endif

		/* psp/adhoc.c*/
#ifdef ADHOC
		"ロビー",
		"サーバー",
		"クライアント",
		"他のPSPの接続を待機中。\n",
		"%sに接続中...",
		"接続しました。",
		"%sから切断中...",
		"切断しました。",
		"接続失敗",
		"接続するサーバーを選択。" FONT_TRIANGLE "を押すと中止します。\n",
		"%sの接続許可を待っています。\n",
		FONT_CROSS "を押すと中止します。\n",
		"%sが接続許可を要求しています。\n",
		FONT_CIRCLE "で接続を許可、" FONT_CROSS "で接続を拒否します。\n",
		"同期を待っています。",
#endif

		/* psp/png.c */
		"PNG画像用のメモリを確保できません。",
		"PNG画像を作成できません。",
#if PSP_VIDEO_32BPP || (EMU_SYSTEM == NCDZ)
		"PNG画像を展開できません。",
#endif
		"%dbitカラーのPNG画像には対応していません。",

		/* emumain.c */
		"致命的なエラー",
		"スクリーンショットを\"%s_%02d.png\"に保存しました。",

#if USE_CACHE
		/* common/cache.c */
		"キャッシュファイルをオープンできません。\n",
		"メモリが足りません。\n",
		"キャッシュメモリを確保できません。\n",
		"%dKBのキャッシュメモリを確保しました。\n",
		"キャッシュロード中にエラーが発生しました。\n",
		"キャッシュ情報データをロード中...\n",
		"サポートされていないバージョン\"V%c%c\"のキャッシュファイルです。\n",
		"現在要求されるキャッシュファイルのバージョンは\"" CACHE_VERSION "\"です。\n",
		"キャッシュファイルを作成しなおしてください。\n",
#if (EMU_SYSTEM == CPS2)
		"スプライトブロック%03xをオープンできません。\n",
#elif (EMU_SYSTEM == MVS)
		"PCMキャッシュを使用します。\n",
#endif
#endif

		/* common/loadrom.c */
		"何かボタンを押してください。\n",
		"メモリを確保できません。(REGION_%s)\n",
		"CRC32が正しくありません。\"%s\"\n",
		"ファイルが見つかりません。\"%s\"\n",

#ifdef SAVE_STATE
		/* common/state.c */
		"ステート用のバッファを確保できません。",
		"ファイル\"%s.sv%d\"を作成できません。",
		"ファイル\"%s.sv%d\"をオープンできません。",
		"\"%s.sv%d\"を保存中",
		"\"%s.sv%d\"をロード中",
#if (EMU_SYSTEM == MVS)
		"BIOSをロードできません。何かボタンを押すと終了します。",
#elif (EMU_SYSTEM == NCDZ)
		"ステートデータを圧縮できません。",
		"ステートデータを展開できません。",
#endif
#endif

#ifdef COMMAND_LIST
		/* common/cmdlist.c */
		"このゲーム用のコマンドリストがありません。",
		"コマンドリスト - %s",
		"%d/%d 項目",
		"COMMAND.DAT サイズ縮小処理",
		"このエミュレータで対応していないゲームのコマンドリストを削除し、\n",
		"COMMAND.DATのファイルサイズを縮小します。\n",
		"処理を行う場合は、" FONT_CIRCLE "ボタンを押してください。\n",
		FONT_CROSS "ボタンを押すと処理を中止してファイルブラウザに戻ります。\n",
		"COMMAND.DAT を検査中...\n",
		"エラー: 未対応の\"COMMAND.DAT\"かファイルの形式が異なります。\n",
		"エラー: 空のファイルです。\n",
		"エラー: メモリを確保できません。\n",
		"エラー: ファイルをリネームできません。\n",
		"エラー: 出力ファイルを作成できません。\n",
		"\"%s\" のコマンドをコピー中...\n",
		"サイズ %dbytes → %dbytes (%.1f%%減)\n",
#endif

		/* emulation core */
		"完了しました。\n",
		"エミュレーションの終了",
		"しばらくお待ちください。\n",

		/* inptport.c */
		"コントローラー: プレイヤー1",
		"コントローラー: プレイヤー%d",
#ifdef ADHOC
		"同期を失いました。\n",
		"%sによるポーズ",
		"ゲームを再開",
		"切断して終了",
#endif

		/* memintrf.c */
		"ロード中 \"%s\"\n",
		"ROMのロード",
#if (EMU_SYSTEM != NCDZ)
		"ROM情報をチェック中...\n",
		"このゲームはサポートされていません。\n",
		"ROMが見つかりません。(zipファイル名が正しくない)\n",
		"\"%s\"のドライバがありません。\n",
		"ROMセット名\"%s\" (親セット名: %s)\n",
		"ROMセット名\"%s\"\n",
#endif

#if (EMU_SYSTEM == CPS1)

		/* memintrf.c */
		"rominfo.cps1が見つかりません。\n",
		"メモリを確保できません。(0x8000バイト)",

#elif (EMU_SYSTEM == CPS2)

		/* cps2crpt.c */
		"復号処理を実行中 %d%%\r",
		"復号処理を実行中 100%%\n",

		/* memintrf.c */
		"rominfo.cps2が見つかりません。\n",

#ifdef PSP_SLIM
		/* vidhrdw.c */
		"グラフィックデータを展開中...\n",
#endif

#elif (EMU_SYSTEM == MVS)

		/* biosmenu.c */
		"BIOS選択メニュー",
		"BIOSが見つかりません。",
		"NVRAMファイルは全て削除されました。\n",

		/* memintrf.c */
		"rominfo.mvsが見つかりません。\n",
		"ロード中 \"%s (%s)\"\n",
		"復号済みGFX2 ROMをロード中...\n",
		"復号済みSOUND1 ROMをロード中...\n",
		"ROM復号用のメモリを確保できません。\n",
		"スプライトデータ用のメモリを確保できません。\n",
		"スプライトキャッシュの使用を試みます。\n",
		"BIOSをチェック中...\n",

#elif (EMU_SYSTEM == NCDZ)

		/* ncdz.c */
		"IPL.TXT処理中にエラーが発生しました。",
		"ウォッチドッグカウンタによるリセットが発生しました。",

		/* cdrom.c */
		"ファイル\"%s\"をオープンできません。",
		"CD-ROMのステートデータを追加できません。",

		/* vidhrdw.c */
		"CD-ROM速度制限: 無効",
		"CD-ROM速度制限: 有効",

		/* memintrf.c */
		"ゲームIDをチェック中...\n",
#endif
		"チートメニュー",
		"チート",
		"チートを選択します。",
		"このゲーム用のチートがありません。",
/*
		"メモリ解放",
		"メモリ解放設定を変更します。",*/
		NULL
	},
	{
		"\0",
		"\n",

		/* psp/filer.c */
		"请稍等...",
		"无法打开zipname.%s",
#ifdef ADHOC
		"请打开WLAN开关",
		"联机模块读取失败.",
#endif

		/* psp/sound.c */
		"无法确认音频",
		"无法启动音频线程",

		/* psp/ui.c */
		"警告:电量不足(%d%%)，请充电!",

		FONT_CIRCLE " 确定  " FONT_CROSS " 取消",
		FONT_CIRCLE " 确定  " FONT_CROSS " 取消",
#ifdef SAVE_STATE
		FONT_CIRCLE " 返回游戏  " FONT_CROSS " 返回菜单",
#endif
		"请按任意键",
		"开始模拟",
#ifdef ADHOC
		"开始模拟 (联机)",
#endif
		"退出模拟器",
		"重启模拟器",
		"需要重启模拟器",
#if (EMU_SYSTEM != NCDZ)
		"此游戏无法运行",
		"无法正常运行",
		"请勿烦躁",
#endif
		"是否将此文件夹设置为开始目录?",
#ifdef PSP_SLIM
		"此软件需要PSP-2000或以上的机型+3.71 M33或更高系统",
#endif
#ifdef SAVE_STATE
		"开始即时存档",
		"开始即时读档",
#endif
#if defined(SAVE_STATE) || defined(COMMAND_LIST)
		"完成",
#endif
#ifdef SAVE_STATE
		"删除存档文件",
#endif
#if (EMU_SYSTEM == NCDZ)
		"没有找到MP3文件，无法播放CDDA",
		"没有找到IPL.TXT",
		"启动NEO·GEO CDZ BIOS",
		"没有找到NEO·GEO CDZ BIOS (neocd.bin)",
		"无效的NEO·GEO CDZ BIOS",
		"无法运行游戏",
#endif

		"帮助 - %s",
		"未使用",
		"请按任意键返回菜单",
		"游戏目录",
		"移动",
		"翻页",
		"运行游戏",
		"运行游戏 (联机)",
		"退出模拟器",
		"显示BIOS选择菜单",
		"打开颜色管理菜单",
		"显示帮助",
		"启动BIOS",
		"设置为开始目录",
		"更改数值",
		"选择",
		"选择存档",
		"切换功能",
		"执行",
		"选择项目",
		"选择项目/更改数值",
		"RGB数值+5",
		"返回游戏目录",
		"RGB数值-5",
		"所有数据恢复默认",
		"移动/选择项目",
		"打开/关闭项目菜单",
		"返回主菜单/游戏",

		/* psp/ui_menu.c */
		"开",
		"关",
		"是",
		"否",
		"开",
		"关",
		"返回主菜单",
		"游戏设置菜单",
		"光栅效果",
		"画面缩放",
#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
		"360x270 (4:3)",
		"384x270 (24:17)",
		"466x272 (12:7)",
		"480x270 (16:9)",
		"画面旋转",
#else
		"348x224 (14:9)",
		"398x224 (16:9)",
		"360x270 (4:3)",
		"420x270 (14:9)",
		"480x270 (16:9)",
#endif
		"垂直同步",
		"自动跳帧",
		"跳帧级别",
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
		"显示帧数",
		"限制60帧",
		"开启声音",
		"采样率",
		"11025Hz",
		"22050Hz",
		"44100Hz",
		"音量",
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
		"操作者",
		"玩家1",
		"玩家2",
#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
		"玩家3",
		"玩家4",
#endif
		"PSP频率",
		"222MHz",
		"266MHz",
		"300MHz",
		"333MHz",
#if (EMU_SYSTEM == MVS)
		"默认",
#endif
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"机器区域",
		"日版",
		"美版",
		"欧版",
#endif
#if (EMU_SYSTEM == MVS)
		"机器模式",
		"家用机 (AES)",
		"街机 (MVS)",
#endif
#if (EMU_SYSTEM == NCDZ)
		"模拟读取画面",
		"CD-ROM速度限制",
		"启用CDDA",
		"CDDA音量",
#endif

		"按键设置菜单",
		"未使用",
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
		"低",
		"中",
		"高",
#if (EMU_SYSTEM == CPS2)
		"玩家1 开始",
		"玩家2 开始",
#endif
		"%d",
		"%d",
		FONT_UPARROW,
		FONT_DOWNARROW,
		FONT_LEFTARROW,
		FONT_RIGHTARROW,
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"按键A",
		"按键B",
		"按键C",
		"按键D",
#else
		"按键1",
		"按键2",
		"按键3",
		"按键4",
		"按键5",
		"按键6",
#endif
		"开始",
#if (EMU_SYSTEM == NCDZ)
		"Select",
#else
		"投币",
		"调试投币",
#endif
#if (EMU_SYSTEM == MVS)
		"测试开关",
#elif (EMU_SYSTEM != NCDZ)
		"调试开关",
#endif
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"连发A",
		"连发B",
		"连发C",
		"连发D",
#else
		"连发1",
		"连发2",
		"连发3",
		"连发4",
		"连发5",
		"连发6",
#endif
		"连发间隔",
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"热键A+B",
		"热键A+C",
		"热键A+D",
		"热键B+C",
		"热键B+D",
		"热键C+D",
		"热键A+B+C",
		"热键A+B+D",
		"热键A+C+D",
		"热键B+C+D",
		"热键A+B+C+D",
#else
		"热键1+2",
		"热键1+3",
		"热键1+4",
		"热键2+3",
		"热键2+4",
		"热键3+4",
		"热键1+2+3",
		"热键1+2+4",
		"热键1+3+4",
		"热键2+3+4",
		"热键1+2+3+4",
#endif
#if (EMU_SYSTEM != NCDZ)
		"摇杆灵敏度",
#endif
#if (EMU_SYSTEM == CPS1)
		"Dial (左转)",
		"Dial (右转)",
		"Pause",
#endif
#if (EMU_SYSTEM == CPS2)
		"Paddle (左转)",
		"Paddle (右转)",
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
		"保存截图",
		"切换玩家",
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"按键模式",
		"类型1 (NEOGEO按键)",
		"类型2 (街机)",
		"类型3 (PlayStation)",
		"自定",
#endif

		"DIP开关设置菜单",
		"此游戏没有DIP开关",

		"即时存档/读档",
		"存档%d:",
		"空",
		"读取",
		"删除",
		"保存",
		"游玩日期",
		"保存时间",
		"版本",
		"删除\"%s\"失败",

		"主菜单",
		"游戏设置",
		"更改游戏的设置",
		"按键设置",
		"更改按键和连发/热键的设置",
#if (EMU_SYSTEM != NCDZ)
		"DIP开关设置",
		"更改硬件上的DIP开关设置",
#endif
		"重启模拟器",
		"重启"SYSTEM_NAME"模拟器",
		"返回游戏目录",
		"停止模拟器并返回游戏目录",
		"返回游戏",
		"返回模拟器继续游戏",
		"退出模拟器",
		"退出模拟器并返回PSP菜单",
#ifdef COMMAND_LIST
		"显示操作说明",
		"显示操作说明 (MAME Plus格式的\"command.dat\")",
#endif

#if PSP_VIDEO_32BPP
		"颜色管理菜单",
		"背景图类型",
		"背景图亮度",
		"标题框文字",
		"选定文字",
		"普通文字",
		"消息文字",
		"警告文字",
		"文件选择框 (头)",
		"文件选择框 (尾)",
		"标题框/信息框",
		"标题外框/信息外框",
		"背景",
		"红",
		"绿",
		"蓝",
		"默认图像",
		"用户图像",
		"logo.png",
		"文件选择框",
#endif

		"操作说明",

		/* psp/mp3.c */
#if (EMU_SYSTEM == NCDZ)
		"无法重新打开\"%s\"",
		"MP3解码错误",
		"无法确认MP3音频",
		"无法启动MP3线程",
#endif

		/* psp/adhoc.c*/
#ifdef ADHOC
		"大厅",
		"服务器",
		"客户端",
		"等待其他PSP加入\n",
		"正在连接%s",
		"已连接",
		"已从%s断开连接",
		"断开连接",
		"失败",
		"选择一个服务器连接，或按"FONT_TRIANGLE"键返回\n",
		"正在等待%s同意连接\n",
		"按"FONT_CROSS"键取消\n",
		"%s请求连接中\n",
		"按"FONT_CIRCLE"键同意连接，按"FONT_CROSS"键取消\n",
		"等待同步中",
#endif

		/* psp/png.c */
		"无法为PNG分配内存",
		"无法圧缩PNG图像",
#if PSP_VIDEO_32BPP || (EMU_SYSTEM == NCDZ)
		"无法解圧PNG图像",
#endif
		"不支持%d位颜色的PNG图",

		/* emumain.c */
		"严重错误",
		"截图保存为\"%s_%02d.png\"",

#if USE_CACHE
		/* common/cache.c */
		"无法打开缓存文件\n",
		"内存不足\n",
		"无法分配缓存\n",
		"已分配%dKB缓存\n",
		"缓存读取错误!!!\n",
		"正在读取缓存信息数据\n",
		"不支持\"V%c%c\"版的缓存文件\n",
		"目前需要\""CACHE_VERSION"\"版\n",
		"请重新转换缓存文件\n",
#if (EMU_SYSTEM == CPS2)
		"无法打开图像块%03x\n",
#elif (EMU_SYSTEM == MVS)
		"PCM缓存已开启\n",
#endif
#endif

		/* common/loadrom.c */
		"请按任意键\n",
		"无法分配%s内存\n",
		"CRC32不正确\"%s\"\n",
		"没有找到\"%s\"\n",

#ifdef SAVE_STATE
		/* common/state.c */
		"无法分配缓冲区",
		"无法生成\"%s.sv%d\"",
		"无法打开\"%s.sv%d\"",
		"正在保存\"%s.sv%d\"",
		"正在读取\"%s.sv%d\"",
#if (EMU_SYSTEM == MVS)
		"无法重新读取BIOS，请按任意键退出",
#elif (EMU_SYSTEM == NCDZ)
		"无法圧缩即时存档数据",
		"无法解圧即时存档数据",
#endif
#endif

#ifdef COMMAND_LIST
		/* common/cmdlist.c */
		"没有发现此游戏的操作说明",
		"操作说明 - %s",
		"%d/%d",
		"缩减COMMAND.DAT容量",
		"此操作会删除COMMAND.DAT中没有使用到的游戏操作说明\n",
		"此模拟器支持的游戏操作说明不会被删除\n",
		"如果想缩减command.dat的体积，请按"FONT_CIRCLE"键\n",
		"不想缩减请按"FONT_CROSS"键返回游戏目录\n",
		"正在检查COMMAND.DAT格式\n",
		"错误: 未知格式\n",
		"错误: 空文件\n",
		"错误: 无法分配内存\n",
		"错误: 无法重命名\n",
		"错误: 无法生成输出文件\n",
		"正在复制\"%s\"\n",
		"原始容量:%d字节, 缩减后:%d字节 (-%.1f%%)\n",
#endif

		/* emulation core */
		"完成\n",
		"退出模拟器",
		"请稍等\n",

		/* inptport.c */
		"控制者: 玩家1",
		"控制者: 玩家%d",
#ifdef ADHOC
		"同步失败\n",
		"%s已暂停",
		"返回游戏",
		"断开连接",
#endif

		/* memintrf.c */
		"正在读取\"%s\"\n",
		"读取游戏ROM",
#if (EMU_SYSTEM != NCDZ)
		"正在检查ROM信息\n",
		"不支持此游戏\n",
		"没有找到ROM (zip文件名不正确)\n",
		"没有找到\"%s\"的驱动\n",
		"ROM名称\"%s\" (主ROM名: %s)\n",
		"ROM名称\"%s\"\n",
#endif

#if (EMU_SYSTEM == CPS1)

		/* memintrf.c */
		"没有找到rominfo.cps1\n",
		"无法分配内存 (0x8000 bytes)",

#elif (EMU_SYSTEM == CPS2)

		/* cps2crpt.c */
		"正在解密，进度%d%%\r",
		"正在解密，进度100%%\n",

		/* memintrf.c */
		"没有找到rominfo.cps2\n",

#ifdef PSP_SLIM
		/* vidhrdw.c */
		"正在解密图像数据\n",
#endif

#elif (EMU_SYSTEM == MVS)

		/* biosmenu.c */
		"BIOS选择菜单",
		"没有找到BIOS",
		"所有NVRAM文件已被删除\n",

		/* memintrf.c */
		"没有找到rominfo.mvs\n",
		"正在读取\"%s (%s)\"\n",
		"正在读取GFX2解密ROM\n",
		"正在读取SOUND1解密ROM\n",
		"无法为解密ROM分配内存\n",
		"无法为图像数据分配内存\n",
		"尝试使用图像缓存\n",
		"正在检查BIOS\n",

#elif (EMU_SYSTEM == NCDZ)

		/* ncdz.c */
		"处理IPL.TXT出错",
		"重置原因:检测失败",

		/* cdrom.c */
		"无法打开\"%s\"",
		"无法导入CD-ROM数据",

		/* vidhrdw.c */
		"CD-ROM速度限制:关",
		"CD-ROM速度限制:开",

		/* memintrf.c */
		"正在检查游戏ID\n",
#endif
		"金手指菜单",
		"金手指",
		"选择金手指代码",
		"没有发现此游戏的金手指",
/*
		"内存释放",
		"更改内存释放的设置",*/
		NULL
	},
	{
		"\0",
		"\n",

		/* psp/filer.c */
		"請稍等...",
		"無法打開zipname.%s",
#ifdef ADHOC
		"請打開WLAN開關",
		"聯機模塊讀取失敗.",
#endif

		/* psp/sound.c */
		"無法確認音頻",
		"無法啟動音頻線程",

		/* psp/ui.c */
		"警告:電量不足(%d%%)，請充電!",

		FONT_CIRCLE " 確定  " FONT_CROSS " 取消",
		FONT_CIRCLE " 確定  " FONT_CROSS " 取消",
#ifdef SAVE_STATE
		FONT_CIRCLE " 返回遊戲  " FONT_CROSS " 返回菜單",
#endif
		"請按任意鍵",
		"開始模擬",
#ifdef ADHOC
		"開始模擬 (聯機)",
#endif
		"退出模擬器",
		"重啟模擬器",
		"需要重啟模擬器",
#if (EMU_SYSTEM != NCDZ)
		"此遊戲無法運\行",
		"無法正常運\行",
		"請勿煩躁",
#endif
		"是否將此文件夾設置為開始目錄?",
#ifdef PSP_SLIM
		"此軟件需要PSP-2000或以上的機型+3.71 M33或更高系統",
#endif
#ifdef SAVE_STATE
		"開始即時存檔",
		"開始即時讀檔",
#endif
#if defined(SAVE_STATE) || defined(COMMAND_LIST)
		"完成",
#endif
#ifdef SAVE_STATE
		"刪除存檔文件",
#endif
#if (EMU_SYSTEM == NCDZ)
		"沒有找到MP3文件，無法播放CDDA",
		"沒有找到IPL.TXT",
		"啟動NEO·GEO CDZ BIOS",
		"沒有找到NEO·GEO CDZ BIOS (neocd.bin)",
		"無效的NEO·GEO CDZ BIOS",
		"無法運\行遊戲",
#endif

		"幫助 - %s",
		"未使用",
		"請按任意鍵返回菜單",
		"遊戲目錄",
		"移動",
		"翻頁",
		"運\行遊戲",
		"運\行遊戲 (聯機)",
		"退出模擬器",
		"顯示BIOS選擇菜單",
		"打開顏色管理菜單",
		"顯示幫助",
		"啟動BIOS",
		"設置為開始目錄",
		"更改數值",
		"選擇",
		"選擇存檔",
		"切換",
		"執行",
		"選擇項目",
		"選擇項目/更改數值",
		"RGB數值+5",
		"返回遊戲目錄",
		"RGB數值-5",
		"所有數據恢復默認",
		"移動/選擇項目",
		"打開/關閉項目菜單",
		"返回主菜單/遊戲",

		/* psp/ui_menu.c */
		"開",
		"關",
		"是",
		"否",
		"開",
		"關",
		"返回主菜單",
		"遊戲設置菜單",
		"光柵效果",
		"畫面縮放",
#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
		"360x270 (4:3)",
		"384x270 (24:17)",
		"466x272 (12:7)",
		"480x270 (16:9)",
		"畫面旋轉",
#else
		"348x224 (14:9)",
		"398x224 (16:9)",
		"360x270 (4:3)",
		"420x270 (14:9)",
		"480x270 (16:9)",
#endif
		"垂直同步",
		"自動跳幀",
		"跳幀級別",
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
		"顯示幀數",
		"限制60幀",
		"開啟聲音",
		"採樣率",
		"11025Hz",
		"22050Hz",
		"44100Hz",
		"音量",
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
		"操作者",
		"玩家1",
		"玩家2",
#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
		"玩家3",
		"玩家4",
#endif
		"PSP頻率",
		"222MHz",
		"266MHz",
		"300MHz",
		"333MHz",
#if (EMU_SYSTEM == MVS)
		"默認",
#endif
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"機器區域",
		"日版",
		"美版",
		"歐版",
#endif
#if (EMU_SYSTEM == MVS)
		"機器模式",
		"家用機 (AES)",
		"街機 (MVS)",
#endif
#if (EMU_SYSTEM == NCDZ)
		"模擬讀取畫面",
		"CD-ROM速度限制",
		"啟用CDDA",
		"CDDA音量",
#endif

		"按鍵設置菜單",
		"未使用",
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
		"低",
		"中",
		"高",
#if (EMU_SYSTEM == CPS2)
		"玩家1 開始",
		"玩家2 開始",
#endif
		"%d",
		"%d",
		FONT_UPARROW,
		FONT_DOWNARROW,
		FONT_LEFTARROW,
		FONT_RIGHTARROW,
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"按鍵A",
		"按鍵B",
		"按鍵C",
		"按鍵D",
#else
		"按鍵1",
		"按鍵2",
		"按鍵3",
		"按鍵4",
		"按鍵5",
		"按鍵6",
#endif
		"開始",
#if (EMU_SYSTEM == NCDZ)
		"Select",
#else
		"投幣",
		"調試投幣",
#endif
#if (EMU_SYSTEM == MVS)
		"測試開關",
#elif (EMU_SYSTEM != NCDZ)
		"調試開關",
#endif
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"連發A",
		"連發B",
		"連發C",
		"連發D",
#else
		"連發1",
		"連發2",
		"連發3",
		"連發4",
		"連發5",
		"連發6",
#endif
		"連發間隔",
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"熱鍵A+B",
		"熱鍵A+C",
		"熱鍵A+D",
		"熱鍵B+C",
		"熱鍵B+D",
		"熱鍵C+D",
		"熱鍵A+B+C",
		"熱鍵A+B+D",
		"熱鍵A+C+D",
		"熱鍵B+C+D",
		"熱鍵A+B+C+D",
#else
		"熱鍵1+2",
		"熱鍵1+3",
		"熱鍵1+4",
		"熱鍵2+3",
		"熱鍵2+4",
		"熱鍵3+4",
		"熱鍵1+2+3",
		"熱鍵1+2+4",
		"熱鍵1+3+4",
		"熱鍵2+3+4",
		"熱鍵1+2+3+4",
#endif
#if (EMU_SYSTEM != NCDZ)
		"搖桿靈敏度",
#endif
#if (EMU_SYSTEM == CPS1)
		"Dial (左轉)",
		"Dial (右轉)",
		"Pause",
#endif
#if (EMU_SYSTEM == CPS2)
		"Paddle (左轉)",
		"Paddle (右轉)",
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
		"保存截圖",
		"切換玩家",
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"按鍵模式",
		"類型1 (NEOGEO按鍵)",
		"類型2 (街機)",
		"類型3 (PlayStation)",
		"自定",
#endif

		"DIP開關設置菜單",
		"此遊戲沒有DIP開關",

		"即時存檔/讀檔",
		"存檔%d:",
		"空",
		"讀取",
		"刪除",
		"保存",
		"遊玩日期",
		"保存時間",
		"版本",
		"刪除\"%s\"失敗",

		"主菜單",
		"遊戲設置",
		"更改遊戲的設置",
		"按鍵設置",
		"更改按鍵和連發/熱鍵的設置",
#if (EMU_SYSTEM != NCDZ)
		"DIP開關設置",
		"更改硬件上的DIP開關設置",
#endif
		"重啟模擬器",
		"重啟"SYSTEM_NAME"模擬器",
		"返回遊戲目錄",
		"停止模擬器並返回遊戲目錄",
		"返回遊戲",
		"返回模擬器繼續遊戲",
		"退出模擬器",
		"退出模擬器並返回PSP菜單",
#ifdef COMMAND_LIST
		"顯示操作說明",
		"顯示操作說明 (MAME Plus格式的\"command.dat\")",
#endif

#if PSP_VIDEO_32BPP
		"顏色管理菜單",
		"背景圖類型",
		"背景圖亮度",
		"標題框文字",
		"選定文字",
		"普通文字",
		"消息文字",
		"警告文字",
		"文件選擇框 (頭)",
		"文件選擇框 (尾)",
		"標題框/信息框",
		"標題外框/信息外框",
		"背景",
		"紅",
		"綠",
		"藍",
		"默認圖像",
		"用戶圖像",
		"logo.png",
		"文件選擇框",
#endif

		"操作說明",

		/* psp/mp3.c */
#if (EMU_SYSTEM == NCDZ)
		"無法重新打開\"%s\"",
		"MP3解碼錯誤",
		"無法確認MP3音頻",
		"無法啟動MP3線程",
#endif

		/* psp/adhoc.c*/
#ifdef ADHOC
		"大廳",
		"服務器",
		"客戶端",
		"等待其他PSP加入\n",
		"正在連接%s",
		"已連接",
		"已從%s斷開連接",
		"斷開連接",
		"失敗",
		"選擇一個服務器連接，或按"FONT_TRIANGLE"鍵返回\n",
		"正在等待%s同意連接\n",
		"按"FONT_CROSS"鍵取消\n",
		"%s請求連接中\n",
		"按"FONT_CIRCLE"鍵同意連接，按"FONT_CROSS"鍵取消\n",
		"等待同步中",
#endif

		/* psp/png.c */
		"無法為PNG分配內存",
		"無法壓縮PNG圖像",
#if PSP_VIDEO_32BPP || (EMU_SYSTEM == NCDZ)
		"無法解壓PNG圖像",
#endif
		"不支持%d位顏色的PNG圖",

		/* emumain.c */
		"嚴重錯誤",
		"截圖保存為\"%s_%02d.png\"",

#if USE_CACHE
		/* common/cache.c */
		"無法打開緩存文件\n",
		"內存不足\n",
		"無法分配緩存\n",
		"已分配%dKB緩存\n",
		"緩存讀取錯誤!!!\n",
		"正在讀取緩存信息數據\n",
		"不支持\"V%c%c\"版的緩存文件\n",
		"目前需要\""CACHE_VERSION"\"版\n",
		"請重新轉換緩存文件\n",
#if (EMU_SYSTEM == CPS2)
		"無法打開圖像塊%03x\n",
#elif (EMU_SYSTEM == MVS)
		"PCM緩存已開啟\n",
#endif
#endif

		/* common/loadrom.c */
		"請按任意鍵\n",
		"無法分配%s內存\n",
		"CRC32不正確\"%s\"\n",
		"沒有找到\"%s\"\n",

#ifdef SAVE_STATE
		/* common/state.c */
		"無法分配緩衝區",
		"無法生成\"%s.sv%d\"",
		"無法打開\"%s.sv%d\"",
		"正在保存\"%s.sv%d\"",
		"正在讀取\"%s.sv%d\"",
#if (EMU_SYSTEM == MVS)
		"無法重新讀取BIOS，請按任意鍵退出",
#elif (EMU_SYSTEM == NCDZ)
		"無法壓縮即時存檔數據",
		"無法解壓即時存檔數據",
#endif
#endif

#ifdef COMMAND_LIST
		/* common/cmdlist.c */
		"沒有發現此遊戲的操作說明",
		"操作說明 - %s",
		"%d/%d",
		"縮減COMMAND.DAT容量",
		"此操作會刪除COMMAND.DAT中沒有使用到的遊戲操作說明\n",
		"此模擬器支持的遊戲操作說明不會被刪除\n",
		"如果想縮減command.dat的體積，請按"FONT_CIRCLE"鍵\n",
		"不想縮減請按"FONT_CROSS"鍵返回遊戲目錄\n",
		"正在檢查COMMAND.DAT格式\n",
		"錯誤: 未知格式\n",
		"錯誤: 空文件\n",
		"錯誤: 無法分配內存\n",
		"錯誤: 無法重命名\n",
		"錯誤: 無法生成輸出文件\n",
		"正在複製\"%s\"\n",
		"原始容量:%d字節, 縮減後:%d字節 (-%.1f%%)\n",
#endif

		/* emulation core */
		"完成\n",
		"退出模擬器",
		"請稍等\n",

		/* inptport.c */
		"控制者: 玩家1",
		"控制者: 玩家%d",
#ifdef ADHOC
		"同步失敗\n",
		"%s已暫停",
		"返回遊戲",
		"斷開連接",
#endif

		/* memintrf.c */
		"正在讀取\"%s\"\n",
		"讀取遊戲ROM",
#if (EMU_SYSTEM != NCDZ)
		"正在檢查ROM信息\n",
		"不支持此遊戲\n",
		"沒有找到ROM (zip文件名不正確)\n",
		"沒有找到\"%s\"的驅動\n",
		"ROM名稱\"%s\" (主ROM名: %s)\n",
		"ROM名稱\"%s\"\n",
#endif

#if (EMU_SYSTEM == CPS1)

		/* memintrf.c */
		"沒有找到rominfo.cps1\n",
		"無法分配內存 (0x8000 bytes)",

#elif (EMU_SYSTEM == CPS2)

		/* cps2crpt.c */
		"正在解密，進度%d%%\r",
		"正在解密，進度100%%\n",

		/* memintrf.c */
		"沒有找到rominfo.cps2\n",

#ifdef PSP_SLIM
		/* vidhrdw.c */
		"正在解密圖像數據\n",
#endif

#elif (EMU_SYSTEM == MVS)

		/* biosmenu.c */
		"BIOS選擇菜單",
		"沒有找到BIOS",
		"所有NVRAM文件已被刪除\n",

		/* memintrf.c */
		"沒有找到rominfo.mvs\n",
		"正在讀取\"%s (%s)\"\n",
		"正在讀取GFX2解密ROM\n",
		"正在讀取SOUND1解密ROM\n",
		"無法為解密ROM分配內存\n",
		"無法為圖像數據分配內存\n",
		"嘗試使用圖像緩存\n",
		"正在檢查BIOS\n",

#elif (EMU_SYSTEM == NCDZ)

		/* ncdz.c */
		"處理IPL.TXT出錯",
		"重置原因:檢測失敗",

		/* cdrom.c */
		"無法打開\"%s\"",
		"無法導入CD-ROM數據",

		/* vidhrdw.c */
		"CD-ROM速度限制:關",
		"CD-ROM速度限制:開",

		/* memintrf.c */
		"正在檢查遊戲ID\n",
#endif
		"金手指菜單",
		"金手指",
		"選擇金手指代碼",
		"沒有發現此遊戲的金手指",
/*
		"內存釋放",
		"更改內存釋放的設置",*/
		NULL
	}
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
    if(lang == PSP_SYSTEMPARAM_LANGUAGE_ENGLISH)
		lang = 0;
	else if(lang == PSP_SYSTEMPARAM_LANGUAGE_JAPANESE)
		lang = 1;
    else if(lang == PSP_SYSTEMPARAM_LANGUAGE_CHINESE_SIMPLIFIED)
		lang = 2;
    else if(lang == PSP_SYSTEMPARAM_LANGUAGE_CHINESE_TRADITIONAL)
		lang = 3;
    else if(lang == PSP_SYSTEMPARAM_LANGUAGE_FRENCH)
		lang = 0;
    else if(lang == PSP_SYSTEMPARAM_LANGUAGE_SPANISH)
		lang = 0;
    else if(lang == PSP_SYSTEMPARAM_LANGUAGE_GERMAN)
		lang = 0;
    else if(lang == PSP_SYSTEMPARAM_LANGUAGE_ITALIAN)
		lang = 0;
    else if(lang == PSP_SYSTEMPARAM_LANGUAGE_DUTCH)
		lang = 0;
    else if(lang == PSP_SYSTEMPARAM_LANGUAGE_PORTUGUESE)
		lang = 0;
    else if(lang == PSP_SYSTEMPARAM_LANGUAGE_RUSSIAN)
		lang = 0;
    else if(lang == PSP_SYSTEMPARAM_LANGUAGE_KOREAN)
		lang = 0;
	for (i = 0; i < UI_TEXT_MAX; i++)
		ui_text[i] = text[lang][i];
}

int ui_text_get_language(void)
{
	return lang;
}
