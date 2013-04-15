// OKIM6295 module header

#ifndef OKIM6295_H
#define OKIM6295_H

void OKIM6295Init(int clock, int pin7);
void OKIM6295Reset(void);
void OKIM6295_set_samplerate(void);

READ8_HANDLER( OKIM6295_status_r );
WRITE8_HANDLER( OKIM6295_data_w );
WRITE8_HANDLER( OKIM6295_set_pin7_w );

#ifdef SAVE_STATE
STATE_SAVE( okim6295 );
STATE_LOAD( okim6295 );
#endif

#endif /* OKIM6295_H */
