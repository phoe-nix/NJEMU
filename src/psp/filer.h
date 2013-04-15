/******************************************************************************

	filer.c

	PSP ファイルブラウザ

******************************************************************************/

#ifndef PSP_FILEBROWSER_H
#define PSP_FILEBROWSER_H

extern char startupDir[MAX_PATH];

int file_exist(const char *path);
char *find_file(char *pattern, char *path);
void delete_files(const char *dirname, const char *pattern);
#ifdef SAVE_STATE
void find_state_file(UINT8 *slot);
#endif
void show_exit_screen(void);
void file_browser(void);

#endif /* PSP_FILEBROWSER_H */
