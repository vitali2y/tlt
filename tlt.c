/*[]****************************************************[]*/
/*[]							[]*/
/*[]  	    Междугородные  Телефонные  Разговоры	[]*/
/*[] (Гостиница + Переговорный пункт + Отделение связи)	[]*/
/*[]		     Версия  2.5  (ЭТК-КС)		[]*/
/*[]							[]*/
/*[]	     Ермоленко Виталий  &  Бибко Игорь		[]*/
/*[]      Житомир, ПО "Житомирсвязь", оАСУ, СВМТ	[]*/
/*[]							[]*/
/*[]          TLT.c: основной модуль программы 		[]*/
/*[]	     (работа с телефонной базой данных)		[]*/
/*[]            					[]*/
/*[]****************************************************[]*/

/*--------------------------------------------------------*/
/*		   Изменения и добавления		  */
/*--------------------------------------------------------*/
/*   1) Версия  1.0 (TLT-H.EXE) 	       23/  I/92  */
/*   2)	Версия  2.0             	       25/ II/92  */
/*  	  а) Добавлена работа с АПП и ОС;		  */
/*  	  б) Реализован поиск последнего полученного со-  */
/*  	     общения из заданной кабины;		  */
/*  	  в) Реализована передача итоговой информации в	  */
/*  	     канал связи;				  */
/*   3)	Версия  2.1             		6/III/92  */
/*	  а) Устранены незначительные недоработки;	  */
/*   4) Версия  2.2             	       26/III/92  */
/*	  а) Организованы 3 попыток ожидания автоответа	  */
/*	     из канала связи;				  */
/*	  б) Устранены недостатки при аварийном отключе-  */
/*	     нии назначения в канал связи (зависание,     */
/*	     передача нулевых итогов);			  */
/*	  в) Прерывания от таймера (INT 8h) при обработ-  */
/*	     ке информации возникают  в 2 раза чаще	  */
/*	     (27.5 мсек);			 	  */
/*	  г) Переписан алгоритм приема информации с	  */
/*	     адаптера ЕС 7920.01;			  */
/*	  д) Предусмотрено ожидание встречной передачи	  */
/*	     при выдаче итогов;				  */
/*    5) Версия  2.3			      14/VIII/92  */
/*	  а) Оптимизирован алгоритм поиска номера теле-   */
/*	     фона (ускоренный поиск телефонов для меньше- */
/*	     го количества телефонов в базе данных);	  */
/*	  б) Не верно удалялся последний номер телефона   */
/*	     после переполнении базы данных;		  */
/*	  в) Сохранение итоговой информации в файле;	  */
/*	  г) Передача итоговой информации сразу для всех  */
/*	     адресатов (режим ??????);			  */
/*	  д) Поиск последних 2-х сообщений;		  */
/*	  е) Устранена возможность перезаписи и дублиро-  */
/*	     вания информации при чтении ее из адаптера;  */
/*	  ж) Изменен алгоритм выдачи на принтер;	  */
/*	  з) Устранена ошибка после нажатия F6 в режиме	  */
/*	     поиска номера телефона;			  */
/*	  и) При блокировке канала постоянная переполю-	  */
/*	     совка до восстановления возможности передачи */
/*    6) Версия  2.4			      28/IX/92    */
/*        a) Изменен протокол обмена с АПК "Телеграф";	  */
/*	  б) При обработке информации изменен вывод ин-   */
/*	     формации в верхнее окно (из основ. прог-мы); */
/*	  в) Реализован новый скроллинг окон обработки;   */
/*    7) Версия  2.5  (ЭТК-КС)				  */
/*	  a) Реализовано: после 10 минут отсутствия рабо- */
/*           ты по каналу запрашивается квитанция	  */
/*           Метка: mikle.  		      08/II/93	  */
/*--------------------------------------------------------*/

#ifndef			__SMALL__
#error			Используйте малую модель памяти!
#endif

/*[]****************************************************[]*/
/*[]		 Режимы  работы  программы		[]*/
/*[]****************************************************[]*/
#define			WORK_WITHOUT_TC         	/*  работа программы вне среды Turbo C	*/
#define			WORK_WITH_ADAP			/* 	работа с реальным адаптером	*/

/*[]****************************************************[]*/
/*[]		Включаемые  файлы  программы		[]*/
/*[]****************************************************[]*/
#include		<dos.h>
#include		<fcntl.h>
#include		<io.h>
#include		<stdio.h>				/* mikle */
#include		<limits.h>
#include		<mem.h>
#include		<stdlib.h>
#include		<time.h>
#include		"TLT.h"
#include		"Declare.c"
#include		"InpAdapt.c"
#include		"Process.c"

/*[]****************************************************[]*/
/*[]             Главная  функция  программы		[]*/
/*[]****************************************************[]*/
main ()
 {
  uint i;

  write (STDOUT, MSGTITLE, sizeof (MSGTITLE));
  _AH = 0x19;  Intr (0x21);				/*     получение текущего устройства	*/
  cur_drive = _AL;

#ifdef WORK_WITH_ADAP

  if (inp (0x220) & BIT_DUMMY)				/* 	   отсутствует адаптер?		*/
   {
    write (STDOUT, MSGERRADAP, sizeof (MSGERRADAP));
    Signal ();
    write (STDOUT, MSGBYE, sizeof (MSGBYE));
    return (1);
   }

#endif

  tmp_ptr     = (uchar *)   malloc (sizeof (uchar *));
  ptr_addpay  = (uchar *)   malloc (sizeof (uchar *));
  cur_num_inf = (uchar *)   malloc (MAX_INF);
  db          = (tdb *)     malloc (sizeof (tdb));
  ptr_dst     = (get_inf *) malloc (sizeof (get_inf));
  ptr_ch      = (ch_s *)    malloc (sizeof (ch_s));
  ptr_total   = (total_s *) malloc (sizeof (total_s));
  if (!db || !ptr_dst || !ptr_ch || !cur_num_inf || !ptr_total)
   {
    write (STDOUT, MSGERRNOMEM, sizeof (MSGERRNOMEM));
    Signal ();
    write (STDOUT, MSGBYE, sizeof (MSGBYE));
    return (2);
   }
  count_ch = head_ch = tail_ch = 0;
  memset (&ptr_ch->inf [0].Ntty [0], NULL, sizeof (ch_s));

#ifndef WORK_WITH_TC

  Old_0x09 = getvect (9);  setvect (9, New_0x09);

#endif

  setvect (0x24, New_0x24);
  PushInf (INFMAINMENU);
  ptr_addpay = (uchar *) &db->status.sumaddpay;
  InstallWnd ();					/* 	  установка рабочих окон	*/
  CursorXY (WORK_WND, 0, INVISIBLE);
  LoadTlphDB ();  if (!_DL)  InitTlphDB ();
  InitEquip (INITFORWORK);				/*       инициализация hardware		*/
  if (!TestInsideStub ())				/* проверка последов. шлейфом успешна?	*/
   {
    Error (ERRINSSTUB);
    sign.errinsstub = ON;  db->status.chann = OFF;  db->status.speed = 0;
   }
  if ((f_hlp = open (FILE_HLP, O_RDWR | O_BINARY, S_IREAD)) == -1)  sign.hlp = OFF;
  else
   {
    read (f_hlp, name, sizeof (MSGTLPHHLP));
    if (!memcmp (name, MSGTLPHHLP, sizeof (MSGTLPHHLP)))
     {
      close (f_hlp);  sign.hlp = OFF;
     }
    else arr_pos_hlp [(cur_pos_hlp = 0)] = tell (f_hlp);
   }
  _DL = db->status.tmp_drive - 'A';  _AH = 0x0e;
  Intr (0x21);						/*     установка времен. устройства	*/
  if (!access (FILE_SUM, 0))  i = O_RDWR | O_APPEND | O_TEXT;
  else			      i = O_RDWR | O_CREAT  | O_TEXT;
  if ((sum_f = open (FILE_SUM, i, S_IREAD | S_IWRITE)) == -1)
   {
    Error (ERRSUMFILE);  sign.sum_file = OFF;
   }
  else
   {
    sign.sum_file = ON;
    if (i == (O_RDWR | O_CREAT | O_TEXT))
      write (sum_f, MSGTITLETOTAL, sizeof (MSGTITLETOTAL));
    write (sum_f, MSGTOTALFILE, sizeof (MSGTOTALFILE) - 1);
   }
  PushInf (INFMAINMENU);
  SetVideoPage (WORK_WND);
  _AH = 0x2a;  Intr (0x21);				/*	получение текущей даты		*/
  if (_CX < 1992)  SetDate ();
   type_month = (_DH > 9) ? '1' : '0';			/*    	 установка типа месяца		*/
  while (TRUE)
   {
    while (VertSelect (X_MENU_WND+1, Y_MENU_WND+1, Y_MENU_WND+1, MenuFunc, NULL) != UINT_MAX);
    if (!sign.exit)  ExitToDos ();
    if (sign.exit)   break;
   }
  if (sign.change)  Ask (SAVE);
  PopInf ();
  SetVideoPage (VP_DOS);
  ClrVideoPage (VP_WORK);  ClrVideoPage (VP_WAIT);
  close (f_hlp);
  close (sum_f);
  outp (0x3f9, 0x15);					/* 	       отключение от АПК	*/
  write (STDOUT, MSGGRATIT, sizeof (MSGGRATIT));
  write (STDOUT, MSGBYE, sizeof (MSGBYE));
  _DL = cur_drive;  _AH = 0x0e;  Intr (0x21);		/*     установка устройства 		*/

#ifndef WORK_WITH_TC

  setvect (9, Old_0x09);

#endif

  return (0);
 }

/*[]****************************************************[]*/
/*[]   	     Ввод  и  установка  текущей  даты		[]*/
/*[]****************************************************[]*/
void SetDate (void)
 {
  uchar arr [3] [9], flag, cur_day, cur_month,
	far *ptr_setdate;
  uint i, j, k, cur_year;

  ptr_setdate = SaveWnd (ARG_WND_SETDATE);
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_SETDATE);
  ToXY (VP_WORK, LINE, X_MENU_WND+31, Y_MENU_WND+9, " Дата ");
  PushInf (INFDATE);
  while (TRUE)                                          /*      ввод текущей даты	*/
   {
    flag = DAY;  i = k = 0;
    memset ((char *) &arr, NULL, sizeof (arr));
    ClrStr (MAX_STR, X_MENU_WND+24, Y_MENU_WND+10);
    while (TRUE)
     {
      CursorXY (WORK_WND, X_MENU_WND+30+k, Y_MENU_WND+10);
      if ((j = GetKey (NO_KEYS)) == ENTER_KEY)  break;
      ToXY (VP_WORK, SYMB, X_MENU_WND+30+k++, Y_MENU_WND+10, (uchar *) &j);
      if ((j == '/') && (flag < 2))			/* перейти к вводу след. пар-ра	*/
       {
	++flag;  i = 0;  continue;
       }
      if ((j >= '0') && (j <= '9') && (i < 2))  arr [flag] [i++] = j;
      else
       {
	Error (ERRINP);  break;
       }
     }
    if (j != ENTER_KEY)  continue;
    if (arr [DAY] [2] || arr [MONTH] [2] || arr [YEAR] [2] || (flag != 2))
     {
      Error (ERRINP);  continue;
     }
    i = atoi (arr [DAY]);          cur_day   = (((i / 16) << 4) + i % 16);
    i = atoi (arr [MONTH]);        cur_month = (((i / 16) << 4) + i % 16);
    i = atoi (arr [YEAR]) + 1900;  cur_year  = (((i / 16) << 4) + i % 16);
    type_month = (cur_month > 9) ? '1' : '0';		/*    установка типа месяца	*/
    _CX = cur_year;  _DH = cur_month;  _DL = cur_day;
    _AH = 0x2b;  Intr (0x21);				/*        установка даты	*/
    if (!_AL)  break;
    Error (ERRINP);
   }
  PopInf ();
  CursorXY (WORK_WND, 0, INVISIBLE);
  RestoreWnd (ptr_setdate, ARG_WND_SETDATE);
 }

/*[]****************************************************[]*/
/*[]        INT 9h:  новый  обработчик  IRQ 1		[]*/
/*[]           (прерывание  от  клавиатуры)		[]*/
/*[]****************************************************[]*/
void interrupt New_0x09 (void)
 {
  register uint i;

  switch (inp (0x60))
   {
    case 0x4f:					/*                Ctrl + End		*/
     if (*ptr_shift_keys & CTRL_KEY_POS)  sign.ctrl_end_key = ON;
    case 0x46:  case 0xc6:			/*               Ctrl + Break		*/
    case 0x2e:  case 0xae:			/*                  Ctrl + C		*/
     if (*ptr_shift_keys & CTRL_KEY_POS)
      {
       outp (0x61, (i = inp (0x61)) | 0x80);
       outp (0x61, i);
       outp (0x20, 0x20);
       return;
      }
   }
  if (inp (0x60) & 0x80)  sign.kbhit = ON;	/* признак отпускания клавиши установлен*/
  (*Old_0x09) ();				/*   выполнение оригинального вектора	*/
 }

/*[]****************************************************[]*/
/*[]       Справочная  информация  по пользованию	[]*/
/*[]****************************************************[]*/
void Help (void)
 {
  uchar arr_help [SIZE_HELP], *ptr_help_out, x, y, far *ptr_help;
  int j;

  if (sign.wnd_processout)  return;
  if (!sign.hlp)
   {
    Error (ERRHLP);  return;
   }
  ptr_help = SaveWnd (ARG_WND_HELP);
  if (cur_pos_hlp != 0)  --cur_pos_hlp;
  lseek (f_hlp, arr_pos_hlp [cur_pos_hlp], SEEK_SET);
  PushInf (INFHELP);
  do
   {
    sign.end_help = OFF;
    ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_HELP);
    ToXY (VP_WORK, LINE, X_HELP_WND+15, Y_HELP_WND, " Помощь пользователю ");
    y = Y_HELP_WND+1;
    do							/* чтение и заполнение окна	*/
     {
      ptr_help_out = (char *) &arr_help;  memset (&arr_help, NULL, sizeof (arr_help));
      do						/*         чтение строки	*/
       {
	read (f_hlp, (char *) ptr_help_out, sizeof (char));
	if (ptr_help_out >= &arr_help [SIZE_HELP-1])  break;
/*	if (*ptr_help_out == EOF)			/* достигнут конец файла	*/ */
	if (*ptr_help_out == 0x1a)			/* достигнут конец файла	*/ /* mikle */
         {
          sign.end_help = ON;  break;
         }
        ++ptr_help_out;
	if (sign.err)
	 {
	  sign.err = OFF;  Error (ERRHLP);  return;
	 }
       }
      while (*(ptr_help_out-1) != '\n');
      if (sign.end_help)  break;
      x = X_HELP_WND+2;
      ptr_help_out = (char *) &arr_help;  arr_help [SIZE_HELP-1] = NULL;
      do						/*       отображение строки	*/
       {
	if ((*ptr_help_out == '\n') || (*ptr_help_out == '\r') || (*ptr_help_out == '\f'))  continue;
	ToXY (VP_WORK, SYMB, x++, y, (char *) ptr_help_out);
       }
      while (*(++ptr_help_out) != NULL);
      FromXY (VP_WORK, SYMB, X_HELP_WND, ++y, NULL);
     }
    while (_AL != (uchar) '└');
    if (cur_pos_hlp < MAX_SIZE_HLP)
      arr_pos_hlp [++cur_pos_hlp] = tell (f_hlp);	/* запоминание текущей позиции	*/
    while (TRUE)
     {
      switch (j = GetKey (NO_KEYS))
       {
	case HOME_KEY:
	 lseek (f_hlp, arr_pos_hlp [(cur_pos_hlp = 0)], SEEK_SET);
         break;
	case PG_UP_KEY:
	 if (cur_pos_hlp)  --cur_pos_hlp;
	 if (cur_pos_hlp)  --cur_pos_hlp;
	 lseek (f_hlp, arr_pos_hlp [cur_pos_hlp], SEEK_SET);
	 break;
	case PG_DN_KEY:
	 if ((cur_pos_hlp == MAX_SIZE_HLP-1) || sign.end_help)
	  {
	   lseek (f_hlp, arr_pos_hlp [--cur_pos_hlp], SEEK_SET);
	  }
	case ESC_KEY:
         break;
	default:
         Squeak ();  continue;
       }
      break;
     }
   }
  while (j != ESC_KEY);
  RestoreWnd (ptr_help, ARG_WND_HELP);
  PopInf ();
 }

/*[]****************************************************[]*/
/*[]      Включение / выключение  режимов  работы	[]*/
/*[]****************************************************[]*/
void SetRegime (void)
 {
  uchar far *ptr_setregime;

  if (sign.wnd_processout)  return;
  if (sign.wnd_setregime || sign.wnd_processout)  return;
  sign.wnd_setregime = ON;
  if (sign.wnd_listtlphdb >= ON)  sign.wnd_listtlphdb+=2;
  ptr_setregime = SaveWnd (ARG_WND_SETREGIME);
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_SETREGIME);
  ToXY (VP_WORK, LINE, X_MENU_WND+23, Y_MENU_WND+5, " Вывод информации ");
  ToXY (VP_WORK, LINE, X_MENU_WND+4, Y_MENU_WND+6, "... на дисковое устройство");
  ToXY (VP_WORK, LINE, X_MENU_WND+4, Y_MENU_WND+7, "... на принтер");
  ToXY (VP_WORK, LINE, X_MENU_WND+4, Y_MENU_WND+8, "... в канал связи");
  tmp_c = '├';  ToXY (VP_WORK, SYMB, X_MENU_WND+2, Y_MENU_WND+9, (char *) &tmp_c);
  tmp_c = '─';  ToXY (VP_WORK, 56, X_MENU_WND+3, Y_MENU_WND+9, (char *) &tmp_c);
  tmp_c = '┤';  ToXY (VP_WORK, SYMB, X_MENU_WND+59, Y_MENU_WND+9, (char *) &tmp_c);
  ToXY (VP_WORK, LINE, X_MENU_WND+23, Y_MENU_WND+9, " Параметры вывода ");
  ToXY (VP_WORK, LINE, X_MENU_WND+4, Y_MENU_WND+10, "Дисковое устройство хранения информации              \x10");
  ToXY (VP_WORK, LINE, X_MENU_WND+4, Y_MENU_WND+11, "Скорость обмена с телеграфным каналом связи          \x10");
  tmp_c = '├';  ToXY (VP_WORK, SYMB, X_MENU_WND+2, Y_MENU_WND+12, (char *) &tmp_c);
  tmp_c = '─';  ToXY (VP_WORK, 56, X_MENU_WND+3, Y_MENU_WND+12, (char *) &tmp_c);
  tmp_c = '┤';  ToXY (VP_WORK, SYMB, X_MENU_WND+59, Y_MENU_WND+12, (char *) &tmp_c);
  ToXY (VP_WORK, LINE, X_MENU_WND+4, Y_MENU_WND+13, "Дополнительные режимы и параметры                    \x10");
  PushInf (INFMAINMENU);
  do
   {
    PrintRegime ();
   }
  while (VertSelect (X_MENU_WND+3, Y_MENU_WND+6, Y_MENU_WND+6, NULL, PosSetRegimeFunc) != UINT_MAX);
  RestoreWnd (ptr_setregime, ARG_WND_SETREGIME);
  PopInf ();
  if (sign.wnd_listtlphdb-2 >= ON)  sign.wnd_listtlphdb-=2;
  sign.wnd_setregime = OFF;
 }

/*[]****************************************************[]*/
/*[]           Отображение  режимов  работы		[]*/
/*[]****************************************************[]*/
void PrintRegime (void)
 {
  if (db->status.flop)  tmp_c = '√';
  else tmp_c = ' ';
  ToXY (VP_WORK, SYMB, X_MENU_WND+50, Y_MENU_WND+6, &tmp_c);
  if (db->status.prn)  tmp_c = '√';
  else tmp_c = ' ';
  ToXY (VP_WORK, SYMB, X_MENU_WND+50, Y_MENU_WND+7, &tmp_c);
  if (db->status.chann)  tmp_c = '√';
  else tmp_c = ' ';
  ToXY (VP_WORK, SYMB, X_MENU_WND+50, Y_MENU_WND+8, &tmp_c);
  ToXY (VP_WORK, SYMB, X_MENU_WND+50, Y_MENU_WND+10, (uchar *) &db->status.tmp_drive);
  if (db->status.tmp_drive == ' ')  tmp_c = ' ';
  else tmp_c = ':';
  ToXY (VP_WORK, SYMB, X_MENU_WND+51, Y_MENU_WND+10, (uchar *) &tmp_c);
  ToXY (VP_WORK, LINE, X_MENU_WND+49, Y_MENU_WND+11, (uchar *) speed_transm [db->status.speed]);
  if (db->status.chann)  ToXY (VP_WORK, LINE, X_MENU_WND+53, Y_MENU_WND+11, "бод");
  else
   {
    tmp_c = ' ';  ToXY (VP_WORK, 3, X_MENU_WND+53, Y_MENU_WND+11, (uchar *) &tmp_c);
   }
 }

/*[]****************************************************[]*/
/*[]    Отработка  позиций  меню  "установка  режимов"	[]*/
/*[] Дополнительный входной параметр: DL - y-координата	[]*/
/*[]****************************************************[]*/
void PosSetRegimeFunc (void)
 {
  switch (_DL)
   {
    case 0:
     if (db->status.flop)
      { db->status.tmp_drive = ' ';  db->status.flop = OFF; }
     else  SetInfDrive ();
     break;
    case 1:
     db->status.prn = !db->status.prn;  sign.change = ON;
     break;
    case 2:
     if (db->status.chann)
      { db->status.chann = OFF;  db->status.speed = 0; }
     else  SetSpeed ();
     break;
    case 3:
     SetInfDrive ();
     break;
    case 4:
     SetSpeed ();
     break;
    case 5:
     AddArg ();
   }
  PrintRegime ();
 }

/*[]****************************************************[]*/
/*[]    Установка  устройства  временного  хранения	[]*/
/*[]****************************************************[]*/
void SetInfDrive (void)
 {
  uint i = 0;
  uchar pos, far *ptr_drive;

  ptr_drive = SaveWnd (ARG_WND_TMPDRIVE);
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_TMPDRIVE);
  for (pos = 0; pos < 5; pos++)
   {
    tmp_c = pos + 'A';
    ToXY (VP_WORK, SYMB, X_MENU_WND+5, Y_MENU_WND+12+pos, (uchar *) &tmp_c);
   }
  PushInf (INFTMPDRV);
  do
   {
    if (i == ENTER_KEY)  { db->status.flop = ON;  break; }
   }
  while ((i = VertSelect (X_MENU_WND+4, Y_MENU_WND+12, Y_MENU_WND+13, NULL, PosInfDriveFunc)) != UINT_MAX);
  RestoreWnd (ptr_drive, ARG_WND_TMPDRIVE);
  PopInf ();
 }

/*[]****************************************************[]*/
/*[]          Установка  скорости  передачи		[]*/
/*[]****************************************************[]*/
void SetSpeed (void)
 {
  uint i = 0;
  uchar pos, far *ptr_speed;

  if (sign.errinsstub)
   {
    Error (ERRINSSTUB);  return;
   }
  ptr_speed = SaveWnd (ARG_WND_SPEED);
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_SPEED);
  ToXY (VP_WORK, LINE, X_MENU_WND+4, Y_MENU_WND+12, "бит/с");
  for (pos = 1; pos < 5; pos++)
   {
    ToXY (VP_WORK, LINE, X_MENU_WND+5, Y_MENU_WND+12+pos, (uchar *) speed_transm [pos]);
   }
  PushInf (INFSPEED);
  do
   {
    if (i == ENTER_KEY)
     {
      db->status.chann = ON;  InitSpeed ();  break;
     }
   }
  while ((i = VertSelect (X_MENU_WND+4, Y_MENU_WND+13, Y_MENU_WND+15, NULL, PosSpeedFunc)) != UINT_MAX);
  RestoreWnd (ptr_speed, ARG_WND_SPEED);
  PopInf ();
 }

/*[]****************************************************[]*/
/*[]   Отработка  позиций  меню  "установка  скорости"	[]*/
/*[] Дополнительный входной параметр: DL - y-координата	[]*/
/*[]****************************************************[]*/
void PosSpeedFunc (void)
 {
  db->status.speed = ++_DL;
  sign.change = ON;
 }

/*[]****************************************************[]*/
/*[]  Отработка  позиций  меню  "установка  устройства"	[]*/
/*[] Дополнительный входной параметр: DL - y-координата	[]*/
/*[]****************************************************[]*/
void PosInfDriveFunc (void)
 {
  db->status.tmp_drive = _DL + 'A';
  sign.change = ON;
 }

/*[]****************************************************[]*/
/*[]		Дополнительные  параметры		[]*/
/*[]****************************************************[]*/
void AddArg (void)
 {
  uchar arr_tmp [5], far *ptr_addarg;

  ptr_addarg = SaveWnd (ARG_WND_ADDARG);
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_ADDARG);
  ToXY (VP_WORK, LINE, X_MENU_WND+13, Y_MENU_WND+6, " Дополнительные режимы и параметры ");
  ToXY (VP_WORK, LINE, X_MENU_WND+5, Y_MENU_WND+7, "Поиск телефонов по категории абонента");
  ToXY (VP_WORK, LINE, X_MENU_WND+5, Y_MENU_WND+8, "Доплата за услуги");
  ToXY (VP_WORK, LINE, X_MENU_WND+5, Y_MENU_WND+9, "Интервал времени между пакетами сообщений");
  PushInf (INFMAINMENU);
  do
   {
    if (db->status.categ)
     {
      ClrStr (3, X_MENU_WND+47, Y_MENU_WND+7);
      ToXY (VP_WORK, SYMB, X_MENU_WND+48, Y_MENU_WND+7, (uchar *) &db->status.categ);
     }
    else  ToXY (VP_WORK, LINE, X_MENU_WND+47, Y_MENU_WND+7, "Нет");
    memcpy (&arr_tmp, &db->status.sumaddpay, sizeof (db->status.sumaddpay));
    if (db->status.chann && (arr_tmp [0] || arr_tmp [2] || arr_tmp [3]))
     {
      arr_tmp [0] |= '0';  arr_tmp [2] |= '0';
      arr_tmp [3] |= '0';  arr_tmp [4] = NULL;
      ToXY (VP_WORK, LINE, X_MENU_WND+47, Y_MENU_WND+8, (uchar *) &arr_tmp);
      ToXY (VP_WORK, LINE, X_MENU_WND+52, Y_MENU_WND+8, "руб");
     }
    else  ToXY (VP_WORK, LINE, X_MENU_WND+47, Y_MENU_WND+8, "Нет     ");
    if (db->status.chann)
     {
      itoa ((db->status.load_wait / 20), (uchar *) &arr_tmp, 10);
      if (!arr_tmp [1])
       {
	arr_tmp [1] = arr_tmp [0];  arr_tmp [0] = ' ';
       }
      arr_tmp [2] = NULL;
      ClrStr (7, X_MENU_WND+47, Y_MENU_WND+9);
      ToXY (VP_WORK, LINE, X_MENU_WND+47, Y_MENU_WND+9, (uchar *) &arr_tmp);
      ToXY (VP_WORK, LINE, X_MENU_WND+52, Y_MENU_WND+9, "сек");
     }
    else  ToXY (VP_WORK, LINE, X_MENU_WND+47, Y_MENU_WND+9, "Нет      ");
   }
  while (VertSelect (X_MENU_WND+4, Y_MENU_WND+7, Y_MENU_WND+7, AddArgFunc, NULL) != UINT_MAX);
  RestoreWnd (ptr_addarg, ARG_WND_ADDARG);
  PopInf ();
 }

/*[]****************************************************[]*/
/*[]	       Установка  категории  абонента		[]*/
/*[]****************************************************[]*/
void SetCateg (void)
 {
  uchar arr_tmp [MAX_STR], far *ptr_categ;

  ptr_categ = SaveWnd (ARG_WND_CATEG);
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_CATEG);
  ToXY (VP_WORK, LINE, X_MENU_WND+5, Y_MENU_WND+8, " Категория абонента ");
  PushInf (INFCATEG);
  memset (&arr_tmp, NULL, sizeof (arr_tmp));  arr_tmp [0] = db->status.categ;
  if (EditStr ((char *) &arr_tmp, X_MENU_WND+5, Y_MENU_WND+9) == ENTER_KEY)
   {
    if (arr_tmp [1])  Error (ERRCATEG);
    else
     {
      if (!arr_tmp [0])
       {
	db->status.categ = OFF;				/* очистить значение категории гостин. номеров	*/
	sign.change = ON;
       }
      else
       {
	if ((arr_tmp [0] >= '0') && (arr_tmp [0] <= '8'))
	 {
	  db->status.categ = arr_tmp [0];		/*     установить категорию гостин. номеров	*/
	  sign.change = ON;
	 }
	else  Error (ERRCATEG);
       }
     }
   }
  RestoreWnd (ptr_categ, ARG_WND_CATEG);
  PopInf ();
 }

/*[]****************************************************[]*/
/*[]	      Установка  доплаты  за  услуги		[]*/
/*[]****************************************************[]*/
void SetAddPay (void)
 {
  uchar arr_tmp [MAX_STR], far *ptr_addpay;

  if (db->status.chann)
   {
    ptr_addpay = SaveWnd (ARG_WND_ADDPAY);
    ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_ADDPAY);
    ToXY (VP_WORK, LINE, X_MENU_WND+10, Y_MENU_WND+9, " Доплата ");
    PushInf (INFADDPAY);
    memcpy (&arr_tmp, &db->status.sumaddpay, sizeof (db->status.sumaddpay));
    arr_tmp [0] |= '0';  arr_tmp [2] |= '0';
    arr_tmp [3] |= '0';  arr_tmp [4] = NULL;
    if (EditStr ((char *) &arr_tmp, X_MENU_WND+5, Y_MENU_WND+10) == ENTER_KEY)
     {
      if ((arr_tmp [0] > '9') || (arr_tmp [2] > '9') || (arr_tmp [3] > '9') ||
	  arr_tmp [4]         || (arr_tmp [1] != '.'))  Error (ERRADDPAY);
      else
       {
	memcpy (&db->status.sumaddpay, &arr_tmp, sizeof (db->status.sumaddpay));
	db->status.sumaddpay [0] &= 0x0f;  db->status.sumaddpay [2] &= 0x0f;  db->status.sumaddpay [3] &= 0x0f;
	sign.change = ON;
       }
     }
    RestoreWnd (ptr_addpay, ARG_WND_ADDPAY);
    PopInf ();
   }
  else Error (ERRABSCHAN);
 }

/*[]****************************************************[]*/
/*[] 	        Установка  интервала  времени		[]*/
/*[]    между  пакетами  сообщений  в  канал  связи	[]*/
/*[]****************************************************[]*/
void SetSpaceTime (void)
 {
  uchar arr_tmp [MAX_STR], far *ptr_spacetime;

  if (db->status.chann)
   {
    ptr_spacetime = SaveWnd (ARG_WND_SPACETIME);
    ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_SPACETIME);
    ToXY (VP_WORK, LINE, X_MENU_WND+6, Y_MENU_WND+10, " Интервал времени ");
    PushInf (INFSPACETIME);
    itoa (db->status.load_wait >> 1, (uchar *) &arr_tmp, 10);
    arr_tmp [2] = NULL;
    if (EditStr ((char *) &arr_tmp, X_MENU_WND+5, Y_MENU_WND+11) != ESC_KEY)
     {
      if (arr_tmp [2] || ((db->status.load_wait = atoi((uchar *)&arr_tmp) << 1) == 0))
       {
	 Error (ERRSPACTIME);  db->status.load_wait = 300;
       }
      else
       { db->status.load_wait = db->status.load_wait * 10; sign.change = ON; }
     }
    RestoreWnd (ptr_spacetime, ARG_WND_SPACETIME);
    PopInf ();
   }
  else Error (ERRABSCHAN);
 }

/*[]****************************************************[]*/
/*[]  Вынужденное  удаление  установленных  назначений	[]*/
/*[]****************************************************[]*/
void DelAssign (char num)
 {
  static char *arr_ass [] = {"в канал связи!", "на диск!", "на принтер!"};
  char far *ptr_ass;

  ptr_ass = SaveWnd (ARG_WND_DELASS);
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_DELASS);
  ToXY (VP_WORK, LINE, X_MENU_WND+8, Y_MENU_WND+11, "Внимание! Вынужденное отключение назначения");
  ToXY (VP_WORK, LINE, X_MENU_WND+52, Y_MENU_WND+11, (char *) arr_ass [num]);
  PushInf (INFANYKEY);
  Signal ();
  _AX = 0x0c07;  Intr (0x21);		/* очистка буфера и ожидание нажатия клавиши	*/
  switch (num)
   {
    case CHANN:
     db->status.chann = OFF;  db->status.speed = 0;
     break;
    case FLOP:
     db->status.flop = OFF;  db->status.tmp_drive = ' ';
     break;
    case PRN:
     db->status.prn = OFF;
   }
  sign.change = ON;
  RestoreWnd (ptr_ass, ARG_WND_DELASS);
  PopInf ();
 }

/*[]****************************************************[]*/
/*[]             Меню:  обработка  информации		[]*/
/*[]****************************************************[]*/
void Process (void)
 {
  char far *ptr_process;

  sign.wnd_process = ON;
  ptr_process = SaveWnd (ARG_WND_TREAT);
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_TREAT);
  ToXY (VP_WORK, LINE, X_MENU_WND+16, Y_MENU_WND+3, " Обработка информации ");
  ToXY (VP_WORK, LINE, X_MENU_WND+3, Y_MENU_WND+4, "Установка режимов работы                    {F2} \x10");
  ToXY (VP_WORK, LINE, X_MENU_WND+3, Y_MENU_WND+5, "Прием, обработка, передача информации       {F3}");
  ToXY (VP_WORK, LINE, X_MENU_WND+3, Y_MENU_WND+6, "Повторная передача информации в канал связи");
  ToXY (VP_WORK, LINE, X_MENU_WND+3, Y_MENU_WND+7, "Тестирование периферийного оборудования        \x10");

/*
  tmp_c = '├';  ToXY (VP_WORK, SYMB, X_MENU_WND+1, Y_MENU_WND+8, (char *) &tmp_c);
  tmp_c = '─';  ToXY (VP_WORK, 51, X_MENU_WND+2, Y_MENU_WND+8, (char *) &tmp_c);
  tmp_c = '┤';  ToXY (VP_WORK, SYMB, X_MENU_WND+52, Y_MENU_WND+8, (char *) &tmp_c);
  ToXY (VP_WORK, LINE, X_MENU_WND+3, Y_MENU_WND+9, "Дополнительное меню обработки информации       \x10");
*/

  PushInf (INFMAINMENU);
  while (VertSelect (X_MENU_WND+2, Y_MENU_WND+4, Y_MENU_WND+4, ProcessFunc, NULL) != UINT_MAX);
  RestoreWnd (ptr_process, ARG_WND_TREAT);
  PopInf ();
  sign.wnd_process = OFF;
 }

/*[]****************************************************[]*/
/*[]              Поиск  номера  телефона		[]*/
/*[]****************************************************[]*/
void FindNTlph (uint first_char)
 {
  uint i;
  uchar far *ptr_find;
  static uchar arr_tmp [MAX_STR];

  if ((sign.wnd_listtlphdb == ON) || !sign.wnd_listtlphdb)
   {
    sign.find = ON;
    ptr_find = SaveWnd (ARG_WND_FIND);
    ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_FIND);
    ToXY (VP_WORK, LINE, X_MENU_WND+5, Y_MENU_WND+9, " Номер телефона ");
    PushInf (INFFIND);
    memset (&arr_tmp, NULL, sizeof (arr_tmp));
    arr_tmp [0] = (uchar) first_char;
    i = EditStr ((char *) &arr_tmp, X_MENU_WND+3, Y_MENU_WND+10);
    RestoreWnd (ptr_find, ARG_WND_FIND);
    PopInf ();
    sign.find = OFF;
    if (sign.exit)  return;
    if (i != ESC_KEY)				/* осуществлять поиск телефона?	*/
     {
      SearchNTlph (atol (arr_tmp), (uint *) &i);
      if (!_DL)  Error (ERRTLPHNO);
      else
       {
	cur_addr = db->tlph [cur_pos = i].ind_addr;
	cur_x_pos = X_MENU_WND+3;  cur_y_pos = Y_MENU_WND+7;
	if (sign.wnd_listtlphdb == ON)  ListPageTlph ();
	else			 	ListTlphDB ();
       }
     }
   }
 }

/*[]****************************************************[]*/
/*[]        Добавление,  изменение  или  удаление  	[]*/
/*[]	             номера  телефона			[]*/
/*[] Дополнительный входной параметр: DL - номер позиции[]*/
/*[]****************************************************[]*/
void AddChangDelNTlph (void)
 {
  ulong old_tlph, tlph, old_tty, tty;
  uint i, j, old_room, room;
  char old_type, type, code, old_town [MAX_NAME], old_addr [MAX_NAME], arr_tmp [5] [MAX_STR],
       far *ptr_add, far *ptr_del, far *ptr_addenv;

  code = _DL;
  sign.wnd_addchangdel = ON;
  ptr_addenv = SaveWnd (ARG_WND_ADDENV);
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_ADDENV);
  ToXY (VP_WORK, LINE, X_MENU_WND+42, Y_MENU_WND,   "Город:");
  ToXY (VP_WORK, LINE, X_MENU_WND+40, Y_MENU_WND+1, "Адресат:");
  ToXY (VP_WORK, LINE, X_MENU_WND+32, Y_MENU_WND+2, "Номер телетайпа:");
  ToXY (VP_WORK, LINE, X_MENU_WND+33, Y_MENU_WND+3, "Номер телефона:");
  ToXY (VP_WORK, LINE, X_MENU_WND+33, Y_MENU_WND+4, "Комната/кабина:");
  ptr_add = SaveWnd (ARG_WND_INP);
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_INP);
  setmem (&arr_tmp, sizeof (arr_tmp), NULL);
  memcpy ((char *) &arr_tmp [0], (char *) &db->addr [cur_addr].town, sizeof (arr_tmp [0]));
  ToXY (VP_WORK, LINE, X_MENU_WND+49, Y_MENU_WND, (char *) &arr_tmp [0]);
  memcpy ((char *) &arr_tmp [1], (char *) &db->addr [cur_addr].addr, sizeof (arr_tmp [0]));
  ToXY (VP_WORK, LINE, X_MENU_WND+49, Y_MENU_WND+1, (char *) &arr_tmp [1]);
  ltoa (db->addr [cur_addr].Ntty, (char *) &arr_tmp [2], 10);
  arr_tmp [2] [6] = db->addr [cur_addr].type;
  ToXY (VP_WORK, LINE, X_MENU_WND+49, Y_MENU_WND+2, (char *) &arr_tmp [2]);
  ltoa (db->tlph [cur_pos].Ntlph, (char *) &arr_tmp [3], 10);
  i = 0;
  switch (code)
   {
    case ADD:  							/* добавление номера телефона	*/
     ToXY (VP_WORK, LINE, X_MENU_WND+41, Y_MENU_WND-1, " Вводимые данные ");
     memset (&arr_tmp [3], NULL, 2 * sizeof (arr_tmp [0]));
     break;
    case CHANG:							/* изменение номера телефона	*/
     old_tty  = db->addr [cur_addr].Ntty;
     memcpy (&old_addr, &db->addr [cur_addr].addr, sizeof (db->addr [0].addr));
     memcpy (&old_town,  &db->addr [cur_addr].town,  sizeof (db->addr [0].addr));
     old_type = db->addr [cur_addr].type;
     old_tlph = db->tlph [cur_pos].Ntlph;
     old_room = db->tlph [cur_pos].Nroom;
     ToXY (VP_WORK, LINE, X_MENU_WND+40, Y_MENU_WND-1, " Изменяемые данные ");
     ToXY (VP_WORK, LINE, X_MENU_WND+49, Y_MENU_WND+3, (char *) &arr_tmp [3]);
     ltoa (old_room, (char *) &arr_tmp [4], 10);
     ToXY (VP_WORK, LINE, X_MENU_WND+49, Y_MENU_WND+4, (char *) &arr_tmp [4]);
     break;
    case DEL:							/* удаление номера телефона	*/
     ToXY (VP_WORK, LINE, X_MENU_WND+41, Y_MENU_WND-1, " Удаляемые данные ");
     ToXY (VP_WORK, LINE, X_MENU_WND+49, Y_MENU_WND+3, (char *) &arr_tmp [3]);
     ltoa (db->tlph [cur_pos].Nroom, (char *) &arr_tmp [4], 10);
     ToXY (VP_WORK, LINE, X_MENU_WND+49, Y_MENU_WND+4, (char *) &arr_tmp [4]);
     ToXY (VP_WORK, LINE, X_MENU_WND+2, Y_MENU_WND, arr_inf [3]);
     ToXY (VP_WORK, LINE, X_MENU_WND+2, Y_MENU_WND+1, (char *) &arr_tmp [3]);
     ptr_del = SaveWnd (ARG_WND_DEL);
     ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_DEL);
     ToXY (VP_WORK, LINE, X_MENU_WND-2, Y_MENU_WND+6, "Желаете удалить номер телефона?");
     PushInf (INFDEL);
     if (GetKey (NO_KEYS) == ENTER_KEY)  DelNTlph (db->tlph [cur_pos].Ntlph);
     RestoreWnd (ptr_del, ARG_WND_DEL);
     PopInf ();
   }
  switch (code)
   {
    case ADD:
    case CHANG:
     while (i < 5)
      {
       ToXY (VP_WORK, LINE, X_MENU_WND+2, Y_MENU_WND, arr_inf [i]);
       PushInf (i + 9);
       j = EditStr ((char *) &arr_tmp [i], X_MENU_WND+2, Y_MENU_WND+1);
       PopInf ();
       if (sign.exit)						/* установлен признак возврата	*/
	{
	 RestoreWnd (ptr_add, ARG_WND_INP);
	 RestoreWnd (ptr_addenv, ARG_WND_ADDENV);
	 return;
	}
       if (j == ESC_KEY)  break;
       ClrStr (sizeof (arr_tmp [0]), X_MENU_WND+2, Y_MENU_WND+1);
       ClrStr (sizeof (arr_tmp [0]), X_MENU_WND+49, Y_MENU_WND+i);
       ToXY (VP_WORK, LINE, X_MENU_WND+49, Y_MENU_WND+i, (char *) &arr_tmp [i]);
       if (j == TAB_KEY)					/*   переход к следующему полю	*/
	{
	 i = (!i) ? 4 : --i;
	}
       else  ++i;
      }
   }
  if (j != ESC_KEY)						/*	проводить изменения?	*/
   {
    switch (code)
     {
      case CHANG:						/* изменение номера телефона	*/
       DelNTlph (db->tlph [cur_pos].Ntlph);
      case ADD:							/*  добавление номера телефона	*/
       j = TRUE;
       for (i = 0; i < 6; i++)					/*  корректный номер телетайпа?	*/
	{
	 if (arr_tmp [2] [i] > '9')  j = FALSE;
	 if (!arr_tmp [2] [i])       j = FALSE;
	}
       switch (arr_tmp [2] [6])
	{
	 case NULL:
	 case '-':
	  type = arr_tmp [2] [6];
	  break;
	 default:
	  j = FALSE;
	}
       if (!j)
	{
	 Error (ERRINP);  break;
	}
       else tty = atol ((char *) &arr_tmp [2]);
       i = UINT_MAX;
       do							/*  корректный номер телефона?	*/
	{
	 if (arr_tmp [3] [++i] > '9')  j = FALSE;
	}
       while (arr_tmp [3] [i]);
       if (!j || (i > 7))
	{
	 Error (ERRINP);  j = FALSE;  break;
	}
       else tlph = atol ((char *) &arr_tmp [3]);
       i = UINT_MAX;
       do							/*   корректный номер комнаты?	*/
	{
	 if (arr_tmp [4] [++i] > '9')  j = FALSE;
	}
       while (arr_tmp [4] [i]);
       if (!j || (i > 3))
	{
	 Error (ERRINP);  j = FALSE;  break;
	}
       else room = atoi ((char *) &arr_tmp [4]);
       if (tty && tlph && room)
	 j = AddNTlph (tlph, tty, room, (char *) &arr_tmp [0], (char *) &arr_tmp [1], type);
       else
	{
	 Error (ERRINP);  j = FALSE;
	}
     }
    if ((code == CHANG) && !j)					/*        произошла ошибка?	*/
      AddNTlph (old_tlph, old_tty, old_room, (char *) &old_town, (char *) &old_addr, old_type);
   }
  RestoreWnd (ptr_add, ARG_WND_INP);
  RestoreWnd (ptr_addenv, ARG_WND_ADDENV);
  if (sign.wnd_listtlphdb == ON)  ListPageTlph ();
  sign.wnd_addchangdel = OFF;
 }

/*[]****************************************************[]*/
/*[]    Добавление  номера  телефона  в  базу  данных	[]*/
/*[]****************************************************[]*/
uchar AddNTlph (ulong ta, ulong tty, uint room, char *town, char *nam, char type)
 {
  uint i = UINT_MAX, addr, add_addr = ON;

  if (db->serv_tlph == MAX_TLPH-1 || db->serv_addr == MAX_ADDR-1)
   {
    Error (ERROVER);  return (FALSE);		/*      переполнение базы данных	*/
   }
  while (++i < MAX_ADDR)			/* проверка правил-сти вводим. инфор-и	*/
   {
    if (!db->addr [i].Ntty)  break;		/*     последний элемент структуры?	*/
    if ((db->addr [i].Ntty == tty) &&		/*      найден требуемый адресат?	*/
	(db->addr [i].type == type))
     {
      if (memcmp (town, &db->addr [i].town, sizeof (db->addr [0].town)))
       {
	Error (ERRINP);  return (FALSE);
       }
      if (memcmp (nam, &db->addr [i].addr, sizeof (db->addr [0].addr)))
       {
	Error (ERRINP);  return (FALSE);
       }
      add_addr = OFF;  addr = i;  break;
     }
   }
  if (add_addr)  addr = i;
  SearchNTlph (ta, (uint *) &i);
  if (_DL)
   {
    Error (ERRTLPHYES);  return (FALSE);	/*        дублирование телефона		*/
   }
  if (ta > db->tlph [i].Ntlph)	++i;		/*         коррекция указателя		*/
  movmem (&db->tlph [i].ind_addr, &db->tlph [i+1].ind_addr, sizeof (db->tlph [0]) * ((db->serv_tlph+1) - i));
  if (add_addr)					/*       добавлять нового адресата?	*/
   {
    db->addr [addr].Ntty = tty;			/* добавление нового номера телетайпа	*/
    db->addr [addr].type = type;		/*	добавить тип телеграфной сети	*/
    memcpy ((char *) &db->addr [addr].addr, (char *) &nam [0],  sizeof (db->addr [0].addr));
    memcpy ((char *) &db->addr [addr].town, (char *) &town [0], sizeof (db->addr [0].town));
    db->serv_addr++;				/* инкремент количества адресатов	*/
   }
  db->tlph [i].Ntlph                 = ta;	/*  добавление телефонного номера	*/
  db->tlph [i].Nroom                 = room;	/*  добавление номера комнаты адресата	*/
  db->tlph [cur_pos = i].ind_addr    = addr;	/*     добавление индекса адресата  	*/
  db->addr  [cur_addr = addr].count_tlph++;	/* инкремент обсл. адресатом телефонов	*/
  if (++db->serv_tlph > (beg_pos_tlph << 1) + 1)/* инкремент обслуженных телефонов	*/
    beg_pos_tlph = (beg_pos_tlph << 1) + 1;
  sign.change = ON;
  return (TRUE);
 }

/*[]****************************************************[]*/
/*[]     Удаление  номера  телефона  из  базы  данных	[]*/
/*[]****************************************************[]*/
void DelNTlph (ulong td)
 {
  uint pos, addr;

  SearchNTlph (td, (uint *) &pos);
  if (!_DL || (td == ULONG_MAX))
   {
    Error (ERRTLPHNO);   return;
   }
  addr = db->tlph [pos].ind_addr;		/*        получение индекса адресата		*/
  memcpy (&db->tlph [pos].ind_addr, &db->tlph [pos+1].ind_addr,
	  sizeof (int) * (&db->tlph [MAX_TLPH-1].ind_addr - &db->tlph [pos].ind_addr));
  setmem (&db->tlph [MAX_TLPH-1], sizeof (db->tlph [0]), UCHAR_MAX);
  setmem (&db->tlph [MAX_TLPH-1].ind_addr, sizeof (db->tlph [0].ind_addr), NULL);
  if (db->addr [addr].count_tlph-- == 1)	/* удаляется последн. номер телефона адресата?	*/
   {
    memcpy (&db->addr [addr], &db->addr [addr+1], (MAX_ADDR - addr) * sizeof (db->addr [0]));
    setmem (&db->addr [MAX_ADDR-1], sizeof (db->addr [0]), NULL);
    for (pos = 0; pos < MAX_TLPH-1; pos++)
     {
      if (db->tlph [pos].ind_addr > addr)  --db->tlph [pos].ind_addr;
     }
    db->serv_addr--;
   }
  if (--db->serv_tlph &&                     	/*        декремент обслуженных телефонов	*/
      (db->serv_tlph <= (beg_pos_tlph >> 1)))
    beg_pos_tlph >>= 1;				/*	    коррекция начальн. указателя	*/
  sign.change = ON;
 }

/*[]****************************************************[]*/
/*[]		Очистка  строки  в  окне		[]*/
/*[]****************************************************[]*/
void ClrStr (uint len, uchar x, uchar y)
 {
  tmp_c = NULL;  ToXY (VP_WORK, len, x, y, &tmp_c);
 }

/*[]****************************************************[]*/
/*[]        Работа  с  телефонной  базой  данных	[]*/
/*[]****************************************************[]*/
void TlphDB (void)
 {
  char far *ptr_tlphdb;

  sign.wnd_tlphdb = ON;
  if (sign.wnd_listtlphdb >= ON)  sign.wnd_listtlphdb+=2;
  ptr_tlphdb = SaveWnd (ARG_WND_TLPHTBL);
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_TLPHTBL);
  ToXY (VP_WORK, LINE, X_MENU_WND+9, Y_MENU_WND+4, " Работа с телефонной базой данных ");
  ToXY (VP_WORK, LINE, X_MENU_WND+3, Y_MENU_WND+5, "Обслуживаемые адресаты                       {F4}");
  ToXY (VP_WORK, LINE, X_MENU_WND+3, Y_MENU_WND+6, "Обслуживаемые телефонные номера              {F5}");
  tmp_c = '├';  ToXY (VP_WORK, SYMB, X_MENU_WND+1, Y_MENU_WND+7, (char *) &tmp_c);
  tmp_c = '─';  ToXY (VP_WORK, 51, X_MENU_WND+2, Y_MENU_WND+7, (char *) &tmp_c);
  tmp_c = '┤';  ToXY (VP_WORK, SYMB, X_MENU_WND+53, Y_MENU_WND+7, (char *) &tmp_c);
  ToXY (VP_WORK, LINE, X_MENU_WND+3, Y_MENU_WND+8, "Поиск номера телефона");
  ToXY (VP_WORK, LINE, X_MENU_WND+3, Y_MENU_WND+9, "Добавить новый номер телефона в базу данных  {F6}");
  ToXY (VP_WORK, LINE, X_MENU_WND+3, Y_MENU_WND+10, "Изменить номер телефона базы данных");
  ToXY (VP_WORK, LINE, X_MENU_WND+3, Y_MENU_WND+11, "Удалить номер телефона из базы данных");
  ToXY (VP_WORK, LINE, X_MENU_WND+3, Y_MENU_WND+12, "Скопировать базу данных                         \x10");
  ToXY (VP_WORK, LINE, X_MENU_WND+3, Y_MENU_WND+13, "Загрузить базу данных c дискового устройства");
  ToXY (VP_WORK, LINE, X_MENU_WND+3, Y_MENU_WND+14, "Инициализировать базу данных");
  PushInf (INFMAINMENU);
  while (VertSelect (X_MENU_WND+2, Y_MENU_WND+5, Y_MENU_WND+5, NULL, PosTlphDBFunc) != UINT_MAX);
  RestoreWnd (ptr_tlphdb, ARG_WND_TLPHTBL);
  PopInf ();
  if (sign.wnd_listtlphdb-2 >= ON)  sign.wnd_listtlphdb-=2;
  sign.wnd_tlphdb = OFF;
 }

/*[]****************************************************[]*/
/*[]      Отработка  позиций  меню  "база  данных".	[]*/
/*[] Дополнительный входной параметр: DL - y-координата	[]*/
/*[]****************************************************[]*/
void PosTlphDBFunc (void)
 {
  switch (_DL)
   {
    case 0:
     ListAddrDB ();
     break;
    case 1:
     ListTlphDB ();
     break;
    case 2:
     FindNTlph (NULL);
     break;
    case ADD:		/*  добавление	*/
    case CHANG:		/*   изменение	*/
    case DEL:		/*   удаление	*/
     AddChangDelNTlph ();
     break;
    case 6:
     OutTlphDB ();
     break;
    case 7:
     if (!Ask (LOAD))  InitTlphDB ();
     break;
    case 8:
     Ask (INIT);
   }
 }

/*[]****************************************************[]*/
/*[]	   	   Вывод  базы  данных			[]*/
/*[]        на  принтер  и  дисковое  устройство	[]*/
/*[]****************************************************[]*/
void OutTlphDB (void)
 {
  char far *ptr_out;

  ptr_out = SaveWnd (ARG_WND_OUTTLPHTBL);
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_OUTTLPHTBL);
  ToXY (VP_WORK, LINE, X_OUTTLPHTBL_WND+2, Y_OUTTLPHTBL_WND+1, "... на текущее дисковое устройство");
  ToXY (VP_WORK, LINE, X_OUTTLPHTBL_WND+2, Y_OUTTLPHTBL_WND+2, "... на принтер");
  PushInf (INFOUT);
  while (VertSelect (X_OUTTLPHTBL_WND+1, Y_OUTTLPHTBL_WND+1, Y_OUTTLPHTBL_WND+1, OutFunc, NULL) != UINT_MAX);
  RestoreWnd (ptr_out, ARG_WND_OUTTLPHTBL);
  PopInf ();
 }

/*[]****************************************************[]*/
/*[]            Инициализация  базы  данных		[]*/
/*[]****************************************************[]*/
void InitTlphDB (void)
 {
  int i;

  PushInf (INFINITDB);
  memset (db, NULL, sizeof (tdb));
  memset (&db->tlph, (int) UCHAR_MAX, sizeof (db->tlph));
  for (i = 0; i < MAX_TLPH; i++)  db->tlph [i].ind_addr = 0;
  db->status.tmp_drive     = ' ';
  db->status.sumaddpay [1] = '.';
  db->status.load_wait = 300;				/*     установка счетчика ожидания получения ЗТВ (time_out)  	*/
							/*         time_out = 15 (сек.) = load_wait * 0,055		*/
  beg_pos_tlph = 1;
  PopInf ();
 }

/*[]****************************************************[]*/
/*[]   Отображение  страниц  телефонной  базы  данных	[]*/
/*[]****************************************************[]*/
void ListTlphDB (void)
 {
  char far *ptr_listtlph, far *ptr_name;
  uint i;

  if (sign.wnd_processout || sign.find)  return;
  if (!sign.wnd_listtlphdb)  sign.wnd_listtlphdb = ON;
  else return;
  ptr_listtlph = SaveWnd (ARG_WND_TLPH);
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_TLPH);
  ToXY (VP_WORK, LINE, X_MENU_WND+20, Y_MENU_WND+4, " База данных телефонных номеров ");
  ToXY (VP_WORK, LINE, X_MENU_WND+4, Y_MENU_WND+5, "Номер телефона  Комната/кабина    Номер телефона  Комната/кабина");
  ptr_name = SaveWnd (ARG_WND_TLPH_NAME);
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_TLPH_NAME);
  ToXY (VP_WORK, LINE, X_MENU_WND+51, Y_MENU_WND-1, " Текущий адресат ");
  ToXY (VP_WORK, LINE, X_MENU_WND+49, Y_MENU_WND+1, "Телетайп:");
  PushInf (INFLISTTLPH);
  do
   {
    i = 0;
    switch (comm)
     {
      case HOME_KEY:
       cur_addr = 0;  cur_pos = 0;
       while (db->addr [cur_addr].Ntty == ULONG_MAX)
	{
	 if (cur_addr >= MAX_ADDR - 1)  break;
	 ++cur_addr;
	}
       break;
      case UP_KEY:
       i = cur_pos;
       if (i)  --i;
       else    break;
       do
	{
	 if (db->tlph [i].ind_addr == cur_addr)		/* провер. номер телефона - текущего адресата?	*/
	  {
	   cur_pos = i;
	   break;
	  }
	}
       while (--i != UINT_MAX);
       break;
      case DOWN_KEY:
       i = cur_pos;
       while (++i <= db->serv_tlph - 1)
	{
	 if (db->tlph [i].ind_addr == cur_addr)		/*          есть телефоны для просмотра?	*/
	  {
	   ++cur_pos;
	   break;
	  }
	}
       break;
      case PG_UP_KEY:
       if (!cur_pos && cur_addr && (db->addr [cur_addr-1].Ntty != ULONG_MAX))
	{
	 cur_pos = db->serv_tlph-1;  --cur_addr;	/*         переход к предыдущему адресату	*/
	}
       while (cur_pos)
	{
	 if (i == TLPHinPAG)  break;
	 if (db->tlph [cur_pos--].ind_addr == cur_addr)  i++;
	}
       break;
      case PG_DN_KEY:
       i = 0;
       while (cur_pos < db->serv_tlph - 1)
	{
	 if (i == TLPHinPAG)  break;			/* заполнено окно достат. колич-вом телефонов?	*/
	 if (db->tlph [cur_pos++].ind_addr == cur_addr)  i++;
	}
       if (i == TLPHinPAG)  break;			/* заполнено окно достат. колич-вом телефонов?	*/
       if (cur_pos == db->serv_tlph - 1)        	/*  последний номер телефона текущего адресата?	*/
	{
	 if (!db->addr [cur_addr+1].Ntty)  break;	/*           все адресаты просмотрены?		*/
	 cur_pos = 0;  ++cur_addr;			/*         переход к следующему адресату	*/
	}
     }
    if (!db->addr [cur_addr].Ntty)  break;
    ListPageTlph ();					/*   отображение телефонов базы данных		*/
   }
  while ((comm = VertSelect (cur_x_pos, Y_MENU_WND+7, cur_y_pos, NULL, PosChangDelTlphFunc)) != UINT_MAX);
  RestoreWnd (ptr_name, ARG_WND_TLPH_NAME);
  RestoreWnd (ptr_listtlph, ARG_WND_TLPH);
  PopInf ();
  sign.wnd_listtlphdb = OFF;
 }

/*[]****************************************************[]*/
/*[]  Функция  изменения / удаления  номера  телефона.	[]*/
/*[] Дополнительный входной параметр: CL - код операции,[]*/
/*[] DH - текущ. x координата, DL - текущ. y координата	[]*/
/*[]****************************************************[]*/
void PosChangDelTlphFunc (void)
 {
  uchar x = _DH, y = _DL + 9, code = _CL;
  char arr_tmp [MAX_STR];
  long lt;

  FromXY (VP_WORK, sizeof (arr_tmp), x, y, (uchar *) &arr_tmp);
  lt = atol ((char *) &arr_tmp);
  SearchNTlph (lt, (uint *) &cur_pos);
  _DL = code;  AddChangDelNTlph ();
 }

/*[]****************************************************[]*/
/*[]  Отображение  одной  страницы  номеров  телефонов	[]*/
/*[]****************************************************[]*/
void ListPageTlph (void)
 {
  char arr_tmp [MAX_STR];
  int i = 0, pos, cur_x, cur_y;

  if (!db->addr [cur_addr].Ntty || sign.find)  return;
  ClrStr (sizeof (db->addr [cur_addr].addr), X_MENU_WND+50, Y_MENU_WND);
  ToXY (VP_WORK, LINE, X_MENU_WND+50, Y_MENU_WND, (char *) &db->addr [cur_addr].addr);
  ClrStr (10, X_MENU_WND+60, Y_MENU_WND+1);
  ltoa (db->addr [cur_addr].Ntty, (char *) &arr_tmp, 10);
  ToXY (VP_WORK, LINE, X_MENU_WND+60, Y_MENU_WND+1, (char *) &arr_tmp);
  if (db->addr [cur_addr].type == '-')  memcpy (&arr_tmp, &arr_type [0], sizeof (arr_type [0]));
  else			                memcpy (&arr_tmp, &arr_type [1], sizeof (arr_type [0]));
  ToXY (VP_WORK, LINE, X_MENU_WND+66, Y_MENU_WND+1, (char *) &arr_tmp);
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_OFF, ARG_WND_TLPH_LEFT);
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_OFF, ARG_WND_TLPH_RIGHT);
  pos = cur_pos;	/* получение текущей позиции	*/
  cur_x = X_MENU_WND+7;  cur_y = Y_MENU_WND+7;
  while (TRUE)
   {
    if (i == TLPHinPAG / 2)
     {
      cur_x = X_MENU_WND+42;  cur_y = Y_MENU_WND+7;
     }
    if ((i == TLPHinPAG)  ||  (db->tlph [pos].Ntlph == ULONG_MAX))  break;
    if (db->tlph [pos].ind_addr == cur_addr)
     {
      ltoa (db->tlph [pos].Ntlph, (char *) &arr_tmp, 10);
      ToXY (VP_WORK, LINE, cur_x, cur_y, (char *) &arr_tmp);
      ltoa (db->tlph [pos].Nroom, (char *) &arr_tmp, 10);
      ToXY (VP_WORK, LINE, cur_x+18, cur_y, (char *) &arr_tmp);
      cur_y++;
      i++;
     }
    pos++;
   }
 }

/*[]****************************************************[]*/
/*[]    Просмотр  имеющихся  адресатов  базы  данных	[]*/
/*[]****************************************************[]*/
void ListAddrDB (void)
 {
  ulong tty;
  uint y = Y_MENU_WND+7, i = 0, j;
  char type, arr_tmp [MAX_STR],
       far *ptr_listaddr, far *ptr_ttl;

  if (sign.wnd_processout)  return;
  if (sign.wnd_listaddrdb || sign.wnd_processout)  return;
  sign.wnd_listaddrdb = ON;
  if (sign.wnd_listtlphdb >= ON)  sign.wnd_listtlphdb+=2;
  ptr_listaddr = SaveWnd (ARG_WND_ADDR);
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_ADDR);
  ToXY (VP_WORK, LINE, X_MENU_WND+24, Y_MENU_WND+4, " Обслуживаемые адресаты ");
  ToXY (VP_WORK, LINE, X_MENU_WND+8, Y_MENU_WND+5, "Город              Адресат      Телетайп    Сеть  Телефонов");
  ptr_ttl = SaveWnd (ARG_WND_TOTAL);
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_TOTAL);
  ToXY (VP_WORK, LINE, X_MENU_WND+57, Y_MENU_WND-1, " Итого ");
  ToXY (VP_WORK, LINE, X_MENU_WND+49, Y_MENU_WND, "Всего адресатов:");
  ToXY (VP_WORK, LINE, X_MENU_WND+49, Y_MENU_WND+1, "Всего телефонов:");
  PushInf (INFLISTADDR);
  do
   {
    switch (i)
     {
      case SPACE_KEY:
       y = Y_MENU_WND + 7 + _DL;
       FromXY (VP_WORK, sizeof (arr_tmp), X_MENU_WND+41, y, (uchar *) &arr_tmp);
       if (!arr_tmp [0])  break;
       tty = atol ((uchar *) &arr_tmp);
       type = FromXY (VP_WORK, SYMB, X_MENU_WND+53, y, NULL);
       type = (type == 'А') ? '-' : NULL;
       j = UINT_MAX;
       while (++j < MAX_ADDR)
	{
	 if (!db->addr [j].Ntty)  break;
	 if ((tty == db->addr [j].Ntty) && (type == db->addr [j].type))
	  {
	   cur_addr = j;  cur_pos = 0;		/*  переход к зафиксированному адресату	*/
	   if (!sign.wnd_listtlphdb)  ListTlphDB ();
	  }
	}
       break;
      case UP_KEY:
       if (addr)  --addr;
       y = Y_MENU_WND+7;
       break;
      case DOWN_KEY:
       while (TRUE)
	{
	 ++addr;
	 if ((addr == MAX_ADDR) || !db->addr [addr].Ntty)
	  {
	   --addr;  break;
	  }
	 if (db->addr [addr].Ntty == UINT_MAX)  continue;
	 break;
	}
       y = Y_MENU_WND+14;
       break;
      default:
       addr = 0;
     }
    ListPageAddr ();
   }
  while ((i = VertSelect (X_MENU_WND+3, Y_MENU_WND+7, y, NULL, PosAddrDBFunc)) != UINT_MAX);
  RestoreWnd (ptr_ttl, ARG_WND_TOTAL);
  RestoreWnd (ptr_listaddr, ARG_WND_ADDR);
  PopInf ();
  if (sign.wnd_listtlphdb-2 >= ON)  sign.wnd_listtlphdb-=2;
  sign.wnd_listaddrdb = OFF;
 }

/*[]****************************************************[]*/
/*[]    Отработка  позиций  меню  "общая информация"	[]*/
/*[] Дополнительный входной параметр: DL - y-координата	[]*/
/*[]****************************************************[]*/
void PosAddrDBFunc (void)
 {
  ulong old_tty, new_tty;
  uint j, pos_tty;
  uchar i, type, old_type, y = _DL + 9, arr_tmp [3] [MAX_STR],
	far *ptr_change, far *ptr_inp;

  if (sign.wnd_processout)  return;
  ptr_inp = SaveWnd (ARG_WND_INP);
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_INP);
  ptr_change = SaveWnd (ARG_WND_CHANGEADDR);
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_CHANGEADDR);
  ToXY (VP_WORK, LINE, X_MENU_WND+44, Y_MENU_WND-1, " Изменяемые данные ");
  ToXY (VP_WORK, LINE, X_MENU_WND+42, Y_MENU_WND, "Город:");
  ToXY (VP_WORK, LINE, X_MENU_WND+40, Y_MENU_WND+1, "Адресат:");
  ToXY (VP_WORK, LINE, X_MENU_WND+39, Y_MENU_WND+2, "Телетайп:");
  FromXY (VP_WORK, sizeof (arr_tmp [0]), X_MENU_WND+41, y, (uchar *) &arr_tmp [2]);
  old_tty  = atol ((uchar *) &arr_tmp [2]);
  if (!old_tty)							/*      возможны изменения?	*/
   {
    RestoreWnd (ptr_inp, ARG_WND_INP);
    RestoreWnd (ptr_change, ARG_WND_CHANGEADDR);
    return;
   }
  old_type = FromXY (VP_WORK, SYMB, X_MENU_WND+53, y, NULL);
  old_type = (old_type == 'А') ? '-' : NULL;
  j = 0;
  do
   {
    if (!db->addr [j].Ntty)  break;
    if ((old_tty == db->addr [j].Ntty) && (old_type == db->addr [j].type))
      {
       memcpy (&arr_tmp [0], (uchar *) &db->addr [j].town, sizeof (db->addr [0].town));
       memcpy (&arr_tmp [1], (uchar *) &db->addr [j].addr, sizeof (db->addr [0].addr));
       break;
      }
   }
  while (db->addr [++j].Ntty);
  ToXY (VP_WORK, LINE, X_MENU_WND+49, Y_MENU_WND,   (uchar *) &arr_tmp [0]);
  ToXY (VP_WORK, LINE, X_MENU_WND+49, Y_MENU_WND+1, (uchar *) &arr_tmp [1]);
  ToXY (VP_WORK, LINE, X_MENU_WND+49, Y_MENU_WND+2, (uchar *) &arr_tmp [2]);
  ToXY (VP_WORK, SYMB, X_MENU_WND+55, Y_MENU_WND+2, (uchar *) &old_type);
  i = 0;
  while (i < 3)
   {
    ToXY (VP_WORK, LINE, X_MENU_WND+2, Y_MENU_WND, (uchar *) arr_inf [i]);
    PushInf (i+10);
    memset ((char *) &arr_tmp [i], (uchar) NULL, sizeof (arr_tmp [0]));
    ClrStr (sizeof (arr_tmp [0]), X_MENU_WND+2, Y_MENU_WND+1);
    j = EditStr ((char *) &arr_tmp [i], X_MENU_WND+2, Y_MENU_WND+1);
    PopInf ();
    if (sign.exit)						/* установлен признак возврата	*/
     {
      RestoreWnd (ptr_inp, ARG_WND_INP);
      RestoreWnd (ptr_change, ARG_WND_CHANGEADDR);
      return;
     }
    if (j == ESC_KEY)  break;
    ClrStr (sizeof (arr_tmp [0]), X_MENU_WND+49, Y_MENU_WND+i);
    ToXY (VP_WORK, LINE, X_MENU_WND+49, Y_MENU_WND+i, (uchar *) &arr_tmp [i]);
    if (j == TAB_KEY)				/*   переход к следующему полю		*/
     {
      i = (!i) ? 2 : --i;
     }
    else  ++i;
   }
  if (j != ESC_KEY)				/*          проводить замену?		*/
   {
    i = OFF;
    for (j = 0; j < 6; j++)			/*      корректный номер телетайпа?	*/
     {
      if  (arr_tmp [2] [j] > '9')  i = ON;
      if (!arr_tmp [2] [j])        i = ON;
     }
    switch (arr_tmp [2] [6])
     {
      case '-':
      case NULL:
       type = arr_tmp [2] [6];
       break;
      default:
       i = ON;
     }
    if (i)  Error (ERRINP);
    else
     {
      new_tty = atol ((uchar *) &arr_tmp [2]);
      pos_tty = UINT_MAX;  i = OFF;
      while (++j <= MAX_ADDR-1)			/* проверка на наличие вводимого номера	*/
       {
	if ((new_tty == db->addr [pos_tty].Ntty) && (type == db->addr [pos_tty].type))
	 {
	  i = ON;  break;
	 }
	if (!db->addr [pos_tty].Ntty)  break;
       }
      if (i)  Error (ERRTTYYES);
      else
       {
	pos_tty = UINT_MAX;  i = OFF;
	while (++pos_tty <= MAX_ADDR-1)
	 {
	  if ((old_tty == db->addr [pos_tty].Ntty) && (old_type == db->addr [pos_tty].type))
	   {
	    j = pos_tty;  ++i;
	   }
	  if (!db->addr [pos_tty].Ntty)  break;
	 }
	if (i == 1)
	 {
	  db->addr [j].Ntty = new_tty;
	  db->addr [j].type = type;
	  memcpy ((char *) &db->addr [j].addr, (char *) &arr_tmp [1], sizeof (arr_tmp [0]));
	  memcpy ((char *) &db->addr [j].town, (char *) &arr_tmp [0], sizeof (arr_tmp [0]));
	  sign.change = ON;
	 }
	else  Error (ERRINP);
       }
     }
   }
  RestoreWnd (ptr_change, ARG_WND_CHANGEADDR);
  RestoreWnd (ptr_inp, ARG_WND_INP);
 }

/*[]****************************************************[]*/
/*[]    	 Отобразить  окно  адресатов		[]*/
/*[]****************************************************[]*/
void ListPageAddr (void)
 {
  char arr_tmp [MAX_STR];
  int cur_addr, y;

  if (!sign.wnd_listaddrdb)  return;
  itoa (db->serv_addr, (char *) &arr_tmp, 10);
  ToXY (VP_WORK, LINE, X_MENU_WND+68, Y_MENU_WND, (char *) &arr_tmp);
  itoa (db->serv_tlph, (char *) &arr_tmp, 10);
  ToXY (VP_WORK, LINE, X_MENU_WND+67, Y_MENU_WND+1, (char *) &arr_tmp);
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_OFF, ARG_WND_ADDR1);
  cur_addr = addr;  y = Y_MENU_WND+7;
  do
   {
    if (!db->addr [cur_addr].Ntty)  continue;
    memcpy (&arr_tmp [0], (char *) &db->addr [cur_addr].town, 15);  arr_tmp [15] = NULL;
    ToXY (VP_WORK, LINE, X_MENU_WND+4, y, (char *) &arr_tmp);
    memcpy (&arr_tmp [0], (char *) &db->addr [cur_addr].addr, 15);  arr_tmp [15] = NULL;
    ToXY (VP_WORK, LINE, X_MENU_WND+23, y, (char *) &arr_tmp);
    ltoa (db->addr [cur_addr].Ntty, (char *) &arr_tmp, 10);
    ToXY (VP_WORK, LINE, X_MENU_WND+41, y, (char *) &arr_tmp);
    if (db->addr [cur_addr].type == '-')  memcpy (&arr_tmp, &arr_type [0], sizeof (arr_type [0]));
    else			          memcpy (&arr_tmp, &arr_type [1], sizeof (arr_type [0]));
    ToXY (VP_WORK, LINE, X_MENU_WND+52, y, (char *) &arr_tmp);
    itoa (db->addr [cur_addr].count_tlph, (char *) &arr_tmp, 10);
    ToXY (VP_WORK, LINE, X_MENU_WND+62, y, (char *) &arr_tmp);
    FromXY (VP_WORK, SYMB, X_MENU_WND+2, ++y, NULL);
    if (_AL == (uchar) '└')  break;
   }
  while (db->addr [++cur_addr].Ntty);
 }

/*[]****************************************************[]*/
/*[]  	     Вывод  базы  данных  на  принтер		[]*/
/*[]****************************************************[]*/
void PrnTlphDB (void)
 {
  char arr_tmp [50], count_lines = 11;
  register uint i = 0, j;

  PushInf (INFPRINT);
  j = inp (0x379);				/*        чтение статуса принтера	*/
  if (!(j & BIT_PRN_BUSY) || !(j & BIT_PRN_ERR)	/* ошибка принтера, занято, нет бумаги?	*/
      || (j & BIT_PRN_PE))
   {
    Error (ERRPRN);  PopInf ();  return;
   }
  write (STDPRN, MSGTITLEPRN , sizeof (MSGTITLEPRN));
  write (STDPRN, MSGTOTALADDR , sizeof (MSGTOTALADDR));
  memset (&arr_tmp, NULL, sizeof (arr_tmp));
  itoa (db->serv_addr, (char *) &arr_tmp, 10);
  write (STDPRN, (char *) &arr_tmp, sizeof (arr_tmp));
  write (STDPRN, MSGTOTALTLPH, sizeof (MSGTOTALTLPH));
  memset (&arr_tmp, NULL, sizeof (arr_tmp));
  itoa (db->serv_tlph, (char *) &arr_tmp, 10);
  write (STDPRN, (char *) &arr_tmp, sizeof (arr_tmp));
  do
   {
    if (!db->addr [i].Ntty)  break;
    write (STDPRN, MSGTOWN, sizeof (MSGTOWN));
    write (STDPRN, (char *) &db->addr [i].town, sizeof (db->addr [0].town));
    write (STDPRN, MSGADDR, sizeof (MSGADDR));
    write (STDPRN, (char *) &db->addr [i].addr, sizeof (db->addr [0].addr));
    write (STDPRN, MSGTTY , sizeof (MSGTTY));
    memset (&arr_tmp, NULL, sizeof (arr_tmp));
    ltoa (db->addr [i].Ntty, (char *) &arr_tmp, 10);
    write (STDPRN, (char *) &arr_tmp, sizeof (arr_tmp));
    if (db->addr [i].type == '-')  write (STDPRN, (char *) &arr_type [0], sizeof (arr_type [0]));
    else			   write (STDPRN, (char *) &arr_type [1], sizeof (arr_type [0]));
    write (STDPRN, MSGTLPH, sizeof (MSGTLPH));
    memset (&arr_tmp, NULL, sizeof (arr_tmp));
    ltoa (db->addr [i].count_tlph, (char *) &arr_tmp, 10);
    write (STDPRN, (char *) &arr_tmp, sizeof (arr_tmp));
    write (STDPRN, MSGTLPHROOM, sizeof (MSGTLPHROOM));
    memset (&arr_tmp, ' ', sizeof (arr_tmp));
    count_lines+=4;  sign.next_line = ON;  j = 0;
    do
     {
      if (db->tlph [j].ind_addr == i)
       {
	if ((sign.next_line = !sign.next_line) != 0)	/* mikle */
	 {
	  ltoa (db->tlph [j].Ntlph, (char *) &arr_tmp [32], 10);
	  ltoa (db->tlph [j].Nroom, (char *) &arr_tmp [47], 10);
	  write (STDPRN, (char *) &arr_tmp, sizeof (arr_tmp));
	  write (STDPRN, "\n", 1);
	  memset (&arr_tmp, ' ', sizeof (arr_tmp));
	  if (count_lines++ >= MAX_PRN_LINES)		/*   отпечатано MAX_PRN_LINES строчек?	*/
	   {
	    count_lines = 1;
	    write (STDPRN, "\f\n", 2);
	    write (STDPRN, MSGTLPHROOM, sizeof (MSGTLPHROOM));
	   }
	 }
	else
	 {
	  ltoa (db->tlph [j].Ntlph, (char *) &arr_tmp [0], 10);
	  ltoa (db->tlph [j].Nroom, (char *) &arr_tmp [15], 10);
	 }
       }
     }
    while (++j != db->serv_tlph);
    if (!sign.next_line)
     {
      write (STDPRN, (char *) &arr_tmp, sizeof (arr_tmp));
     }
   }
  while (++i < MAX_ADDR);
  write (STDPRN, "\n\f", 2);
  PopInf ();
 }

/*[]****************************************************[]*/
/*[]         Загрузка  телефонной  базы  данных		[]*/
/*[]              с  дискового  устройства		[]*/
/*[]****************************************************[]*/
void LoadTlphDB (void)
 {
  uint i, j;

  InitTlphDB ();
  PushInf (INFLOADDB);
  _AH = 0x0e;  _DL = cur_drive;  Intr (0x21);		/* установка текущего устройства	*/
  if ((fl = open (FILE_TDB, O_RDWR | O_BINARY)) == -1)
   {
    Error (ERRLOAD);  PopInf ();  _DL = FALSE;  return;
   }
  memset (&name, (int) NULL, sizeof (name));
  read (fl, name, sizeof (MSGTLPHTBL));
  if (memcmp (name, MSGTLPHTBL, sizeof (MSGTLPHTBL)))
   {
    close (fl);  Error (ERRLOAD);  PopInf ();  _DL = FALSE;  return;
   }
  read (fl, (char *) db, sizeof (db->num_vers) + sizeof (db->serv_tlph) + sizeof (db->serv_addr));

  if ((db->num_vers == NUM_VERS) ||		/*             проверка номера версии		*/
      (db->num_vers == NUM_VERS - 1));
  else
   {
    Error (ERRCHECK);  PopInf ();  _DL = FALSE;  return;
   }
  if ((db->serv_tlph > MAX_TLPH) || (db->serv_addr > MAX_ADDR))
   {
    Error (ERRCHECK);  PopInf ();  _DL = FALSE;  return;
   }
  read (fl, (char *) &db->addr [0].Ntty, db->serv_addr * sizeof (db->addr [0]));
  read (fl, (char *) &db->rsrv2, sizeof (db->rsrv2) + db->serv_tlph * sizeof (db->tlph [0]));
  read (fl, (char *) &db->rsrv4, sizeof (db->rsrv4) +
	sizeof (db->status) + sizeof (db->rsrv5) + sizeof (db->check_sum));
  j = 0;  tmp_ptr = (char *) db;
  for (i = 0; i < (sizeof (tdb) - sizeof (db->check_sum)); i++)  j ^= *tmp_ptr++;
  if (j != db->check_sum)
   {
    Error (ERRCHECK);  PopInf ();  _DL = FALSE;  return;
   }
  close (fl);
  if (sign.errinsstub)  { db->status.chann = OFF;  db->status.speed = 0; }
  beg_pos_tlph = 1;
  do						/* вычисление нач. позиции для поиска телефонов	*/
   {
    beg_pos_tlph = (beg_pos_tlph << 1) + 1;
   }
  while (beg_pos_tlph < db->serv_tlph);
  beg_pos_tlph >>= 1;
  sign.change = OFF;
  cur_pos = cur_addr = 0;
  PopInf ();
  _DL = TRUE;
 }

/*[]****************************************************[]*/
/*[]          Запись  телефонной  базы  данных		[]*/
/*[]              на  дисковое  устройство		[]*/
/*[]****************************************************[]*/
void SaveTlphDB (void)
 {
  uint i;

  PushInf (INFSAVEDB);
  _AH = 0x0e;  _DL = cur_drive;  Intr (0x21);	/* установка текущего устройства	*/
  if (!access (FILE_TDB, 0))
   {
    if (!access (FILE_OLD, 0))  unlink (FILE_OLD);
    if (rename (FILE_TDB, FILE_OLD))
     {
      Error (ERRSAVE);  PopInf ();  return;
     }
   }
  if ((fs = open (FILE_TDB, O_RDWR | O_TRUNC | O_CREAT | O_BINARY, S_IREAD | S_IWRITE)) == -1)
   {
    Error (ERRSAVE);  PopInf ();  return;
   }
  db->check_sum = 0;  tmp_ptr = (char *) db;
  db->num_vers = NUM_VERS;
  for (i = 0; i < (sizeof (tdb) - sizeof (db->check_sum)); i++)  db->check_sum ^= *tmp_ptr++;
  write (fs, MSGTLPHTBL, sizeof (MSGTLPHTBL));
  write (fs, (char *) db, sizeof (db->num_vers) + sizeof (db->serv_addr) + sizeof (db->serv_tlph) +
	 db->serv_addr * sizeof (db->addr [0]));
  write (fs, (char *) &db->rsrv2, sizeof (db->rsrv2) + db->serv_tlph * sizeof (db->tlph [0]));
  write (fs, (char *) &db->rsrv4, sizeof (db->rsrv4) + sizeof (db->status) +
	 sizeof (db->rsrv5) + sizeof (db->check_sum));
  close (fs);
  PopInf ();
  sign.change = OFF;						/* сброс признака обновления		*/
 }
