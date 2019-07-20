/*[]****************************************************[]*/
/*[]  	     Междугородные  телефонные  разговоры	[]*/
/*[]			 Версия  2.4			[]*/
/*[]							[]*/
/*[]          TLT.h:  головной  файл  описаний		[]*/
/*[]****************************************************[]*/

#define	ulong			unsigned long
#define	uint			unsigned int
#define	uchar			unsigned char

#define	NULL			0
#define	FALSE			NULL
#define TRUE			!FALSE
#define ON			TRUE
#define OFF			FALSE

#define	BACKSPACE_KEY		8
#define	TAB_KEY			9
#define ENTER_KEY		0x0d
#define ESC_KEY			0x1b
#define	SPACE_KEY		0x20
#define	F1_KEY			0x13b
#define	F2_KEY			0x13c
#define	F3_KEY			0x13d
#define	F4_KEY			0x13e
#define	F5_KEY			0x13f
#define F6_KEY			0x140
#define	F10_KEY			0x144
#define HOME_KEY		0x147
#define	UP_KEY			0x148
#define PG_UP_KEY		0x149
#define LEFT_KEY		0x14b
#define RIGHT_KEY		0x14d
#define END_KEY			0x14f
#define	DOWN_KEY		0x150
#define	PG_DN_KEY		0x151
#define INS_KEY			0x152
#define DEL_KEY			0x153
#define	CTRL_END_KEY		0x175

#define	CTRL_KEY_POS		0x04		/*    положение клавиши Ctrl по адресу 0x40:0x17		*/
#define	ALT_KEY_POS		0x08		/*    положение клавиши Alt по адресу 0x40:0x17			*/

#define MAX_REC_DST		100		/*   максимальное количество записей в обрабатываемом массиве	*/
#define MAX_REC_CH      	40		/*  максимал. количество записей в передаваемом в канал массиве	*/
#define MAX_PAG         	10		/* 	максим. количество страниц телефонной базы данных	*/
#define MAX_ADDR		50		/* 	   макс. количество обслуживаемых адресатов		*/
#define MAX_TLPH        	511		/* 		макс. кол-во телефонов (MAX_TLPH = N**2)	*/
#define MAX_NAME		20		/* 		   макс. размер имени гостиницы			*/
#define	MAX_SIZE_HLP		50		/*    максимальное количество обрабатываемых страниц TLT-H.HLP	*/
#define	MAX_STR			20
#define	MAX_INF			40		/*         максимальное количество выводимых сообщений		*/
#define	MAX_PRN_LINES		40		/* макс. кол-во строк в стр-це при выводе базы данных на принтер*/
#define	MAX_LAST_MSG		2		/*    макс. количество записей для поиска последн. сообщений	*/
#define	MAX_TMP_INF		5		/*     макс. кол-во записей для вывода инф-ции в верхн. окно	*/
#define	MAX_MSG_CH		5		/*    количество сообщений, послe которых дается квитанция	*/

/*
#define	EOF                     0x1a		/* 			признак конца файла 			*/ /* mikle */
*/

#define ADAP_POS_N_TLPH		23      	/* 	  позиция номера телефона при чтении из адаптера	*/
#define ADAP_POS_CATEG		2	      	/* 	позиция категории абонента при чтении из адаптера	*/
#define	ADAP_POS_DATE		32		/* 	   позиция даты в принимаемой с адаптера строке		*/
#define POS_TYPE_MONTH		35		/* 	  позиция месяца для IV квартала (как и в arr_info)	*/
#define	ADAP_POS_SUM		48		/* 		позиция суммы в получаемой записи		*/

#define TLPHinPAG		16		/* количество телефонов в отображаемой странице			*/
#define LENGTH_REC		58		/* длина обрабатываемой записи					*/
#define RSRV_SIZ		3		/* по (3 * 2) байт резервируется				*/
#define	SIZE_HELP		51		/* 	     размер одной отображаемой строчки помощи		*/

#define	NUM_VERS		0x23		/* 		номер версии программы и базы данных		*/

#define S_IREAD			0x0100		/* 			   <-- stat.h				*/
#define S_IWRITE		0x0080

#define	ADAP_READ_FIRST		0		/*		   коды команд для адаптера			*/
#define	ADAP_CLR_MAIN_STAT	3
#define	ADAP_READ_NEXT		9
#define	ADAP_INACCESS		0x0b

#define	BIT_RXRDY		0x02		/* бит 1 - готовность приемника ВВ51				*/
#define	BIT_TXRDY		0x01		/* бит 0 - готовность передатчика				*/
#define	BIT_CTS			0x01		/* бит 0 (цепь 106) байта состояния 2				*/
#define	BIT_DCD			0x02		/* бит 1 (цепь 109) байта состояния 2				*/
#define	BIT_GAP			0x40		/* бит 6 - разрыв последовательности передаваемых кодов		*/
#define BIT_BUFF_MODIF		0x02		/* бит 1 байта основного состояния - буфер модифицирован	*/
#define BIT_INP_FORBID		0x10		/* бит 4 байта основного состояния - ввод запрещен		*/
#define BIT_INQUIRY_COM		0x40		/* бит 6 регистра управления обменом - запрос команды		*/
#define	BIT_TEST_ADAP		0x10		/* бит 4 регистра управления обменом - внутрен. тест адаптера	*/
#define	BIT_DUMMY		0x65		/* биты адаптера, установленные в "логич. 0"			*/
#define	BIT_PRN_BUSY		0x80		/* бит 7 регистра статуса принтера: offline			*/
#define	BIT_PRN_PE		0x20		/* бит 5 регистра статуса принтера: нет бумаги			*/
#define	BIT_PRN_ERR		0x08		/* бит 3 регистра статуса принтера: ошибка			*/
#define	BIT_ALIEN		0x04

#define	TIMEOUT_CODEC		15		/* 	     время ожидания готовности модема в сек		*/
#define	TIMEOUT_ADAP		1		/* 	    время ожидания готовности адаптера в сек		*/
#define	INIT_ATTEMPT		4		/*	   количество попыток получения автоответа - 1		*/

#define	DAY			0
#define	MONTH			1
#define	YEAR			2
#define	HOUR			0
#define	MIN			1

#define	SAVE			0
#define	LOAD                    1
#define	INIT			2
#define	UNLINK			3
#define	SEND			4

#define	INITFORWORK		0
#define INITFORTEST		1
#define INITFORSTUB		2

#define	CHANN			0
#define	FLOP			1
#define	PRN			2

#define	FILE_HLP		"tlt.hlp"	/* файл, содержащий помощь программы				*/
#define	FILE_TDB		"tlt.tdb"	/* файл, хранящий телефонную базу данных и установки		*/
#define	FILE_OLD		"tlt.old"	/* файл, содержащий "старую" телефонную базу данных и установки	*/
#define	FILE_INF		"tlt.inf"	/* файл временного хранения принятой с адаптера информации	*/
#define	FILE_SUM		"tlt.sum"	/* 			итоговый файл				*/

#define	MSGTITLE		"TLT   Междугородные телефонные разговоры   Версия 2.5 (ЭТК-КС & ЕС 184X)   1993\n"
#define MSGTLPHTBL		"Телефонная база данных TLТ.EXE"
#define MSGTLPHHLP		"\tПомощь программы TLT.EXE\r\n"
#define MSGTMPINF		"Временнoе хранeние информaции TLT.EXE"
#define MSGGRATIT		"\nСпасибо за пользование!"
#define	MSGERRADAP		"\nOтсутствует плата адаптера ЕС 7920.01!"
#define	MSGERRNOMEM		"\nНе хватает памяти!"
#define	MSGBYE			"\nПри необходимости контактный телефон: (041) 220-41-37!\n"
#define	MSGTITLEPRN		"\nTLT   База данных телефонных номеров\n"
#define	MSGTOTALADDR		"\n\tОбслуживаемых адресатов - "
#define	MSGTOTALTLPH		"\n\tОбслуживаемых телефонных номеров - "
#define	MSGTOWN			"\n\n█ Город - "
#define	MSGADDR			"\n█ Адресат - "
#define	MSGTTY			"\n█ Номер телетайпа - "
#define	MSGTLPH			"\n█ Количество телефонов - "
#define	MSGTLPHROOM		"\nТелефон    Комн./Каб.         Телефон    Комн./Каб.\n"
#define	MSGTITLETOTAL		"\t\t\tTLT   Итоговая информация\n"
#define	MSGTOTALFILE		"\n\t\tГород, адресат\t\t      Дата      Кол.    Сумма\n"

#define X_WORK_WND		0
#define Y_WORK_WND		0
#define WIDTH_WORK_WND		79
#define HEIGTH_WORK_WND		24
#define X_MENU_WND		4
#define Y_MENU_WND		2
#define X_INFO_WND		3
#define Y_INFO_WND		20
#define X_ASK_WND		9
#define	Y_ERR_WND		15
#define	X_MENU_WND		4
#define	Y_MENU_WND		2
#define	X_TREATOUT_WND		8
#define	Y_TREATOUT_WND		2
#define	X_HELP_WND      	13
#define	Y_HELP_WND		2
#define X_STAT_OUT_WND		14
#define Y_STAT_OUT_WND		8
#define	X_KEYCODEC_WND		5
#define	Y_KEYCODEC_WND		8
#define	X_OUTTLPHTBL_WND	6
#define	Y_OUTTLPHTBL_WND	Y_MENU_WND+13
#define	X_ADD_PROCESS_WND       X_MENU_WND+2
#define	Y_ADD_PROCESS_WND	12

#define	ARG_WND_HELP    	X_HELP_WND, Y_HELP_WND, 52, 15
#define	ARG_WND_TREAT		5, Y_MENU_WND+3, 51, 5
#define	ARG_WND_TREATOUT	X_TREATOUT_WND, Y_TREATOUT_WND, 63, 16
#define ARG_WND_FILETMP		6, 8, 69, 2
#define	ARG_WND_SETREGIME	X_MENU_WND+2, Y_MENU_WND+5, 57, 9
#define ARG_WND_TEST 		X_MENU_WND+2, Y_MENU_WND+8, 49, 5
#define	ARG_WND_TLPHTBL		5, Y_MENU_WND+4, 52, 11
#define ARG_WND_SPEED		X_MENU_WND+3, Y_MENU_WND+12, 6, 5
#define	ARG_WND_TMPDRIVE 	X_MENU_WND+3, Y_MENU_WND+11, 4, 6
#define	ARG_WND_FIND		X_MENU_WND+2, Y_MENU_WND+9, 21, 2
#define	ARG_WND_TLPH		X_MENU_WND+1, Y_MENU_WND+4, 69, 12
#define	ARG_WND_TLPH_NAME 	X_MENU_WND+47, Y_MENU_WND-1, 25, 3
#define ARG_WND_TLPH_LEFT 	X_MENU_WND+2, Y_MENU_WND+6, 33, 9
#define ARG_WND_TLPH_RIGHT 	X_MENU_WND+36, Y_MENU_WND+6, 33, 9
#define	ARG_WND_INP		X_MENU_WND+1, Y_MENU_WND, MAX_STR+1, 2
#define	ARG_WND_ADDENV		X_MENU_WND+30, Y_MENU_WND-1, 39, 6
#define	ARG_WND_DEL		X_MENU_WND-3, Y_MENU_WND+5, 32, 2
#define	ARG_WND_ADDR		X_MENU_WND+1, Y_MENU_WND+4, 68, 12
#define	ARG_WND_ADDR1		X_MENU_WND+2, Y_MENU_WND+6, 66, 9
#define	ARG_WND_TOTAL		X_MENU_WND+47, Y_MENU_WND-1, 25, 3
#define	ARG_WND_OPER		1, 1, 4, 2
#define	ARG_WND_CHANGEADDR 	X_MENU_WND+37, Y_MENU_WND-1, 32, 4
#define	ARG_WND_STAT_OUT 	X_STAT_OUT_WND, Y_STAT_OUT_WND, 47, i
#define	ARG_WND_ASK		X_ASK_WND, 9, 60, 2
#define ARG_WND_EXIT		8, 8, 63, 2
#define	ARG_WND_ERR		5, Y_ERR_WND, 69, 2
#define	ARG_WND_KEYCODEC	X_KEYCODEC_WND, Y_KEYCODEC_WND, 68, 2
#define	ARG_WND_OUTTLPHTBL	X_OUTTLPHTBL_WND, Y_OUTTLPHTBL_WND, 37, 3
#define	ARG_WND_CATEG		X_MENU_WND+4, Y_MENU_WND+8, 21, 2
#define	ARG_WND_ADDPAY		X_MENU_WND+4, Y_MENU_WND+9, 21, 2
#define	ARG_WND_DELASS		9, Y_MENU_WND+10, 61, 2
#define	ARG_WND_SETDATE		X_MENU_WND+23, Y_MENU_WND+9, 21, 2
#define	ARG_WND_SETTIME		X_MENU_WND+26, Y_MENU_WND+11, 21, 2
#define	ARG_WND_ADDARG		X_MENU_WND+3, Y_MENU_WND+6, 53, 4
#define	ARG_WND_SPACETIME	X_MENU_WND+4, Y_MENU_WND+10, 21, 2
#define	ARG_WND_ADD_PROCESS	X_ADD_PROCESS_WND, Y_ADD_PROCESS_WND, 32, 3
#define	ARG_WND_LAST_MSG	X_ADD_PROCESS_WND+1, Y_ADD_PROCESS_WND+2, 21, 2
#define	ARG_WND_LAST_MSG1	X_ADD_PROCESS_WND+2, Y_ADD_PROCESS_WND+4, 63, 3
#define	ARG_WND_SEND_TOTAL	X_ADD_PROCESS_WND+1, Y_ADD_PROCESS_WND+3, 21, 2

#define STDOUT			1		/* 		стандартное устройство вывода - дисплей		*/
#define	STDPRN			4

#define	ADD			3
#define	CHANG			4
#define	DEL			5

#define	INFMAINMENU		0
#define	INFTMPDRV		1
#define	INFSPEED		2
#define	INFHELP			3
#define	INFENDTREAT		4
#define	INFTESTADAP		5
#define	INFTESTCODEC		6
#define	INFTOTMPDRVCHAN		7
#define	INFFIND			8
#define	INFTOWN			9
#define	INFADDR			10
#define	INFTTY			11
#define	INFTLPH			12
#define	INFROOM			13
#define	INFDEL			14
#define	INFINITDB		15
#define	INFLISTTLPH		16
#define	INFLISTADDR		17
#define	INFLOADDB		18
#define	INFSAVEDB		19
#define	INFEXIT			20
#define	INFASK			21
#define	INFANYKEY		22
#define	INFPRINT		23
#define	INFOUT			24
#define	INFCATEG		25
#define	INFDATE			26
#define	INFADDPAY		27
#define	INFSPACETIME		28
#define	INFFINDLAST		29
#define	INFSENDTOTAL		30

#define	ERRLOAD			0
#define	ERRSAVE			1
#define	ERRCHECK		2
#define	ERRHLP			3
#define	ERRINF			4
#define	ERRPRN			5
#define	ERRTTYYES		6
#define	ERRTLPHYES		7
#define	ERRTLPHNO		8
#define	ERRADDR			9
#define	ERROVER			10
#define	ERRADAP			11
#define	ERRCODEC		12
#define	ERRFATAL		13
#define	ERRINP			14
#define	ERROFFLINE		15
#define	ERRCATEG		16
#define	ERRADDPAY		17
#define	ERRINSSTUB		18
#define	ERRINVINF		19
#define	ERRSPACTIME		20
#define	ERRABSCHAN		21
#define	ERRINSSPEED		22
#define	ERRNOLASTMSG		23
#define	ERRTTYNO		24
#define	ERRSUMFILE		25
#define	ERRNOCOMM		26

#define	NUM_FRAME_MENU		0		/* рамка окон меню - одиночная линия				*/

#define INVISIBLE		25		/* строка невидимости курсора					*/
#define	BLINK_ATTR		0x80		/* атрибут мерцания						*/
#define	INVERT_ATTR		0x70		/* атрибут инвертирования					*/
#define	NORM_ATTR		7		/* нормальный атрибут						*/
#define	HIGH_ATTR		8		/* высокая интенсивность					*/
#define	VP_DOS			0xb800		/* 0 видео страница - текущая					*/
#define	VP_WORK			0xb900		/* 1 видео страница - рабочая					*/
#define	VP_WAIT			0xba00		/* 2 видео страница - окно ожидания				*/
#define	WORK_WND		1
#define	WAIT_WND		2

#define	LINE			0		/* отображать строку, заканчивающуюся NULL			*/
#define	SYMB			1		/* одиночный символ						*/

#define	SINGLE_FRAME		0
#define DOUBLE_FRAME            1
#define	SHADOW_OFF              0
#define SHADOW_ON		1

#define	NO_KEYS			0
#define	GLOBAL_KEYS		1
#define	WITH_FIND_KEYS		2

#define TIMEREST_10		0x553a		/* mikle: в основном режиме - количество тиков равное 10 мин.	*/

/*[]****************************************************[]*/
/*[]		       Макроопределения  		[]*/
/*[]****************************************************[]*/
#define	Intr(num_int)		__emit__ (0xcd, num_int)
#define	ExecComm	        outp (0x226, 0);			\
			        while (inp (0x227) & BIT_INQUIRY_COM)
#define	ExecCommVerify		outp (0x226, 0);			\
				while (inp (0x227) & BIT_INQUIRY_COM)	\
				 {					\
				  if (inp (0x220) & BIT_INP_FORBID)	\
				   {					\
				    disable ();  outp (0x20, 0x20);	\
				    return;				\
				   }					\
				 }
#define	ClrVideoPage(vp)	ToXY (vp, 80 * 25 * 2, 0, 0, NULL)
#define	Delay3Nop		__emit__ (0x90, 0x90, 0x90)

/*[]****************************************************[]*/
/*[]                 Прототипы  функций			[]*/
/*[]****************************************************[]*/
extern void
	interrupt SendChan ();
void
	SaveTlphDB (void),			Help (void),
	ListPageTlph (void),			PrintLett (char *),
	ListAddrDB (void),	    		SetDate (void),
	Process (void),                         InitTlphDB (void),
	DrivOutChann (void),			ProcessOut (void),
	ProcessOutRec (void),   		SendTotalRecord (uint),
	TlphDB (void),				TestEquip (void),
	InitEquip (uchar),			InitEquipOn (void),
	SetRegime (void),			ClearTotal (void),
	TestAdap (void),			TestCodec (void),
	LoadTlphDB (void),			ClrStr (uint, uchar, uchar),
	FindNTlph (uint),			AddChangDelNTlph (void),
	PrintRegime (void),			DelAssign (char),
	SetInfDrive (void),			SetSpeed (void),
	ListPageAddr (void),			PosSetRegimeFunc (void),
	OutTlphDB (void),			PosChangDelTlphFunc (void),
	PosDelTlphFunc (void),			PosSpeedFunc (void),
	PosInfDriveFunc (void),			PosAddrDBFunc (void),
	PosTlphDBFunc (void),			PrnTlphDB (void),
	InstallWnd (void),			CursorXY (int, uint, uint),
	SetVideoPage (char),			ClrWnd (uint, char, char, uint, uint, uint, uint),
	Signal (void),				ReversStr (uint, uint),
	ListTlphDB (void),			RestoreWnd (char far *, uint, uint, uint, uint),
	PushInf (uchar),                        ScrollWnd (uchar),
	ExitToDos (void),			Error (char),
	PopInf (void),				ToXY (uint, uint, uchar, uchar, uchar *),
	Squeak (void),				WaitCodec (char),
	UnlinkInfFile (void),			SetCateg (void),
	SetAddPay (void),			SetSpaceTime (void),
	BinarySearchFunc (void),		CategFunc (void),
	SearchNTlph (ulong, uint *),		Inform (void),
	AddArg (void),				InitSpeed (void),
	LastMsg (void),				SendTotal (void),
	DelNTlph (ulong),			AddProcessMenu (void),
	VerifyRetreat (void),			interrupt New_0x24 (void),
	interrupt InpAdap (void),		interrupt New_0x09 (void),
	GetLastNum (void),			ExitProcessOut (uchar far *, uchar far *),
	PrepareUpperWnd (void),			ReceiveMain (void);
uint
	GetKey (uchar),				VertSelect (uint, uint, uint, void (* []) (), void ()),
	Ask (char),				FromXY (uint, uint, uchar, uchar, uchar *),
	FindLastMsg (uchar *, uint *);
int
	EditStr (char *, uint, uint);
uchar
	SetComm (void),
	TestInsideStub (void),			AddNTlph (ulong, ulong, uint, char *, char *, char);
char far *
	SaveWnd (uint, uint, uint, uint);

/*[]****************************************************[]*/
/*[]    Структура  базы  данных  телефонных  номеров  	[]*/
/*[]****************************************************[]*/
typedef struct tdb		    /* ┌───────────── db ──────────────┐						*/
     {				    /* │                               │						*/
				    /* │   ┌─┐                         │						*/
      uchar	    num_vers;       /* │   └─┘                         │  номер версии базы данных			*/
				    /* │   ┌───┐                       │						*/
      uint          serv_addr;      /* │   └───┘                       │  общее количество обслуживаемых адресатов	*/
                                    /* │   ┌───┐                       │						*/
      uint          serv_tlph;      /* │   └───┘                       │  общее количество обслужив. телефон. номеров	*/
				    /* │┌────────── db.addr ──────────┐│						*/
				    /* ││┌──── addr [MAX_ADDR-1] ────┐││                                                */
				    /* │││    .        .        .    │││                                                */
      struct                        /* │││┌──────── addr [0] ───────┐│││						*/
         {                          /* ││││┌───────┐                ││││						*/
	  ulong	    Ntty;           /* ││││└───────┘                ││││  номер телетайпа адресата			*/
                                    /* ││││┌─┬─┬─┬─┬──────┬─┬─┬─┬─┐ ││││						*/
	  char      town [MAX_NAME];/* ││││└─┴─┴─┴─┴ . . .┴─┴─┴─┴─┘ ││││  название города				*/
                                    /* ││││ <──────MAX_NAME───────> ││││						*/
                                    /* ││││┌─┬─┬─┬─┬──────┬─┬─┬─┬─┐ ││││						*/
	  char      addr [MAX_NAME];/* ││││└─┴─┴─┴─┴ . . .┴─┴─┴─┴─┘ ││││  название адресата (гостиницы, АПП или ОС)	*/
                                    /* ││││ <──────MAX_NAME───────> ││││						*/
				    /* ││││┌─┐                      ││││						*/
	  char      type;           /* ││││└─┘                      ││││  тип телеграфной сети, куда подключен телетайп	*/
                                    /* ││││┌───┐                    ││││						*/
	  uint      count_tlph;     /* ││││└───┘                    ││││  количество телефонов адресата			*/
                                    /* ││││┌───┬───┬───┐            ││││                                                */
	  uint      rsrv1[RSRV_SIZ];/* ││││└───┴───┴───┘            ││││  зарезервировано				*/
	 }          addr [MAX_ADDR];/* │││└─────────────────────────┘│││  внутренняя структура адресатов		*/
                                    /* │││    .        .        .    │││                                                */
                                    /* ││└───────────────────────────┘││						*/
                                    /* │└─────────────────────────────┘│						*/
				    /* │   ┌───┬───┬───┐               │						*/
      uint          rsrv2[RSRV_SIZ];/* │   └───┴───┴───┘               │  зарезервировано				*/
				    /* │┌───────── db.tlph ───────────┐│						*/
				    /* ││┌─── tlph [MAX_TLPH-1] ─────┐││                                        	*/
                                    /* │││     .       .        .    │││						*/
                                    /* │││┌─────── tlph [0] ────────┐│││						*/
     struct                         /* ││││                         ││││						*/
	 {			    /* ││││┌───┐                    ││││						*/
	  uint      ind_addr;       /* ││││└───┘                    ││││  номер индекса телетайпа адресата		*/
				    /* ││││┌───────┐                ││││						*/
	  ulong     Ntlph;          /* ││││└───────┘                ││││  номер телефона			        */
				    /* ││││┌───┐                    ││││						*/
	  uint      Nroom;	    /* ││││└───┘                    ││││  номер комнаты					*/
				    /* ││││┌───┐                    ││││						*/
	  uint      rsrv3;	    /* ││││└───┘                    ││││  зарезервировано				*/
	 }          tlph [MAX_TLPH];/* │││└─────────────────────────┘│││  внутренняя структура телефонных номеров	*/
				    /* │││      .       .       .    │││						*/
				    /* ││└───────────────────────────┘││     						*/
                                    /* │└─────────────────────────────┘│						*/
				    /* │   ┌───┬───┬───┐               │						*/
      uint          rsrv4[RSRV_SIZ];/* │   └───┴───┴───┘               │  зарезервировано				*/
				    /* │			       │						*/
      struct                        /* │┌───────── db.status ─────────┐│						*/
       {			    /* ││  ┌─┐	 		      ││						*/
	uchar	    chann;          /* ││  └─┘    		      ││  признак выдачи в канал			*/
				    /* ││  ┌─┐			      ││						*/
	uchar	    flop;           /* ││  └─┘   		      ││  признак выдачи на дисковое устройство		*/
				    /* ││  ┌─┐			      ││						*/
	uchar	    prn;            /* ││  └─┘   		      ││  признак выдачи на принтер			*/
				    /* ││  ┌─┐			      ││						*/
	uchar	    tmp_drive;      /* ││  └─┘   		      ││  устройство временного хранения		*/
				    /* ││  ┌─┐			      ││						*/
	uchar	    speed;          /* ││  └─┘   		      ││  скорость передачи в канал связи		*/
				    /* ││  ┌─┐	 		      ││						*/
	uchar	    categ;          /* ││  └─┘    		      ││  категория вызова для гостиничн. телефонов	*/
				    /* ││  ┌───┐		      ││						*/
	uint	    load_wait;      /* ││  └───┘    		      ││  значение интервала между пакетами сообщений	*/
				    /* ││  ┌─┬─┬─┬─┐		      ││						*/
	uchar	    sumaddpay [4];  /* ││  └─┴─┴─┴─┘ 	     	      ││  сумма доплаты за услуги			*/
				    /* ││  ┌─┐	 		      ││						*/
	uchar	    mop;            /* ││  └─┘    		      ││  специальное назначение			*/
       } status;                    /* │└─────────────────────────────┘│  внутренняя структура состояний режимов работы	*/
				    /* │   ┌───┬───┬───┐               │						*/
      uint          rsrv5[RSRV_SIZ];/* │   └───┴───┴───┘               │  зарезервировано				*/
				    /* │   ┌───┐                       │						*/
      uint          check_sum;      /* │   └───┘                       │  контрольная сумма базы данных			*/
     } tdb;			    /* └───────────────────────────────┘						*/

/*[]****************************************************[]*/
/*[]	   Определение  вспомогательных  структур	[]*/
/*[]****************************************************[]*/
typedef struct get_inf			 /* ┌──────────── get_inf ────────────┐ структура получен. инф-ции с адаптера	*/
 {					 /* │┌───msg [0 ... MAX_REC_DST-1]───┐│						*/
					 /* ││    ┌─┬─┬─┬─┬──────┬─┬─┬─┬─┐   ││						*/
  uchar msg [MAX_REC_DST] [LENGTH_REC];	 /* ││    └─┴─┴─┴─┴ . . .┴─┴─┴─┴─┘   ││						*/
 } get_inf;				 /* ││    <──────LENGTH_REC───────>  ││						*/
					 /* │└───────────────────────────────┘│						*/
					 /* └─────────────────────────────────┘						*/

typedef struct ch_s			 /* ┌───────────── ch_s ───────────────┐ структура для передачи инф-ции в канал	*/
 {	                                 /* │┌─── inf [0 ... MAX_REC_CH-1] ───┐│					*/
  struct				 /* ││ 				      ││					*/
   {				 	 /* ││    ┌─┬─┬──────┬─┬─┐   	      ││					*/
    uchar Ntty [7];			 /* ││    └─┴─┴ . . .┴─┴─┘   	      ││ номер телетайпа			*/
					 /* ││    <──────7───────>   	      ││					*/
					 /* ││    ┌─┐		     	      ││					*/
    uchar type;				 /* ││    └─┘			      ││ тип сети, куда подключен телетайп	*/
					 /* ││    ┌─┬─┬──────┬─┬─┐    	      ││					*/
    uchar room [4];			 /* ││    └─┴─┴ . . .┴─┴─┘    	      ││ комната абонента			*/
					 /* ││    <──────4───────>    	      ││					*/
					 /* ││    ┌─┬─┬──────┬─┬─┐    	      ││					*/
    uchar town [15];            	 /* ││    └─┴─┴ . . .┴─┴─┘    	      ││ город					*/
					 /* ││    <──────15──────>    	      ││					*/
					 /* ││    ┌─┬─┬──────┬─┬─┐    	      ││					*/
    uchar addr [15];			 /* ││    └─┴─┴ . . .┴─┴─┘            ││ название гостиницы, ПП или ОС		*/
					 /* ││    <──────15──────>            ││					*/
    union				 /* ││┌───────────── msg ────────────┐││					*/
     {                                   /* │││┌─────────── total ──────────┐│││					*/
      struct 				 /* ││││			    ││││ структура для передачи итогов в канал	*/
       {                                 /* ││││  ┌─┬─┬──────┬─┬─┐  	    ││││					*/
	uchar date [5];			 /* ││││  └─┴─┴ . . .┴─┴─┘  	    ││││ текущая дата				*/
					 /* ││││  <───────5──────>   	    ││││					*/
					 /* ││││  ┌─┬─┬──────┬─┬─┐    	    ││││					*/
	uchar quan [4];			 /* ││││  └─┴─┴ . . .┴─┴─┘   	    ││││ итоговое количество разговоров		*/
					 /* ││││  <───────4──────>   	    ││││					*/
					 /* ││││  ┌─┬─┬──────┬─┬─┐   	    ││││					*/
	uchar sum  [7];			 /* ││││  └─┴─┴ . . .┴─┴─┘          ││││ итоговая сумма				*/
					 /* ││││  <───────7──────>          ││││					*/
       } total;				 /* │││└────────────────────────────┘│││					*/
					 /* │││┌─────────── info ───────────┐│││					*/
					 /* ││││  ┌─┬─┬─┬─┬──────┬─┬─┬─┬─┐  ││││					*/
      uchar info [LENGTH_REC];		 /* ││││  └─┴─┴─┴─┴ . . .┴─┴─┴─┴─┘  ││││ полезная информация о разговорах	*/
					 /* ││││  <──────LENGTH_REC──────>  ││││					*/
     } msg;				 /* │││└────────────────────────────┘│││					*/
					 /* ││└──────────────────────────────┘││					*/
   } inf [MAX_REC_CH];			 /* │└────────────────────────────────┘│					*/
 } ch_s;				 /* └──────────────────────────────────┘					*/

typedef struct total_s			 /* ┌─────────── total_s ──────────────┐ структура итоговой информации		*/
 {                                       /* │				       │					*/
  uint quan [MAX_ADDR];			 /* │     ┌─┬─┬──────┬─┬─┐   	       │					*/
					 /* │     └─┴─┴ . . .┴─┴─┘             │ итоговое количество разговоров		*/
					 /* │     <───MAX_ADDR───>             │					*/
  long sum  [MAX_ADDR];			 /* │     ┌─┬─┬──────┬─┬─┐   	       │					*/
					 /* │     └─┴─┴ . . .┴─┴─┘             │ итоговые суммы				*/
					 /* │     <───MAX_ADDR───>             │					*/
 } total_s;				 /* └──────────────────────────────────┘					*/
