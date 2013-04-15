/***************************************************************************

	png.c

    PSP PNG format image I/O functions. (based on M.A.M.E. PNG functions)

***************************************************************************/

#ifndef PSP_PNG_H
#define PSP_PNG_H

int load_png(const char *name, int number);
int save_png(const char *path);

#endif /* PSP_PNG_H */
