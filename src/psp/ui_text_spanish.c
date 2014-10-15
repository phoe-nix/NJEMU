static const char *text_SPANISH[UI_TEXT_MAX] =
{
		"\0",
		"\n",

		/* psp/filer.c */
		"Por favor espere...",
		"No se pudo abrir zipname.%s",
#ifdef ADHOC
		"Por favor, active el interruptor WLAN.",
		"No se pudo cargar los módulos AdHoc.",
#endif

		/* psp/sound.c */
		"No se puede reservar canal de audio para el sonido.",
		"No se pudo iniciar la tarea de sonido.",

		/* psp/ui.c */
		"Advertencia: La batería esta baja (%d%%). Por favor, carga la batería!",

		FONT_CIRCLE " para cargar, " FONT_CROSS " para cancelar",
		FONT_CIRCLE " para confirmar, " FONT_CROSS " para cancelar",
#ifdef SAVE_STATE
		FONT_CIRCLE " regresar al juego, " FONT_CROSS " regresar al menu",
#endif
		"Pulsa cualquier botón.",
		"Comenzar emulación.",
#ifdef ADHOC
		"Comenzar emulación. (AdHoc)",
#endif
		"Salir del emulador.",
		"Reiniciar la emulación.",
		"Es necesario reiniciar la emulación.",
#if (EMU_SYSTEM != NCDZ)
		"ESTE JEUGO NO FUNCIONA.",
		"No sera capaz de hacer que funcione correctamente.",
		"No se moleste.",
#endif
		"Convertir este directorio en el de inicio?",
#ifdef PSP_SLIM
		"Este programa requiere PSP-2000 + FW 3.71 M33 o posterior.",
#endif
#ifdef SAVE_STATE
		"Iniciar guardado.",
		"Iniciar carga.",
#endif
#if defined(SAVE_STATE) || defined(COMMAND_LIST)
		"Completo.",
#endif
#ifdef SAVE_STATE
		"Borrar archivo de estado.",
#endif
#if (EMU_SYSTEM == NCDZ)
		"Archivos MP3 no encontrados. No se puede reproducir CDDA.",
		"IPL.TXT no encontrado.",
		"Boot NEO·GEO CDZ BIOS.",
		"NEO·GEO CDZ BIOS (neocd.bin) no encontrado.",
		"Encontrado NEO·GEO CDZ BIOS no valido.",
		"No se puede iniciar el juego.",
#endif

		"Ayuda - %s",
		"No usar",
		"Pulsa cualquier botón para regresar al menú.",
		"Explorador de archivos",
		"Desplazar",
		"Desplazar 1 pagina",
		"Cargar juego",
		"Cargar juego (AdHoc)",
		"Salir del emulador",
		"Mostrar menú de selección de BIOS",
		"Abrir el menú de ajustes de color",
		"Muestra esta ayuda",
		"Boot BIOS",
		"Fija el directorio seleccionado como inicial",
		"Cambiar valor",
		"Seleccionar",
		"Seleccionar slot",
		"Cambiar función",
		"Ejecutar función",
		"Seleccionar ítem",
		"Seleccionar ítem / Cambiar valor",
		"Valor RGB +5",
		"Regresar al explorador de archivos",
		"Valor RGB -5",
		"Restaurar todos los valores por defecto",
		"Desplazar/Seleccionar ítem",
		"Abrir/Cerrar elemento del menú",
		"Regresar al menú principal/juego",

		/* psp/ui_menu.c */
		"Encendido",
		"Apagado",
		"Si",
		"No",
		"Habilitar",
		"Inhabilitar",
		"Regresar al menú principal",
		"Menú de configuración del juego",
		"Raster Effects",
		"Estirar Pantalla",
#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
		"360x270 (4:3)",
		"384x270 (24:17)",
		"466x272 (12:7)",
		"480x270 (16:9)",
		"Rotar Pantalla",
#else
		"320x224 (4:3)",
		"360x270 (4:3)",
		"366x270 (19:14)",
		"420x270 (14:9)",
		"480x270 (16:9)",
#endif
		"Sincronización de vídeo",
		"Salto de cuadros automático",
		"Salto de cuadros",
		"nivel 1",
		"nivel 2",
		"nivel 3",
		"nivel 4",
		"nivel 5",
		"nivel 6",
		"nivel 7",
		"nivel 8",
		"nivel 9",
		"nivel 10",
		"nivel 11",
		"Mostrar FPS",
		"Limitar a 60fps",
		"Habilitar Sonido",
		"Frecuencia de muestreo",
		"11025Hz",
		"22050Hz",
		"44100Hz",
		"Volumen del sonido",
		"0%",
		"10%",
		"20%",
		"30%",
		"40%",
		"50%",
		"60%",
		"70%",
		"80%",
		"90%",
		"100%",
		"Control",
		"Jugador 1",
		"Jugador 2",
#if (EMU_SYSTEM == CPS1 || EMU_SYSTEM == CPS2)
		"Jugador 3",
		"Jugador 4",
#endif
		"PSP clock",
		"222MHz",
		"266MHz",
		"300MHz",
		"333MHz",
#if (EMU_SYSTEM == MVS)
		"Predeterminado",
#endif
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"Región de la maquina",
		"Japón",
		"USA",
		"Europa",
#endif
#if (EMU_SYSTEM == MVS)
		"Modo de la maquina",
		"Consola (AES)",
		"Arcade (MVS)",
#endif
#if (EMU_SYSTEM == NCDZ)
		"Emular la pantalla de carga",
		"Limite de velocidad del CD-ROM",
		"Habilitar CDDA",
		"Volumen CDDA",
#endif

		"Menú de configuración de botones",
		"Sin uso",
		FONT_UPARROW,
		FONT_DOWNARROW,
		FONT_LEFTARROW,
		FONT_RIGHTARROW,
		FONT_CIRCLE,
		FONT_CROSS,
		FONT_SQUARE,
		FONT_TRIANGLE,
		"L TRIGGER",
		"R TRIGGER",
		"START",
		"SELECT",
		"Bajo",
		"Normal",
		"Alto",
#if (EMU_SYSTEM == CPS2)
		"Jugador1 Start",
		"Jugador2 Start",
#endif
		"%d cuadro",
		"%d cuadros",
		FONT_UPARROW,
		FONT_DOWNARROW,
		FONT_LEFTARROW,
		FONT_RIGHTARROW,
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"Botón A",
		"Botón B",
		"Botón C",
		"Botón D",
#else
		"Botón 1",
		"Botón 2",
		"Botón 3",
		"Botón 4",
		"Botón 5",
		"Botón 6",
#endif
		"Start",
#if (EMU_SYSTEM == NCDZ)
		"Select",
#else
		"Moneda",
		"Moneda de Servicio",
#endif
#if (EMU_SYSTEM == MVS)
		"Interruptor de pruebas",
#elif (EMU_SYSTEM != NCDZ)
		"Interruptor de servicio",
#endif
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"Auto disparo A",
		"Auto disparo B",
		"Auto disparo C",
		"Auto disparo D",
#else
		"Auto disparo 1",
		"Auto disparo 2",
		"Auto disparo 3",
		"Auto disparo 4",
		"Auto disparo 5",
		"Auto disparo 6",
#endif
		"Intervalo de Auto disparo",
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"Hotkey A+B",
		"Hotkey A+C",
		"Hotkey A+D",
		"Hotkey B+C",
		"Hotkey B+D",
		"Hotkey C+D",
		"Hotkey A+B+C",
		"Hotkey A+B+D",
		"Hotkey A+C+D",
		"Hotkey B+C+D",
		"Hotkey A+B+C+D",
#else
		"Hotkey 1+2",
		"Hotkey 1+3",
		"Hotkey 1+4",
		"Hotkey 2+3",
		"Hotkey 2+4",
		"Hotkey 3+4",
		"Hotkey 1+2+3",
		"Hotkey 1+2+4",
		"Hotkey 1+3+4",
		"Hotkey 2+3+4",
		"Hotkey 1+2+3+4",
		"Hotkey 4+5+6",
#endif
#if (EMU_SYSTEM != NCDZ)
		"Sensibilidad del análogo",
#endif
#if (EMU_SYSTEM == CPS1)
		"Dial (Izquierda)",
		"Dial (Derecha)",
		"Pausa",
#endif
#if (EMU_SYSTEM == CPS2)
		"Paddle (Izquierdo)",
		"Paddle (Derecho)",
#endif
#if (EMU_SYSTEM == MVS)
		"Grande",
		"Pequeño",
		"Arriba doble",
		"Iniciar/Recoger",
		"Pago",
		"Cancelar",
		"Poner",
		"Poner/Cancelar todo",
		"Menú de operador",
		"Eliminar créditos",
		"Hopper Out",
#endif
		"Guardar captura",
		"Cambiar Jugador",
#if (EMU_SYSTEM == MVS || EMU_SYSTEM == NCDZ)
		"Disposición de botones",
		"Tipo 1 (NEOGEO PAD)",
		"Tipo 2 (MVS)",
		"Tipo 3 (PlayStation)",
		"Definido por el usuario",
#endif

		"Menú de configuración de interruptores DIP",
		"Este juego no tiene interruptores DIP.",

		"Guardar/Cargar estado",
		"Ranura %d:",
		"Vacía",
		"Cargar",
		"Borrar",
		"Guardar",
		"Fecha",
		"Hora",
		"Versión",
		"Fallo al borrar archivo \"%s\".",

		"Menú principal",
		"Configuración del Juego",
		"Cambiar configuración del juego.",
		"Configuración de botones",
		"Cambiar configuración de botones auto disparo/hotkey.",
#if (EMU_SYSTEM != NCDZ)
		"Configuración de interruptores DIP",
		"Cambiar configuración de interruptores hardware DIP.",
#endif
		"Reiniciar emulación",
		"Reinicia la emulación de "SYSTEM_NAME,
		"Regresar al explorador de archivos",
		"Detiene la emulación y regresa al explorador de archivos.",
		"Regresar al juego",
		"Regresar a la emulación del juego",
		"Salir del emulador",
		"Salir del emulador y volver al menú de PSP.",
#ifdef COMMAND_LIST
		"Mostrar lista de comandos",
		"Mostrar lista de comandos. (Formato MAME Plus! \"command.dat\")",
#endif

#if PSP_VIDEO_32BPP
		"Menú de configuración de color",
		"Tipo de imagen de fondo",
		"Brillo de imagen de fondo",
		"Texto de barra de titulo",
		"Texto seleccionado",
		"Texto normal",
		"Texto de mensaje de información",
		"Texto de mensaje de advertencia",
		"Barra de selección de archivo (inicio)",
		"Barra de selección de archivo (fin)",
		"Barra de titulo/cuadro de mensaje",
		"Barra de titulo/marco del cuadro de mensaje",
		"Fondo",
		"Rojo",
		"Verde",
		"Azul",
		"Imagen predeterminada",
		"Imagen de usuario",
		"Solo logo.png",
		"Barra de selección de archivo",
#endif

		"Lista de comandos",

		/* psp/mp3.c */
#if (EMU_SYSTEM == NCDZ)
		"No se pudo re-abrir el archivo mp3 \"%s\".",
		"Error de decodificación de MP3.",
		"No se pudo asignar el canal de audio par el MP3.",
		"No se pudo iniciar el hilo del MP3.",
#endif

		/* psp/adhoc.c*/
#ifdef ADHOC
		"sala",
		"servidor",
		"cliente",
		"Esperando a que otra PSP se una.\n",
		"Conectando a %s.",
		"Conectado.",
		"Desconectando de %s.",
		"Desconectado.",
		"error.",
		"Seleccione un servidor para conectarse, o presione" FONT_TRIANGLE " para regresar.\n",
		"Esperando a que %s acepte la conexión.\n",
		"Para cancelar pulsa " FONT_CROSS ".\n",
		"%s a solicitado conectarse.\n",
		"Para aceptar la conexión presione " FONT_CIRCLE ", para cancelar presione " FONT_CROSS ".\n",
		"Esperando la sincronización.",
#endif

		/* psp/png.c */
		"No se pudo asignar memoria para el PNG.",
		"No se pudo codificar la imagen PNG.",
#if PSP_VIDEO_32BPP || (EMU_SYSTEM == NCDZ)
		"No se pudo decodificar la imagen PNG.",
#endif
		"Imagen de %d bit no soportada.",

		/* emumain.c */
		"Error fatal",
		"Captura guardada como \"%s_%02d.png\".",

#if USE_CACHE
		/* common/cache.c */
		"No se pudo abrir el archivo de cache.\n",
		"No hay suficiente memoria.\n",
		"No se pudo asignar memoria para el cache.\n",
		"%dKB de cache asignados.\n",
		"Error al cargar el cache!!!\n",
		"Cargando datos de información del cache...\n",
		"Versión \"V%c%c\" de cache no soportada.\n",
		"La versión actual requerida es \"" CACHE_VERSION "\".\n",
		"Reconstruya el archivo de cache.\n",
#if (EMU_SYSTEM == CPS2)
		"No se pudo abrir el bloque de sprites %03x\n",
#elif (EMU_SYSTEM == MVS)
		"Cache de PCM habilitado.\n",
#endif
#endif

		/* common/loadrom.c */
		"Presione cualquier botón.\n",
		"No se pudo asignar %s de memoria.\n",
		"CRC32 incorrecto. \"%s\"\n",
		"Archivo no encontrado. \"%s\"\n",

#ifdef SAVE_STATE
		/* common/state.c */
		"No se pudo asignar el buffer para el estado.",
		"No se pudo crear el archivo \"%s.sv%d\"",
		"No se pudo abrir el archivo \"%s.sv%d\"",
		"Guardando \"%s.sv%d\"",
		"Cargando \"%s.sv%d\"",
#if (EMU_SYSTEM == MVS)
		"No re pudo volver a cargar la BIOS. Presione cualquier botón para salir.",
#elif (EMU_SYSTEM == NCDZ)
		"No se pudo comprimir los datos del estado.",
		"No se pudo descomprimir los datos del estado.",
#endif
#endif

#ifdef COMMAND_LIST
		/* common/cmdlist.c */
		"Lista de comando para este juego no encontrada.",
		"%s - Lista de comandos",
		"%d/%d items",
		"Reducir tamaño de COMMAND.DAT",
		"Este proceso elimina de la lista de comandos los juegos que no están\n",
		"soportados por este emulador del archivo COMMAND.DAT.\n",
		"Si quieres reducir el tamaño de command.dat, presiona el botón " FONT_CIRCLE ".\n",
		"De lo contrario, presiona el botón " FONT_CROSS " para regresar al explorador de archivos.\n",
		"Verificando el formato del COMMAND.DAT...\n",
		"ERROR: Formato desconocido.\n",
		"ERROR: Archivo vacío.\n",
		"ERROR: No se pudo asignar memoria.\n",
		"ERROR: No se pudo renombrar el archivo.\n",
		"ERROR: Se pudo crear el archivo de salida.\n",
		"Copiando \"%s\"...\n",
		"Tamaño original:%dbytes, Resultado:%dbytes (-%.1f%%)\n",
#endif

		/* emulation core */
		"Hecho.\n",
		"Salir del emulador",
		"Espere.\n",

		/* inptport.c */
		"Control: Jugador 1",
		"Control: Jugador %d",
#ifdef ADHOC
		"Sincronización perdida.\n",
		"Pausado por %s",
		"Regresar al juego",
		"Desconectar",
#endif

		/* memintrf.c */
		"Cargando \"%s\"\n",
		"Carga de ROM",
#if (EMU_SYSTEM != NCDZ)
		"Verificando información del ROM...\n",
		"Este juego no esta soportado.\n",
		"ROM no encontrado. (nombre del archivo zip incorrecto)\n",
		"Driver para \"%s\" no encontrado.\n",
		"ROM set \"%s\" (parent: %s)\n",
		"ROM set \"%s\"\n",
#endif

#if (EMU_SYSTEM == CPS1)

		/* memintrf.c */
		"rominfo.cps1 no encontrado.\n",
		"No se pudo asignar memoria. (0x8000 bytes)",

#elif (EMU_SYSTEM == CPS2)

		/* cps2crpt.c */
		"Descifrando %d%%\r",
		"Descifrando 100%%\n",

		/* memintrf.c */
		"rominfo.cps2 no encontrado.\n",

#ifdef PSP_SLIM
		/* vidhrdw.c */
		"Decodificando GFX...\n",
#endif

#elif (EMU_SYSTEM == MVS)

		/* biosmenu.c */
		"Menú de selección de BIOS",
		"BIOS no encontrada.",
		"Todos los archivos NVRAM fueron eliminados.\n",

		/* memintrf.c */
		"rominfo.mvs no encontrado.\n",
		"Cargando \"%s (%s)\"\n",
		"Descifrando ROM.\n",
		"Cargando ROM GFX2 descifrado...\n",
		"Cargando ROM SOUND1 descifrado...\n",
		"No se pudo asignar memoria para descifrar el ROM.\n",
		"No se pudo asignar memoria para los datos de sprite.\n",
		"Intentando usar cache de sprite...\n",
		"Verificando BIOS...\n",

#elif (EMU_SYSTEM == NCDZ)

		/* ncdz.c */
		"Error cuando se procesaba IPL.TXT.",
		"Reinicio causado por el vigilante del contador.",

		/* cdrom.c */
		"No se pudo abrir el archivo \"%s\".",
		"No se pudo insertar datos de estado del CD-ROM.",

		/* vidhrdw.c */
		"Limite de velocidad CD-ROM: Apagado",
		"Limite de velocidad CD-ROM: Encendido",

		/* memintrf.c */
		"Verificando ID de juego...\n",
#endif
		"Menú de trucos",
		"Selección de trucos",
		"Seleccionar truco",
		"No se encontraron trucos para este juego.",
/*
		"Memory free",
		"Mem free",*/
		NULL
};