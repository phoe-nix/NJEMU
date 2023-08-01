/******************************************************************************

	eeprom.c

	CPS2 EEPROMインタフェース関数

******************************************************************************/

#ifndef CPS2_EEPROM_H
#define CPS2_EEPROM_H

void EEPROM_init(void);
void EEPROM_write_bit(int bit);
int  EEPROM_read_bit(void);
void EEPROM_set_cs_line(int state);
void EEPROM_set_clock_line(int state);
void EEPROM_load(FILE *file);
void EEPROM_save(FILE *file);
uint8_t   EEPROM_read_data(uint32_t address);
void EEPROM_write_data(uint32_t address, uint8_t data);

#ifdef SAVE_STATE
STATE_SAVE( eeprom );
STATE_LOAD( eeprom );
#endif

#endif /* CPS2_EEPROM_H */
