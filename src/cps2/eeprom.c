/******************************************************************************

	eeprom.c

	CPS2 EEPROMインタフェース関数

******************************************************************************/

#include "cps2.h"


#define SERIAL_BUFFER_LENGTH	40


/******************************************************************************
	ローカル変数
******************************************************************************/

static int serial_count;
static UINT8 serial_buffer[SERIAL_BUFFER_LENGTH];
static UINT8 ALIGN_PSPDATA eeprom_data[EEPROM_SIZE];
static int eeprom_data_bits;
static int eeprom_read_address;
static int eeprom_clock_count;
static int latch;
static int reset_line;
static int clock_line;
static int sending;


/******************************************************************************
	ローカル関数
******************************************************************************/

/*--------------------------------------------------------
	コマンドマッチング
--------------------------------------------------------*/

static int EEPROM_command_match(const char *buf, const char *cmd, int len)
{
	if (!cmd || !len) return 0;

	while (len > 0)
	{
		char b = *buf;
		char c = *cmd;

		if (!b || !c) return (b == c);
		if (b != c) return 0;

		buf++;
		cmd++;
		len--;
	}
	return (*cmd == 0);
}


/*--------------------------------------------------------
	EEPROMビット書き込み
--------------------------------------------------------*/

static void EEPROM_write(int bit)
{
	if (serial_count >= SERIAL_BUFFER_LENGTH - 1) return;

	serial_buffer[serial_count++] = (bit ? '1' : '0');
	serial_buffer[serial_count] = '\0';

	if (serial_count > 6)
	{
		int i, address;
		char *buffer = (char *)serial_buffer;

		if (EEPROM_command_match(buffer, "0110", strlen(buffer) - 6))
		{
			// read
			address = 0;
			for (i = serial_count - 6; i < serial_count; i++)
			{
				address <<= 1;
				if (serial_buffer[i] == '1') address |= 1;
			}
			eeprom_data_bits = (eeprom_data[2 * address] << 8) + eeprom_data[2 * address + 1];
			eeprom_read_address = address;
			eeprom_clock_count = 0;
			sending = 1;
			serial_count = 0;
		}
		else if (EEPROM_command_match(buffer, "0111", strlen(buffer) - 6))
		{
			// elase
			address = 0;
			for (i = serial_count - 6; i < serial_count; i++)
			{
				address <<= 1;
				if (serial_buffer[i] == '1') address |= 1;
			}
			eeprom_data[2 * address + 0] = 0x00;
			eeprom_data[2 * address + 1] = 0x00;
			serial_count = 0;
		}
		else if (serial_count > 22 && EEPROM_command_match(buffer, "0101", strlen(buffer) - 22))
		{
			int data;

			// write
			address = 0;
			for (i = serial_count - 22;i < serial_count - 16; i++)
			{
				address <<= 1;
				if (serial_buffer[i] == '1') address |= 1;
			}
			data = 0;
			for (i = serial_count - 16; i < serial_count; i++)
			{
				data <<= 1;
				if (serial_buffer[i] == '1') data |= 1;
			}
			eeprom_data[2 * address + 0] = data >> 8;
			eeprom_data[2 * address + 1] = data & 0xff;
			serial_count = 0;
		}
	}
}


/*--------------------------------------------------------
	リセット
--------------------------------------------------------*/

static void EEPROM_reset(void)
{
	serial_count = 0;
	sending = 0;
}


/******************************************************************************
	グローバル関数
******************************************************************************/

/*--------------------------------------------------------
	初期化
--------------------------------------------------------*/

void EEPROM_init(void)
{
	memset(eeprom_data, 0xff, EEPROM_SIZE);
	serial_count = 0;
	latch = 0;
	reset_line = ASSERT_LINE;
	clock_line = ASSERT_LINE;
	eeprom_read_address = 0;
	sending = 0;
}


/*--------------------------------------------------------
	ビット書き込み
--------------------------------------------------------*/

void EEPROM_write_bit(int bit)
{
	latch = bit;
}


/*--------------------------------------------------------
	ビット読み込み
--------------------------------------------------------*/

int EEPROM_read_bit(void)
{
	if (sending)
		return (eeprom_data_bits >> 16) & 1;
	return 1;
}


/*--------------------------------------------------------
	CS設定
--------------------------------------------------------*/

void EEPROM_set_cs_line(int state)
{
	reset_line = state;

	if (reset_line != CLEAR_LINE)
		EEPROM_reset();
}


/*--------------------------------------------------------
	クロック設定
--------------------------------------------------------*/

void EEPROM_set_clock_line(int state)
{
	if (state == PULSE_LINE || (clock_line == CLEAR_LINE && state != CLEAR_LINE))
	{
		if (reset_line == CLEAR_LINE)
		{
			if (sending)
			{
				eeprom_data_bits = (eeprom_data_bits << 1) | 1;
				eeprom_clock_count++;
			}
			else
				EEPROM_write(latch);
		}
	}

	clock_line = state;
}


/*--------------------------------------------------------
	ファイルからデータを読み込み
--------------------------------------------------------*/

void EEPROM_load(FILE *fp)
{
	fread(eeprom_data, 1, EEPROM_SIZE, fp);
}


/*--------------------------------------------------------
	ファイルにデータを保存
--------------------------------------------------------*/

void EEPROM_save(FILE *fp)
{
	fwrite(eeprom_data, 1, EEPROM_SIZE, fp);
}


/*--------------------------------------------------------
	EEPROMに直接データを書き込む
--------------------------------------------------------*/

UINT8 EEPROM_read_data(UINT32 address)
{
	return eeprom_data[address];
}


/*--------------------------------------------------------
	EEPROMのデータを読み込む
--------------------------------------------------------*/

void EEPROM_write_data(UINT32 address, UINT8 data)
{
	eeprom_data[address] = data;
}


/*------------------------------------------------------
	セーブ/ロード ステート
------------------------------------------------------*/

#ifdef SAVE_STATE

STATE_SAVE( eeprom )
{
	state_save_long(&serial_count, 1);
	state_save_long(&eeprom_data_bits, 1);
	state_save_long(&eeprom_read_address, 1);
	state_save_long(&eeprom_clock_count, 1);
	state_save_long(&latch, 1);
	state_save_long(&reset_line, 1);
	state_save_long(&clock_line, 1);
	state_save_long(&sending, 1);
	state_save_byte(serial_buffer, SERIAL_BUFFER_LENGTH);
	state_save_byte(eeprom_data, EEPROM_SIZE);
}

STATE_LOAD( eeprom )
{
	state_load_long(&serial_count, 1);
	state_load_long(&eeprom_data_bits, 1);
	state_load_long(&eeprom_read_address, 1);
	state_load_long(&eeprom_clock_count, 1);
	state_load_long(&latch, 1);
	state_load_long(&reset_line, 1);
	state_load_long(&clock_line, 1);
	state_load_long(&sending, 1);
	state_load_byte(serial_buffer, SERIAL_BUFFER_LENGTH);
	state_load_byte(eeprom_data, EEPROM_SIZE);
}

#endif /* SAVE_STATE */
