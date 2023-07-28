#include "cps1.h"


#define SERIAL_BUFFER_LENGTH	40

static struct EEPROM_interface *intf;

static int serial_count;
static uint8_t serial_buffer[SERIAL_BUFFER_LENGTH];
static uint8_t eeprom_data[EEPROM_SIZE];
static int eeprom_data_bits;
static int eeprom_read_address;
static int eeprom_clock_count;
static int latch,reset_line,clock_line,sending;
static int locked;
static int reset_delay;

/*
    EEPROM_command_match:

    Try to match the first (len) digits in the EEPROM serial buffer
    string (*buf) with  an EEPROM command string (*cmd).
    Return non zero if a match was found.

    The serial buffer only contains '0' or '1' (e.g. "1001").
    The command can contain: '0' or '1' or these wildcards:

    'x' :   match both '0' and '1'
    "*1":   match "1", "01", "001", "0001" etc.
    "*0":   match "0", "10", "110", "1110" etc.

    Note: (cmd) may be NULL. Return 0 (no match) in this case.
*/
static int EEPROM_command_match(const char *buf, const char *cmd, int len)
{
	if ( cmd == 0 )	return 0;
	if ( len == 0 )	return 0;

	for (;len>0;)
	{
		char b = *buf;
		char c = *cmd;

		if ((b==0) || (c==0))
			return (b==c);

		switch ( c )
		{
			case '0':
			case '1':
				if (b != c)	return 0;
			case 'X':
			case 'x':
				buf++;
				len--;
				cmd++;
				break;

			case '*':
				c = cmd[1];
				switch( c )
				{
					case '0':
					case '1':
					  	if (b == c)	{	cmd++;			}
						else		{	buf++;	len--;	}
						break;
					default:	return 0;
				}
		}
	}
	return (*cmd==0);
}


void EEPROM_init(struct EEPROM_interface *interface)
{
	intf = interface;

	if ((1 << intf->address_bits) * intf->data_bits / 8 > EEPROM_SIZE)
	{
		fatalerror("EEPROM larger than eeprom.c allows");
	}

	memset(eeprom_data,0xff,(1 << intf->address_bits) * intf->data_bits / 8);
	serial_count = 0;
	latch = 0;
	reset_line = ASSERT_LINE;
	clock_line = ASSERT_LINE;
	eeprom_read_address = 0;
	sending = 0;
	if (intf->cmd_unlock) locked = 1;
	else locked = 0;
}

static void EEPROM_write(int bit)
{
	if (serial_count >= SERIAL_BUFFER_LENGTH-1)
	{
//		logerror("error: EEPROM serial buffer overflow\n");
		return;
	}

	serial_buffer[serial_count++] = (bit ? '1' : '0');
	serial_buffer[serial_count] = 0;	/* nul terminate so we can treat it as a string */

	if ( (serial_count > intf->address_bits) &&
	      EEPROM_command_match((char*)serial_buffer,intf->cmd_read,strlen((char*)serial_buffer)-intf->address_bits) )
	{
		int i,address;

		address = 0;
		for (i = serial_count-intf->address_bits;i < serial_count;i++)
		{
			address <<= 1;
			if (serial_buffer[i] == '1') address |= 1;
		}
		if (intf->data_bits == 16)
			eeprom_data_bits = (eeprom_data[2*address+0] << 8) + eeprom_data[2*address+1];
		else
			eeprom_data_bits = eeprom_data[address];
		eeprom_read_address = address;
		eeprom_clock_count = 0;
		sending = 1;
		serial_count = 0;
//		logerror("EEPROM read %04x from address %02x\n",eeprom_data_bits,address);
	}
	else if ( (serial_count > intf->address_bits) &&
	           EEPROM_command_match((char*)serial_buffer,intf->cmd_erase,strlen((char*)serial_buffer)-intf->address_bits) )
	{
		int i,address;

		address = 0;
		for (i = serial_count-intf->address_bits;i < serial_count;i++)
		{
			address <<= 1;
			if (serial_buffer[i] == '1') address |= 1;
		}
//		logerror("EEPROM erase address %02x\n",address);
		if (locked == 0)
		{
			if (intf->data_bits == 16)
			{
				eeprom_data[2*address+0] = 0x00;
				eeprom_data[2*address+1] = 0x00;
			}
			else
				eeprom_data[address] = 0x00;
		}
#if 0
		else
		{
			logerror("Error: EEPROM is locked\n");
		}
#endif
		serial_count = 0;
	}
	else if ( (serial_count > (intf->address_bits + intf->data_bits)) &&
	           EEPROM_command_match((char*)serial_buffer,intf->cmd_write,strlen((char*)serial_buffer)-(intf->address_bits + intf->data_bits)) )
	{
		int i,address,data;

		address = 0;
		for (i = serial_count-intf->data_bits-intf->address_bits;i < (serial_count-intf->data_bits);i++)
		{
			address <<= 1;
			if (serial_buffer[i] == '1') address |= 1;
		}
		data = 0;
		for (i = serial_count-intf->data_bits;i < serial_count;i++)
		{
			data <<= 1;
			if (serial_buffer[i] == '1') data |= 1;
		}
//		logerror("EEPROM write %04x to address %02x\n",data,address);
		if (locked == 0)
		{
			if (intf->data_bits == 16)
			{
				eeprom_data[2*address+0] = data >> 8;
				eeprom_data[2*address+1] = data & 0xff;
			}
			else
				eeprom_data[address] = data;
		}
#if 0
		else
		{
			logerror("Error: EEPROM is locked\n");
		}
#endif
		serial_count = 0;
	}
	else if ( EEPROM_command_match((char*)serial_buffer,intf->cmd_lock,strlen((char*)serial_buffer)) )
	{
//		logerror("EEPROM lock\n");
		locked = 1;
		serial_count = 0;
	}
	else if ( EEPROM_command_match((char*)serial_buffer,intf->cmd_unlock,strlen((char*)serial_buffer)) )
	{
//		logerror("EEPROM unlock\n");
		locked = 0;
		serial_count = 0;
	}
}

static void EEPROM_reset(void)
{
#if 0
	if (serial_count)
		logerror("EEPROM reset, buffer = %s\n",serial_buffer);
#endif

	serial_count = 0;
	sending = 0;
	reset_delay = intf->reset_delay;	/* delay a little before returning setting data to 1 (needed by wbeachvl) */
}


void EEPROM_write_bit(int bit)
{
	latch = bit;
}

int EEPROM_read_bit(void)
{
	int res;

	if (sending)
		res = (eeprom_data_bits >> intf->data_bits) & 1;
	else
	{
		if (reset_delay > 0)
		{
			/* this is needed by wbeachvl */
			reset_delay--;
			res = 0;
		}
		else
			res = 1;
	}

	return res;
}

void EEPROM_set_cs_line(int state)
{
	reset_line = state;

	if (reset_line != CLEAR_LINE)
		EEPROM_reset();
}

void EEPROM_set_clock_line(int state)
{
	if (state == PULSE_LINE || (clock_line == CLEAR_LINE && state != CLEAR_LINE))
	{
		if (reset_line == CLEAR_LINE)
		{
			if (sending)
			{
				if (eeprom_clock_count == intf->data_bits && intf->enable_multi_read)
				{
					eeprom_read_address = (eeprom_read_address + 1) & ((1 << intf->address_bits) - 1);
					if (intf->data_bits == 16)
						eeprom_data_bits = (eeprom_data[2*eeprom_read_address+0] << 8) + eeprom_data[2*eeprom_read_address+1];
					else
						eeprom_data_bits = eeprom_data[eeprom_read_address];
					eeprom_clock_count = 0;
//					logerror("EEPROM read %04x from address %02x\n",eeprom_data_bits,eeprom_read_address);
				}
				eeprom_data_bits = (eeprom_data_bits << 1) | 1;
				eeprom_clock_count++;
			}
			else
				EEPROM_write(latch);
		}
	}

	clock_line = state;
}


void EEPROM_load(FILE *fp)
{
	fread(eeprom_data, 1, (1 << intf->address_bits) * intf->data_bits / 8, fp);
}

void EEPROM_save(FILE *fp)
{
	fwrite(eeprom_data, 1, (1 << intf->address_bits) * intf->data_bits / 8, fp);
}

uint8_t EEPROM_read_data(uint32_t address)
{
	return eeprom_data[address];
}

void EEPROM_write_data(uint32_t address, uint8_t data)
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
	state_save_long(&locked, 1);
	state_save_long(&reset_delay, 1);
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
	state_load_long(&locked, 1);
	state_load_long(&reset_delay, 1);
	state_load_byte(serial_buffer, SERIAL_BUFFER_LENGTH);
	state_load_byte(eeprom_data, EEPROM_SIZE);
}

#endif /* SAVE_STATE */
