/******************************************************************************

	video.c

	PSPビデオ制御関数

******************************************************************************/

#include "psp_video.h"


/******************************************************************************
	ローカル変数/構造体
******************************************************************************/

static const ScePspIMatrix4 dither_matrix =
{
	// Bayer dither
	{  0,  8,  2, 10 },
	{ 12,  4, 14,  6 },
	{  3, 11,  1,  9 },
	{ 15,  7, 13,  5 }
};

static int pixel_format;


/******************************************************************************
	グローバル変数/構造体
******************************************************************************/

uint8_t ALIGN16_DATA gulist[GULIST_SIZE];
RECT full_rect = { 0, 0, SCR_WIDTH, SCR_HEIGHT };


/******************************************************************************
	グローバル関数
******************************************************************************/

typedef struct psp_video {
} psp_video_t;

/*--------------------------------------------------------
	ビデオ処理初期化
--------------------------------------------------------*/
static void psp_start(void) {
	#if VIDEO_32BPP
	if (video_mode == 32)
	{
		pixel_format = GU_PSM_8888;

		show_frame = (void *)(FRAMESIZE32 * 0);
		draw_frame = (void *)(FRAMESIZE32 * 1);
		work_frame = (void *)(FRAMESIZE32 * 2);
		tex_frame  = (void *)(FRAMESIZE32 * 3);
	}
	else
#endif
	{
		pixel_format = GU_PSM_5551;

		show_frame = (void *)(FRAMESIZE * 0);
		draw_frame = (void *)(FRAMESIZE * 1);
		work_frame = (void *)(FRAMESIZE * 2);
		tex_frame  = (void *)(FRAMESIZE * 3);
	}

	sceGuDisplay(GU_FALSE);
	sceGuInit();

	sceGuStart(GU_DIRECT, gulist);
	sceGuDrawBuffer(pixel_format, draw_frame, BUF_WIDTH);
	sceGuDispBuffer(SCR_WIDTH, SCR_HEIGHT, show_frame, BUF_WIDTH);
	sceGuOffset(2048 - (SCR_WIDTH / 2), 2048 - (SCR_HEIGHT / 2));
	sceGuViewport(2048, 2048, SCR_WIDTH, SCR_HEIGHT);

	sceGuEnable(GU_SCISSOR_TEST);
	sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);

	sceGuDisable(GU_ALPHA_TEST);
	sceGuAlphaFunc(GU_LEQUAL, 0, 0x01);

	sceGuDisable(GU_BLEND);
	sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);

	sceGuDisable(GU_DEPTH_TEST);
	sceGuDepthRange(65535, 0);
	sceGuDepthFunc(GU_GEQUAL);
	sceGuDepthMask(GU_TRUE);

	sceGuEnable(GU_TEXTURE_2D);
	sceGuTexMode(pixel_format, 0, 0, GU_FALSE);
	sceGuTexScale(1.0f / BUF_WIDTH, 1.0f / BUF_WIDTH);
	sceGuTexOffset(0, 0);
	sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);

	sceGuClutMode(GU_PSM_5551, 0, 0xff, 0);

	sceGuSetDither(&dither_matrix);
	sceGuDisable(GU_DITHER);

	sceGuClearDepth(0);
	sceGuClearColor(0);

	sceGuFinish();
	sceGuSync(0, GU_SYNC_FINISH);

	video_driver->clearFrame(NULL, show_frame);
	video_driver->clearFrame(NULL, draw_frame);
	video_driver->clearFrame(NULL, work_frame);

	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);

	ui_init();
}

static void psp_init(void)
{
	psp_video_t *psp = (psp_video_t*)calloc(1, sizeof(psp_video_t));

	psp_start();
	return psp;
}


/*--------------------------------------------------------
	ビデオ処理終了(共通)
--------------------------------------------------------*/

static void psp_exit() {
	sceGuDisplay(GU_FALSE);
	sceGuTerm();
}

static void psp_free(void *data)
{
	psp_video_t *psp = (psp_video_t*)data;
	
	psp_exit();
	free(psp);
}

/*--------------------------------------------------------
	ビデオモード設定
--------------------------------------------------------*/


static void psp_setMode(int mode)
{
#if VIDEO_32BPP
	if (video_mode != mode)
	{
		if (video_mode) psp_exit();

		video_mode = mode;

		psp_start();
	}
#endif
}

/*--------------------------------------------------------
	VSYNCを待つ
--------------------------------------------------------*/

static void psp_waitVsync(void *data)
{
	sceDisplayWaitVblankStart();
}


/*--------------------------------------------------------
	スクリーンをフリップ
--------------------------------------------------------*/

static void psp_flipScreen(void *data, bool vsync)
{
	if (vsync) sceDisplayWaitVblankStart();
	show_frame = draw_frame;
	draw_frame = sceGuSwapBuffers();
}


/*--------------------------------------------------------
	VRAMのアドレスを取得
--------------------------------------------------------*/

static void *psp_frameAddr(void *data, void *frame, int x, int y)
{
#if VIDEO_32BPP
	if (video_mode == 32)
		return (void *)(((uint32_t)frame | 0x44000000) + ((x + (y << 9)) << 2));
	else
#endif
		return (void *)(((uint32_t)frame | 0x44000000) + ((x + (y << 9)) << 1));
}


/*--------------------------------------------------------
	描画/表示フレームをクリア
--------------------------------------------------------*/

static void psp_clearScreen(void *data)
{
	video_driver->clearFrame(NULL, show_frame);
	video_driver->clearFrame(NULL, draw_frame);
}

/*--------------------------------------------------------
	指定したフレームをクリア
--------------------------------------------------------*/

static void psp_clearFrame(void *data, void *frame)
{
	sceGuStart(GU_DIRECT, gulist);
	sceGuDrawBufferList(pixel_format, frame, BUF_WIDTH);
	sceGuScissor(0, 0, BUF_WIDTH, SCR_HEIGHT);
	sceGuClearColor(0);
	sceGuClear(GU_COLOR_BUFFER_BIT | GU_FAST_CLEAR_BIT);
	sceGuFinish();
	sceGuSync(0, GU_SYNC_FINISH);
}


/*--------------------------------------------------------
	指定したフレームを塗りつぶし
--------------------------------------------------------*/

static void psp_fillFrame(void *data, void *frame, uint32_t color)
{
	sceGuStart(GU_DIRECT, gulist);
	sceGuDrawBufferList(pixel_format, frame, BUF_WIDTH);
	sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
	sceGuClearColor(color);
	sceGuClear(GU_COLOR_BUFFER_BIT | GU_FAST_CLEAR_BIT);
	sceGuFinish();
	sceGuSync(0, GU_SYNC_FINISH);
}


/*--------------------------------------------------------
	指定した矩形範囲を塗りつぶし
--------------------------------------------------------*/

static void psp_fillRect(void *data, void *frame, uint32_t color, RECT *rect)
{
	sceGuStart(GU_DIRECT, gulist);
	sceGuDrawBufferList(pixel_format, frame, BUF_WIDTH);
	sceGuScissor(rect->left, rect->top, rect->right, rect->bottom);
	sceGuClearColor(color);
	sceGuClear(GU_COLOR_BUFFER_BIT | GU_FAST_CLEAR_BIT);
	sceGuFinish();
	sceGuSync(0, GU_SYNC_FINISH);
}


/*--------------------------------------------------------
	矩形範囲をコピー
--------------------------------------------------------*/

static void psp_copyRect(void *data, void *src, void *dst, RECT *src_rect, RECT *dst_rect)
{
	int j, sw, dw, sh, dh;
	struct Vertex *vertices;

	sw = src_rect->right - src_rect->left;
	dw = dst_rect->right - dst_rect->left;
	sh = src_rect->bottom - src_rect->top;
	dh = dst_rect->bottom - dst_rect->top;

	sceGuStart(GU_DIRECT, gulist);

	sceGuDrawBufferList(pixel_format, dst, BUF_WIDTH);
	sceGuScissor(dst_rect->left, dst_rect->top, dst_rect->right, dst_rect->bottom);
	sceGuDisable(GU_ALPHA_TEST);

	sceGuTexMode(pixel_format, 0, 0, GU_FALSE);
	sceGuTexImage(0, BUF_WIDTH, BUF_WIDTH, BUF_WIDTH, GU_FRAME_ADDR(src));
	if (sw == dw && sh == dh)
		sceGuTexFilter(GU_NEAREST, GU_NEAREST);
	else
		sceGuTexFilter(GU_LINEAR, GU_LINEAR);

	for (j = 0; (j + SLICE_SIZE) < sw; j = j + SLICE_SIZE)
	{
		vertices = (struct Vertex *)sceGuGetMemory(2 * sizeof(struct Vertex));

		vertices[0].u = src_rect->left + j;
		vertices[0].v = src_rect->top;
		vertices[0].x = dst_rect->left + j * dw / sw;
		vertices[0].y = dst_rect->top;

		vertices[1].u = src_rect->left + j + SLICE_SIZE;
		vertices[1].v = src_rect->bottom;
		vertices[1].x = dst_rect->left + (j + SLICE_SIZE) * dw / sw;
		vertices[1].y = dst_rect->bottom;

		sceGuDrawArray(GU_SPRITES, TEXTURE_FLAGS, 2, NULL, vertices);
	}

	if (j < sw)
	{
		vertices = (struct Vertex *)sceGuGetMemory(2 * sizeof(struct Vertex));

		vertices[0].u = src_rect->left + j;
		vertices[0].v = src_rect->top;
		vertices[0].x = dst_rect->left + j * dw / sw;
		vertices[0].y = dst_rect->top;

		vertices[1].u = src_rect->right;
		vertices[1].v = src_rect->bottom;
		vertices[1].x = dst_rect->right;
		vertices[1].y = dst_rect->bottom;

		sceGuDrawArray(GU_SPRITES, TEXTURE_FLAGS, 2, NULL, vertices);
	}

	sceGuFinish();
	sceGuSync(0, GU_SYNC_FINISH);
}


/*--------------------------------------------------------
	矩形範囲を左右反転してコピー
--------------------------------------------------------*/

static void psp_copyRectFlip(void *data, void *src, void *dst, RECT *src_rect, RECT *dst_rect)
{
	int16_t j, sw, dw, sh, dh;
	struct Vertex *vertices;

	sw = src_rect->right - src_rect->left;
	dw = dst_rect->right - dst_rect->left;
	sh = src_rect->bottom - src_rect->top;
	dh = dst_rect->bottom - dst_rect->top;

	sceGuStart(GU_DIRECT, gulist);

	sceGuDrawBufferList(pixel_format, dst, BUF_WIDTH);
	sceGuScissor(dst_rect->left, dst_rect->top, dst_rect->right, dst_rect->bottom);
	sceGuDisable(GU_ALPHA_TEST);

	sceGuTexMode(pixel_format, 0, 0, GU_FALSE);
	sceGuTexImage(0, 512, 512, BUF_WIDTH, GU_FRAME_ADDR(src));
	if (sw == dw && sh == dh)
		sceGuTexFilter(GU_NEAREST, GU_NEAREST);
	else
		sceGuTexFilter(GU_LINEAR, GU_LINEAR);

	for (j = 0; (j + SLICE_SIZE) < sw; j = j + SLICE_SIZE)
	{
    	vertices = (struct Vertex *)sceGuGetMemory(2 * sizeof(struct Vertex));

		vertices[0].u = src_rect->left + j;
		vertices[0].v = src_rect->top;
		vertices[0].x = dst_rect->right - j * dw / sw;
		vertices[0].y = dst_rect->bottom;

		vertices[1].u = src_rect->left + j + SLICE_SIZE;
		vertices[1].v = src_rect->bottom;
		vertices[1].x = dst_rect->right - (j + SLICE_SIZE) * dw / sw;
		vertices[1].y = dst_rect->top;

		sceGuDrawArray(GU_SPRITES, TEXTURE_FLAGS, 2, NULL, vertices);
	}

	if (j < sw)
	{
		vertices = (struct Vertex *)sceGuGetMemory(2 * sizeof(struct Vertex));

		vertices[0].u = src_rect->left + j;
		vertices[0].v = src_rect->top;
		vertices[0].x = dst_rect->right - j * dw / sw;
		vertices[0].y = dst_rect->bottom;

		vertices[1].u = src_rect->right;
		vertices[1].v = src_rect->bottom;
		vertices[1].x = dst_rect->left;
		vertices[1].y = dst_rect->top;

		sceGuDrawArray(GU_SPRITES, TEXTURE_FLAGS, 2, NULL, vertices);
	}

	sceGuFinish();
	sceGuSync(0, GU_SYNC_FINISH);
}


/*--------------------------------------------------------
	矩形範囲を270度回転してコピー
--------------------------------------------------------*/

static void psp_copyRectRotate(void *data, void *src, void *dst, RECT *src_rect, RECT *dst_rect)
{
	int16_t j, sw, dw, sh, dh;
	struct Vertex *vertices;

	sw = src_rect->right - src_rect->left;
	dw = dst_rect->right - dst_rect->left;
	sh = src_rect->bottom - src_rect->top;
	dh = dst_rect->bottom - dst_rect->top;

	sceGuStart(GU_DIRECT, gulist);

	sceGuDrawBufferList(pixel_format, dst, BUF_WIDTH);
	sceGuScissor(dst_rect->left, dst_rect->top, dst_rect->right, dst_rect->bottom);
	sceGuDisable(GU_ALPHA_TEST);

	sceGuTexMode(pixel_format, 0, 0, GU_FALSE);
	sceGuTexImage(0, 512, 512, BUF_WIDTH, GU_FRAME_ADDR(src));
	if (sw == dh && sh == dw)
		sceGuTexFilter(GU_NEAREST, GU_NEAREST);
	else
		sceGuTexFilter(GU_LINEAR, GU_LINEAR);

	vertices = (struct Vertex *)sceGuGetMemory(2 * sizeof(struct Vertex));

	for (j = 0; (j + SLICE_SIZE) < sw; j = j + SLICE_SIZE)
	{
		vertices = (struct Vertex *)sceGuGetMemory(2 * sizeof(struct Vertex));

		vertices[0].u = src_rect->right - j;
		vertices[0].v = src_rect->bottom;
		vertices[0].x = dst_rect->right;
		vertices[0].y = dst_rect->top - j * dh / sw;

		vertices[1].u = src_rect->right - j + SLICE_SIZE;
		vertices[1].v = src_rect->top;
		vertices[1].x = dst_rect->right;
		vertices[1].y = dst_rect->bottom - (j + SLICE_SIZE) * dh / sw;

		sceGuDrawArray(GU_SPRITES, TEXTURE_FLAGS, 2, NULL, vertices);
	}

	if (j < sw)
	{
		vertices = (struct Vertex *)sceGuGetMemory(2 * sizeof(struct Vertex));

		vertices[0].u = src_rect->right + j;
		vertices[0].v = src_rect->bottom;
		vertices[0].x = dst_rect->right;
		vertices[0].y = dst_rect->top - j * dh / sw;

		vertices[1].u = src_rect->left;
		vertices[1].v = src_rect->top;
		vertices[1].x = dst_rect->left;
		vertices[1].y = dst_rect->bottom;

		sceGuDrawArray(GU_SPRITES, TEXTURE_FLAGS, 2, NULL, vertices);
	}

	sceGuFinish();
	sceGuSync(0, GU_SYNC_FINISH);
}


/*--------------------------------------------------------
	テクスチャを矩形範囲を指定して描画
--------------------------------------------------------*/

static void psp_drawTexture(void *data, uint32_t src_fmt, uint32_t dst_fmt, void *src, void *dst, RECT *src_rect, RECT *dst_rect)
{
	int j, sw, dw, sh, dh;
	struct Vertex *vertices;

	sw = src_rect->right - src_rect->left;
	dw = dst_rect->right - dst_rect->left;
	sh = src_rect->bottom - src_rect->top;
	dh = dst_rect->bottom - dst_rect->top;

	sceGuStart(GU_DIRECT, gulist);
	sceGuDrawBufferList(dst_fmt, dst, BUF_WIDTH);
	sceGuScissor(dst_rect->left, dst_rect->top, dst_rect->right, dst_rect->bottom);

	sceGuTexMode(src_fmt, 0, 0, GU_FALSE);
	sceGuTexImage(0, BUF_WIDTH, BUF_WIDTH, BUF_WIDTH, GU_FRAME_ADDR(src));
	if (sw == dw && sh == dh)
		sceGuTexFilter(GU_NEAREST, GU_NEAREST);
	else
		sceGuTexFilter(GU_LINEAR, GU_LINEAR);

	for (j = 0; (j + SLICE_SIZE) < sw; j = j + SLICE_SIZE)
	{
    	vertices = (struct Vertex *)sceGuGetMemory(2 * sizeof(struct Vertex));

		vertices[0].u = src_rect->left + j;
		vertices[0].v = src_rect->top;
		vertices[0].x = dst_rect->left + j * dw / sw;
		vertices[0].y = dst_rect->top;

		vertices[1].u = src_rect->left + j + SLICE_SIZE;
		vertices[1].v = src_rect->bottom;
		vertices[1].x = dst_rect->left + (j + SLICE_SIZE) * dw / sw;
		vertices[1].y = dst_rect->bottom;

		sceGuDrawArray(GU_SPRITES, TEXTURE_FLAGS, 2, NULL, vertices);
	}

	if (j < sw)
	{
		vertices = (struct Vertex *)sceGuGetMemory(2 * sizeof(struct Vertex));

		vertices[0].u = src_rect->left + j;
		vertices[0].v = src_rect->top;
		vertices[0].x = dst_rect->left + j * dw / sw;
		vertices[0].y = dst_rect->top;

		vertices[1].u = src_rect->right;
		vertices[1].v = src_rect->bottom;
		vertices[1].x = dst_rect->right;
		vertices[1].y = dst_rect->bottom;

		sceGuDrawArray(GU_SPRITES, TEXTURE_FLAGS, 2, NULL, vertices);
	}

	sceGuFinish();
	sceGuSync(0, GU_SYNC_FINISH);
}


video_driver_t video_psp = {
	"psp",
	psp_init,
	psp_free,
	psp_setMode,
	psp_waitVsync,
	psp_flipScreen,
	psp_frameAddr,
	psp_clearScreen,
	psp_clearFrame,
	psp_fillFrame,
	psp_fillRect,
	psp_copyRect,
	psp_copyRectFlip,
	psp_copyRectRotate,
	psp_drawTexture,
};