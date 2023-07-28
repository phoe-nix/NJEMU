/******************************************************************************

	ui_menu.c

	PSP メニュー

******************************************************************************/

#include "emumain.h"

#define MENU_BLANK	{ LF, }
#define MENU_RETURN	{ RETURN_TO_MAIN_MENU, }
#define MENU_END	{ EOM, }

/*------------------------------------------------------
	DAVEX: VARIABLES GLOBALES DE CHEATS
------------------------------------------------------*/
int cheat_num = 0;
gamecheat_t* gamecheat[MAX_CHEATS];


/*------------------------------------------------------
	エミュレーションリセット
------------------------------------------------------*/

static int menu_reset(void)
{
	if (messagebox(MB_RESETEMULATION))
	{
		video_clear_screen();
		video_flip_screen(1);
		cheat_num = 0; 			//disable cheat
		Loop = LOOP_RESET;
		Sleep = 0;
		return 1;
	}
	return 0;
}


/*------------------------------------------------------
	エミュレーション再起動
------------------------------------------------------*/
#if (EMU_SYSTEM != CPS2 && EMU_SYSTEM != NCDZ)
static int menu_resetdip(void)
{
	if (messagebox(MB_RESTARTEMULATION))
	{
		video_clear_screen();
		video_flip_screen(1);
		cheat_num = 0; 			//disable cheat
		Loop = LOOP_RESET;		//dip no need to restart
//		Loop = LOOP_RESTART;
		Sleep = 0;
	}
	return 1;
}
#endif

static int menu_restart(void)
{
	if (messagebox(MB_RESTARTEMULATION))
	{
		video_clear_screen();
		video_flip_screen(1);
		cheat_num = 0; 			//disable cheat
		Loop = LOOP_RESTART;
		Sleep = 0;
	}
	return 1;
}


/*------------------------------------------------------
	ファイルブラウザに戻る
------------------------------------------------------*/

static int menu_browser(void)
{
	if (messagebox(MB_RETURNTOFILEBROWSER))
	{
	cheat_num = 0;  			//disable cheat
	Loop = LOOP_BROWSER;
	Sleep = 0;
	return 1;
	}
	return 0;
}


/*------------------------------------------------------
	エミュレータ終了
------------------------------------------------------*/

static int menu_exit(void)
{
	if (messagebox(MB_EXITEMULATION))
	{
		Loop = LOOP_EXIT;
		Sleep = 0;
		return 1;
	}
	return 0;
}


/*------------------------------------------------------
	コマンドリスト表示
------------------------------------------------------*/

#ifdef COMMAND_LIST
static int menu_cmdlist(void)
{
	commandlist(0);

	if (Loop == LOOP_EXIT) return 1;
	return 0;
}
#endif


/*------------------------------------------------------
	ゲーム設定メニュー
------------------------------------------------------*/

#define GAMECFG_MAX_ITEMS	32
#define CHEATCFG_MAX_ITEMS	MAX_CHEATS //davex cheats

typedef struct {
	const char *label;
	int *value;
	int enable;
	int flag;
	int old_value;
	int value_max;
	const char *values_label[MAX_CHEAT_OPTION];
} cheatcfg_t;

typedef struct {
	const char *label;
	int *value;
	int enable;
	int flag;
	int old_value;
	int value_max;
	const char *values_label[12];
} gamecfg_t;

typedef struct {
	int label;
	int *value;
	int flag;
	int value_max;
	int values_label[12];
} gamecfg2_t;


#define CFG_CONTINUE	0
#define CFG_RESTART		1
#define CFG_RESET		2

#define INCLUDE_GAMECFG_STRUCT

#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
#include "menu/cps.c"
#elif (EMU_SYSTEM == MVS)
#include "menu/mvs.c"
#elif (EMU_SYSTEM == NCDZ)
#include "menu/ncdz.c"
#endif

#undef INCLUDE_GAMECFG_STRUCT

static int menu_gamecfg(void)
{
	int sel = 0, rows = 13, top = 0;
	int i, arrowl, arrowr, prev_sel, update = 1;
	gamecfg_t gamecfg[GAMECFG_MAX_ITEMS];
	gamecfg2_t *gamecfg2;
	int gamecfg_num;

	for (i = 0; i < GAMECFG_MAX_ITEMS; i++)
		gamecfg[i].enable = 1;

#define INCLUDE_GAMECFG_MENU

#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
#include "menu/cps.c"
#elif (EMU_SYSTEM == MVS)
#include "menu/mvs.c"
#elif (EMU_SYSTEM == NCDZ)
#include "menu/ncdz.c"
#endif

#undef INCLUDE_GAMECFG_MENU

	i = 0;
	while (gamecfg2[i].label)
	{
		int j;

		gamecfg[i].label     = TEXT(gamecfg2[i].label);
		gamecfg[i].value     = gamecfg2[i].value;
		gamecfg[i].flag      = gamecfg2[i].flag;
		gamecfg[i].value_max = gamecfg2[i].value_max;

		for (j = 0; j <= gamecfg2[i].value_max; j++)
			gamecfg[i].values_label[j] = TEXT(gamecfg2[i].values_label[j]);

		if (gamecfg[i].value)
		{
			if (*gamecfg[i].value < 0)
				*gamecfg[i].value = 0;
			if (*gamecfg[i].value > gamecfg[i].value_max)
				*gamecfg[i].value = gamecfg[i].value_max;

			gamecfg[i].old_value = *gamecfg[i].value;
		}
		i++;
	}
	gamecfg_num = i;

	pad_wait_clear();
	load_background(WP_GAMECFG);
	ui_popup_reset();

	do
	{
		if (update)
		{
			show_background();

			arrowl = 0;
			arrowr = 0;
			if (gamecfg[sel].value)
			{
				int cur = *gamecfg[sel].value;

				if (cur > 0) arrowl = 1;
				if (cur < gamecfg[sel].value_max) arrowr = 1;
			}

			small_icon_shadow(8, 3, UI_COLOR(UI_PAL_TITLE), ICON_CONFIG);
			uifont_print_shadow(36, 5, UI_COLOR(UI_PAL_TITLE), TEXT(GAME_CONFIGURATION_MENU));
			draw_scrollbar(469, 26, 479, 270, rows, gamecfg_num, sel);

			for (i = 0; i < rows; i++)
			{
				if (top + i >= gamecfg_num) break;

				if (gamecfg[top + i].label[0] == '\n')
					continue;

				if (gamecfg[top + i].enable)
				{
					if ((top + i) == sel)
					{
						uifont_print_shadow(16, 40 + i * 17, UI_COLOR(UI_PAL_SELECT), gamecfg[top + i].label);
						if (arrowl)
						{
							uifont_print_shadow(190, 40 + i * 17, UI_COLOR(UI_PAL_SELECT), FONT_LEFTTRIANGLE);
						}
					}
					else
						uifont_print(16, 40 + i * 17, UI_COLOR(UI_PAL_NORMAL), gamecfg[top + i].label);
				}
				else
				{
					if ((top + i) == sel)
						uifont_print_shadow(16, 40 + i * 17, COLOR_GRAY, gamecfg[top + i].label);
					else
						uifont_print(16, 40 + i * 17, COLOR_DARKGRAY, gamecfg[top + i].label);
				}

				if (gamecfg[top + i].value)
				{
					int val = *gamecfg[top + i].value;

					if (gamecfg[top + i].enable)
					{
						if ((top + i) == sel)
						{
							uifont_print_shadow(210, 40 + i * 17, UI_COLOR(UI_PAL_SELECT), gamecfg[top + i].values_label[val]);
							if (arrowr)
							{
								int width = uifont_get_string_width(gamecfg[top + i].values_label[val]);
								uifont_print_shadow(214 + width, 40 + i * 17, UI_COLOR(UI_PAL_SELECT), FONT_RIGHTTRIANGLE);
							}
						}
						else
							uifont_print(210, 40 + i * 17, UI_COLOR(UI_PAL_NORMAL), gamecfg[top + i].values_label[val]);
					}
					else
					{
						if ((top + i) == sel)
							uifont_print_shadow(210, 40 + i * 17, COLOR_GRAY, gamecfg[top + i].values_label[val]);
						else
							uifont_print(210, 40 + i * 17, COLOR_DARKGRAY, gamecfg[top + i].values_label[val]);
					}
				}
			}

			update  = draw_battery_status(1);
			update |= draw_volume_status(1);
			update |= ui_show_popup(1);
			video_flip_screen(1);
		}
		else
		{
			update  = draw_battery_status(0);
			update |= draw_volume_status(0);
			update |= ui_show_popup(0);
			video_wait_vsync();
		}

		prev_sel = sel;

		if (pad_pressed(PLATFORM_PAD_UP))
		{
			sel--;
			if (sel < 0) sel = gamecfg_num - 1;
			if (gamecfg[sel].label[0] == '\n') sel--;
		}
		else if (pad_pressed(PLATFORM_PAD_DOWN))
		{
			sel++;
			if (sel > gamecfg_num - 1) sel = 0;
			if (gamecfg[sel].label[0] == '\n') sel++;
		}
		else if (pad_pressed(PLATFORM_PAD_L))
		{
			sel -= rows;
			if (sel < 0) sel = 0;
			if (gamecfg[sel].label[0] == '\n') sel--;
		}
		else if (pad_pressed(PLATFORM_PAD_R))
		{
			sel += rows;
			if (sel >= gamecfg_num) sel = gamecfg_num - 1;
			if (gamecfg[sel].label[0] == '\n') sel++;
		}
		else if (pad_pressed(PLATFORM_PAD_LEFT))
		{
			if (gamecfg[sel].value && gamecfg[sel].enable)
			{
				int cur = *gamecfg[sel].value;

				if (cur > 0)
				{
					*gamecfg[sel].value = cur - 1;
					update = 1;
				}
			}
		}
		else if (pad_pressed(PLATFORM_PAD_RIGHT))
		{
			if (gamecfg[sel].value && gamecfg[sel].enable)
			{
				int cur = *gamecfg[sel].value;

				if (cur < gamecfg[sel].value_max)
				{
					*gamecfg[sel].value = cur + 1;
					update = 1;
				}
			}
		}
		else if (pad_pressed(PLATFORM_PAD_B1))
		{
			if (sel == gamecfg_num - 1)
				break;
		}
		else if (pad_pressed(PLATFORM_PAD_SELECT))
		{
			help(HELP_GAMECONFIG);
			update = 1;
		}

		if (top > gamecfg_num - rows) top = gamecfg_num - rows;
		if (top < 0) top = 0;
		if (sel >= gamecfg_num) sel = 0;
		if (sel < 0) sel = gamecfg_num - 1;
		if (sel >= top + rows) top = sel - rows + 1;
		if (sel < top) top = sel;

		if (prev_sel != sel) update = 1;

		pad_update();

		if (Loop == LOOP_EXIT) break;

	} while (!pad_pressed(PLATFORM_PAD_B2));

	for (i = 0; i < gamecfg_num; i++)
	{
		if (gamecfg[i].value && gamecfg[i].flag == CFG_RESTART)
		{
			if (gamecfg[i].old_value != *gamecfg[i].value)
			{
				menu_restart();
				return 1;
			}
		}
	}

	return 0;
}

/*------------------------------------------------------
	CHEAT OPTIONS DAVEX
------------------------------------------------------*/


int add_new_cheat(int type, char *cheat_name, gamecheat_t** new_cheat){
    gamecheat_t *cheat;
    int i;

		if( cheat_num >= MAX_CHEATS){ *(new_cheat) = NULL; return -1; }

    cheat = (gamecheat_t *) malloc( sizeof(gamecheat_t) );
		if( cheat == NULL) return -1;

	//Se reserva la memoria dinamica del nombre del cheat
	cheat->cheat_name = (char *) malloc( strlen(cheat_name) + 1 ); //incluir null al final
	if( cheat->cheat_name == NULL)
		return -2;

	//se copia el nombre del cheat
	strcpy(cheat->cheat_name, cheat_name);

    //se inicializan el resto de los valores
	//cheat->type = type; //mem save
	cheat->num_cheat_options = 0;
	cheat->curr_option = 0;

	for( i = 0; i< MAX_CHEAT_OPTION; i++)
		cheat->cheat_option[i] = NULL;
	
    //Se agrega el cheat al arreglo de cheats
    gamecheat[cheat_num++] = cheat;

	*(new_cheat) = cheat; //se regresa la referencia para agregar los addres_value
	return 0;

}

const char *DISABLED_LABEL = "Disabled";
const char *ENABLED_LABEL = "Enabled";

int add_new_cheat_option(char *label, gamecheat_t* cheat,cheat_option_t** new_cheat_option){
	int i;
	cheat_option_t *cheat_option;
	
	//Se verifican indices
	//if( cheat->num_cheat_options >= MAX_CHEAT_OPTION  ) return -1;
	if( cheat == NULL){ *(new_cheat_option) = NULL; return -1;}
	if( cheat->num_cheat_options >= MAX_CHEAT_OPTION  ){ *(new_cheat_option) = NULL;  return -1; }
		
	
	cheat_option = (cheat_option_t*) malloc( sizeof(cheat_option_t) );
	if( cheat_option == NULL)
		return -10;
		
	 //Se verifica si se puede ahorrar memoria usando etiquetas fijas para 'Enabled' y 'Disabled'
	if( (strncmp(label, "Disabled", 8 ) == 0 ) || (strncmp(label, "Enabled", 7) == 0 )   ){
		if(  strncmp(label, "Disabled", 8 ) == 0 )
			cheat_option->label = (char *)DISABLED_LABEL;
		else
			cheat_option->label = (char *)ENABLED_LABEL;
	
	}else{ //No coincide, reservar memoria
		
		//Se reserva memoria para la etiqueta y se copia
		cheat_option->label =  (char *) malloc( strlen(label) + 1);
		if( cheat_option->label == NULL)
			return -20;

		strcpy(cheat_option->label, label);
		
	}

	//se inicializan los cheat_values
	cheat_option->num_cheat_values = 0;
	for(i = 0; i< MAX_CHEAT_VALUE; i++){
		cheat_option->cheat_value[i] = NULL;
	}

	cheat->cheat_option[ cheat->num_cheat_options++] =  cheat_option;
	*(new_cheat_option) = cheat_option;

	return 0;
}

int add_new_cheat_value(int cpu, int address, int value, cheat_option_t *cheat_option){
	
	cheat_value_t *cheat_value;
	
	if( cheat_option == NULL) return -200;
	if( cheat_option->num_cheat_values >= MAX_CHEAT_VALUE) return -201;	
	

	cheat_value = (cheat_value_t *) malloc( sizeof(cheat_value_t) );
	if( cheat_value == NULL)
		return -100;

	//cheat_value->cpu = cpu; //mem_save
	cheat_value->address = address;
	cheat_value->value = value;
	cheat_option->cheat_value[ cheat_option->num_cheat_values++ ] = cheat_value;

	return 0;
}

int cheat_clear(void)
{
  gamecheat_t *a_cheat = NULL;
	cheat_option_t *a_cheat_option = NULL;
	cheat_value_t *a_cheat_value = NULL;
	int c,i,j;
    for( c = 0; c < cheat_num; c++)
	{ //arreglo de cheats
        a_cheat = gamecheat[c];
        free( a_cheat->cheat_name); //etiqueta del cheat

        //Se limpian los cheat option
		for(i = 0; i < a_cheat->num_cheat_options; i++)
		{
			a_cheat_option = a_cheat->cheat_option[i];

            free( a_cheat_option->label ); //se libera la etiqueta de la opcion

			//Se imprimen los cheat_value
			for(  j = 0; j< a_cheat_option->num_cheat_values; j++)
			{
                a_cheat_value = a_cheat_option->cheat_value[j];
                free( a_cheat_value);	
			}
            //aqui ya se puede eliminar el cheat_option
            free( a_cheat_option);

	    }

        //se libera el cheat
        free( a_cheat);
    }

    cheat_num = 0; //se pone el numero de cheats a 0 

    return 0;
}

int axtoi(char *str_num){
	int n;
	sscanf( str_num,"%X", &n);
  return n;
}

int parse_cheat_option_line(char *line, gamecheat_t* cheat ){

    //se analiza una linea de tipo 
    //1 "Blanka - Rolling Attack", 0, 0xFF8456, 0x06, 0, 0xFF8458, 0x06
    cheat_option_t* new_cheat_option = NULL;
    char buff[512] = {0};
    char *ptrTok = NULL;
    char *tmpPtr = NULL;
    char *tmpPtrDest = NULL;
    int primTok = 0;
    int tokCounter = 0;
    strcpy(buff, line);
    int cpu = 0;
    int address = 0;
    int value = 0;
    int i = 0;
    

    const int MIN_CPU = 0;
    const int MAX_CPU = 3;
    const int MIN_ADDRESS = 0x000000;
    const int MAX_ADDRESS = 0xFFFFFF;
    const int MIN_VALUE = 0x00;
    const int MAX_VALUE = 0xFF;
    
    

    ptrTok = strtok(buff, ","); //se parten los gdes bloques por el caracter ','
    while(ptrTok != NULL){

        tmpPtr = ptrTok; //depuracion

        if( primTok == 0){ //Se verifica que contega el label
            

            tmpPtr = ptrTok;
            tmpPtrDest = buff;

            //agotar los primeros caracteres hasta la primera '"'
            while( 1){
                if( *tmpPtr == '"' || *tmpPtr == '\0'  )
                    break;
                tmpPtr++;
            }
            //Se encontro o el fin de cadena o la primera comilla doble
            if( *tmpPtr == '\0' ){ //no hubo comilla doble, fue un mal token, por lo que retorna la funcion completa
                return -1;
            }

            tmpPtr++; //la primera comilla doble no se copia
            i = 0;
            while( 1){
                if( *tmpPtr == '"' || *tmpPtr == '\0' || *tmpPtr == '\r' || *tmpPtr == '\n'  )break;
                
                if( i > 23 ) break; //las opciones tienen limite fijo de 23 caracteres	
                *(tmpPtrDest++) = *(tmpPtr++);
                i++;
                 
            }
            *tmpPtrDest = '\0'; //se termina la cadena

            add_new_cheat_option(buff, cheat, &new_cheat_option); //se agrega el cheat option


            primTok = 1;
            ptrTok = strtok(NULL, ","); //siguiente token

            continue;
        }

        if( tokCounter == 0){ //cada token 0 contiene el CPU
            cpu = atoi(ptrTok);
            if( cpu < MIN_CPU || cpu > MAX_CPU) //CPU incorrecto
                cpu = -1;
        }

        if( tokCounter == 1 ){//cada token 1 contiene address
            address = axtoi(ptrTok);
            if( address < MIN_ADDRESS || address > MAX_ADDRESS )
                address = -1;
        }

        if( tokCounter == 2 ){//cada token 2 contiene value
            value = axtoi(ptrTok);
            if( value < MIN_VALUE || value > MAX_VALUE )
                value = -1;
        }

        if( tokCounter == 2 ){ //despues del token 2 se agrega el cheat value
            //Se valida si el cheat_value es valido
            if( cpu >=0 && address >=0 && value >=0 )
                add_new_cheat_value(cpu, address, value, new_cheat_option);
        }


        tokCounter++;
        if( tokCounter > 2) tokCounter = 0;

	    ptrTok = strtok(NULL, ","); //siguiente token
    }
        
    return 0;
}


#define MAX_BUFF 512

void cheats_load(void){
		
    char fn[512];
    char buff[ MAX_BUFF];
		FILE *fp;
		char include_fn[50];
		int i = 0;
		char* buff_ptr = NULL;
    
    //Si ya hay elementos, no agregar mas a la funcion
		if( cheat_num > 0) return; 
		
		sprintf(fn, "%scheats/%s.ini", launchDir, game_name);
		
    fp = fopen(fn, "r");
    if( fp == NULL)
	{
		ui_popup(TEXT(CHEAT_FOR_THIS_GAME_NOT_FOUND));
		return;
	}
    	
    //se revisa si la primera linea es un include
    if(  fgets(buff, MAX_BUFF, fp)	){
    	if( strncmp(buff, "include",7) == 0){
    		
    			buff_ptr = buff;
    			//se eliminan todos los caracteres hasta la primer comilla doble	
    			while(1){
    					if( *buff_ptr == '\0' || *buff_ptr == '\r' || *buff_ptr == '\n') break;
    					if( *buff_ptr == '"'){ buff_ptr++; break;} //Se encontro la primera comilla doble, se consume el caracter y  termina el ciclo
    					buff_ptr++;	
    			}
    			//Se revisa el ultimo caracter
    			if( *buff_ptr == '\0' || *buff_ptr == '\r' || *buff_ptr == '\n') {
    				fclose(fp);
    				return; //mal cadena de include, no se encontro la primera comilla doble
    			}
    			//Se copian el resto de los caracteres
    			i = 0;
    			while(1){
    				if( *buff_ptr == '\0' || *buff_ptr == '\r' || *buff_ptr == '\n') {
    					fclose(fp);
    					return; //mal cadena de include, no se encontro la ultima comilla doble
    				}
    				if( *buff_ptr == '"'){
    					include_fn[i] = '\0'; //ultima comilla doble, se termina la cadena y el ciclo
    					break;
    				}
    				
    				include_fn[i++] = *(buff_ptr++);
    			}
    			
    			sprintf(fn, "%scheats/%s.ini", launchDir, include_fn);
    	}
    }
    fclose(fp);
    
    //ahora si, se abre el archivo correcto
    cheats_load_file(fn);
    
}

void cheats_load_file(char *fn){
	  
    int cheat_detected = 0;
    char buff[ MAX_BUFF];
    gamecheat_t* new_cheat = NULL;
		FILE *fp;
		char * ptr_tmp = NULL;
    char clean_label[50];
    int i;
    
    //Si ya hay elementos, no agregar mas a la funcion
		if( cheat_num > 0) return; 
		
		
    fp = fopen(fn, "r");
    if( fp == NULL) return;

    
    while ( fgets(buff, MAX_BUFF, fp) ){

        if( buff[0] == '\r' || buff[0] == '\n' ){
            cheat_detected = 0;
            continue;
        }

        if( strncmp(buff, "default",7) == 0) continue;

        if( strncmp(buff, "cheat",5) == 0){ //comienza un cheat
            //Se limpia el nombre del cheat
            //a)quitar la palabra cheat
            //b)remover " comilla doble
            
            ptr_tmp = &(buff[5]);
            i = 0;
            while(1){
                if( *ptr_tmp == '\r' || *ptr_tmp == '\n' || *ptr_tmp == '\0')break;
                if( *ptr_tmp == '"'){ ptr_tmp++; continue;}
                if( i> 23) break; //maximo 23 caracteres

                clean_label[i++] = *(ptr_tmp++);
            }
            clean_label[i] = '\0'; //se agrega el final de la cadena


            cheat_detected = 1;
            add_new_cheat(0, clean_label, &new_cheat);

        }else if( cheat_detected == 1){ //linea de cheat option
            parse_cheat_option_line( buff, new_cheat);
        }        
    }
    fclose(fp);
	
}


int get_free_memory()
{
  const int 
    chunk_size = 65536, // 64 kB
    chunks = 1024; // 65536 * 1024 = 64 MB
  void *mem_reserv[chunks];
  int total_mem = 0, i;
 
  /* Initialize */	
  for (i = 0; i < chunks; i++)
    mem_reserv[i] = NULL;
 
  /* Allocate */
  for (i = 0; i < chunks; i++)
  {
    if (!(mem_reserv[i] = malloc(chunk_size)))
      break;
 
    total_mem += chunk_size;
  }
 
  /* Free */
  for (i = 0; i < chunks; i++)
  {
    if (!mem_reserv[i])
      break;
    free(mem_reserv[i]);
  }
 
	return total_mem;
}
/*------------------------------------------------------
	CHEAT OPTIONSDAVEX
------------------------------------------------------*/
//menu que se llena a partir de los cheatgame
//static int menu_gamecfg_davex(void)
int menu_cheatcfg(void)
{
	static int sel = 0, prev_sel = 0;//keep sel
	static int rows = 13, top = 0;
	int i, arrowl, arrowr, update = 1;
	cheatcfg_t cheatcfg[CHEATCFG_MAX_ITEMS];
	int cheatcfg_num;
	int c;

	gamecheat_t *a_cheat = NULL;
	cheat_option_t *a_cheat_option = NULL;
	
	
	//Advertencia:limite de menus a GAMECFG_MAX_ITEMS
	for (i = 0; i < CHEATCFG_MAX_ITEMS; i++)
		cheatcfg[i].enable = 1;
		
	//Inicializar los cheats	
	cheats_load();


	for( c = 0; c < cheat_num; c++)
	{
		a_cheat = gamecheat[c];

    if( a_cheat->cheat_name == NULL)

		return 0;
    else
		cheatcfg[c].label     = (const char*)a_cheat->cheat_name;
		cheatcfg[c].value     = &(a_cheat->curr_option);
		cheatcfg[c].flag      = CFG_CONTINUE;
		cheatcfg[c].value_max = (int) (a_cheat->num_cheat_options - 1);

			//ADVERTENCIA: el limite es MAX_CHEAT_OPTION
			int z = 0;
			for( z = 0; z< a_cheat->num_cheat_options; z++)
			{
				a_cheat_option = a_cheat->cheat_option[z];
				
				if(a_cheat_option->label == NULL)
					return 0;
				else
					cheatcfg[c].values_label[z] = a_cheat_option->label;
				
			}
			
			
			if (cheatcfg[c].value)
			{
				if (*cheatcfg[c].value < 0) *cheatcfg[c].value = 0;
				if (*cheatcfg[c].value > cheatcfg[c].value_max)*cheatcfg[c].value = cheatcfg[c].value_max;
					
				cheatcfg[c].old_value = *cheatcfg[c].value;
			}
	}
	cheatcfg_num = cheat_num;
	
	if( cheat_num <= 0)
		return 0;

	
	pad_wait_clear();
	load_background(WP_DIPSW);
	ui_popup_reset();

	do
	{
	int cur = *cheatcfg[sel].value;
		if (update)
		{
			show_background();

			arrowl = 0;
			arrowr = 0;
			if (cheatcfg[sel].value)
			{
				if (cur > 0) arrowl = 1;
				if (cur < cheatcfg[sel].value_max) arrowr = 1;
			}

			small_icon_shadow(8, 3, UI_COLOR(UI_PAL_TITLE), ICON_DIPSWITCH);
			uifont_print_shadow(36, 5, UI_COLOR(UI_PAL_TITLE), TEXT(CHEAT_CONFIGURATION_MENU));
			draw_scrollbar(469, 26, 479, 270, rows, cheatcfg_num, sel);

			for (i = 0; i < rows; i++)
			{
				if (top + i >= cheatcfg_num) break;

				if (cheatcfg[top + i].label[0] == '\n')
					continue;

				if (cheatcfg[top + i].enable)
				{
					int inc = 0;
					if ((top + i) == sel)
					{
						uifont_print_shadow(16, 40 + i * 17, UI_COLOR(UI_PAL_SELECT), cheatcfg[top + i].label);
						if (arrowl)
						{
							uifont_print_shadow(190 + inc, 40 + i * 17, UI_COLOR(UI_PAL_SELECT), FONT_LEFTTRIANGLE);
						}
					}
					else
						uifont_print(16, 40 + i * 17, UI_COLOR(UI_PAL_NORMAL), cheatcfg[top + i].label);
				}
				else
				{
					if ((top + i) == sel)
						uifont_print_shadow(16, 40 + i * 17, COLOR_GRAY, cheatcfg[top + i].label);
					else
						uifont_print(16, 40 + i * 17, COLOR_DARKGRAY, cheatcfg[top + i].label);
				}

				if (cheatcfg[top + i].value)
				{
					int val = *cheatcfg[top + i].value;
					int inc = 0;

					if (cheatcfg[top + i].enable)
					{
						if ((top + i) == sel)
						{
							uifont_print_shadow(210 + inc, 40 + i * 17, UI_COLOR(UI_PAL_SELECT), cheatcfg[top + i].values_label[val]);
							if (arrowr)
							{
								int width = uifont_get_string_width(cheatcfg[top + i].values_label[val]);
								uifont_print_shadow(214 + width + inc, 40 + i * 17, UI_COLOR(UI_PAL_SELECT), FONT_RIGHTTRIANGLE);
							}
						}
						else
							uifont_print(210 + inc, 40 + i * 17, UI_COLOR(UI_PAL_NORMAL), cheatcfg[top + i].values_label[val]);
						
					}
					else
					{
						if ((top + i) == sel)
							uifont_print_shadow(210 + inc, 40 + i * 17, COLOR_GRAY, cheatcfg[top + i].values_label[val]);
						else
							uifont_print(210 + inc, 40 + i * 17, COLOR_DARKGRAY, cheatcfg[top + i].values_label[val]);
					}
				}
			}

			update  = draw_battery_status(1);
			update |= draw_volume_status(1);
			update |= ui_show_popup(1);
			video_flip_screen(1);
		}
		else
		{
			update  = draw_battery_status(0);
			update |= draw_volume_status(0);
			update |= ui_show_popup(0);
			video_wait_vsync();
		}

		prev_sel = sel;

		if (pad_pressed(PLATFORM_PAD_UP))
		{
			sel--;
			if (sel < 0) sel = cheatcfg_num - 1;
			if (cheatcfg[sel].label[0] == '\n') sel--;
		}
		else if (pad_pressed(PLATFORM_PAD_DOWN))
		{
			sel++;
			if (sel > cheatcfg_num - 1) sel = 0;
			if (cheatcfg[sel].label[0] == '\n') sel++;
		}
		else if (pad_pressed(PLATFORM_PAD_L))
		{
			sel -= rows;
			if (sel < 0) sel = 0;
			if (cheatcfg[sel].label[0] == '\n') sel--;
		}
		else if (pad_pressed(PLATFORM_PAD_R))
		{
			sel += rows;
			if (sel >= cheatcfg_num) sel = cheatcfg_num - 1;
			if (cheatcfg[sel].label[0] == '\n') sel++;
		}
		else if (pad_pressed(PLATFORM_PAD_LEFT))
		{
			if (cheatcfg[sel].value && cheatcfg[sel].enable)
			{
				if (cur > 0)
				{
					*cheatcfg[sel].value = cur - 1;
					update = 1;
				}
			}
		}
		else if (pad_pressed(PLATFORM_PAD_RIGHT))
		{
			if (cheatcfg[sel].value && cheatcfg[sel].enable)
			{
				if (cur < cheatcfg[sel].value_max)
				{
					*cheatcfg[sel].value = cur + 1;
					update = 1;
				}
			}
		}
		else if (pad_pressed(PLATFORM_PAD_B1))
		{
			if (cheatcfg[sel].value && cheatcfg[sel].enable)
			{
				if (cur > 0)
				{
					*cheatcfg[sel].value = 0;
					update = 1;
				}
			}
		}/*
		else if (pad_pressed(PLATFORM_PAD_B3))
		{
			if (cheatcfg[sel].value && cheatcfg[sel].enable)
			{
				if (cur > 0)
				{
					*cheatcfg[sel].value = 0;
					update = 1;
				}
			}
		}*/
		else if (pad_pressed(PLATFORM_PAD_SELECT))
		{
			help(HELP_CHEATCONFIG);
			update = 1;
		}
		if (top > cheatcfg_num - rows) top = cheatcfg_num - rows;
		if (top < 0) top = 0;
		if (sel >= cheatcfg_num) sel = 0;
		if (sel < 0) sel = cheatcfg_num - 1;
		if (sel >= top + rows) top = sel - rows + 1;
		if (sel < top) top = sel;

		if (prev_sel != sel) update = 1;

		pad_update();

		if (Loop == LOOP_EXIT) break;

	} while (!pad_pressed(PLATFORM_PAD_B2));
/*
	for (i = 0; i < cheatcfg_num; i++)
	{
		if (cheatcfg[i].value && cheatcfg[i].flag == CFG_RESTART)
		{
			if (cheatcfg[i].old_value != *cheatcfg[i].value)
			{
				menu_restart();
				return 1;
			}
		}
	}
*/
	return 0;
}
/*------------------------------------------------------
	menu que crea estructuras dinamicas
------------------------------------------------------*/
/*int menu_cheatcfg_memfree(void)
{
	
	int sel = 0, rows = 13, top = 0;
	int i, arrowl, arrowr, prev_sel, update = 1;
	gamecfg_t gamecfg[GAMECFG_MAX_ITEMS];
	int gamecfg_num;
	int mem_free = 0;
	mem_free = get_free_memory();

	//Advertencia:limite de menus a GAMECFG_MAX_ITEMS
	for (i = 0; i < GAMECFG_MAX_ITEMS; i++)
		gamecfg[i].enable = 1;


	//Inicia la parte para copiar de gamecfg2 a gamecfg
	//Crear unos menus de forma dinamica
	int MAX_MENUS = 3;
	char *label_menus[MAX_MENUS];
	int  menu_values[MAX_MENUS];
	const char *menu_op0 = "MENU OP_A";
	const char *menu_op1 = "MENU OP_B";
	const char *menu_op2 = "MENU OP_C";
	
	for( i = 0; i< MAX_MENUS; i++){ //reservar memoria y crear cadenas
			label_menus[i] = (char *) malloc(30); //limite fijo para etiqueta de menu
			if( i == 0)
				sprintf( label_menus[i], "MFree:%d", mem_free);
			else
				sprintf( label_menus[i], "MENU DYN_%d", i);
				
			menu_values[i] = 0;
			
			gamecfg[i].label     = label_menus[i];
			gamecfg[i].value     = &menu_values[i];
			gamecfg[i].flag      = CFG_CONTINUE;
			gamecfg[i].value_max = 2;
			
			gamecfg[i].values_label[0] = menu_op0;
			gamecfg[i].values_label[1] = menu_op1;
			gamecfg[i].values_label[2] = menu_op2;
			
			if (gamecfg[i].value){
				if (*gamecfg[i].value < 0)
						*gamecfg[i].value = 0;
				if (*gamecfg[i].value > gamecfg[i].value_max)
					*gamecfg[i].value = gamecfg[i].value_max;

			gamecfg[i].old_value = *gamecfg[i].value;
		}
	}
	gamecfg_num = MAX_MENUS;
	
	pad_wait_clear();
	load_background(WP_GAMECFG);
	ui_popup_reset();

	do
	{
		if (update)
		{
			show_background();

			arrowl = 0;
			arrowr = 0;
			if (gamecfg[sel].value)
			{
				int cur = *gamecfg[sel].value;

				if (cur > 0) arrowl = 1;
				if (cur < gamecfg[sel].value_max) arrowr = 1;
			}

			small_icon_shadow(8, 3, UI_COLOR(UI_PAL_TITLE), ICON_CONFIG);
			uifont_print_shadow(36, 5, UI_COLOR(UI_PAL_TITLE), TEXT(GAME_CONFIGURATION_MENU));
			draw_scrollbar(469, 26, 479, 270, rows, gamecfg_num, sel);

			for (i = 0; i < rows; i++)
			{
				if (top + i >= gamecfg_num) break;

				if (gamecfg[top + i].label[0] == '\n')
					continue;

				if (gamecfg[top + i].enable)
				{
					if ((top + i) == sel)
					{
						uifont_print_shadow(16, 40 + i * 17, UI_COLOR(UI_PAL_SELECT), gamecfg[top + i].label);
						if (arrowl)
						{
							uifont_print_shadow(180, 40 + i * 17, UI_COLOR(UI_PAL_SELECT), FONT_LEFTTRIANGLE);
						}
					}
					else
						uifont_print(16, 40 + i * 17, UI_COLOR(UI_PAL_NORMAL), gamecfg[top + i].label);
				}
				else
				{
					if ((top + i) == sel)
						uifont_print_shadow(16, 40 + i * 17, COLOR_GRAY, gamecfg[top + i].label);
					else
						uifont_print(16, 40 + i * 17, COLOR_DARKGRAY, gamecfg[top + i].label);
				}

				if (gamecfg[top + i].value)
				{
					int val = *gamecfg[top + i].value;

					if (gamecfg[top + i].enable)
					{
						if ((top + i) == sel)
						{
							uifont_print_shadow(200, 40 + i * 17, UI_COLOR(UI_PAL_SELECT), gamecfg[top + i].values_label[val]);
							if (arrowr)
							{
								int width = uifont_get_string_width(gamecfg[top + i].values_label[val]);
								uifont_print_shadow(204 + width, 40 + i * 17, UI_COLOR(UI_PAL_SELECT), FONT_RIGHTTRIANGLE);
							}
						}
						else
							uifont_print(200, 40 + i * 17, UI_COLOR(UI_PAL_NORMAL), gamecfg[top + i].values_label[val]);
					}
					else
					{
						if ((top + i) == sel)
							uifont_print_shadow(200, 40 + i * 17, COLOR_GRAY, gamecfg[top + i].values_label[val]);
						else
							uifont_print(200, 40 + i * 17, COLOR_DARKGRAY, gamecfg[top + i].values_label[val]);
					}
				}
			}

			update  = draw_battery_status(1);
			update |= ui_show_popup(1);
			video_flip_screen(1);
		}
		else
		{
			update  = draw_battery_status(0);
			update |= ui_show_popup(0);
			video_wait_vsync();
		}

		prev_sel = sel;

		if (pad_pressed(PLATFORM_PAD_UP))
		{
			sel--;
			if (sel < 0) sel = gamecfg_num - 1;
			if (gamecfg[sel].label[0] == '\n') sel--;
		}
		else if (pad_pressed(PLATFORM_PAD_DOWN))
		{
			sel++;
			if (sel > gamecfg_num - 1) sel = 0;
			if (gamecfg[sel].label[0] == '\n') sel++;
		}
		else if (pad_pressed(PLATFORM_PAD_LEFT))
		{
			if (gamecfg[sel].value && gamecfg[sel].enable)
			{
				int cur = *gamecfg[sel].value;

				if (cur > 0)
				{
					*gamecfg[sel].value = cur - 1;
					update = 1;
				}
			}
		}
		else if (pad_pressed(PLATFORM_PAD_RIGHT))
		{
			if (gamecfg[sel].value && gamecfg[sel].enable)
			{
				int cur = *gamecfg[sel].value;

				if (cur < gamecfg[sel].value_max)
				{
					*gamecfg[sel].value = cur + 1;
					update = 1;
				}
			}
		}
		else if (pad_pressed(PLATFORM_PAD_B1))
		{
			if (sel == gamecfg_num - 1)
				break;
		}
		else if (pad_pressed(PLATFORM_PAD_R))
		{
			help(HELP_GAMECONFIG);
			update = 1;
		}

		if (top > gamecfg_num - rows) top = gamecfg_num - rows;
		if (top < 0) top = 0;
		if (sel >= gamecfg_num) sel = 0;
		if (sel < 0) sel = gamecfg_num - 1;
		if (sel >= top + rows) top = sel - rows + 1;
		if (sel < top) top = sel;

		if (prev_sel != sel) update = 1;

		pad_update();

		if (Loop == LOOP_EXIT) break;

	} while (!pad_pressed(PLATFORM_PAD_B2));

	for (i = 0; i < gamecfg_num; i++)
	{
		if (gamecfg[i].value && gamecfg[i].flag == CFG_RESTART)
		{
			if (gamecfg[i].old_value != *gamecfg[i].value)
			{
				menu_restart();
				return 1;
			}
		}
	}

	return 0;
}

*/
/*------------------------------------------------------
	ボタン設定メニュー
------------------------------------------------------*/

#define KEYCFG_MAX_ITEMS	40

static const int button_value[13] =
{
	0,
	PLATFORM_PAD_UP,
	PLATFORM_PAD_DOWN,
	PLATFORM_PAD_LEFT,
	PLATFORM_PAD_RIGHT,
	PLATFORM_PAD_B1,
	PLATFORM_PAD_B2,
	PLATFORM_PAD_B3,
	PLATFORM_PAD_B4,
	PLATFORM_PAD_L,
	PLATFORM_PAD_R,
	PLATFORM_PAD_START,
	PLATFORM_PAD_SELECT
};

typedef struct {
	const char *label;
	int type;
	int value;
	int code;
} keycfg_t;

typedef struct {
	int label;
	int type;
	int code;
} keycfg2_t;

#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
enum
{
	NEOGEO_PAD = 0,
	NEOGEO_MVS,
	NEOGEO_PS,
	USER_DEFINE
};
#endif

#define KEYCFG_COMMENT 0
#define KEYCFG_BUTTON  1
#define KEYCFG_NUMBER  2
#define KEYCFG_ANALOG  3
#define KEYCFG_LAYOUT  4

#define INCLUDE_KEYCFG_STRUCT

#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
#include "menu/cps.c"
#elif (EMU_SYSTEM == MVS)
#include "menu/mvs.c"
#elif (EMU_SYSTEM == NCDZ)
#include "menu/ncdz.c"
#endif

#undef INCLUDE_KEYCFG_STRUCT

static int menu_keycfg(void)
{
	int sel = 0, prev_sel, rows = 13, top = 0;
	int i, j, arrowl, arrowr, update = 1;
	keycfg_t keycfg[KEYCFG_MAX_ITEMS];
	keycfg2_t *keycfg2;
	int keycfg_num;
	const char *button_name[13];
	const char *sensitivity[3];
#if (EMU_SYSTEM == CPS2)
	const char *progear_p2[2];
#elif (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
	const char *layout[4];
#endif

#define INCLUDE_KEYCFG_MENU

#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
#include "menu/cps.c"
#elif (EMU_SYSTEM == MVS)
#include "menu/mvs.c"
#elif (EMU_SYSTEM == NCDZ)
#include "menu/ncdz.c"
#endif

#undef INCLUDE_KEYCFG_MENU

	for (i = 0; i < 13; i++)
		button_name[i] = TEXT(NOT_USE + i);

	sensitivity[0] = TEXT(SENSITIVITY_LOW);
	sensitivity[1] = TEXT(SENSITIVITY_NORMAL);
	sensitivity[2] = TEXT(SENSITIVITY_HIGH);

#if (EMU_SYSTEM == CPS2)
	progear_p2[0] = TEXT(PLAYER1_START);
	progear_p2[1] = TEXT(PLAYER2_START);
#elif (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
	layout[0] = TEXT(LAYOUT_TYPE1);
	layout[1] = TEXT(LAYOUT_TYPE2);
	layout[2] = TEXT(LAYOUT_TYPE3);
	layout[3] = TEXT(LAYOUT_USER);
#endif

	i = 0;
	while (keycfg2[i].label)
	{
		keycfg[i].label = TEXT(keycfg2[i].label);
		keycfg[i].type  = keycfg2[i].type;
		keycfg[i].value = 0;
		keycfg[i].code  = keycfg2[i].code;

		switch (keycfg[i].type)
		{
		case KEYCFG_BUTTON:
			{
				int code = input_map[keycfg[i].code];

				if (code)
				{
					for (j = 0; j < 13; j++)
					{
						if (code == button_value[j])
						{
							keycfg[i].value = j;
							break;
						}
					}
				}
			}
			break;

		case KEYCFG_NUMBER:
			keycfg[i].value = af_interval;
			break;

		case KEYCFG_ANALOG:
			keycfg[i].value = analog_sensitivity;
			break;
		}
		i++;
	}
	keycfg_num = i;

#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
	if (keycfg[6].value == 6 && keycfg[7].value == 5 && keycfg[8].value == 7 && keycfg[9].value == 8)
	{
		keycfg[0].value = NEOGEO_PAD;
	}
	else if (keycfg[6].value == 7 && keycfg[7].value == 8 && keycfg[8].value == 6 && keycfg[9].value == 5)
	{
		keycfg[0].value = NEOGEO_MVS;
	}
	else if (keycfg[6].value == 7 && keycfg[7].value == 6 && keycfg[8].value == 8 && keycfg[9].value == 5)
	{
		keycfg[0].value = NEOGEO_PS;
	}
	else
	{
		keycfg[0].value = USER_DEFINE;
	}
#endif

	pad_wait_clear();
	load_background(WP_KEYCFG);
	ui_popup_reset();

	do
	{
		if (update)
		{
			show_background();

			small_icon_shadow(8, 3, UI_COLOR(UI_PAL_TITLE), ICON_KEYCONFIG);
			uifont_print_shadow(36, 5, UI_COLOR(UI_PAL_TITLE), TEXT(KEY_CONFIGURATION_MENU));
			draw_scrollbar(469, 26, 479, 270, rows, keycfg_num, sel);

			arrowl = 0;
			arrowr = 0;
			if (keycfg[sel].type)
			{
				if (keycfg[sel].value > 0) arrowl = 1;
				if (keycfg[sel].type == KEYCFG_BUTTON && keycfg[sel].value < 12) arrowr = 1;
				if (keycfg[sel].type == KEYCFG_NUMBER && keycfg[sel].value < 10) arrowr = 1;
				if (keycfg[sel].type == KEYCFG_ANALOG && keycfg[sel].value < 2) arrowr = 1;
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
				if (keycfg[sel].type == KEYCFG_LAYOUT && keycfg[sel].value < 1) arrowr = 1;
#endif
			}

			for (i = 0; i < rows; i++)
			{
				int value = keycfg[top + i].value;
				char temp[16];
				const char *name;
				const char *label = keycfg[top + i].label;

				if ((top + i) >= keycfg_num) break;

				if (label[0] == '\n')
					continue;

				switch (keycfg[top + i].type)
				{
				case KEYCFG_BUTTON:
#if (EMU_SYSTEM == CPS2)
					if (!strcmp(driver->name, "progear"))
						if (top + i == 10) label = progear_p2[option_controller ^ 1];
#endif
					name = button_name[value];
					break;

				case KEYCFG_NUMBER:
					if (value == 0)
						sprintf(temp, TEXT(xFRAME), value + 1);
					else
						sprintf(temp, TEXT(xFRAMES), value + 1);
					name = (const char *)temp;
					break;

				case KEYCFG_ANALOG:
					name = sensitivity[value];
					break;

#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
				case KEYCFG_LAYOUT:
					name = layout[value];
					break;
#endif

				default:
					temp[0] = '\0';
					name = temp;
					break;
				}

				if ((top + i) == sel)
				{
					uifont_print_shadow(16, 40 + i * 17, UI_COLOR(UI_PAL_SELECT), label);
					uifont_print_shadow(210, 40 + i * 17, UI_COLOR(UI_PAL_SELECT), name);

					if (arrowl)
					{
						uifont_print_shadow(190, 40 + i * 17, UI_COLOR(UI_PAL_SELECT), FONT_LEFTTRIANGLE);
					}
					if (arrowr)
					{
						int width = uifont_get_string_width(name);
						uifont_print_shadow(214 + width, 40 + i * 17, UI_COLOR(UI_PAL_SELECT), FONT_RIGHTTRIANGLE);
					}
				}
				else
				{
					uifont_print(16, 40 + i * 17, UI_COLOR(UI_PAL_NORMAL), label);
					uifont_print(210, 40 + i * 17, UI_COLOR(UI_PAL_NORMAL), name);
				}
			}

			update  = draw_battery_status(1);
			update |= draw_volume_status(1);
			update |= ui_show_popup(1);
			video_flip_screen(1);
		}
		else
		{
			update  = draw_battery_status(0);
			update |= draw_volume_status(0);
			update |= ui_show_popup(0);
			video_wait_vsync();
		}

		prev_sel = sel;

		if (pad_pressed(PLATFORM_PAD_UP))
		{
			sel--;
			if (sel < 0) sel = keycfg_num - 1;
			if (keycfg[sel].label[0] == '\n') sel--;
		}
		else if (pad_pressed(PLATFORM_PAD_DOWN))
		{
			sel++;
			if (sel > keycfg_num - 1) sel = 0;
			if (keycfg[sel].label[0] == '\n') sel++;
		}
		else if (pad_pressed(PLATFORM_PAD_L))
		{
			sel -= rows;
			if (sel < 0) sel = 0;
			if (keycfg[sel].label[0] == '\n') sel--;
		}
		else if (pad_pressed(PLATFORM_PAD_R))
		{
			sel += rows;
			if (sel >= keycfg_num) sel = keycfg_num - 1;
			if (keycfg[sel].label[0] == '\n') sel++;
		}
		else if (pad_pressed(PLATFORM_PAD_LEFT))
		{
			if (keycfg[sel].type)
			{
				if (keycfg[sel].value > 0)
				{
					keycfg[sel].value--;
					update = 1;
				}
			}
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
			if (keycfg[sel].type == KEYCFG_LAYOUT && update)
			{
				switch (keycfg[sel].value)
				{
				case NEOGEO_PAD:
					keycfg[6].value = 6;
					keycfg[7].value = 5;
					keycfg[8].value = 7;
					keycfg[9].value = 8;
					break;

				case NEOGEO_MVS:
					keycfg[6].value = 7;
					keycfg[7].value = 8;
					keycfg[8].value = 6;
					keycfg[9].value = 5;
					break;
				case NEOGEO_PS:
					keycfg[6].value = 7;
					keycfg[7].value = 6;
					keycfg[8].value = 8;
					keycfg[9].value = 5;
					break;
				}
			}
#endif
		}
		else if (pad_pressed(PLATFORM_PAD_RIGHT))
		{
			if (keycfg[sel].type == KEYCFG_BUTTON)
			{
				if (keycfg[sel].value < 12)
				{
					keycfg[sel].value++;
					update = 1;
				}
			}
			else if (keycfg[sel].type == KEYCFG_NUMBER)
			{
				if (keycfg[sel].value < 10)
				{
					keycfg[sel].value++;
					update = 1;
				}
			}
			else if (keycfg[sel].type == KEYCFG_ANALOG)
			{
				if (keycfg[sel].value < 2)
				{
					keycfg[sel].value++;
					update = 1;
				}
			}
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
			else if (keycfg[sel].type == KEYCFG_LAYOUT)
			{
				if (keycfg[sel].value == 0)
				{
					keycfg[sel].value++;
					update = 1;

					keycfg[6].value = 7;
					keycfg[7].value = 8;
					keycfg[8].value = 6;
					keycfg[9].value = 5;
				}
				else if (keycfg[sel].value == 1)
				{
					keycfg[sel].value++;
					update = 1;
					keycfg[6].value = 7;
					keycfg[7].value = 6;
					keycfg[8].value = 8;
					keycfg[9].value = 5;
				}
			}
#endif
		}
		else if (pad_pressed(PLATFORM_PAD_B1))
		{
			if (sel == keycfg_num - 1)
				break;
		}
		else if (pad_pressed(PLATFORM_PAD_SELECT))
		{
			help(HELP_KEYCONFIG);
			update = 1;
		}

		if (top > keycfg_num - rows) top = keycfg_num - rows;
		if (top < 0) top = 0;
		if (sel >= keycfg_num) sel = 0;
		if (sel < 0) sel = keycfg_num - 1;
		if (sel >= top + rows) top = sel - rows + 1;
		if (sel < top) top = sel;

		if (prev_sel != sel) update = 1;

#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		if (update)
		{
			if (keycfg[6].value == 6 && keycfg[7].value == 5 && keycfg[8].value == 7 && keycfg[9].value == 8)
			{
				keycfg[0].value = NEOGEO_PAD;
			}
			else if (keycfg[6].value == 7 && keycfg[7].value == 8 && keycfg[8].value == 6 && keycfg[9].value == 5)
			{
				keycfg[0].value = NEOGEO_MVS;
			}
			else if (keycfg[6].value == 7 && keycfg[7].value == 6 && keycfg[8].value == 8 && keycfg[9].value == 5)
			{
				keycfg[0].value = NEOGEO_PS;
			}
			else
			{
				keycfg[0].value = USER_DEFINE;
			}
		}
#endif

		pad_update();

		if (Loop == LOOP_EXIT) break;

	} while (!pad_pressed(PLATFORM_PAD_B2));

	for (i = 0; i < keycfg_num; i++)
	{
		if (keycfg[i].type == KEYCFG_BUTTON)
		{
			input_map[keycfg[i].code] = button_value[keycfg[i].value];
		}
		else if (keycfg[i].type == KEYCFG_NUMBER)
		{
			af_interval = keycfg[i].value;
		}
		else if (keycfg[i].type == KEYCFG_ANALOG)
		{
			analog_sensitivity = keycfg[i].value;
		}
	}

	return 0;
}


/*------------------------------------------------------
	ディップスイッチ設定メニュー
------------------------------------------------------*/

#if (EMU_SYSTEM != CPS2 && EMU_SYSTEM != NCDZ)

static int menu_dipswitch(void)
{
	int sel = 0, rows = 13, top = 0, sx = 240;
	int i, arrowl, arrowr, prev_sel, update = 1;
	dipswitch_t *dipswitch;
	int dipswitch_num = 0;

#define INCLUDE_LOAD_DIPSWITCH

#if (EMU_SYSTEM == CPS1)
#include "menu/cps.c"
#elif (EMU_SYSTEM == MVS)
#include "menu/mvs.c"
#endif

#undef INCLUDE_LOAD_DIPSWITCH

	while (dipswitch[dipswitch_num].label[0])
		dipswitch_num++;

	pad_wait_clear();
	load_background(WP_DIPSW);
	ui_popup_reset();

	do
	{
		if (update)
		{
			show_background();

			arrowl = 0;
			arrowr = 0;
			if (dipswitch[sel].mask)
			{
				int cur = dipswitch[sel].value;

				if (cur > 0) arrowl = 1;
				if (cur < dipswitch[sel].value_max) arrowr = 1;
			}

			small_icon_shadow(8, 3, UI_COLOR(UI_PAL_TITLE), ICON_DIPSWITCH);
			uifont_print_shadow(36, 5, UI_COLOR(UI_PAL_TITLE), TEXT(DIP_SWITCH_SETTINGS_MENU));
			draw_scrollbar(469, 26, 479, 270, rows, dipswitch_num, sel);

			for (i = 0; i < rows; i++)
			{
				if ((top + i) >= dipswitch_num) break;

				if (dipswitch[top + i].label[0] == '\n')
					continue;

				if (top + i == sel)
				{
					if (!dipswitch[top + i].enable)
					{
						uifont_print_shadow(16, 40 + i * 17, COLOR_GRAY, dipswitch[top + i].label);
					}
					else
					{
						uifont_print_shadow(16, 40 + i * 17, UI_COLOR(UI_PAL_SELECT), dipswitch[top + i].label);
						if (arrowl)
							uifont_print_shadow(sx - 20, 40 + i * 17, UI_COLOR(UI_PAL_SELECT), FONT_LEFTTRIANGLE);
					}
				}
				else
				{
					if (!dipswitch[top + i].enable)
						uifont_print(16, 40 + i * 17, COLOR_DARKGRAY, dipswitch[top + i].label);
					else
						uifont_print(16, 40 + i * 17, UI_COLOR(UI_PAL_NORMAL), dipswitch[top + i].label);
				}

				if (dipswitch[top + i].mask)
				{
					int val = dipswitch[top + i].value;

					if (top + i == sel)
					{
						if (!dipswitch[top + i].enable)
						{
							uifont_print_shadow(sx, 40 + i * 17, COLOR_GRAY, dipswitch[top + i].values_label[val]);
						}
						else
						{
							uifont_print_shadow(sx, 40 + i * 17, UI_COLOR(UI_PAL_SELECT), dipswitch[top + i].values_label[val]);
							if (arrowr)
							{
								int width = uifont_get_string_width(dipswitch[top + i].values_label[val]);
								uifont_print_shadow(sx + 4 + width, 40 + i * 17, UI_COLOR(UI_PAL_SELECT), FONT_RIGHTTRIANGLE);
							}
						}
					}
					else
					{
						if (!dipswitch[top + i].enable)
							uifont_print(sx, 40 + i * 17, COLOR_DARKGRAY, dipswitch[top + i].values_label[val]);
						else
							uifont_print(sx, 40 + i * 17, UI_COLOR(UI_PAL_NORMAL), dipswitch[top + i].values_label[val]);
					}
				}
			}

			update  = draw_battery_status(1);
			update |= draw_volume_status(1);
			update |= ui_show_popup(1);
			video_flip_screen(1);
		}
		else
		{
			update  = draw_battery_status(0);
			update |= draw_volume_status(0);
			update |= ui_show_popup(0);
			video_wait_vsync();
		}

		prev_sel = sel;

		if (pad_pressed(PLATFORM_PAD_UP))
		{
			sel--;
			if (sel < 0) sel = dipswitch_num - 1;
			if (dipswitch[sel].label[0] == '\n') sel--;
		}
		else if (pad_pressed(PLATFORM_PAD_DOWN))
		{
			sel++;
			if (sel > dipswitch_num - 1) sel = 0;
			if (dipswitch[sel].label[0] == '\n') sel++;
		}
		else if (pad_pressed(PLATFORM_PAD_L))
		{
			sel -= rows;
			if (sel < 0) sel = 0;
			if (dipswitch[sel].label[0] == '\n') sel--;
		}
		else if (pad_pressed(PLATFORM_PAD_R))
		{
			sel += rows;
			if (sel >= dipswitch_num) sel = dipswitch_num - 1;
			if (dipswitch[sel].label[0] == '\n') sel++;
		}
		else if (pad_pressed(PLATFORM_PAD_LEFT))
		{
			if (dipswitch[sel].mask)
			{
				int cur = dipswitch[sel].value;

				if (cur > 0)
				{
					dipswitch[sel].value = cur - 1;
					update = 1;
				}
			}
		}
		else if (pad_pressed(PLATFORM_PAD_RIGHT))
		{
			if (dipswitch[sel].mask)
			{
				int cur = dipswitch[sel].value;

				if (cur < dipswitch[sel].value_max)
				{
					dipswitch[sel].value = cur + 1;
					update = 1;
				}
			}
		}
		else if (pad_pressed(PLATFORM_PAD_B1))
		{
			if (sel == dipswitch_num - 1)
				break;
		}
		else if (pad_pressed(PLATFORM_PAD_SELECT))
		{
			help(HELP_DIPSWITCH);
			update = 1;
		}

		if (top > dipswitch_num - rows) top = dipswitch_num - rows;
		if (top < 0) top = 0;
		if (sel >= dipswitch_num) sel = 0;
		if (sel < 0) sel = dipswitch_num - 1;
		if (sel >= top + rows) top = sel - rows + 1;
		if (sel < top) top = sel;

		if (prev_sel != sel) update = 1;

		pad_update();

		if (Loop == LOOP_EXIT) break;

	} while (!pad_pressed(PLATFORM_PAD_B2));

#define INCLUDE_SAVE_DIPSWITCH

#if (EMU_SYSTEM == CPS1)
#include "menu/cps.c"
#elif (EMU_SYSTEM == MVS)
#include "menu/mvs.c"
#endif

#undef INCLUDE_SAVE_DIPSWITCH

	return 0;
}

#endif


/*------------------------------------------------------
	セーブ/ロードステート
------------------------------------------------------*/

#ifdef SAVE_STATE

#define STATE_FUNC_SAVE		0
#define STATE_FUNC_LOAD		1
#define STATE_FUNC_DEL		2


static uint8_t slot[10];
static int state_func;
static int state_sel;

static void state_draw_thumbnail(void)
{
	void *tex = UI_TEXTURE;

#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
	if (machine_screen_type)
	{
		RECT clip1 = { 0, 0, 112, 152 };
		RECT clip2 = { 317, 34, 317+112, 34+152 };
		video_draw_texture(GU_PSM_5551, VRAM_FMT, tex, draw_frame, &clip1, &clip2);
	}
	else
#endif
	{
		RECT clip1 = { 0, 0, 152, 112 };
		RECT clip2 = { 298, 52, 298+152, 52+112 };
		video_draw_texture(GU_PSM_5551, VRAM_FMT, tex, draw_frame, &clip1, &clip2);
	}
}

static void state_refresh_screen(int reload_thumbnail)
{
	int i, x;
	char name[16], state[32], buf[64];

	if (reload_thumbnail)
	{
		find_state_file(slot);

		if (!slot[state_sel])
		{
			state_clear_thumbnail();
			state_func = STATE_FUNC_SAVE;
		}
		else
		{
			state_load_thumbnail(state_sel);
			if (state_version == current_state_version)
				state_func = STATE_FUNC_LOAD;
			else
			{
				state_func = STATE_FUNC_SAVE;
				strcpy(date_str, "----/--/--");
				strcpy(time_str, "--:--:--");
			}
		}
	}

	show_background();
	if (machine_screen_type)
		draw_box_shadow(318, 33, 429, 184);
	else
		draw_box_shadow(298, 52, 449, 163);
	state_draw_thumbnail();

	sprintf(buf, "%s %s", FONT_LTRIGGER, TEXT(MAIN_MENU));
	i = uifont_get_string_width(buf);
	uifont_print_shadow(8, 5, UI_COLOR(UI_PAL_NORMAL), buf);

	sprintf(buf, "%s %s", TEXT(SAVE_LOAD_STATE), FONT_RTRIGGER);
	uifont_print_shadow(i + 16, 5, UI_COLOR(UI_PAL_NORMAL), "|");
	uifont_print_shadow(i + 24, 5, UI_COLOR(UI_PAL_TITLE), buf);

	if (ui_text_get_language() == LANG_JAPANESE)
		x = 20;
	else
		x = 0;

	for (i = 0; i < 10; i++)
	{
		sprintf(name, TEXT(SLOTx), i);
		if (slot[i])
			sprintf(state, "%s.sv%d", game_name, i);
		else
			strcpy(state, TEXT(EMPTY));

		if (i == state_sel)
		{
			small_icon_light(12, 38 + i * 22, UI_COLOR(UI_PAL_SELECT), ICON_MEMSTICK);
			uifont_print_shadow(40, 40 + i * 22, UI_COLOR(UI_PAL_SELECT), name);
			uifont_print_shadow(92 + x, 40 + i * 22, UI_COLOR(UI_PAL_SELECT), state);

			if (slot[state_sel])
			{
				if (state_func == STATE_FUNC_LOAD)
				{
					uifont_print_shadow(200, 40 + i * 22, UI_COLOR(UI_PAL_SELECT), FONT_LEFTTRIANGLE);
					uifont_print_shadow(222, 40 + i * 22, UI_COLOR(UI_PAL_SELECT), TEXT(LOAD));
					uifont_print_shadow(262, 40 + i * 22, UI_COLOR(UI_PAL_SELECT), FONT_RIGHTTRIANGLE);
				}
				else if (state_func == STATE_FUNC_DEL)
				{
					uifont_print_shadow(200, 40 + i * 22, UI_COLOR(UI_PAL_SELECT), FONT_LEFTTRIANGLE);
					uifont_print_shadow(222, 40 + i * 22, UI_COLOR(UI_PAL_SELECT), TEXT(DELETE));
				}
				else
				{
					uifont_print_shadow(222, 40 + i * 22, UI_COLOR(UI_PAL_SELECT), TEXT(SAVE));
					uifont_print_shadow(262, 40 + i * 22, UI_COLOR(UI_PAL_SELECT), FONT_RIGHTTRIANGLE);
				}
			}
			else
			{
				uifont_print_shadow(222, 40 + i * 22, UI_COLOR(UI_PAL_SELECT), TEXT(SAVE));
			}

			hline_gradation(40, 278, 56 + i * 22, UI_COLOR(UI_PAL_NORMAL), UI_COLOR(UI_PAL_SELECT), 14);
			hline_gradation(41, 279, 57 + i * 22, COLOR_BLACK, COLOR_BLACK, 8);
		}
		else
		{
			small_icon(12, 38 + i * 22, UI_COLOR(UI_PAL_NORMAL), ICON_MEMSTICK);
			uifont_print(40, 40 + i * 22, UI_COLOR(UI_PAL_NORMAL), name);
			uifont_print(92 + x, 40 + i * 22, UI_COLOR(UI_PAL_NORMAL), state);
		}
	}

	if (state_version == current_state_version || !slot[state_sel])
	{
#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
		if (machine_screen_type)
		{
			uifont_print(300, 198, UI_COLOR(UI_PAL_NORMAL), TEXT(PLAY_DATE));
			uifont_print(378, 198, UI_COLOR(UI_PAL_NORMAL), date_str);
			uifont_print(300, 218, UI_COLOR(UI_PAL_NORMAL), TEXT(SAVE_TIME));
			uifont_print(378, 218, UI_COLOR(UI_PAL_NORMAL), time_str);
			uifont_print(300, 238, UI_COLOR(UI_PAL_NORMAL), TEXT(STATE_VERSION));
			uifont_print(378, 238, UI_COLOR(UI_PAL_NORMAL), stver_str);
		}
		else
#endif
		{
			uifont_print(290, 190, UI_COLOR(UI_PAL_NORMAL), TEXT(PLAY_DATE));
			uifont_print(368, 190, UI_COLOR(UI_PAL_NORMAL), date_str);
			uifont_print(290, 210, UI_COLOR(UI_PAL_NORMAL), TEXT(SAVE_TIME));
			uifont_print(368, 210, UI_COLOR(UI_PAL_NORMAL), time_str);
			uifont_print(290, 230, UI_COLOR(UI_PAL_NORMAL), TEXT(STATE_VERSION));
			uifont_print(368, 230, UI_COLOR(UI_PAL_NORMAL), stver_str);
		}
	}
	else
	{
#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
		if (machine_screen_type)
		{
			uifont_print(300, 198, COLOR_GRAY, TEXT(PLAY_DATE));
			uifont_print(378, 198, COLOR_GRAY, date_str);
			uifont_print(300, 218, COLOR_GRAY, TEXT(SAVE_TIME));
			uifont_print(378, 218, COLOR_GRAY, time_str);
			uifont_print(300, 238, COLOR_GRAY, TEXT(STATE_VERSION));
			uifont_print(378, 238, COLOR_GRAY, stver_str);
		}
		else
#endif
		{
			uifont_print(290, 190, COLOR_GRAY, TEXT(PLAY_DATE));
			uifont_print(368, 190, COLOR_GRAY, date_str);
			uifont_print(290, 210, COLOR_GRAY, TEXT(SAVE_TIME));
			uifont_print(368, 210, COLOR_GRAY, time_str);
			uifont_print(290, 230, COLOR_GRAY, TEXT(STATE_VERSION));
			uifont_print(368, 230, COLOR_GRAY, stver_str);
		}
	}
}

static int state_save_slot(void)
{
	if (messagebox(MB_STARTSAVESTATE))
	{
		int res;

		state_refresh_screen(0);
		video_copy_rect(draw_frame, work_frame, &full_rect, &full_rect);

		set_cpu_clock(platform_cpuclock);
		res = state_save(state_sel);
		set_cpu_clock(PSPCLOCK_222);

		load_background(WP_STATE);

		if (res)
		{
			state_refresh_screen(1);
			draw_battery_status(1);
			video_copy_rect(draw_frame, show_frame, &full_rect, &full_rect);
			res = messagebox(MB_FINISHSAVESTATE);
		}
		return res + 1;
	}
	return 0;
}

static int state_load_slot(void)
{
	if (messagebox(MB_STARTLOADSTATE))
	{
		int res;

		state_refresh_screen(0);
		video_copy_rect(draw_frame, work_frame, &full_rect, &full_rect);

		set_cpu_clock(platform_cpuclock);
		res = state_load(state_sel);
		set_cpu_clock(PSPCLOCK_222);

		if (res)
		{
			show_background();
			draw_battery_status(1);
			video_flip_screen(1);

			messagebox(MB_FINISHLOADSTATE);
		}

		load_background(WP_STATE);
		return res + 1;
	}
	return 0;
}

static void state_delete_slot(void)
{
	if (messagebox(MB_DELETESTATE))
	{
		char path[MAX_PATH];

		sprintf(path, "%sstate/%s.sv%d", launchDir, game_name, state_sel);
		if (sceIoRemove(path) < 0)
		{
			ui_popup(TEXT(FAILD_TO_DELETE_FILEx), strrchr(path, '/') + 1);
		}
		else
		{
			slot[state_sel] = 0;
			state_func = STATE_FUNC_SAVE;
			state_clear_thumbnail();
		}
	}
}

static int menu_state(void)
{
	int prev_sel = -1, prev_func = -1, update = 1;

	state_func = STATE_FUNC_SAVE;
	state_sel = 0;

	pad_wait_clear();
	load_background(WP_STATE);
	ui_popup_reset();

	memset(slot, 0, sizeof(slot));
	state_clear_thumbnail();

	find_state_file(slot);

	do
	{
		if (update & UI_FULL_REFRESH)
		{
			state_refresh_screen((prev_sel == state_sel) ? 0 : 1);

			update  = draw_battery_status(1);
			update |= draw_volume_status(1);
			update |= ui_show_popup(1);
			video_flip_screen(1);
		}
		else if (update & UI_PARTIAL_REFRESH)
		{
			int x, y, w, h;
			RECT clip1, clip2;

			show_background();

			small_icon_light(12, 38 + state_sel * 22, UI_COLOR(UI_PAL_SELECT), ICON_MEMSTICK);

			x = 12;
			y = 38 + state_sel * 22;
			w = 24 + 8;
			h = 18 + 8;

			clip1.left   = x - 4;
			clip1.top    = y - 4;
			clip1.right  = clip1.left + w;
			clip1.bottom = clip1.top  + h;

			clip2.left   = 0;
			clip2.top    = 112;
			clip2.right  = clip2.left + w;
			clip2.bottom = clip2.top  + h;

			video_copy_rect(draw_frame, tex_frame, &clip1, &clip2);
			video_copy_rect(show_frame, draw_frame, &full_rect, &full_rect);
			video_copy_rect(tex_frame, draw_frame, &clip2, &clip1);

			update  = draw_battery_status(0);
			update |= draw_volume_status(0);
			update |= ui_show_popup(0);
			video_flip_screen(1);
		}
		else
		{
			update  = draw_battery_status(0);
			update |= draw_volume_status(0);
			update |= ui_show_popup(0);
			video_wait_vsync();
		}

		update |= ui_light_update();
		prev_sel = state_sel;
		prev_func = state_func;

		if (pad_pressed(PLATFORM_PAD_UP))
		{
			state_sel--;
			if (state_sel < 0) state_sel = 10 - 1;
		}
		else if (pad_pressed(PLATFORM_PAD_DOWN))
		{
			state_sel++;
			if (state_sel >= 10) state_sel = 0;
		}
		else if (pad_pressed(PLATFORM_PAD_LEFT))
		{
			if (state_func > STATE_FUNC_SAVE) state_func--;
			if (state_version != current_state_version && state_func == STATE_FUNC_LOAD)
				state_func--;
		}
		else if (pad_pressed(PLATFORM_PAD_RIGHT))
		{
			if (slot[state_sel])
			{
				if (state_func < STATE_FUNC_DEL) state_func++;
				if (state_version != current_state_version && state_func == STATE_FUNC_LOAD)
					state_func++;
			}
		}
		else if (pad_pressed(PLATFORM_PAD_B1))
		{
			int res = 0;

			if (state_func == STATE_FUNC_LOAD)
			{
				if (slot[state_sel])
				{
					if ((res = state_load_slot()) != 0)
					{
						update = 1;
						res--;
					}
				}
			}
			else if (state_func == STATE_FUNC_DEL)
			{
				if (slot[state_sel])
				{
					state_delete_slot();
					update = 1;
				}
			}
			else
			{
				if ((res = state_save_slot()) != 0)
				{
					update = 1;
					res--;
				}
			}
			if (res) return 1;
		}
		else if (pad_pressed(PLATFORM_PAD_SELECT))
		{
			help(HELP_STATE);
			update = 1;
		}

		if (prev_sel != state_sel || prev_func != state_func)
			update = 1;

		if (prev_sel != state_sel)
			find_state_file(slot);

		pad_update();

		if (Loop == LOOP_EXIT) break;

	} while (!pad_pressed(PLATFORM_PAD_L) && !pad_pressed(PLATFORM_PAD_B2));

	return 0;
}

#endif


/*------------------------------------------------------
	メインメニュー
------------------------------------------------------*/

#define MENU_MAX_ITEMS	16

typedef struct
{
	const char *label;
	int (*menu_func)(void);
	int icon;
	const char *help;
} menu_t;

typedef struct
{
	int label;
	int (*menu_func)(void);
	int icon;
	int help;
} menu2_t;


static menu2_t mainmenu2[] =
{
	{ MENU_GAME_CONFIGURATION,  menu_gamecfg,   ICON_CONFIG,    MENUHELP_GAME_CONFIGURATION  },
	{ MENU_KEY_CONFIGURATION,   menu_keycfg,    ICON_KEYCONFIG, MENUHELP_KEY_CONFIGURATION   },
#ifdef COMMAND_LIST
	{ MENU_COMMAND_LIST,        menu_cmdlist,   ICON_CMDLIST,   MENUHELP_COMMAND_LIST        },
#endif
#if (EMU_SYSTEM != CPS2 && EMU_SYSTEM != NCDZ)
	{ MENU_DIP_SWITCH_SETTINGS, menu_dipswitch, ICON_DIPSWITCH, MENUHELP_DIP_SWITCH_SETTINGS },
#endif
	{ MENU_RESET_EMULATION,		menu_reset,		 ICON_SYSTEM,		MENUHELP_RESET_EMULATION     },
	{ MENU_RETURN_TO_BROWSER,	menu_browser,	 ICON_FOLDER,		MENUHELP_RETURN_TO_BROWSER   },
	{ MENU_RETURN_TO_GAME,		NULL,			 ICON_RETURN,		MENUHELP_RETURN_TO_GAME      },
	{ MENU_CHEAT_CONFIGURATION,	menu_cheatcfg,	 ICON_DIPSWITCH,	MENUHELP_CHEAT_CONFIGURATION },
/*
	{ MENU_MEM_FREE_DAVEX,  	menu_cheatcfg_memfree,   ICON_FOLDER,    MENUAYUDA_MEM_FREE_DAVEX  },
*/
	{ MENU_EXIT_EMULATOR,		menu_exit,      ICON_EXIT,      MENUHELP_EXIT_EMULATOR       },
	MENU_END
};


void showmenu(void)
{
	static int sel = 0, prev_sel = 0;
	static int rows = 7, top = 0;
	int i, update = 1;
	int mainmenu_num = 0;
	char buf[128];
	menu_t mainmenu[MENU_MAX_ITEMS];

#ifdef SAVE_STATE
	state_make_thumbnail();
#endif
	video_set_mode(32);

#if (EMU_SYSTEM == NCDZ)
	if (cdda_playing == CDDA_PLAY) mp3_pause(1);
#endif
	sound_mute(1);
#if USE_CACHE
	cache_sleep(1);
#endif

	i = 0;
	while (mainmenu2[i].label)
	{
		mainmenu[i].label     = TEXT(mainmenu2[i].label);
		mainmenu[i].menu_func = mainmenu2[i].menu_func;
		mainmenu[i].icon      = mainmenu2[i].icon;
		mainmenu[i].help      = TEXT(mainmenu2[i].help);
		i++;
	}
	mainmenu_num = i;

	set_cpu_clock(PSPCLOCK_222);
	video_clear_screen();
	load_background(WP_LOGO);
	ui_popup_reset();
	pad_wait_clear();

	do
	{
		if (update & UI_FULL_REFRESH)
		{
			show_background();

#ifdef SAVE_STATE
			sprintf(buf, "%s %s", FONT_LTRIGGER, TEXT(MAIN_MENU));
			i = uifont_get_string_width(buf);
			uifont_print_shadow(8, 5, UI_COLOR(UI_PAL_TITLE), buf);

			sprintf(buf, "%s %s", TEXT(SAVE_LOAD_STATE), FONT_RTRIGGER);
			uifont_print_shadow(i + 16, 5, UI_COLOR(UI_PAL_NORMAL), "|");
			uifont_print_shadow(i + 24, 5, UI_COLOR(UI_PAL_NORMAL), buf);
#else
			small_icon_shadow(8, 3, UI_COLOR(UI_PAL_TITLE), ICON_SYSTEM);
			uifont_print_shadow(36, 5, UI_COLOR(UI_PAL_TITLE), TEXT(MAIN_MENU));
#endif
			draw_scrollbar(469, 26, 479, 270, rows, mainmenu_num, sel);

			for (i = 0; i < rows; i++)
			{
				if ((top + i) >= mainmenu_num) break;

				if (mainmenu[top + i].label[0] == '\n')
					continue;

				if (top + i == sel)
				{
					large_icon_light(12, 40 + i * 32, UI_COLOR(UI_PAL_SELECT), mainmenu[sel].icon);
					uifont_print_shadow(54, 37 + i * 32, UI_COLOR(UI_PAL_SELECT), mainmenu[sel].label);
					uifont_print_shadow(54, 56 + i * 32, UI_COLOR(UI_PAL_SELECT), mainmenu[sel].help);

					hline_gradation(54, 460, 53 + i * 32, UI_COLOR(UI_PAL_NORMAL), UI_COLOR(UI_PAL_SELECT), 14);
					hline_gradation(55, 461, 54 + i * 32, COLOR_BLACK, COLOR_BLACK, 8);
				}
				else
				{
					small_icon(16, 42 + i * 32, UI_COLOR(UI_PAL_NORMAL), mainmenu[top + i].icon);
					uifont_print(56, 44 + i * 32, UI_COLOR(UI_PAL_NORMAL), mainmenu[top + i].label);
				}
			}

			update  = draw_battery_status(1);
			update |= draw_volume_status(1);
			update |= ui_show_popup(1);
			video_flip_screen(1);
		}
		else if (update & UI_PARTIAL_REFRESH)
		{
			int x, y, w, h;
			RECT clip1, clip2;

			show_background();

			for (i = 0; i < rows; i++)
				if (top + i == sel) break;

			large_icon_light(12, 40 + i * 32, UI_COLOR(UI_PAL_SELECT), mainmenu[sel].icon);

			x = 12;
			y = 40 + i * 32;
			w = 32 + 8;
			h = 24 + 8;

			clip1.left   = x - 4;
			clip1.top    = y - 4;
			clip1.right  = clip1.left + w;
			clip1.bottom = clip1.top  + h;

			clip2.left   = 0;
			clip2.top    = 112;
			clip2.right  = clip2.left + w;
			clip2.bottom = clip2.top  + h;

			video_copy_rect(draw_frame, tex_frame, &clip1, &clip2);
			video_copy_rect(show_frame, draw_frame, &full_rect, &full_rect);
			video_copy_rect(tex_frame, draw_frame, &clip2, &clip1);

			update  = draw_battery_status(0);
			update |= draw_volume_status(0);
			update |= ui_show_popup(0);
			video_flip_screen(1);
		}
		else
		{
			update  = draw_battery_status(0);
			update |= draw_volume_status(0);
			update |= ui_show_popup(0);
			video_wait_vsync();
		}

		update |= ui_light_update();
		prev_sel = sel;

		if (pad_pressed(PLATFORM_PAD_UP))
		{
			sel--;
			if (sel < 0) sel = mainmenu_num - 1;
		}
		else if (pad_pressed(PLATFORM_PAD_DOWN))
		{
			sel++;
			if (sel >= mainmenu_num) sel = 0;
		}
		else if (pad_pressed(PLATFORM_PAD_B1))
		{
			if (mainmenu[sel].menu_func)
			{
				int res = mainmenu[sel].menu_func();

				if (res == 0)
				{
					pad_wait_clear();
					load_background(WP_LOGO);
					update = 1;
				}
				else break;
			}
			else break;
		}
		else if (pad_pressed(PLATFORM_PAD_SELECT))
		{
			help(HELP_MAINMENU);
			update = 1;
		}
#ifdef SAVE_STATE
		else if (pad_pressed(PLATFORM_PAD_R))
		{
			if (menu_state()) break;
			pad_wait_clear();
			load_background(WP_LOGO);
		}
#endif
		update = 1;

		if (top > mainmenu_num - rows) top = mainmenu_num - rows;
		if (top < 0) top = 0;
		if (sel >= mainmenu_num) sel = 0;
		if (sel < 0) sel = mainmenu_num - 1;
		if (sel >= top + rows) top = sel - rows + 1;
		if (sel < top) top = sel;

		if (prev_sel != sel) update |= 1;

		pad_update();

		if (Loop == LOOP_EXIT) break;

	} while (!pad_pressed(PLATFORM_PAD_B2));

	autoframeskip_reset();
	blit_clear_all_sprite();

	pad_wait_clear();
	ui_popup_reset();
	video_set_mode(16);
	video_clear_screen();
	video_clear_frame(work_frame);
#if (EMU_SYSTEM != CPS2)
	sound_set_samplerate();
#endif
	set_cpu_clock(platform_cpuclock);

#if USE_CACHE
	cache_sleep(0);
#endif

#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
	neogeo_reset_driver_type();
#endif

	if (Loop == LOOP_EXEC)
	{
		sound_thread_set_volume();
#if (EMU_SYSTEM == NCDZ)
		mp3_set_volume();

		if (option_mp3_enable && cdda_playing == CDDA_PLAY)
			mp3_pause(0);
		else
			cdda_stop();
#endif
		sound_mute(0);
	}
	else
	{
#if (EMU_SYSTEM == NCDZ)
		cdda_stop();
#endif
		sound_mute(1);
	}
}


#if VIDEO_32BPP

/*------------------------------------------------------
	カラー設定メニュー
------------------------------------------------------*/

#define COLOR_ITEMS 12

static const int palette[COLOR_ITEMS] =
{
	-1,
	-1,
	UI_PAL_TITLE,
	UI_PAL_SELECT,
	UI_PAL_NORMAL,
	UI_PAL_INFO,
	UI_PAL_WARNING,
	UI_PAL_FILESEL1,
	UI_PAL_FILESEL2,
	UI_PAL_BG1,
	UI_PAL_FRAME,
	UI_PAL_BG2
};

void show_color_menu(void)
{
	static int sel = 0;
	int i, x[3], sel_y = 0, update = 1, reload_bg = 0;
	UI_PALETTE *pal = NULL;
	char buf[256], rgb_value[3][4];
	const char *color_item[COLOR_ITEMS];
	const char *rgb_name[3];
	const char *bgimage_type_name[4];

	for (i = 0; i < COLOR_ITEMS; i++)
		color_item[i] = TEXT(BACKGROUND_IMAGE_TYPE + i);

	rgb_name[0] = TEXT(RED);
	rgb_name[1] = TEXT(GREEN);
	rgb_name[2] = TEXT(BLUE);

	bgimage_type_name[0] = TEXT(DEFAULT_IMAGE);
	bgimage_type_name[1] = TEXT(USERS_IMAGE);
	bgimage_type_name[2] = TEXT(LOGO_PNG_ONLY);
	bgimage_type_name[3] = TEXT(NOT_USE);

	pad_wait_clear();
	load_background(WP_COLORCFG);
	ui_popup_reset();

	do
	{
		if (update)
		{
			show_background();

			small_icon_shadow(8, 3, UI_COLOR(UI_PAL_TITLE), ICON_COLORCFG);
			uifont_print_shadow(36, 5, UI_COLOR(UI_PAL_TITLE), TEXT(COLOR_SETTINGS_MENU));

			for (i = 2; i <= 6; i++)
			{
				if (i < 4)
					uifont_print_shadow(8, 40 + (i - 2) * 18, UI_COLOR(palette[i]), color_item[i]);
				else
					uifont_print(8, 40 + (i - 2) * 18, UI_COLOR(palette[i]), color_item[i]);
			}
			boxfill_gradation(4, 38 + 5 * 18, 475, 58 + 5 * 18, UI_COLOR(UI_PAL_FILESEL1), UI_COLOR(UI_PAL_FILESEL2), 8, 0);
			uifont_print_shadow(8, 40 + 5 * 18, UI_COLOR(UI_PAL_SELECT), TEXT(FILE_SELECT_BAR));

			draw_dialog(48, 160, 431, 258);

			uifont_print_center(174, UI_COLOR(UI_PAL_INFO), color_item[sel]);
			if (!sel_y)
			{
				uifont_print(56, 174, UI_COLOR(UI_PAL_SELECT), FONT_LEFTTRIANGLE);
				uifont_print(409, 174, UI_COLOR(UI_PAL_SELECT), FONT_RIGHTTRIANGLE);
			}

			switch (sel)
			{
			case 0:
				if (sel_y)
				{
					if (bgimage_type > 0)  uifont_print(180, 213, UI_COLOR(UI_PAL_SELECT), FONT_LEFTTRIANGLE);
					if (bgimage_type < 3) uifont_print(282, 213, UI_COLOR(UI_PAL_SELECT), FONT_RIGHTTRIANGLE);
					uifont_print_center(213, UI_COLOR(UI_PAL_SELECT), bgimage_type_name[bgimage_type]);
				}
				else
					uifont_print_center(213, UI_COLOR(UI_PAL_NORMAL), bgimage_type_name[bgimage_type]);
				break;

			case 1:
				sprintf(buf, "%3d%%", bgimage_blightness);
				if (sel_y)
				{
					if (bgimage_blightness != 10)   uifont_print(200, 213, UI_COLOR(UI_PAL_SELECT), FONT_LEFTTRIANGLE);
					if (bgimage_blightness != 100) uifont_print(262, 213, UI_COLOR(UI_PAL_SELECT), FONT_RIGHTTRIANGLE);
					uifont_print_center(213, UI_COLOR(UI_PAL_SELECT), buf);
				}
				else
					uifont_print_center(213, UI_COLOR(UI_PAL_NORMAL), buf);
				break;

			default:
				pal = &ui_palette[palette[sel]];
				sprintf(rgb_value[0], "%d", pal->r);
				sprintf(rgb_value[1], "%d", pal->g);
				sprintf(rgb_value[2], "%d", pal->b);
				x[0] = 420 - uifont_get_string_width(rgb_value[0]);
				x[1] = 420 - uifont_get_string_width(rgb_value[1]);
				x[2] = 420 - uifont_get_string_width(rgb_value[2]);

				if (sel_y != 1)
				{
					uifont_print(59, 198, UI_COLOR(UI_PAL_NORMAL), rgb_name[0]);
					uifont_print(x[0], 198, UI_COLOR(UI_PAL_NORMAL), rgb_value[0]);
				}
				else
				{
					uifont_print(59, 198, UI_COLOR(UI_PAL_SELECT), rgb_name[0]);
					uifont_print(x[0], 198, UI_COLOR(UI_PAL_SELECT), rgb_value[0]);
					box(121, 198, 121 + 255 + 1, 198 + 13, UI_COLOR(UI_PAL_SELECT));
					if (pal->r > 0)   uifont_print(105, 198, UI_COLOR(UI_PAL_SELECT), FONT_LEFTTRIANGLE);
					if (pal->r < 255) uifont_print(377, 198, UI_COLOR(UI_PAL_SELECT), FONT_RIGHTTRIANGLE);
				}
				if (sel_y != 2)
				{
					uifont_print(59, 214, UI_COLOR(UI_PAL_NORMAL), rgb_name[1]);
					uifont_print(x[1], 214, UI_COLOR(UI_PAL_NORMAL), rgb_value[1]);
				}
				else
				{
					uifont_print(59, 214, UI_COLOR(UI_PAL_SELECT), rgb_name[1]);
					uifont_print(x[1], 214, UI_COLOR(UI_PAL_SELECT), rgb_value[1]);
					box(121, 214, 121 + 255 + 1, 214 + 13, UI_COLOR(UI_PAL_SELECT));
					if (pal->g > 0)   uifont_print(105, 214, UI_COLOR(UI_PAL_SELECT), FONT_LEFTTRIANGLE);
					if (pal->g < 255) uifont_print(377, 214, UI_COLOR(UI_PAL_SELECT), FONT_RIGHTTRIANGLE);
				}
				if (sel_y != 3)
				{
					uifont_print(59, 230, UI_COLOR(UI_PAL_NORMAL), rgb_name[2]);
					uifont_print(x[2], 230, UI_COLOR(UI_PAL_NORMAL), rgb_value[2]);
				}
				else
				{
					uifont_print(59, 230, UI_COLOR(UI_PAL_SELECT), rgb_name[2]);
					uifont_print(x[2], 230, UI_COLOR(UI_PAL_SELECT), rgb_value[2]);
					box(121, 230, 121 + 255 + 1, 230 + 13, UI_COLOR(UI_PAL_SELECT));
					if (pal->b > 0)   uifont_print(105, 230, UI_COLOR(UI_PAL_SELECT), FONT_LEFTTRIANGLE);
					if (pal->b < 255) uifont_print(377, 230, UI_COLOR(UI_PAL_SELECT), FONT_RIGHTTRIANGLE);
				}
				boxfill(121 + 1, 198 + 1, 121 + pal->r, 198 + 12, COLOR_RED);
				boxfill(121 + 1, 214 + 1, 121 + pal->g, 214 + 12, COLOR_GREEN);
				boxfill(121 + 1, 230 + 1, 121 + pal->b, 230 + 12, COLOR_BLUE);
				break;
			}

			update  = draw_battery_status(1);
			update |= draw_volume_status(1);
			update |= ui_show_popup(1);
			video_flip_screen(1);
		}
		else
		{
			update  = draw_battery_status(0);
			update |= draw_volume_status(0);
			update |= ui_show_popup(0);
			video_wait_vsync();
		}

		if (pad_pressed(PLATFORM_PAD_UP))
		{
			if (sel_y > 0)
			{
				sel_y--;
				update = 1;
			}
		}
		else if (pad_pressed(PLATFORM_PAD_DOWN))
		{
			if (sel < 2)
			{
				if (sel_y == 0)
				{
					sel_y = 1;
					update = 1;
				}
			}
			else
			{
				if (sel_y < 3)
				{
					sel_y++;
					update = 1;
				}
			}
		}
		else if (pad_pressed(PLATFORM_PAD_LEFT))
		{
			if (sel_y == 0)
			{
				sel--;
				if (sel < 0) sel = COLOR_ITEMS - 1;
			}
			else
			{
				switch (sel)
				{
				case 0:
					if (bgimage_type > 0)
					{
						bgimage_type--;
						set_wallpaper();
						reload_bg = 1;
					}
					break;

				case 1:
					bgimage_blightness -= 5;
					if (bgimage_blightness < 10)
						bgimage_blightness = 10;
					reload_bg = 1;
					break;

				default:
					switch (sel_y)
					{
					case 1: if (pal->r > 0) pal->r--; break;
					case 2: if (pal->g > 0) pal->g--; break;
					case 3: if (pal->b > 0) pal->b--; break;
					}
					if (sel == 2 || sel == 11) reload_bg = 1;
					break;
				}
			}
			update = 1;
		}
		else if (pad_pressed(PLATFORM_PAD_RIGHT))
		{
			if (sel_y == 0)
			{
				sel++;
				if (sel >= COLOR_ITEMS) sel = 0;
			}
			else
			{
				switch (sel)
				{
				case 0:
					if (bgimage_type < 3)
					{
						bgimage_type++;
						set_wallpaper();
						reload_bg = 1;
					}
					break;

				case 1:
					bgimage_blightness += 5;
					if (bgimage_blightness > 100)
						bgimage_blightness = 100;
					reload_bg = 1;
					break;

				default:
					switch (sel_y)
					{
					case 1: if (pal->r < 255) pal->r++; break;
					case 2: if (pal->g < 255) pal->g++; break;
					case 3: if (pal->b < 255) pal->b++; break;
					}
					if (sel == 2 || sel == 11) reload_bg = 1;
					break;
				}
			}
			update = 1;
		}
		else if (pad_pressed(PLATFORM_PAD_B1))
		{
			if (sel >= 2)
			{
				switch (sel_y)
				{
				case 1: pal->r += 5; if (pal->r > 255) pal->r = 255; break;
				case 2: pal->g += 5; if (pal->g > 255) pal->g = 255; break;
				case 3: pal->b += 5; if (pal->b > 255) pal->b = 255; break;
				}
				if (sel == 2 || sel == 11) reload_bg = 1;
			}
			update = 1;
		}
		else if (pad_pressed(PLATFORM_PAD_B3))
		{
			if (sel >= 2)
			{
				switch (sel_y)
				{
				case 1: pal->r -= 5; if (pal->r < 0) pal->r = 0; break;
				case 2: pal->g -= 5; if (pal->g < 0) pal->g = 0; break;
				case 3: pal->b -= 5; if (pal->b < 0) pal->b = 0; break;
				}
				if (sel == 2 || sel == 11) reload_bg = 1;
			}
			update = 1;
		}
		else if (pad_pressed(PLATFORM_PAD_B4))
		{
//			bgimage_type = BG_DEFAULT;
			bgimage_type = 3;
			bgimage_blightness = 50;
			set_ui_color(&ui_palette[UI_PAL_TITLE],    255, 255, 255);
			set_ui_color(&ui_palette[UI_PAL_SELECT],   255, 255, 255);
			set_ui_color(&ui_palette[UI_PAL_NORMAL],   180, 180, 180);
			set_ui_color(&ui_palette[UI_PAL_INFO],     255, 255,  64);
			set_ui_color(&ui_palette[UI_PAL_WARNING],  255,  64,  64);
			set_ui_color(&ui_palette[UI_PAL_BG1],       48,  48,  48);
			set_ui_color(&ui_palette[UI_PAL_BG2],        0,   0, 160);
			set_ui_color(&ui_palette[UI_PAL_FRAME],      0,   0,   0);
			set_ui_color(&ui_palette[UI_PAL_FILESEL1],  40,  40,  40);
			set_ui_color(&ui_palette[UI_PAL_FILESEL2], 120, 120, 120);
			reload_bg = 1;
			update = 1;
		}
		else if (pad_pressed(PLATFORM_PAD_SELECT))
		{
			help(HELP_COLORSETTINGS);
			update = 1;
		}

		if (reload_bg)
		{
			reload_bg = 0;
			load_background(WP_COLORCFG);
		}

		pad_update();

		if (Loop == LOOP_EXIT) break;

	} while (!pad_pressed(PLATFORM_PAD_R) && !pad_pressed(PLATFORM_PAD_B2));

	pad_wait_clear();
	load_background(WP_FILER);
	ui_popup_reset();
}

#endif
