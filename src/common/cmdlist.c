/******************************************************************************

	cmdlist.c

	MAME Plus!形式コマンドリストビューア

******************************************************************************/

#include "emumain.h"


/******************************************************************************
	定数/マクロ
******************************************************************************/

#define gbk1(c)	(((c) >= 0x81 && (c) <= 0xfe))
#define gbk2(c)	((c) >= 0x40 && (c) <= 0xfe && (c) != 0x7f && (c) != 0xff)

#define _CR		0x0d
#define _LF		0x0a
#define _CRLF	"\x0d\x0a"

enum
{
	LF_WIN = 0,
	LF_MAC,
	LF_UNIX,
	LF_MAX
};

enum
{
	TAG_INFO = 0,
	TAG_CMD,
	TAG_END,
	TAG_CHARSET,
	TAG_MAX
};


/******************************************************************************
	ローカル構造体/変数
******************************************************************************/

typedef struct cmdlist_t
{
	int lines;
	char **line;
} CMDLIST;

static CMDLIST **cmd;

static const char *tag_str[TAG_MAX] =
{
	"info",
	"cmd",
	"end",
	"charset"
};

static int lf_code;
static int charset;

static char *cmdbuf;
static char **cmdline;
static int num_items, show_items, rows_item, sel_item, prev_item, top_item;
static int num_lines, show_lines, rows_line, sel_line, prev_line;
static int item_sx;
static int menu_open;


/******************************************************************************
	コマンドリストビューア
******************************************************************************/

/*--------------------------------------------------------
	ラインバッファからタグを取得
--------------------------------------------------------*/

static int cmdlist_get_tag(char *buf)
{
	int i;

	for (i = 0; i < TAG_MAX; i++)
		if (strncasecmp(&buf[1], tag_str[i], strlen(tag_str[i])) == 0)
			return i;

	return -1;
}


/*--------------------------------------------------------
	ラインバッファから値を取得
--------------------------------------------------------*/

static char *cmdlist_get_value(char *buf)
{
	if (strtok(buf, " =\r\n\t") != NULL)
		return strtok(NULL, " =\r\n\t");

	return NULL;
}


/*--------------------------------------------------------
	テキストの改行コードを判別
--------------------------------------------------------*/

static int check_linefeed_code(const char *buf)
{
	if (strstr(buf, _CRLF) != NULL)
	{
		// Windows / MS-DOS
		return LF_WIN;
	}
	else if (strchr(buf, _CR) != NULL)
	{
		// Mac
		return LF_MAC;
	}

	// Unix
	return LF_UNIX;
}


/*--------------------------------------------------------
	テキストの文字コードを判別
--------------------------------------------------------*/

static int check_text_encode(char *buf, int size)
{
	int i;
	int count1, count2, per;
	uint8_t *p = (uint8_t *)buf;

	count1 = 0;
	count2 = 0;

	for (i= 0; i < size;)
	{
		if (p[i] <= 0x20)
		{
			count2++;
			i++;
		}
		else if (p[i] == 0x97 && p[i + 1] == 0x97)
		{
			count2 += 2;
			i += 2;
		}
		else if (gbk1(p[i]) && gbk2(p[i + 1]))
		{
			count1++;
			i += 2;
		}
		else i++;
	}

	per = (count1 * 100) / (size - count2);

	if (per > 20)
		return CHARSET_GBK;
	else
		return CHARSET_LATIN1;
}


/*--------------------------------------------------------
	コマンドリストのロード
--------------------------------------------------------*/

void load_commandlist(const char *game_name, const char *parent_name)
{
	FILE *fp;
	char path[MAX_PATH];
	char lf, *p, *buf, linebuf[512];//256
	const char *name = game_name;
	int i, found, line, item;
	int size, start, end, pos;

	cmdbuf    = NULL;
	cmdline   = NULL;
	cmd       = NULL;
	charset   = CHARSET_DEFAULT;
	num_lines = 0;
	num_items = 0;

	sprintf(path, "%scommand.dat", launchDir);

	if ((fp = fopen(path, "rb")) == NULL)
		return;

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if (size == 0 || (buf = (char *)malloc(size)) == NULL)
	{
		fclose(fp);
		return;
	}

	fread(buf, 1, size, fp);
	fclose(fp);

	// 改行コードチェック
	lf_code = check_linefeed_code(linebuf);
	if (lf_code == LF_MAC)
		lf = _CR;
	else
		lf = _LF;

	// コマンド解析開始
retry:
	found = 0;
	line  = 0;
	start = 0;
	end   = 0;
	pos   = 0;

	i = 0;
	while (i < size)
	{
		p = &buf[i];

		while (buf[i] != lf && buf[i] != '\0' && buf[i] != EOF)
			i++;

		buf[i++] = '\0';

		strcpy(linebuf, p);
		strcat(linebuf, "\n");

		if (linebuf[0] == '$')
		{
			switch (cmdlist_get_tag(linebuf))
			{
			case TAG_INFO:
				if (found && start && end)
				{
					// 正常終了
					found = 2;
				}
				else if (found)
				{
					// コマンドが無いか、書式が異常
					found = 0;
				}
				else if ((p = cmdlist_get_value(linebuf)) != NULL)
				{
					char *name2 = strtok(p, ",");

					do
					{
						if (strcasecmp(name2, name) == 0)
						{
							found = 1;
							break;
						}
					} while ((name2 = strtok(NULL, ",")) != NULL);
				}
				break;

			case TAG_CMD:
				if (found)
				{
					if (!start) start = pos;
					num_items++;
				}
				break;

			case TAG_END:
				if (start)
				{
					end = i;
					num_lines = line + 1;
				}
				break;

			case TAG_CHARSET:
				if ((p = cmdlist_get_value(linebuf)) != NULL)
				{
					if (strcasecmp(p, "GBK") == 0)
					{
						charset = CHARSET_GBK;
					}
					else if (strcasecmp(p, "Shift_JIS") == 0)
					{
						charset = CHARSET_SHIFTJIS;
					}
					else if (strcasecmp(p, "ISO-8859-1") == 0)
					{
						charset = CHARSET_ISO8859_1;
					}
					else if (strcasecmp(p, "Latin1") == 0)
					{
						charset = CHARSET_ISO8859_1;
					}
				}
				break;
			}

			if (found == 2) break;
		}

		if (start) line++;
		else pos = i;
	}

	if (!found)
	{
		if (parent_name && name != parent_name)
		{
			name = parent_name;
			goto retry;
		}

		// 見つからなかった場合は終了
		free(buf);
		return;
	}

	free(buf);

	// バッファを確保し、読み込み
	size = end - start;

	if ((cmdbuf = calloc(1, size)) == NULL)
		return;

	fp = fopen(path, "rb");
	fseek(fp, start, SEEK_SET);
	fread(cmdbuf, 1, size, fp);
	fclose(fp);

	// 文字コードチェック
	if (charset == CHARSET_DEFAULT)
		charset = check_text_encode(cmdbuf, size);

	// 行分割用メモリを確保
	if ((cmdline = calloc(num_lines, sizeof(char *))) == NULL)
		goto error;

	// バッファを行毎に分割
	p = cmdbuf;
	for (line = 0; line < num_lines; line++)
	{
		cmdline[line] = p;

		switch (lf_code)
		{
		case LF_WIN:
			p = strstr(cmdline[line], _CRLF);
			*p++ = '\0';
			*p++ = '\0';
			break;

		case LF_MAC:
			p = strchr(cmdline[line], _CR);
			*p++ = '\0';
			break;

		case LF_UNIX:
			p = strchr(cmdline[line], _LF);
			*p++ = '\0';
			break;
		}
	}

	// コマンドリスト構造体の確保
	if ((cmd = (CMDLIST **)calloc(num_items, sizeof(CMDLIST *))) == NULL)
		goto error;

	for (i = 0; i < num_items; i++)
	{
		if ((cmd[i] = (CMDLIST *)calloc(1, sizeof(CMDLIST))) == NULL)
			goto error;
	}

	// 各項目の行数をチェック後、登録
	for (i = 0; i < 2; i++)
	{
		int item_line = 0;

		line  = 0;
		item  = 0;
		found = 0;

		while (line < num_lines && item < num_items)
		{
			strcpy(linebuf, cmdline[line]);

			if (linebuf[0] == '$')
			{
				switch (cmdlist_get_tag(linebuf))
				{
				case TAG_CMD:
					found = 1;
					item_line = 0;
					break;

				case TAG_END:
					found = 0;
					item++;
					break;
				}
			}
			else if (found)
			{
				if (i == 0)
					cmd[item]->lines++;
				else
					cmd[item]->line[item_line++] = cmdline[line];
			}
			line++;
		}

		if (i == 0)
		{
			for (item = 0; item < num_items; item++)
			{
				if ((cmd[item]->line = calloc(1, sizeof(char *) * cmd[item]->lines)) == NULL)
					goto error;
			}
		}
	}

	free(cmdline);
	cmdline = NULL;

	// 表示情報の初期化
	sel_line   = 0;
	prev_line  = 0;
	rows_line  = (charset & CHARSET_GBK) ? 16 : 14;
	show_lines = rows_line;
	num_lines  = cmd[0]->lines;
	if (num_lines < show_lines) show_lines = num_lines;

	top_item   = 0;
	sel_item   = 0;
	prev_item  = 0;
	rows_item  = 13;
	show_items = rows_item;
	if (num_items < show_items) show_items = num_items;

	// 項目メニューの幅を計算
	item_sx = 480;
	for (item = 0; item < num_items; item++)
	{
		int x;

		x = 480 - (strlen(cmd[item]->line[0]) * 7 + 16);
		if (item_sx > x) item_sx = x;
	}

	menu_open = 1;

	return;

error:
	free_commandlist();
}


/*--------------------------------------------------------
	コマンドリストの解放
--------------------------------------------------------*/

void free_commandlist(void)
{
	int i;

	if (cmd)
	{
		for (i = 0; i < num_items; i++)
		{
			if (cmd[i])
			{
				if (cmd[i]->line) free(cmd[i]->line);
				free(cmd[i]);
			}
		}
		free(cmd);
		cmd = NULL;
	}

	if (cmdline)
	{
		free(cmdline);
		cmdline = NULL;
	}

	if (cmdbuf)
	{
		free(cmdbuf);
		cmdbuf = NULL;
	}
}


/*--------------------------------------------------------
	コマンドリスト表示
--------------------------------------------------------*/

void commandlist(int flag)
{
	int x, y, alpha;
	int update = 1, menu_counter = 0;
#if (EMU_SYSTEM == NCDZ)
	int mp3_paused = 0;
#endif
	char title[80], temp[32];

	if (cmdbuf == NULL)
	{
		ui_popup(TEXT(COMMAND_LIST_FOR_THIS_GAME_NOT_FOUND));
		return;
	}

	if (flag)
	{
#if (EMU_SYSTEM == NCDZ)
		if (mp3_get_status() == MP3_PLAY)
		{
			mp3_pause(1);
			mp3_paused = 1;
		}
#endif
		sound_thread_enable(0);
		video_set_mode(32);
		set_lowest_cpu_clock();
	}

	pad_wait_clear();
	load_background(WP_CMDLIST);
	ui_popup_reset();

	sprintf(title, TEXT(COMMAND_LIST_TITLE), game_name);

	do
	{
		if (update)
		{
			update = 0;

			show_background();

			small_icon_shadow(8, 3, UI_COLOR(UI_PAL_TITLE), ICON_CMDLIST);
			uifont_print_shadow(36, 5, UI_COLOR(UI_PAL_TITLE), title);
			draw_battery_status(1);

			if (charset & CHARSET_GBK)
			{
				for (y = 0; y < show_lines; y++)
					textfont_print(6, 37 + 14 * y, UI_COLOR(UI_PAL_SELECT), cmd[sel_item]->line[y + sel_line], charset);
			}
			else
			{
				for (y = 0; y < show_lines; y++)
					textfont_print(6, 37 + 16 * y, UI_COLOR(UI_PAL_SELECT), cmd[sel_item]->line[y + sel_line], charset);
			}

			x = 480;
			if (menu_open)
			{
				alpha = 14;
				x = item_sx;
			}
			else if (menu_counter > 0)
			{
				alpha = 14 - ((4 - menu_counter) << 1);
				x = item_sx + ((480 - item_sx) >> 2) * (4 - menu_counter);
			}
			if (x < 480)
			{
				boxfill_alpha(x, 25, 479, 271, UI_COLOR(UI_PAL_BG1), alpha);

				for (y = 0; y < rows_item; y++)
				{
					if (top_item + y >= num_items) break;

					if (top_item + y == sel_item)
					{
						uifont_print(x, 37 + 16 * y, UI_COLOR(UI_PAL_SELECT), FONT_RIGHTTRIANGLE);
						textfont_print(x + 14, 37 + 16 * y, UI_COLOR(UI_PAL_SELECT), cmd[top_item + y]->line[0], charset);
					}
					else
						textfont_print(x + 14, 37 + 16 * y, UI_COLOR(UI_PAL_NORMAL), cmd[top_item + y]->line[0], charset);
				}

				sprintf(temp, TEXT(COMMAND_LIST_ITEMS), sel_item + 1, num_items);
				x = uifont_get_string_width(temp);
				uifont_print(475 - x, 250, UI_COLOR(UI_PAL_SELECT), temp);
			}
			else
			{
				if (num_lines > rows_line)
					draw_scrollbar(469, 26, 479, 270, 0, num_lines - rows_line + 1, sel_line);
			}

			update |= ui_show_popup(1);
			video_flip_screen(1);
		}
		else
		{
			update = ui_show_popup(0);
			video_wait_vsync();
		}

		if (menu_counter)
		{
			update = 1;
			menu_counter--;
		}

		prev_item = sel_item;
		prev_line = sel_line;
		pad_update();

		if (pad_pressed(PSP_CTRL_UP))
		{
			if (menu_open)
			{
				if (sel_item > 0)
				{
					sel_item--;
					show_lines = rows_line;
					num_lines = cmd[sel_item]->lines;
					if (num_lines < show_lines)
						show_lines = num_lines;
					sel_line = 0;
				}
			}
			else
			{
				if (sel_line > 0) sel_line--;
			}
		}
		else if (pad_pressed(PSP_CTRL_DOWN))
		{
			if (menu_open)
			{
				if (sel_item < num_items - 1)
				{
					sel_item++;
					show_lines = rows_line;
					num_lines = cmd[sel_item]->lines;
					if (num_lines < show_lines)
						show_lines = num_lines;
					sel_line = 0;
				}
			}
			else
			{
				if (sel_line + show_lines < num_lines) sel_line++;
			}
		}
		else if (pad_pressed(PSP_CTRL_LTRIGGER))
		{
			if (menu_open)
			{
				if (sel_item > rows_item)
				{
					sel_item -= rows_item;
				}
				else
				{
					sel_item = 0;
				}
					show_lines = rows_line;
					num_lines = cmd[sel_item]->lines;
					if (num_lines < show_lines)
						show_lines = num_lines;
					sel_line = 0;
			}
			else
			{
				if (sel_line > 0)
				{
					sel_line -= rows_line;
					if (sel_line < 0) sel_line = 0;
				}
			}
		}
		else if (pad_pressed(PSP_CTRL_RTRIGGER))
		{
			if (menu_open)
			{
				if (sel_item < num_items - rows_item)
				{
					sel_item += rows_item;
				}
				else
				{
					sel_item = num_items-1;
				}
					show_lines = rows_line;
					num_lines = cmd[sel_item]->lines;
					if (num_lines < show_lines)
						show_lines = num_lines;
					sel_line = 0;
			}
			else
			{
				if (sel_line + show_lines < num_lines)
				{
					sel_line += rows_line;
					if (sel_line + show_lines > num_lines)
					sel_line = num_lines - show_lines;
				}
			}
		}
		else if (pad_pressed(PSP_CTRL_LEFT))
		{
			if (sel_line > 0)
			{
				sel_line -= rows_line;
				if (sel_line < 0) sel_line = 0;
			}
		}
		else if (pad_pressed(PSP_CTRL_RIGHT))
		{
			if (sel_line + show_lines < num_lines)
			{
				sel_line += rows_line;
				if (sel_line + show_lines > num_lines)
					sel_line = num_lines - show_lines;
			}
		}
		else if (pad_pressed(PSP_CTRL_CIRCLE))
		{
			menu_open ^= 1;
			update = 1;
			if (menu_open == 0) menu_counter = 4;
			pad_wait_clear();
		}
		else if (pad_pressed(PSP_CTRL_SELECT))
		{
			help(HELP_COMMANDLIST);
			update = 1;
		}

		if (menu_open)
		{
			if (top_item > num_items - rows_item) top_item = num_items - rows_item;
			if (top_item < 0) top_item = 0;
			if (sel_item >= num_items) sel_item = 0;
			if (sel_item < 0) sel_item = num_items - 1;
			if (sel_item >= top_item + rows_item) top_item = sel_item - rows_item + 1;
			if (sel_item < top_item) top_item = sel_item;
		}

		if ((sel_line != prev_line) || (sel_item != prev_item))
			update = 1;

	} while (!pad_pressed(PSP_CTRL_CROSS));

	pad_wait_clear();

	if (flag)
	{
		ui_popup_reset();

		set_cpu_clock(platform_cpuclock);

		video_set_mode(16);
		autoframeskip_reset();
		blit_clear_all_sprite();

		sound_thread_set_volume();
		sound_thread_enable(1);

#if (EMU_SYSTEM == NCDZ)
		mp3_set_volume();
		if (mp3_paused) mp3_pause(0);
#endif
	}
	else
	{
		ui_popup_reset();
		load_background(WP_LOGO);
	}
}


/******************************************************************************
	コマンドリストサイズ縮小処理
******************************************************************************/

#define INFO_SEEK	0
#define CMD_SEEK	1
#define END_SEEK	2

#if (EMU_SYSTEM == CPS1)
#define EXT		"cps1"
#elif (EMU_SYSTEM == CPS2)
#define EXT		"cps2"
#elif (EMU_SYSTEM == MVS)
#define EXT		"mvs"
#endif

int commandlist_size_reduction(void)
{
	FILE *fp;
	char path[MAX_PATH], path2[MAX_PATH];
	char *p, linebuf[512], rom_name[512][16];//256
	int i, j, l, found = 0, total_roms = 0;
	int num_games, charset, progress;
	int header_end, body_start, body_end;
	int line = 0, line2 = 0, num_cmd;
	int org_size, new_size;
	char *textbuf = NULL, **line_ptr = NULL;

#if (EMU_SYSTEM == NCDZ)
	for (i = 0; i < 97; i++)
	{
		strcpy(rom_name[i], games[i].name);
	}
	total_roms = 97;
#else
	sprintf(path, "%szipname." EXT, launchDir);
	if ((fp = fopen(path, "r")) == NULL)
	{
		sprintf(path, "%szipnamej." EXT, launchDir);
		if ((fp = fopen(path, "r")) == NULL)
		{
			return 0;
		}
	}

	while (fgets(linebuf, 511, fp))//255
	{
		char *name = strtok(linebuf, ",");
		strcpy(rom_name[total_roms++], name);
	}
	fclose(fp);
#endif

	sprintf(path, "%scommand.dat", launchDir);
	if ((fp = fopen(path, "rb")) == NULL)
		return 0;

	pad_wait_clear();
	ui_popup_reset();
	msg_screen_init(WP_CMDLIST, ICON_COMMANDDAT, TEXT(COMMAND_DAT_SIZE_REDUCTION));

	num_games  = 0;
	header_end = -1;
	body_start = -1;
	body_end   = -1;
	charset    = CHARSET_DEFAULT;

	msg_printf(TEXT(CMDLIST_MESSAGE1));
	msg_printf(TEXT(CMDLIST_MESSAGE2));
	msg_printf(TEXT(CMDLIST_MESSAGE3));
	msg_printf(TEXT(CMDLIST_MESSAGE4));

	i = 0;
	do
	{
		video_wait_vsync();
		pad_update();

		if (pad_pressed(PSP_CTRL_CIRCLE))
		{
			i = 1;
			break;
		}

		if (Loop == LOOP_EXIT) break;

	} while (!pad_pressed(PSP_CTRL_CROSS));

	pad_wait_clear();

	if (!i)
	{
		fclose(fp);
		goto cancel;
	}

	fseek(fp, 0, SEEK_END);
	org_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	msg_printf("\n");
	msg_printf(TEXT(CHECKING_COMMAND_DAT_FORMAT));

	// 登録されているゲーム数をチェック
	while (fgets(linebuf, 511, fp) != NULL)//255
	{
		if (strrchr(linebuf, '\r') == NULL)
		{
			if (strrchr(linebuf, '\n') == NULL)
			{
				linebuf[509] = '\r';//253
				linebuf[510] = '\n';//254
			}
		}

		if (linebuf[0] == '$')
		{
			p = strtok(linebuf, "\r\n");

			if (p && linebuf[0] == '$')
			{
				if (!strncasecmp(linebuf, "$charset", 8) && strchr(linebuf, '=') != NULL)
				{
					char *type;

					strtok(linebuf, " =");
					if ((type = strtok(NULL, " =")) != NULL)
					{
						if (!strcasecmp(type, "GBK"))
						{
							charset = CHARSET_GBK;
						}
						else if (!strcasecmp(type, "Shift_JIS"))
						{
							charset = CHARSET_SHIFTJIS;
						}
						else if (!strcasecmp(type, "ISO-8859-1") || !strcasecmp(type, "Latin1"))
						{
							charset = CHARSET_LATIN1;
						}
					}
				}
				else if (!strncmp(linebuf, "$info", 5) && strchr(linebuf, '=') != NULL)
				{
					strtok(linebuf, " =");
					if (strtok(NULL, " =") != NULL)
					{
						if (body_start == -1)
						{
							// 最初の $info を記録
							body_start = line;
						}
						num_games++;
						found = 1;
					}
				}
				else if (!strncmp(linebuf, "$end", 4))
				{
					// 最後の $end を記録
					body_end = line;
				}
			}
		}
		else if (body_start == -1)
		{
			if (!strcmp(linebuf, "\r\n")) header_end = line;
		}

		line++;
	}

	if (!found)
	{
		msg_printf(TEXT(UNKNOWN_FORMAT));
		fclose(fp);
		goto error;
	}
	if (line == 0)
	{
		msg_printf(TEXT(EMPTY_FILE));
		fclose(fp);
		goto error;
	}

	if ((line_ptr = (char **)malloc(sizeof(char *) * line)) == NULL)
	{
		msg_printf(TEXT(COULD_NOT_ALLOCATE_MEMORY));
		goto error;
	}
	if ((textbuf = (char *)malloc(org_size + 1)) == NULL)
	{
		msg_printf(TEXT(COULD_NOT_ALLOCATE_MEMORY));
		goto error;
	}
	memset(textbuf, 0, org_size + 1);

	fseek(fp, 0, SEEK_SET);
	fread(textbuf, 1, org_size, fp);
	fclose(fp);

	if (charset == CHARSET_DEFAULT)
	{
		charset = check_text_encode(textbuf, org_size);
	}

	// 行の位置を保存
	line_ptr[0] = strtok(textbuf, "\n");

	i = 1;
	while (1)
	{
		if ((p = strtok(NULL, "\n")) == NULL) break;
		line_ptr[i++] = p;
	}

	for (i = 0; i < line; i++)
	{
		if ((p = strrchr(line_ptr[i], '\r')) != NULL)
			*p = '\0';
	}

	// 退避ファイル名を作成
	sprintf(path2, "%scommand.org", launchDir);

	remove(path2);

	// リネームして退避 (退避ファイル名: command.org)
	if (rename(path, path2) < 0)
	{
		msg_printf(TEXT(COULD_NOT_RENAME_FILE));
		goto error;
	}

	//------------------------------------------------------------------
	// 縮小処理開始
	//------------------------------------------------------------------
	if ((fp = fopen(path, "w")) == NULL)
	{
		msg_printf(TEXT(COULD_NOT_CREATE_OUTPUT_FILE));
		goto error;
	}

	l = 0;
	line2 = 0;
	num_cmd = 0;
	progress = INFO_SEEK;

	msg_printf("\n");

	if (charset != CHARSET_DEFAULT)
	{
		if (charset == CHARSET_GBK)
			fprintf(fp, "$charset=gbk\r\n");
		else if (charset == CHARSET_SHIFTJIS)
			fprintf(fp, "$charset=shift_jis\r\n");
		else
			fprintf(fp, "$charset=latin1\r\n");

		line2++;
	}

	// ヘッダをコピー
	if (header_end != -1)
	{
		for (; l <= header_end; l++)
		{
			strcpy(linebuf, line_ptr[l]);

			if (strncasecmp(linebuf, "$charset", 8) != 0)
			{
				fprintf(fp, "%s\r\n", linebuf);
				line2++;
			}
		}
	}

	for (; l <= body_end; l++)
	{
		strcpy(linebuf, line_ptr[l]);

		switch (progress)
		{
		case INFO_SEEK:
			if (linebuf[0] == '$')
			{
				// コマンドリスト開始
				if (!strncasecmp(linebuf, "$info", 5) && strchr(linebuf, '=') != NULL)
				{
					char *name;

					strtok(linebuf, " =");
					if ((name = strtok(NULL, " =\r\n")) != NULL)
					{
						found = 0;

						if (strchr(name, ','))
						{
							char linebuf2[512], *name2;//256

							strcpy(linebuf2, name);
							name2 = strtok(linebuf2, ",");

							do
							{
#if (EMU_SYSTEM == NCDZ)
								if (!strcasecmp(name2, "trally")) strcpy(name2, "rallych");
#endif
								for (i = 0; i < total_roms; i++)
								{
									if (!strcasecmp(name2, rom_name[i]))
									{
										found = 2;
										break;
									}
								}
								if (found) break;

							} while ((name2 = strtok(NULL, ",")) != NULL);
						}
						else
						{
#if (EMU_SYSTEM == NCDZ)
							if (!strcasecmp(name, "trally")) strcpy(name, "rallych");
#endif
							for (i = 0; i < total_roms; i++)
							{
								if (!strcasecmp(name, rom_name[i]))
								{
									found = 1;
									break;
								}
							}
						}

						if (found)
						{
							if (l != 0)
							{
								j = l;
								while (j > body_start)
								{
									if (line_ptr[j - 1][0] == '#') j--;
									else break;
								}

								while (j < l)
								{
									fprintf(fp, "%s\r\n", line_ptr[j]);
									j++;
								}
							}

							msg_printf(TEXT(COPYING_x), rom_name[i]);
							fprintf(fp, "$info=%s\r\n", name);
							progress = CMD_SEEK;
							num_cmd++;
							line2++;
						}
					}
				}
			}
			break;

		case CMD_SEEK:
			if (!strncasecmp(linebuf, "$cmd", 4))
			{
				// コマンド開始
				progress = END_SEEK;
				fprintf(fp, "$cmd\r\n");
				line2++;
			}
			else if (!strncasecmp(linebuf, "$info", 5))
			{
				// 次のコマンド - 1行戻す
				fprintf(fp, "\r\n");
				progress = INFO_SEEK;
				l--;
			}
			break;

		case END_SEEK:
			if (!strncasecmp(linebuf, "$end", 4))
			{
				// コマンド終了
				progress = CMD_SEEK;
				fprintf(fp, "$end\r\n");
				line2++;
			}
			else
			{
				// コマンドリストの中身 - そのまま出力
				fprintf(fp, "%s\r\n", linebuf);
				line2++;
			}
			break;
		}
	}

	if (body_end < line - 1)
	{
		if (strlen(line_ptr[l]) == 0)
			l++;

		for (; l < line; l++)
		{
			strcpy(linebuf, line_ptr[l]);
			fprintf(fp, "%s\r\n", linebuf);
			line2++;
		}
	}

	fclose(fp);

	fp = fopen(path, "rb");
	fseek(fp, 0, SEEK_END);
	new_size = ftell(fp);
	fclose(fp);

	msg_printf("\n");
	msg_printf(TEXT(REDUCTION_RESULT), org_size, new_size, 100.0 - ((float)new_size / (float)org_size) * 100.0);

	msg_printf("\n");
	msg_printf(TEXT(COMPLETE));

error:
	if (textbuf) free(textbuf);
	if (line_ptr) free(line_ptr);

	msg_printf("\n");
	msg_printf(TEXT(PRESS_ANY_BUTTON2));
	pad_wait_press(PAD_WAIT_INFINITY);

cancel:
	pad_wait_clear();
	ui_popup_reset();

	load_background(WP_FILER);
	return 1;
}
