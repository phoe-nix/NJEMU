/***************************************************************************

	adhoc.c

	PSPアドホック通信制御

***************************************************************************/

#include "emumain.h"
#include <pspsdk.h>
#include <pspnet.h>
#include <pspwlan.h>
#ifdef KERNEL_MODE
#include <psputilsforkernel.h>
#else
#include <psputility_netmodules.h>	// AHMAN
#endif
#include "adhoc_include/pspnet_adhoc.h"
#include "adhoc_include/pspnet_adhocctl.h"
#include "adhoc_include/pspnet_adhocmatching.h"


#define NUM_ENTRIES			16

#define MODE_LOBBY			0
#define MODE_P2P			1

#define PSP_LISTING			1
#define PSP_SELECTED		2
#define PSP_SELECTING		3
#define PSP_WAIT_EST		4
#define PSP_ESTABLISHED		5

#define ADHOC_BUFFER_SIZE	0x400
#define PDP_BUFFER_SIZE		(ADHOC_BUFFER_SIZE * 2)
#define PDP_PORT			(0x309)


/***************************************************************************
	関数パラメータ (長いので・・・)
***************************************************************************/

#define MATCHING_CREATE_PARAMS	\
	3,							\
	0xa,						\
	0x22b,						\
	0x800,						\
	0x2dc6c0,					\
	0x5b8d80,					\
	3,							\
	0x7a120,					\
	matchingCallback

#define MATCHING_START_PARAMS	\
	matchingId,					\
	0x10,						\
	0x2000,						\
	0x10,						\
	0x2000,						\
	strlen(matchingData) + 1,	\
	(void *)matchingData


/***************************************************************************
	ローカル変数
***************************************************************************/

#ifdef KERNEL_MODE
static int adhoc_modules_loaded;
#endif

static int mode;
static int Server;
static int pdpId;

static unsigned char g_mac[6];
static unsigned char g_mymac[6];
static unsigned char g_ssid[8];
static int  g_unk1;
static int  g_matchEvent;
static int  g_matchOptLen;
static char g_matchOptData[256];
static char g_matchingData[32];
static int  matchChanged;
static int  matchingId;

static struct psplist_t
{
	char name[48];
	unsigned char mac[6];
} psplist[NUM_ENTRIES];

static int max;
static int pos;

static int adhoc_initialized = 0;
static unsigned char adhoc_buffer[ADHOC_BUFFER_SIZE];
static unsigned char adhoc_work[ADHOC_BUFFER_SIZE];


/***************************************************************************
	ローカル関数
***************************************************************************/

/*--------------------------------------------------------
	プログレスバー初期化
--------------------------------------------------------*/

static void adhoc_init_progress(int total, const char *text)
{
	char buf[32];

	load_background(WP_LOGO);
	video_copy_rect(work_frame, draw_frame, &full_rect, &full_rect);

	small_icon(6, 3, UI_COLOR(UI_PAL_TITLE), ICON_SYSTEM);
	sprintf(buf, "AdHoc - %s", game_name);
	uifont_print(32, 5, UI_COLOR(UI_PAL_TITLE), buf);

	video_copy_rect(draw_frame, work_frame, &full_rect, &full_rect);

	init_progress(total, text);
}


/*--------------------------------------------------------
	リスト消去
--------------------------------------------------------*/

static void ClearPspList(void)
{
	max = 0;
	pos = 0;
	memset(&psplist, 0, sizeof(psplist));
}


/*--------------------------------------------------------
	リストに追加
--------------------------------------------------------*/

static int AddPsp(unsigned char *mac, char *name, int length)
{
	int i;

	if (max == NUM_ENTRIES) return 0;
	if (length == 1) return 0;

	for (i = 0; i < max; i++)
	{
		if (memcmp(psplist[i].mac, mac, 6) == 0)
			return 0;
	}

	memcpy(psplist[max].mac, mac, 6);

	if (length)
	{
		if (length < 47)
			strcpy(psplist[max].name, name);
		else
			strncpy(psplist[max].name, name, 47);
	}
	else
		psplist[max].name[0] = '\0';

	max++;

	return 1;
}


/*--------------------------------------------------------
	リストから削除
--------------------------------------------------------*/

static int DelPsp(unsigned char *mac)
{
	int i, j;

	for (i = 0; i < max; i++)
	{
		if (memcmp(psplist[i].mac, mac, 6) == 0)
		{
			if (i != max - 1)
			{
				for (j = i + 1; j < max; j++)
				{
					memcpy(psplist[j - 1].mac, psplist[j].mac, 6);
					strcpy(psplist[j - 1].name, psplist[j].name);
				}
			}

			if (pos == i) pos = 0;
			if (pos > i) pos--;
			max--;

			return 0;
		}
	}

	return -1;
}


/*--------------------------------------------------------
	リストを表示
--------------------------------------------------------*/

static void DisplayPspList(int top, int rows)
{
	if (max == 0)
	{
		msg_printf(TEXT(WAITING_FOR_ANOTHER_PSP_TO_JOIN));
	}
	else
	{
		int i;
		char temp[20];

		video_copy_rect(show_frame, draw_frame, &full_rect, &full_rect);

		draw_scrollbar(470, 26, 479, 270, rows, max, pos);

		for (i = 0; i < rows; i++)
		{
			if ((top + i) >= max) break;

			sceNetEtherNtostr(psplist[top + i].mac, temp);

			if ((top + i) == pos)
			{
				uifont_print(24, 40 + (i + 2) * 17, UI_COLOR(UI_PAL_SELECT), temp);
				uifont_print(190, 40 + (i + 2) * 17, UI_COLOR(UI_PAL_SELECT), psplist[top + i].name);
			}
			else
			{
				uifont_print(24, 40 + (i + 2) * 17, UI_COLOR(UI_PAL_NORMAL), temp);
				uifont_print(190, 40 + (i + 2) * 17, UI_COLOR(UI_PAL_NORMAL), psplist[top + i].name);
			}
		}

		video_flip_screen(1);
	}
}


/*--------------------------------------------------------
	選択中のPSPの情報を取得
--------------------------------------------------------*/

static int GetPspEntry(unsigned char *mac, char *name)
{
	if (max == 0) return -1;

	memcpy(mac, psplist[pos].mac, 6);
	strcpy(name, psplist[pos].name);

	return 1;
}


/*--------------------------------------------------------
	Matching callback
--------------------------------------------------------*/

static void matchingCallback(int unk1, int event, unsigned char *mac, int optLen, void *optData)
{
	switch (event)
	{
	case MATCHING_JOINED:
		AddPsp(mac, optData, optLen);
		break;

	case MATCHING_DISCONNECT:
		DelPsp(mac);
		break;

	default:
		g_unk1        = unk1;
		g_matchEvent  = event;
		g_matchOptLen = optLen;
		strncpy(g_matchOptData, optData, optLen);
		memcpy(g_mac, mac, 6);
		matchChanged = 1;
		break;
	}
}


/***************************************************************************
	AdHocインタフェース関数
***************************************************************************/

/*--------------------------------------------------------
	AdHocモジュールのロード (Kernelモード用)
--------------------------------------------------------*/

#ifdef KERNEL_MODE
int pspSdkLoadAdhocModules(void)
{
	int modID;

	adhoc_modules_loaded = -1;

	modID = pspSdkLoadStartModule("flash0:/kd/ifhandle.prx", PSP_MEMORY_PARTITION_KERNEL);
	if (modID < 0)
		return modID;

	modID = pspSdkLoadStartModule("flash0:/kd/memab.prx", PSP_MEMORY_PARTITION_KERNEL);
	if (modID < 0)
		return modID;

	modID = pspSdkLoadStartModule("flash0:/kd/pspnet_adhoc_auth.prx", PSP_MEMORY_PARTITION_KERNEL);
	if (modID < 0)
		return modID;

	modID = pspSdkLoadStartModule("flash0:/kd/pspnet.prx", PSP_MEMORY_PARTITION_USER);
	if (modID < 0)
		return modID;
	else
		pspSdkFixupImports(modID);

	modID = pspSdkLoadStartModule("flash0:/kd/pspnet_adhoc.prx", PSP_MEMORY_PARTITION_USER);
	if (modID < 0)
		return modID;
	else
		pspSdkFixupImports(modID);

	modID = pspSdkLoadStartModule("flash0:/kd/pspnet_adhocctl.prx", PSP_MEMORY_PARTITION_USER);
	if (modID < 0)
		return modID;
	else
		pspSdkFixupImports(modID);

	modID = pspSdkLoadStartModule("flash0:/kd/pspnet_adhoc_matching.prx", PSP_MEMORY_PARTITION_USER);
	if (modID < 0)
		return modID;
	else
		pspSdkFixupImports(modID);

	adhoc_modules_loaded = 0;

	return 0;
}
#endif


/*--------------------------------------------------------
	AdHocモジュールのロード
--------------------------------------------------------*/

int adhocLoadModules(void)
{
#ifdef KERNEL_MODE
	return adhoc_modules_loaded;
#else
	if (devkit_version >= 0x02000010)
	{
		int error;

		if ((error = sceUtilityLoadNetModule(PSP_NET_MODULE_COMMON)) < 0)
			return error;

		if ((error = sceUtilityLoadNetModule(PSP_NET_MODULE_ADHOC)) < 0)
			return error;

		return 0;
	}
	return -1;
#endif
}


/*--------------------------------------------------------
	AdHocモジュールのアンロード
--------------------------------------------------------*/

int adhocUnloadModules(void)
{
#ifdef KERNEL_MODE
	return 0;
#else
	if (devkit_version >= 0x02000010)
	{
		int error;

		if ((error = sceUtilityUnloadNetModule(PSP_NET_MODULE_ADHOC)) < 0)
			return error;

		if ((error = sceUtilityUnloadNetModule(PSP_NET_MODULE_COMMON)) < 0)
			return error;

		return 0;
	}
	return -1;
#endif
}


/*--------------------------------------------------------
	初期化
--------------------------------------------------------*/

#if (EMU_SYSTEM == CPS1)
#define PRODUCT	"CPS1"
#elif (EMU_SYSTEM == CPS2)
#define PRODUCT	"CPS2"
#elif (EMU_SYSTEM == MVS)
#define PRODUCT	"MVS_"
#elif (EMU_SYSTEM == NCDZ)
#define PRODUCT	"NCDZ"
#endif

int adhocInit(const char *matchingData)
{
	struct productStruct product;
	int error = 0, state = 0;
	unsigned char mac[6];
	const char *unknown = "";
	char message[256];

	video_set_mode(32);

	mode = MODE_LOBBY;
	Server = 0;
	adhoc_initialized = 0;

	g_unk1        = 0;
	g_matchEvent  = 0;
	g_matchOptLen = 0;
	matchChanged  = 0;
	memset(g_mac, 0, sizeof(g_mac));
	memset(g_mymac, 0, sizeof(g_mymac));

	sprintf((char *)product.product, PRODUCT "00%d%d%d", VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD);
	product.unknown = 0;

	ClearPspList();

	if (strlen(matchingData) == 0)
		return -1;

	strcpy(g_matchingData, matchingData);

	sprintf(message, TEXT(CONNECTING_TO_x), TEXT(LOBBY));
	adhoc_init_progress(10, message);

	if ((error = sceNetInit(0x20000, 0x20, 0x1000, 0x20, 0x1000)) == 0)
	{
		update_progress();
		if ((error = sceNetAdhocInit()) == 0)
		{
			update_progress();
			if ((error = sceNetAdhocctlInit(0x2000, 0x20, &product)) == 0)
			{
				update_progress();
				if ((error = sceNetAdhocctlConnect(unknown)) == 0)
				{
					update_progress();
					do
					{
						if ((error = sceNetAdhocctlGetState(&state)) != 0) break;
						sceKernelDelayThread(1000000/60);

					} while (state != 1);

					if (!error)
					{
						update_progress();

						sceWlanGetEtherAddr(mac);
						update_progress();

						if ((pdpId = sceNetAdhocPdpCreate(mac, PDP_PORT, PDP_BUFFER_SIZE, 0)) > 0)
						{
							update_progress();
							if ((error = sceNetAdhocMatchingInit(0x20000)) == 0)
							{
								update_progress();
								if ((matchingId = sceNetAdhocMatchingCreate(MATCHING_CREATE_PARAMS)) >= 0)
								{
									update_progress();
									if ((error = sceNetAdhocMatchingStart(MATCHING_START_PARAMS)) == 0)
									{
										update_progress();
										show_progress(TEXT(CONNECTED));
										sceKernelDelayThread(1000000);
										adhoc_initialized = 1;
										return 0;
									}
									sceNetAdhocMatchingDelete(matchingId);
								}
								error = 2;
								sceNetAdhocMatchingTerm();
							}
							sceNetAdhocPdpDelete(pdpId, 0);
						}
						error = 1;
					}
					sceNetAdhocctlDisconnect();
				}
				sceNetAdhocctlTerm();
			}
			sceNetAdhocTerm();
		}
		sceNetTerm();
	}

	switch (error)
	{
	case 1:  sprintf(message, "%s (PDP ID = %08x)", TEXT(FAILED), pdpId); break;
	case 2:  sprintf(message, "%s (Matching ID = %08x)", TEXT(FAILED), matchingId); break;
	default: sprintf(message, "%s (Error Code = %08x)", TEXT(FAILED), error); break;
	}

	show_progress(message);

	pad_wait_clear();
	pad_wait_press(PAD_WAIT_INFINITY);

	return -1;
}


/*--------------------------------------------------------
	切断
--------------------------------------------------------*/

int adhocTerm(void)
{
	if (adhoc_initialized > 0)
	{
		char message[256];

		sprintf(message, TEXT(DISCONNECTING_FROM_x), Server ? TEXT(CLIENT) : TEXT(SERVER));
		adhoc_init_progress(5, message);

		sceNetAdhocctlDisconnect();
		update_progress();

		sceNetAdhocPdpDelete(pdpId, 0);
		update_progress();

		sceNetAdhocctlTerm();
		update_progress();

		sceNetAdhocTerm();
		update_progress();

		sceNetTerm();
		update_progress();

		show_progress(TEXT(DISCONNECTED));

		adhoc_initialized = 0;
	}

	return 0;
}

/*--------------------------------------------------------
	ロビーから切断
--------------------------------------------------------*/

static void adhocDisconnect(void)
{
	char message[256];

	sprintf(message, TEXT(DISCONNECTING_FROM_x), TEXT(LOBBY));
	adhoc_init_progress(8, message);

	sceNetAdhocMatchingStop(matchingId);
	update_progress();

	sceNetAdhocMatchingDelete(matchingId);
	update_progress();

	sceNetAdhocMatchingTerm();
	update_progress();

	sceNetAdhocctlDisconnect();
	update_progress();

	sceNetAdhocPdpDelete(pdpId, 0);
	update_progress();

	sceNetAdhocctlTerm();
	update_progress();

	sceNetAdhocTerm();
	update_progress();

	sceNetTerm();
	update_progress();

	show_progress(TEXT(DISCONNECTED));

	adhoc_initialized = 0;
}


/*--------------------------------------------------------
	ロビーから切断し、P2P開始
--------------------------------------------------------*/

static int adhocStartP2P(void)
{
	int error = 0, state = 1;
	unsigned char mac[6];
	char message[256];

	sprintf(message, TEXT(DISCONNECTING_FROM_x), TEXT(LOBBY));
	adhoc_init_progress(6, message);

	sceNetAdhocMatchingStop(matchingId);
	update_progress();

	sceNetAdhocMatchingDelete(matchingId);
	update_progress();

	sceNetAdhocMatchingTerm();
	update_progress();

	sceNetAdhocPdpDelete(pdpId, 0);
	update_progress();

	sceNetAdhocctlDisconnect();
	update_progress();

	do
	{
		if ((error = sceNetAdhocctlGetState(&state)) != 0) break;
			sceKernelDelayThread(1000000/60);
	} while (state == 1);

	update_progress();
	show_progress(TEXT(DISCONNECTED));


	mode = MODE_P2P;
	sprintf(message, TEXT(CONNECTING_TO_x), Server ? TEXT(CLIENT) : TEXT(SERVER));
	adhoc_init_progress(4, message);

	if ((error = sceNetAdhocctlConnect((int *)g_ssid)) == 0)
	{
		update_progress();
		do
		{
			if ((error = sceNetAdhocctlGetState(&state)) != 0) break;
			sceKernelDelayThread(1000000/60);
		} while (state != 1);

		if (!error)
		{
			update_progress();

			sceWlanGetEtherAddr(mac);
			memcpy(g_mymac, mac, 6);
			update_progress();

			if ((pdpId = sceNetAdhocPdpCreate(mac, PDP_PORT, PDP_BUFFER_SIZE, 0)) > 0)
			{
				update_progress();
				adhoc_initialized = 2;

				show_progress(TEXT(WAITING_FOR_SYNCHRONIZATION));
				if ((error = adhocSync()) == 0)
					return Server;
			}
			else
			{
				error = 1;
			}
		}
		sceNetAdhocctlDisconnect();

		if (state == 1)
		{
			do
			{
				if ((error = sceNetAdhocctlGetState(&state)) != 0) break;
				sceKernelDelayThread(1000000/60);
			} while (state == 1);
		}
	}

	sceNetAdhocctlTerm();
	sceNetAdhocTerm();
	sceNetTerm();

	adhoc_initialized = 0;

	switch (error)
	{
	case 1:  sprintf(message, "%s (PDP ID = %08x)", TEXT(FAILED), pdpId); break;
	default: sprintf(message, "%s (Error Code = %08x)", TEXT(FAILED), error); break;
	}

	show_progress(message);

	pad_wait_clear();
	pad_wait_press(PAD_WAIT_INFINITY);

	return -1;
}


/*--------------------------------------------------------
	接続先の選択
--------------------------------------------------------*/

int adhocSelect(void)
{
	int top = 0;
	int rows = 11;
	int currentState = PSP_LISTING;
	int prev_max = 0;
	int update = 1;
	unsigned char mac[6];
	char name[64];
	char temp[64];
	char title[32];

	sprintf(title, "AdHoc - %s", game_name);
	msg_screen_init(WP_LOGO, ICON_SYSTEM, title);

	while (1)
	{
		pad_update();

		msg_set_text_color(0xffff);

		switch (currentState)
		{
		case PSP_LISTING:
			Server = 0;
			if (update)
			{
				msg_screen_init(WP_LOGO, ICON_SYSTEM, title);
				msg_printf(TEXT(SELECT_A_SERVER_TO_CONNECT_TO));
				msg_printf("\n");
				DisplayPspList(top, rows);
				update = 0;
			}
			if (pad_pressed(PSP_CTRL_UP))
			{
				if (pos > 0) pos--;
				update = 1;
			}
			else if (pad_pressed(PSP_CTRL_DOWN))
			{
				if (pos < max - 1) pos++;
				update = 1;
			}
			else if (pad_pressed(PSP_CTRL_CIRCLE))
			{
				if (GetPspEntry(mac, name) > 0)
				{
					if (strcmp(name, g_matchingData) == 0)
					{
						currentState = PSP_SELECTING;
						sceNetAdhocMatchingSelectTarget(matchingId, mac, 0, NULL);
						update = 1;
					}
				}
			}
			else if (pad_pressed(PSP_CTRL_TRIANGLE))
			{
				msg_set_text_color(0xffffffff);
				adhocDisconnect();
				pad_wait_clear();
				return -1;
			}
			if (matchChanged)
			{
				if (g_matchEvent == MATCHING_SELECTED)
				{
					memcpy(mac, g_mac, 6);
					strcpy(name, g_matchOptData);
					currentState = PSP_SELECTED;
				}
				update = 1;
			}
			break;

		case PSP_SELECTING:
			if (update)
			{
				msg_screen_init(WP_LOGO, ICON_SYSTEM, title);
				sceNetEtherNtostr(mac, temp);
				msg_printf(TEXT(WAITING_FOR_x_TO_ACCEPT_THE_CONNECTION), temp);
				msg_printf(TEXT(TO_CANCEL_PRESS_CROSS));
				update = 0;
			}
			if (pad_pressed(PSP_CTRL_CROSS))
			{
				sceNetAdhocMatchingCancelTarget(matchingId, mac);
				currentState = PSP_LISTING;
				update = 1;
			}
			if (matchChanged)
			{
				switch (g_matchEvent)
				{
				case MATCHING_SELECTED:
					sceNetAdhocMatchingCancelTarget(matchingId, mac);
					break;

				case MATCHING_ESTABLISHED:
					currentState = PSP_ESTABLISHED;
					break;

				case MATCHING_REJECTED:
					currentState = PSP_LISTING;
					break;
				}
				update = 1;
			}
			break;

		case PSP_SELECTED:
			Server = 1;
			if (update)
			{
				msg_screen_init(WP_LOGO, ICON_SYSTEM, title);
				sceNetEtherNtostr(mac, temp);
				msg_printf(TEXT(x_HAS_REQUESTED_A_CONNECTION), temp);
				msg_printf(TEXT(TO_ACCEPT_THE_CONNECTION_PRESS_CIRCLE_TO_CANCEL_PRESS_CIRCLE));
				update = 0;
			}
			if (pad_pressed(PSP_CTRL_CROSS))
			{
				sceNetAdhocMatchingCancelTarget(matchingId, mac);
				currentState = PSP_LISTING;
				update = 1;
			}
			else if (pad_pressed(PSP_CTRL_CIRCLE))
			{
				sceNetAdhocMatchingSelectTarget(matchingId, mac, 0, NULL);
				currentState = PSP_WAIT_EST;
				update = 1;
			}
			if (matchChanged)
			{
				if (g_matchEvent == MATCHING_CANCELED)
				{
					currentState = PSP_LISTING;
				}
				update = 1;
			}
			break;

		case PSP_WAIT_EST:
			if (matchChanged)
			{
				if (g_matchEvent == MATCHING_ESTABLISHED)
				{
					currentState = PSP_ESTABLISHED;
				}
				update = 1;
			}
			break;
		}

		matchChanged = 0;
		if (currentState == PSP_ESTABLISHED)
			break;

		if (top > max - rows) top = max - rows;
		if (top < 0) top = 0;
		if (pos >= top + rows) top = pos - rows + 1;
		if (pos < top) top = pos;

		if (max != prev_max)
		{
			prev_max = max;
			update = 1;
		}

		sceDisplayWaitVblankStart();
	}

	msg_set_text_color(0xffffffff);

	if (Server) sceWlanGetEtherAddr(mac);

	sceNetEtherNtostr(mac, temp);

	g_ssid[0] = temp[ 9];
	g_ssid[1] = temp[10];
	g_ssid[2] = temp[12];
	g_ssid[3] = temp[13];
	g_ssid[4] = temp[15];
	g_ssid[5] = temp[16];
	g_ssid[6] = '\0';

	return adhocStartP2P();
}


/*--------------------------------------------------------
	データを送信
--------------------------------------------------------*/

int adhocSend(void *buffer, int length, int type)
{
	int error;

	memset(adhoc_buffer, 0, ADHOC_BUFFER_SIZE);

	adhoc_buffer[0] = type;
	memcpy(&adhoc_buffer[1], buffer, length);

	if ((error = sceNetAdhocPdpSend(pdpId, g_mac, PDP_PORT, adhoc_buffer, length + 1, 0, 1)) < 0)
		return error;

	return length;
}


/*--------------------------------------------------------
	データを受信
--------------------------------------------------------*/

int adhocRecv(void *buffer, int timeout, int type)
{
	int error;
	int length = ADHOC_BUFFER_SIZE;
	unsigned short port = 0;
	unsigned char mac[6];

	memset(adhoc_buffer, 0, ADHOC_BUFFER_SIZE);

	if ((error = sceNetAdhocPdpRecv(pdpId, mac, &port, adhoc_buffer, &length, timeout, 0)) < 0)
		return error;

	if (adhoc_buffer[0] & type)
	{
		memcpy(buffer, &adhoc_buffer[1], length - 1);
		return length - 1;
	}

	return -1;
}


/*--------------------------------------------------------
	データを送信し、ackを受信するまで待つ
--------------------------------------------------------*/

int adhocSendRecvAck(void *buffer, int length, int timeout, int type)
{
	int temp_length = length;
	int sent_length = 0;
	int error = 0;
	unsigned char *buf = (unsigned char *)buffer;

	do
	{
		if (temp_length > ADHOC_BUFFER_SIZE - 1)
			temp_length = ADHOC_BUFFER_SIZE - 1;

		adhocSend(buf, temp_length, type);

		if ((error = adhocRecv(adhoc_work, timeout, ADHOC_DATATYPE_ACK)) != 4)
			return error;

		if (*(int *)adhoc_work != sent_length + temp_length)
			return -1;

		buf += temp_length;
		sent_length += temp_length;
		temp_length = length - sent_length;

	} while (sent_length < length);

	return sent_length;
}


/*--------------------------------------------------------
	データの受信を待ち、ackを送信する
--------------------------------------------------------*/

int adhocRecvSendAck(void *buffer, int length, int timeout, int type)
{
	int temp_length = length;
	int rcvd_length = 0;
	int error = 0;
	unsigned char *buf = (unsigned char *)buffer;

	do
	{
		if (temp_length > ADHOC_BUFFER_SIZE - 1)
			temp_length = ADHOC_BUFFER_SIZE - 1;

		if ((error = adhocRecv(buf, timeout, type)) != temp_length)
			return error;

		*(int *)adhoc_work = rcvd_length + temp_length;
		adhocSend(adhoc_work, 4, ADHOC_DATATYPE_ACK);

		buf += temp_length;
		rcvd_length += temp_length;
		temp_length = length - rcvd_length;

	} while (rcvd_length < length);

	return rcvd_length;
}


/*--------------------------------------------------------
	相手との同期を待つ
--------------------------------------------------------*/

int adhocSync(void)
{
	int size = 0;
	int retry = 60;

	if (Server)
	{
		while (retry--)
		{
			adhocSend(adhoc_work, 1, ADHOC_DATATYPE_SYNC);

			if (adhocRecv(adhoc_work, 1000000, ADHOC_DATATYPE_SYNC) == 1)
				goto check_packet;
		}
	}
	else
	{
		while (retry--)
		{
			if (adhocRecv(adhoc_work, 1000000, ADHOC_DATATYPE_SYNC) == 1)
			{
				adhocSend(adhoc_work, 1, ADHOC_DATATYPE_SYNC);
				goto check_packet;
			}
		}
	}

	return -1;

check_packet:
	while (1)
	{
		pdpStatStruct pdpStat;

		size = sizeof(pdpStat);

		if (sceNetAdhocGetPdpStat(&size, &pdpStat) >= 0)
		{
			// 余分なパケットを破棄
			if (pdpStat.rcvdData == ADHOC_DATASIZE_SYNC)
				adhocRecv(adhoc_work, 0, ADHOC_DATATYPE_SYNC);
			else
				break;
		}

		if (Loop != LOOP_EXEC) return 0;

		sceKernelDelayThread(100);
	}

	return 0;
}


/*--------------------------------------------------------
	指定サイズのデータを受信するか、バッファが空に
	なるまで待つ
--------------------------------------------------------*/

void adhocWait(int data_size)
{
	pdpStatStruct pdpStat;
	int size = sizeof(pdpStat);

	if (data_size > ADHOC_BUFFER_SIZE)
		data_size = ADHOC_BUFFER_SIZE;

	while (1)
	{
		if (sceNetAdhocGetPdpStat(&size, &pdpStat) >= 0)
		{
			if (pdpStat.rcvdData == 0 || (int)pdpStat.rcvdData == data_size)
				break;
			else
				adhocRecv(adhoc_work, 0, ADHOC_DATATYPE_ANY);
		}

		if (Loop != LOOP_EXEC) break;

		sceKernelDelayThread(100);
	}
}
