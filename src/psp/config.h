/******************************************************************************

	config.c

	アプリケーション設定ファイル管理

******************************************************************************/

#ifndef PSP_CONFIG_H
#define PSP_CONFIG_H

void load_settings(void);
void save_settings(void);
void load_gamecfg(const char *name);
void save_gamecfg(const char *name);

#endif /* PSP_CONFIG_H */
