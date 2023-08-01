/******************************************************************************

	ui_text_driver.h

******************************************************************************/

#ifndef UI_TEXT_DRIVER_H
#define UI_TEXT_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

#define LANG_ENGLISH	            0
#define LANG_JAPANESE	            1
#define LANG_CHINESE_SIMPLIFIED	    2
#define LANG_CHINESE_TRADITIONAL    3
#define LANG_FRENCH                 0
#define LANG_SPANISH                0
#define LANG_GERMAN                 0
#define LANG_ITALIAN                0
#define LANG_DUTCH                  0
#define LANG_PORTUGUESE             0
#define LANG_RUSSIAN                0
#define LANG_KOREAN                 0

typedef struct ui_text_driver
{
	/* Human-readable identifier. */
	const char *ident;
	/* Creates and initializes handle to ui_text driver.
	*
	* Returns: ui_text driver handle on success, otherwise NULL.
	**/
	void *(*init)(void);
	/* Stops and frees driver data. */
   	void (*free)(void *data);
	int32_t (*getLanguage)(void *data);
	const char *(*getText)(void *data, int32_t id);

} ui_text_driver_t;


extern ui_text_driver_t ui_text_psp;
extern ui_text_driver_t ui_text_null;

extern ui_text_driver_t *ui_text_drivers[];

#define ui_text_driver ui_text_drivers[0]

extern void *ui_text_data;

#endif /* UI_TEXT_DRIVER_H */