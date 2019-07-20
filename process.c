/*[]****************************************************[]*/
/*[]  	    Междугородные  телефонные  разговоры	[]*/
/*[]			 Версия  2.5			[]*/
/*[]							[]*/
/*[]      Process.c:  модуль  обработки  информации	[]*/
/*[]****************************************************[]*/

/*[]****************************************************[]*/
/*[]          Прием  информации  с  адаптера,		[]*/
/*[]       ее  обработка  и  передача  адресатам	[]*/
/*[]****************************************************[]*/
void ProcessOut (void)
 {
  ulong diskfree;
  uint a, b, c;
  uchar i, arr_tmp [MAX_STR], arr_yesno [] [4] = {"Да", "Нет"};
  uchar far *ptr_processout, far *ptr_tmp;

  if (sign.wnd_processout)  return;
  if (db->status.chann)  outp (0x3f9, 0x37);
  sign.ctrl_end_key = sign.out_prn = OFF;
  count_mess   = 1;
  count_ch     = count_dst     =				/*               инициализация указателей и счетчиков		*/
  head_tmp_inf = tail_tmp_inf  =
  tail_dst     = head_dst      =
  tail_ch      = head_ch       =
  count_wait   = count_tmp_inf =
  flag_send    = count_dig     =
  kto          = pns           =
  pos_buff     = hopping       = 0;
  time_rest    = 0;						/*          mikle: время отсутствия передачи ( для Караганды )	*/
  ptr_processout = SaveWnd (ARG_WND_TREATOUT);
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_TREATOUT);
  ToXY (VP_WORK, LINE, X_TREATOUT_WND, Y_TREATOUT_WND,    "┌─────┬───────┬─────────┬─────────┬───┬──┬───┬────┬──────┬─────┐");
  ToXY (VP_WORK, LINE, X_TREATOUT_WND+2, Y_TREATOUT_WND+1,  "Кат.│  Код  │  Номер  │  Номер  │Число │Мин│    │Сумма │ Кат.");
  ToXY (VP_WORK, LINE, X_TREATOUT_WND+1, Y_TREATOUT_WND+2, "абон.│гор. Б │ абон. Б │ абон. А │ Месяц│   │    │ платы│ выз.");
  ToXY (VP_WORK, LINE, X_TREATOUT_WND, Y_TREATOUT_WND+3,  "├─────┴───────┴─────────┴─────────┴───┴──┴───┴────┴──────┴─────┤");
  ToXY (VP_WORK, LINE, X_TREATOUT_WND+15, Y_TREATOUT_WND+4, "Поступающие телефонные номера");
  ToXY (VP_WORK, LINE, X_TREATOUT_WND+16, Y_TREATOUT_WND+10, "Поступающие номера адресатов");
  db->status.flop ? (i = 5) : (i = 4);
  ptr_tmp = SaveWnd (ARG_WND_STAT_OUT);
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_STAT_OUT);
  ToXY (VP_WORK, LINE, X_STAT_OUT_WND+3, Y_STAT_OUT_WND, " Назначенные устройства вывода информации ");
  ToXY (VP_WORK, LINE, X_STAT_OUT_WND+21, Y_STAT_OUT_WND+1, "Канал связи:");
  ToXY (VP_WORK, LINE, X_STAT_OUT_WND+25, Y_STAT_OUT_WND+2, "Принтер:");
  ToXY (VP_WORK, LINE, X_STAT_OUT_WND+13, Y_STAT_OUT_WND+3, "Дисковое устройство:");
  (db->status.chann) ? (tmp_c = 0) : (tmp_c = 1);
  ToXY (VP_WORK, LINE, X_STAT_OUT_WND+40, Y_STAT_OUT_WND+1, (char *) &arr_yesno [tmp_c]);
  (db->status.prn)   ? (tmp_c = 0) : (tmp_c = 1);
  ToXY (VP_WORK, LINE, X_STAT_OUT_WND+40, Y_STAT_OUT_WND+2, (char *) &arr_yesno [tmp_c]);
  if (db->status.flop)
   {
    ToXY (VP_WORK, SYMB, X_STAT_OUT_WND+40, Y_STAT_OUT_WND+3, (char *) &db->status.tmp_drive);
    ToXY (VP_WORK, LINE, X_STAT_OUT_WND+4, Y_STAT_OUT_WND+4, "Свободного места на диске, K:");
    _DL = db->status.tmp_drive - 'A' + 1;
    _AH = 0x36;  Intr (0x21);					/*        информация о состоянии дискового устрройства		*/
    a = _AX;  b = _BX;  c = _CX;
    diskfree = ((ulong) a * (ulong) b * (ulong) c / 1024L);	/*        вычисление свободного дискового пространства		*/
    ltoa (diskfree, (char *) &arr_tmp, 10);
    ToXY (VP_WORK, LINE, X_STAT_OUT_WND+40, Y_STAT_OUT_WND+4, (char *) &arr_tmp);
   }
  else  ToXY (VP_WORK, LINE, X_STAT_OUT_WND+40, Y_STAT_OUT_WND+3, (char *) &arr_yesno [1]);
  PushInf (INFANYKEY);
  GetKey (NO_KEYS);
  RestoreWnd (ptr_tmp, ARG_WND_STAT_OUT);
  PopInf ();
  sleep (1);

#ifdef WORK_WITH_ADAP

  TestAdap ();							/* 			тестирование адаптера			*/
  if (!_DL)
   {
    RestoreWnd (ptr_processout, ARG_WND_TREATOUT);
    outp (0x3f9, 0x35);  return;
   }

#endif

  if (db->status.prn)
   {
    a = inp (0x379);						/*        	     чтение статуса принтера			*/
    if (!(a & BIT_PRN_BUSY) || !(a & BIT_PRN_ERR)		/* 	     ошибка принтера, занято, нет бумаги?		*/
	|| (a & BIT_PRN_PE))
     {
      Error (ERRPRN);  DelAssign (PRN);
     }
   }
  if (db->status.chann &&
      (((sign.alien = inp (0x3fa)) & (BIT_CTS || BIT_DCD)) ||	/* проверка 106 и 109 линий : наблюдается при уходе АПК в отбой */
       (inp (0x3f9) & BIT_GAP)))				/*	  разрыв последовательности передаваемых кодов?		*/
   {								/*        наблюдается при неподключении модема к линии		*/
    Error (ERROFFLINE);
    RestoreWnd (ptr_processout, ARG_WND_TREATOUT);
    sign.wnd_processout = OFF;
    outp (0x3f9, 0x35);  return;
   }
  if (db->status.flop)
   {
    _DL = db->status.tmp_drive - 'A';  _AH = 0x0e;  Intr (0x21);/*                  установка устройства 			*/
    if (!access (FILE_INF, 0))					/*                     доступ имеется				*/
     {
      if (Ask (SEND) == ESC_KEY)  DelAssign (FLOP);
     }
    if (db->status.flop && ((tmp_f = open (FILE_INF, O_CREAT | O_BINARY, S_IREAD | S_IWRITE)) == -1))
     {
      Error (ERRINF);  DelAssign (FLOP);
     }
    else write (tmp_f, MSGTMPINF, sizeof (MSGTMPINF));
   }
  sign.wnd_processout = ON;
  ToXY (VP_WAIT, 80 * 25 * 2, 0, 0, NULL);			/*                   очистка окна ожидания			*/
  cur_attr |= BLINK_ATTR;
  ToXY (VP_WAIT, LINE, 33, 8, "T  L  T");
  ToXY (VP_WAIT, LINE, 25, 10, "Обработка информации ...");
  ToXY (VP_WAIT, LINE, 24, 14, "Нажмите любую клавишу ...");
  cur_attr &= ~BLINK_ATTR;
  CursorXY (WAIT_WND, 0, INVISIBLE);
  ptr_tmp = SaveWnd (ARG_WND_OPER);
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_OPER);
  PushInf (INFENDTREAT);
  memset (&ptr_dst->msg,             NULL, sizeof (get_inf));
  memset (&ptr_ch->inf [0].Ntty [0], NULL, sizeof (ch_s));
  ClearTotal ();
  if (db->status.categ)  FindFunc = CategFunc;			/*              поиск телефона по категории вызова?		*/
  else		         FindFunc = BinarySearchFunc;
  Old_0x0c = getvect (0x0c);
  if (db->status.chann)
   {
    setvect (0x0c, SendChan);
    permit = (uchar) ((sign.alien & BIT_ALIEN) >> 2);
    (void) inp (0x3f8);						/*            		сброс RxRDY				*/
    outp (0x21, (inp (0x21) & 0xef));				/*            разрешение IRQ 4 для контроллера 8259		*/
    if (SetComm ())
     {
      ExitProcessOut (ptr_tmp, ptr_processout);  return;
     }
   }
  Old_0x08 = getvect (8);  setvect (8, InpAdap);
  outp (0x40, 0xff);  outp (0x40, 0x7f);			/* 	   8253, канал 0: прерывания от таймера 27.5 мсек	*/
  db->status.load_wait <<= 1;					/*  коррекция time_out, в связи с перепрограммированием таймера	*/
  i = WAIT_WND;
  outp (0x3fe, 0);  Delay3Nop;  outp (0x3fe, 3);
/*==============================================================*/
  do								/*                ожидание нажатия клавиши Ctrl+End		*/
   {
    if (count_tmp_inf)    PrepareUpperWnd ();			/*       имеется информация для выдачи в верхнее окно?		*/
    if (db->status.chann) ReceiveMain ();			/*		  разрешена передача в канал?			*/
    if (count_dst)        ProcessOutRec ();			/*     обработать и передать на диск и принтер одну запись	*/
    if (*ptr_shift_keys & ALT_KEY_POS)  AddProcessMenu ();	/*    нажата Alt: отобразить дополнительное меню обработки?	*/
    if (sign.kbhit)						/*                   была нажата любая клавиша?			*/
     {
      i = (i == WAIT_WND) ? WORK_WND : WAIT_WND;
      SetVideoPage (i);
      *(ptr_kbd_head + 1) = *ptr_kbd_head;			/*                  обнуление буфера клавиатуры			*/
      sign.kbhit = OFF;
     }
   }
  while (!sign.ctrl_end_key);
/*==============================================================*/
  setvect (8, Old_0x08);					/*                  установка старого вектора 8h		*/
  outp (0x40, 0);  outp (0x40, 0);				/*  8253, канал 0: стандартные прерывания от таймера (55 мсек)	*/
  db->status.load_wait >>= 1;					/*     	            обратная коррекция time_out	 		*/
  SetVideoPage (WORK_WND);
  *(ptr_kbd_head + 1) = *ptr_kbd_head;				/*                  обнуление буфера клавиатуры			*/
  while (count_dst)  ProcessOutRec ();
  outp (0x3f9, 0x35);

/*
  if (db->status.chann && !hopping)
   {
    while (count_ch)						/* 	   передача в канал всей оставшейся информации		*/
     {
      if (inp (0x3fa) & BIT_CTS)				/*		   106: готов модем к передаче?			*/
       { Error (ERROFFLINE);  break; }
      if ((inp(0x3fe) == 0x11) && (!pos_buff))			/* 	     разрешена передача и буфер передачи пустой      	*/
       {
	outp (0x3fe, 0); Delay3Nop; outp (0x3fe, 1);		/*   "зацепка" для извлечения после перерыва записи из arr_ch	*/
       }
      if ((inp(0x3fe) == 0x22) && (!count_wait))		/*              истекло время выделеное на прием ЗТВ ?		*/
       { Error (ERROFFLINE);  break; }
      delay(50); count_wait = --count_wait;
     }
   }
*/

  outp (0x21, (inp (0x21) | 0x10));				/*              	  запрещение IRQ 4			*/
  setvect (0x0c, Old_0x0c);
  if (db->status.flop)  close (tmp_f);
  RestoreWnd (ptr_tmp, ARG_WND_OPER);
  RestoreWnd (ptr_processout, ARG_WND_TREATOUT);
  PopInf ();
  sign.wnd_processout = OFF;
 }

/*[]****************************************************[]*/
/*[]****************************************************[]*/
uchar SetComm (void)
 {
  uchar i = 0;

  pos_buff = messKG_0;
  outp (0x3fe, 0);  outp (0x3fe, 3);				/* 			разрешить передачу и прием		*/
  while (count_dig != 5)
   {
    sleep (5);
    if (i++ == 6)  return (0xff);
   }
  if (memcmp (&origSI1_3_13, &buff_msg, 7))  return (0xff);
  GetLastNum ();
  flag_send = 0;  count_dig = 0;
  pos_buff = messKG_1;  i = 0;
  outp (0x3fe, 0);  outp (0x3fe, 3);				/* 			разрешить передачу и прием		*/
  while (!flag_send)
   {
    sleep (5);
    if ((inp (0x3fe) == 0x22) && !flag_send)
     {
      pos_buff = messKG_1;  i = 0;
      outp (0x3fe, 0);  outp (0x3fe, 3);
     }
    if (i++ == 6)  return (0xff);
   }
  i = 0;
  while (count_dig != 5)
   {
    sleep (5);
    if (i++ == 6)  return (0xff);
   }
  flag_send = 0;  count_dig = 0;
  if (memcmp (&origSI0, &buff_msg, 7))  return (0xff);
  return (0);
 }

/*[]****************************************************[]*/
/*[]****************************************************[]*/
void ExitProcessOut (uchar far *ptr_tmp, uchar far * ptr_processout)
 {
  outp (0x21, (inp (0x21) | 0x10));			/*              	  запрещение IRQ 4			*/
  outp (0x3f9, 0x35);
  setvect (0x0c, Old_0x0c);
  Error (ERRNOCOMM);
  RestoreWnd (ptr_tmp, ARG_WND_OPER);
  RestoreWnd (ptr_processout, ARG_WND_TREATOUT);
  PopInf ();
  sign.wnd_processout = OFF;
 }

/*[]****************************************************[]*/
/*[]****************************************************[]*/
void GetLastNum (void)
 {
  static uchar mtk_koi [] [3] =
   {
    {0x00,0x00,0x00}, {0x45,0x45,0x33}, {0x00,0x00,0x00}, {0x41,0x41,0x2d},
    {0x20,0x20,0x20}, {0x53,0x91,0x22}, {0x49,0x88,0x38}, {0x55,0x93,0x37},
    {0x0d,0x0d,0x0d}, {0x44,0x84,0x23}, {0x52,0x90,0x34}, {0x4a,0x89,0x9e},
    {0x4e,0x8d,0x2c}, {0x46,0x94,0x9d}, {0x43,0x96,0x3a}, {0x4b,0x4b,0x28},
    {0x54,0x54,0x35}, {0x5a,0x87,0x2b}, {0x4c,0x8b,0x29}, {0x57,0x82,0x32},
    {0x48,0x95,0x99}, {0x59,0x9b,0x36}, {0x50,0x8f,0x30}, {0x51,0x9f,0x31},
    {0x4f,0x4f,0x39}, {0x42,0x81,0x3f}, {0x47,0x83,0x98}, {0x00,0x00,0x00},
    {0x4d,0x4d,0x2e}, {0x58,0x9c,0x2f}, {0x56,0x86,0x3d}
   };
  uchar i = 0;

  do
   {
    last_num [i] = mtk_koi [buff_msg [i + 9]] [2];
   }
  while (++i != 3);
  count_mess = ((uint) (last_num [0] & 0x0f) << 8);
  count_mess = count_mess + (uint) ((last_num [1] & 0x0f) << 4);
  count_mess = count_mess + (uint) (last_num [2] & 0x0f);
 }

/*[]****************************************************[]*/
/*[]    Скроллинг  и  выдача  записи  в  верхнее  окно  []*/
/*[]****************************************************[]*/
void PrepareUpperWnd (void)
 {
  ScrollWnd (Y_MENU_WND + 5);	/*      скроллинг окна информации приним. номеров телефонов	*/
  ToXY (VP_WORK, LINE, X_MENU_WND+7, Y_MENU_WND+9, (uchar *) &arr_tmp_inf [tail_tmp_inf]);
  count_tmp_inf--;
  if (++tail_tmp_inf == MAX_TMP_INF)  tail_tmp_inf = 0;
 }

/*[]****************************************************[]*/
/*[]	  Работа  с  каналом  в  основном  режиме	[]*/
/*[]****************************************************[]*/
void ReceiveMain (void)
 {
  uchar sy;

  if (count_dig == 5)					/*  		получено полное сообщение?			*/
   {
    flag_send = 0;  count_dig = 0;  hopping = 0;  pos_buff = 0;
    time_rest = 0;					/* mikle: сброс и запуск времени покоя канала приема и передачи	*/
    sy = '0';
    if (!memcmp (&origSI1_3_13, &buff_msg, 7))
     {
      GetLastNum ();
      if (!memcmp (&origSI3, &buff_msg [40], 7))  Error (ERRNOCOMM);
      if (!memcmp (&origSI13, &buff_msg [40], 7))
       {
	pos_buff = messKG_1; hopping = 1;
	sy = '3';
       }
     }
    if (!memcmp (&origSI6, &buff_msg, 7))
     {
      pos_buff = messKG_0; hopping = 1;
      sy = '4';
     }
    if (!memcmp (&origSI7, &buff_msg, 7))
     {
      pos_buff = messKG_1; hopping = 1;
      sy = '5';
     }
    if (!memcmp (&origSI17_43, &buff_msg, 7))
     {
      if (!memcmp (&origSI17, &buff_msg [20], 7))
       {
	pos_buff = messKG_0; hopping = 1;
	sy = '6';
       }
      if (!memcmp (&origSI43, &buff_msg [20], 7))
       {
	pos_buff = messKG_0; hopping = 1;
	sy = '7';
       }
     }
    if (!memcmp (&origSI18, &buff_msg, 7))
     {
      pos_buff = messKG_1; count_mess = 1; hopping = 1;
      sy = '8';
     }
    if (!memcmp (&origSI20, &buff_msg, 7))
     {
      pos_buff = messKG_1; hopping = 1;
      sy = '9';
     }
    PrintLett (&sy);
    outp (0x3fe, 3);
   }

  disable ();
  if ((flag_send > MAX_MSG_CH) && !pos_buff && !hopping)/*    			была выдача сообщения?			*/
   {
    pos_buff = messKG_0; flag_send = 0; hopping = 1;
    enable ();
    outp (0x3fe, 0);  Delay3Nop;  outp (0x3fe, 3);
   }
  enable ();

  disable ();						/* mikle: при отсутствии инф. для передачи в течении 10 мин.	*/
  if ((time_rest > TIMEREST_10) && !pos_buff && !hopping)/*    			была выдача сообщения?			*/
   {
    pos_buff = messKG_0; flag_send = 0; hopping = 1;	/*  		выдается запрос квитанции: ЗЗ "пробел"		*/
    enable ();
    outp (0x3fe, 0);  Delay3Nop;  outp (0x3fe, 3);
   }
  enable ();

  if ((inp(0x3fe) == 0x33) && !pos_buff && !hopping)	/* 	     разрешена передача и буфер передачи пустой      	*/
   {
    outp (0x3fe, 0);  Delay3Nop;  outp (0x3fe, 3);	/*   "зацепка" для извлечения после перерыва записи из arr_ch	*/
   }
  if (hopping && !count_wait && flag_send)		/*              истекло время выделенное на прием ?		*/
   {
    tmp_c = '?';  PrintLett (&tmp_c);			/*		    сообщение: где автоответ?			*/
    count_wait = db->status.load_wait;			/* 			новый time_out                          */
    if (!count_attempt--)				/*    	 были INIT_ATTEMPT - 1 попыток ожидания автоответа?	*/
     {
      Error (ERROFFLINE);  count_attempt = INIT_ATTEMPT+1;
     }
   }
 }

/*
/*[]****************************************************[]*/
/*[]	  Проверка  отбоя  при  встречной  передаче	[]*/
/*[]****************************************************[]*/
void VerifyRetreat (void)
 {
  SetVideoPage (WORK_WND);
  if (inp (0x3fe) == 0x11)
   {
    outp (0x3fe, 0); pos_buff = 0;
    tmp_c = 1;  PrintLett (&tmp_c);		/*		сообщение: разъединение с АПК			*/
    count_wait = db->status.load_wait;		/* 			новый time_out                          */
    outp (0x3fe, 2); count_receive = 0;		/* 		прием "готов к приему" и ЗТВ			*/
   }
 }
*/

/*[]****************************************************[]*/
/*[]	 Дополнительное  меню  обработки  информации	[]*/
/*[]****************************************************[]*/
void AddProcessMenu (void)
 {
  uchar far *ptr_process_menu;

  *(ptr_kbd_head + 1) = *ptr_kbd_head;		/* обнуление буфера клавиатуры	*/
  SetVideoPage (WORK_WND);
  ptr_process_menu = SaveWnd (ARG_WND_ADD_PROCESS);
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_ADD_PROCESS);
  ToXY (VP_WORK, LINE, X_ADD_PROCESS_WND+1, Y_ADD_PROCESS_WND, " Дополнительное меню обработки ");
  ToXY (VP_WORK, LINE, X_ADD_PROCESS_WND+2, Y_ADD_PROCESS_WND+1, "Поиск последнего сообщения");
  ToXY (VP_WORK, LINE, X_ADD_PROCESS_WND+2, Y_ADD_PROCESS_WND+2, "Передача итоговой информации");
  PushInf (INFMAINMENU);
  while (VertSelect (X_ADD_PROCESS_WND+1, Y_ADD_PROCESS_WND+1, Y_ADD_PROCESS_WND+2, ProcessAddFunc, NULL) != UINT_MAX)
   {
    if (sign.wnd_processout)  break;
   }
  PopInf ();
  RestoreWnd (ptr_process_menu, ARG_WND_ADD_PROCESS);
  sign.kbhit = OFF;
 }

/*[]****************************************************[]*/
/*[]   Меню  поиска  последних  полученных  сообщений	[]*/
/*[]****************************************************[]*/
void LastMsg (void)
 {
  uchar arr_tmp [MAX_STR],
	far *ptr_last, far *ptr_last1;
  uint arr_num [2] = {UINT_MAX, UINT_MAX};

  memset (&arr_tmp, NULL, sizeof (arr_tmp));
  ptr_last = SaveWnd (ARG_WND_LAST_MSG);
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_LAST_MSG);
  ToXY (VP_WORK, LINE, X_ADD_PROCESS_WND+4, Y_ADD_PROCESS_WND+2, " Номер телефона ");
  PushInf (INFFINDLAST);
  if (EditStr ((uchar *) &arr_tmp, X_ADD_PROCESS_WND+2, Y_ADD_PROCESS_WND+3) != ESC_KEY)
   {
    if (FindLastMsg ((uchar *) &arr_tmp, (uint *) &arr_num) != MAX_LAST_MSG)
     {
      if (!sign.exit)
       {
	ptr_last1 = SaveWnd (ARG_WND_LAST_MSG1);
	ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_LAST_MSG1);
	ToXY (VP_WORK, LINE, X_ADD_PROCESS_WND+16, Y_ADD_PROCESS_WND+4, " Последние полученные сообщения ");
	if (arr_num [0] != UINT_MAX)  ToXY (VP_WORK, LINE, X_MENU_WND+7, Y_ADD_PROCESS_WND+5, (uchar *) &ptr_dst->msg [arr_num [0]]);
	if (arr_num [1] != UINT_MAX)  ToXY (VP_WORK, LINE, X_MENU_WND+7, Y_ADD_PROCESS_WND+6, (uchar *) &ptr_dst->msg [arr_num [1]]);
	PushInf (INFANYKEY);
	(void) GetKey (NO_KEYS);
	PopInf ();
	RestoreWnd (ptr_last1, ARG_WND_LAST_MSG1);
       }
     }
    else Error (ERRNOLASTMSG);
   }
  PopInf ();
  RestoreWnd (ptr_last, ARG_WND_LAST_MSG);
 }

/*[]****************************************************[]*/
/*[]	Поиск  последних  2-х  полученных  сообщений	[]*/
/*[]****************************************************[]*/
uint FindLastMsg (char *arr, uint *num)
 {
  uint i = MAX_REC_DST, j, k = MAX_LAST_MSG;

  disable ();
  j = head_dst;		/* получение текущего указателя на последнее сообщение	*/
  do
   {
    if (!memcmp (&arr [0], (uchar *) &ptr_dst->msg [j] [ADAP_POS_N_TLPH], 7))
     {
      num [--k] = j;	/*	   сохранение номера найденного сообщения	*/
      if (!k)  break;
     }
    if (j)  --j;
    else    j = MAX_REC_DST-1;
   }
  while (--i);
  enable ();
  return (k);
 }

/*[]****************************************************[]*/
/*[]    Выдача  в  канал / файл  итоговой  информации	[]*/
/*[]****************************************************[]*/
void SendTotal (void)
 {
  ulong tty;
  uint i, j;
  uchar arr_tmp [MAX_STR], far *ptr_send;

  if (!db->status.chann)
   {
    Error (ERRABSCHAN);  return;
   }
  if (sign.wnd_processout && hopping) return;		/*	 канал в данный момент не готов		*/
  memset (&arr_tmp, NULL, sizeof (arr_tmp));
  ptr_send = SaveWnd (ARG_WND_SEND_TOTAL);
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_SEND_TOTAL);
  ToXY (VP_WORK, LINE, X_ADD_PROCESS_WND+4, Y_ADD_PROCESS_WND+3, " Номер телетайпа ");
  PushInf (INFSENDTOTAL);
  if (EditStr ((uchar *) &arr_tmp, X_ADD_PROCESS_WND+2, Y_ADD_PROCESS_WND+4) != ESC_KEY)
   {
    i = j = 0;
    for (i = 0; i < 6;)
     {
      if (arr_tmp [i++] == '?')  ++j;
     }
    if (j == 6)					/*     выдача итогов во все адресаты?	*/
     {
      for (i = 0; i < db->serv_addr; i++)
       {
	 SendTotalRecord (i);			/* выдача в канал всех записей по одной */
	 if (hopping) break;			/*   предыдущий итог в канал не выдан	*/
       }
      PopInf ();  RestoreWnd (ptr_send, ARG_WND_SEND_TOTAL);
      return;
     }
    i = 0;  j = OFF;
    if ((tty = atol ((uchar *) &arr_tmp)) != 0)	/* 	был введен номер телетайпа?	*/
     {
      do
       {
	if ((tty == db->addr [i].Ntty) && (arr_tmp [6] == db->addr [i].type))
	 {
	  j = ON;  break;
	 }
       }
      while (i++ != MAX_ADDR);
     }
    if (!j)            				/*   найден введенный номер телетайпа?	*/
     {
      Error (ERRTTYNO);
      PopInf ();  RestoreWnd (ptr_send, ARG_WND_SEND_TOTAL);
      return;
     }
    else  SendTotalRecord (i);			/*	выдача в канал одной записи	*/
   }
  PopInf ();
  RestoreWnd (ptr_send, ARG_WND_SEND_TOTAL);
 }

/*[]****************************************************[]*/
/*[]  Выдача  одной  итоговой  записи  в  канал / файл  []*/
/*[]****************************************************[]*/
void SendTotalRecord (uint i)
 {
  uint j, k;
  uchar far *ptr_tmp;
  static struct
	  {
	   date [5];				/*		текущая дата		*/
	   quan [4];				/*    количество итоговых разговоров	*/
	   sum  [7];				/*	       итоговая сумма		*/
	  } sum_file;

  if (!sign.wnd_processout)			/*     передача в дополнительном меню?	*/
   {
    count_ch   = tail_ch  = head_ch = 0;
    count_wait = pos_buff = 0;
    hopping = ON;
   }
  else
   {
    outp (0x21, (inp (0x21) | 0x1));		/* запрещение IRQ_0 (прер. от таймера)	*/
    if (count_ch == MAX_REC_CH)  return;

/*
    while (count_ch == MAX_REC_CH)		/* нет места в arr_ch для записи итогов */
     if ((inp(0x3fe) == 0x11) && (!pos_buff))	/*   разрешена передача и буфер пустой 	*/
       {
	outp (0x3fe, 0); outp (0x3fe, 1);	/* "зацепка" для извл-ия записи из arr_ch */
       }
*/

   }
  memset (&ptr_ch->inf [head_ch].Ntty [0], NULL, sizeof (ptr_ch->inf [0]));
  ltoa (db->addr [i].Ntty, (uchar *) &ptr_ch->inf [head_ch].Ntty [0], 10);
  ptr_ch->inf [head_ch].type = db->addr [i].type;
  memcpy (&ptr_ch->inf [head_ch].town [0], &db->addr [i].town, 15);
  memcpy (&ptr_ch->inf [head_ch].addr [0], &db->addr [i].addr, 15);
  if (!sign.wnd_processout)  			/*    была передача в дополнит. меню?	*/
   {
    _AH = 0x2a;  Intr (0x21);			/*	  получение текущей даты	*/
    j = _DH;  k = _DL;
    itoa (k, &ptr_ch->inf [head_ch].msg.total.date [0], 10);
    ptr_ch->inf [head_ch].msg.total.date [2] = '.';
    itoa (j, &ptr_ch->inf [head_ch].msg.total.date [3], 10);
   }
  else
   {
    memcpy (&ptr_ch->inf [head_ch].msg.total.date [0], &arr_info [ADAP_POS_DATE], 5);
    ptr_ch->inf [head_ch].msg.total.date [3] = type_month;
   }
  if (!ptr_ch->inf [head_ch].msg.total.date [1])  ptr_ch->inf [head_ch].msg.total.date [1] = ' ';
  if (!ptr_ch->inf [head_ch].msg.total.date [4])
   {
    ptr_ch->inf [head_ch].msg.total.date [4] = ptr_ch->inf [head_ch].msg.total.date [3];
    ptr_ch->inf [head_ch].msg.total.date [3] = type_month;
   }
  itoa (ptr_total->quan [i], (uchar *) &ptr_ch->inf [head_ch].msg.total.quan [0], 10);
  ltoa (ptr_total->sum  [i], (uchar *) &ptr_ch->inf [head_ch].msg.total.sum  [0],  10);
  j = 6;
  do
   {
    if (ptr_ch->inf [head_ch].msg.total.sum [j])
     {
      ptr_ch->inf [head_ch].msg.total.sum [j+1] = ptr_ch->inf [head_ch].msg.total.sum [j--];
      ptr_ch->inf [head_ch].msg.total.sum [j+1] = ptr_ch->inf [head_ch].msg.total.sum [j];
      ptr_ch->inf [head_ch].msg.total.sum [j] = '.';
      break;
     }
   }
  while (--j);
  ptr_total->quan [i] = 0;			/* 		обнуление итогов	*/
  ptr_total->sum  [i] = 0;
  memset (&sum_file, NULL, sizeof (sum_file));
  memcpy (&sum_file.date, &ptr_ch->inf [head_ch].msg.total.date, sizeof (ptr_ch->inf [0].msg.total.date));
  memcpy (&sum_file.quan, &ptr_ch->inf [head_ch].msg.total.quan, sizeof (ptr_ch->inf [0].msg.total.quan));
  memcpy (&sum_file.sum, &ptr_ch->inf [head_ch].msg.total.sum, sizeof (ptr_ch->inf [0].msg.total.sum));
  ++count_ch;
  if (++head_ch == MAX_REC_CH)  head_ch = 0;
  outp (0x21, (inp (0x21) & 0xfe));		/*       разрешение IRQ_0 для 8259	*/
  if (!sign.wnd_processout)			/*     передача в дополнит. меню?	*/
   {
    ptr_tmp = SaveWnd (ARG_WND_OPER);
    ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_OPER);
    if ((inp (0x3fa) & (BIT_CTS || BIT_DCD)) ||
	(inp (0x3f9) & BIT_GAP))		/*   разрыв посл-ти передаваемых кодов?	*/
     {
      Error (ERROFFLINE);  return;
     }
    Old_0x0c = getvect (0x0c);			/* сохранение старого вектора  0Ch	*/
    setvect (0x0c, SendChan);
    (void) inp(0x3f8); outp (0x3fe,0);		/* запретить прер-ия по приему и пер-че */
    outp (0x21, (inp (0x21) & 0xef));		/*       разрешение IRQ 4 для 8259	*/
    outp (0x3fe, 1); 				/*     разрешение передачи в канал	*/
    while (!count_wait);			/*  ожидание передачи итоговой записи	*/
    while (hopping && count_wait)		/* ожидание получения ЗТВ или time_out  */
     {
      delay(50); count_wait = --count_wait;	/* прoверка через 50 ms	сост. приема    */
     }
    if (hopping) Error (ERROFFLINE);		/* поступала информация с канала связи? */
    RestoreWnd (ptr_tmp, ARG_WND_OPER);
    outp (0x21, (inp (0x21) | 0x10));		/* 	     запрещение IRQ 4		*/
    setvect (0x0c, Old_0x0c);
   }
  if (sign.sum_file)				/*	работаем с итоговым файлом?	*/
   {
    j = write (sum_f, &db->addr [i].town, sizeof (db->addr [i].town));
    if (j != -1U)  j = write (sum_f, "   ", 3);
    if (j != -1U)  j = write (sum_f, &db->addr [i].addr, sizeof (db->addr [i].addr));
    if (j != -1U)  j = write (sum_f, "   ", 3);
    if (j != -1U)  j = write (sum_f, &sum_file, sizeof (sum_file));
    if (j != -1U)  j = write (sum_f, "\n", 1);
    if (j == -1U)
     {
      Error (ERRSUMFILE);  sign.sum_file = OFF;
     }
   }
 }

/*[]****************************************************[]*/
/*[]	     Обнуление  итоговой  информации		[]*/
/*[]****************************************************[]*/
void ClearTotal (void)
 {
  memset (&ptr_total->quan [0], NULL, sizeof (total_s));
 }

/*[]****************************************************[]*/
/*[]              Обработка  и  передача		[]*/
/*[]   на  дисплей,  дисковое  устройство  и  принтер	[]*/
/*[]****************************************************[]*/
void ProcessOutRec (void)
 {

/*
  tmp_c = 0x0b;  PrintLett (&tmp_c);				/*      принят гостиничный номер телефона	*/
*/

  ScrollWnd (Y_MENU_WND + 11);
  ToXY (VP_WORK, LINE, X_MENU_WND+7, Y_MENU_WND+15, (char *) &ptr_dst->msg [tail_dst]);
  if (db->status.prn)
   {
    tmp_c = 'П';  PrintLett (&tmp_c);				/*       сообщение: передача на принтер		*/
    write (STDPRN, (uchar *) &ptr_dst->msg [tail_dst], sizeof (ptr_dst->msg [0]));
    write (STDPRN, "\n", 1);
   }
  if (db->status.flop)
   {
    tmp_c = 'Д';  PrintLett (&tmp_c);				/*         сообщение: запись на дискету		*/
    if (write (tmp_f, (uchar *) &ptr_dst->msg [tail_dst], sizeof (ptr_dst->msg [0])) == -1)
     {
      Error (ERRINF);  DelAssign (FLOP);
     }
   }
  disable ();
  --count_dst;
  if (++tail_dst == MAX_REC_DST)  tail_dst = 0;
  enable ();
 }

/*[]****************************************************[]*/
/*[]        Инициализация  оборудования  ЕС 1840	[]*/
/*[]****************************************************[]*/
void InitEquip (char reg)
 {
  outp (0x3ff, 0x8a);			/* ВВ55: А - вывод, B - ввод, С(0...3) - вывод, C(4...7) - ввод	*/
  InitSpeed ();
  outp (0x3f9, 0);			/* 		     программный сброс ВВ51			*/
  Delay3Nop;
  outp (0x3f9, 0);
  Delay3Nop;
  outp (0x3f9, 0);
  Delay3Nop;
  outp (0x3f9, 0x40);
  Delay3Nop;
  if (reg == INITFORTEST)		/*   иницииализация для проведения теста модема ЕС 8133.02?	*/
   {
    outp (0x3f9, 0xfa);			/*  7 информ. бит, 2 стоп бита, контроль по четн., коэф-т - 16	*/
    outp (0x3f9, 0x35);			/* 		отключение модема от линии			*/
    delay (15000);			/* 	    задержка - 15 сек для разъединения линии		*/
    outp (0x3f9, 0x37);			/* 			108 = ON, 106 = ON			*/
   }
  else  outp (0x3f9, 0x82);		/*  управл. слово иниц-ции УСАПП - асинх. обмен: 5 информ. бит,	*/
					/* 1.5 стоп бита, контроль отсутствует, коэф-т деления - 16	*/
 }

/*[]****************************************************[]*/
/*[]	    Инициализация  с  включением  модема	[]*/
/*[]****************************************************[]*/
void InitEquipOn (void)
 {
  InitEquip (INITFORWORK);
  outp (0x3f9, 0x37);	/* 	подсоединение к АПК	*/
 }

/*[]****************************************************[]*/
/*[]		  Инициализация  скорости		[]*/
/*[]****************************************************[]*/
void InitSpeed (void)
 {
  outp (0x3fc, 0x8c);			/* запись в регистр управ. сигналов ВВ55, DLAB=1		*/
  outp (0x3fb, 0x3e);			/* управляющее слово ВИ53, делитель равен 2 байтам		*/
  switch (db->status.speed)
   {
    case 1:				/* 		скорость передачи: 50 бит/сек			*/
     outp (0x3f8, 0xdc);		/* 		скорость передачи в канал связи			*/
     outp (0x3f8, 5);
     break;
    case 2:				/* 		скорость передачи: 100 бит/сек			*/
     outp (0x3f8, 0xee);		/* 		скорость передачи в канал связи			*/
     outp (0x3f8, 2);
     break;
    default:				/* 		скорость передачи: 200 бит/сек			*/
     outp (0x3f8, 0x78);		/* 		скорость передачи в канал связи			*/
     outp (0x3f8, 1);
     break;
    case 4:				/* 		скорость передачи: 600 бит/сек			*/
     outp (0x3f8, 0x7e);		/* 		скорость передачи в канал связи			*/
     outp (0x3f8, 0);
   }
  outp (0x3fc, 0x0c);			/* 			    DLAB = 0				*/
 }

/*[]****************************************************[]*/
/*[] Тесты  периферийного  обслуживаемого  оборудования	[]*/
/*[]****************************************************[]*/
void TestEquip (void)
 {
  uchar far *ptr_test;

  ptr_test = SaveWnd (ARG_WND_TEST);
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_TEST);
  ToXY (VP_WORK, LINE, X_MENU_WND+6, Y_MENU_WND+8, " Тестирование и инициализация оборудования ");
  ToXY (VP_WORK, LINE, X_MENU_WND+4, Y_MENU_WND+9, "Внутренний тест адаптера ЕС 7920.01");
  ToXY (VP_WORK, LINE, X_MENU_WND+4, Y_MENU_WND+10, "Внутренний тест телеграфного модема ЕС 8133.02");
  tmp_c = '├';  ToXY (VP_WORK, SYMB, X_MENU_WND+2, Y_MENU_WND+11, (char *) &tmp_c);
  tmp_c = '─';  ToXY (VP_WORK, 48, X_MENU_WND+3, Y_MENU_WND+11, (char *) &tmp_c);
  tmp_c = '┤';  ToXY (VP_WORK, SYMB, X_MENU_WND+51, Y_MENU_WND+11, (char *) &tmp_c);
  ToXY (VP_WORK, LINE, X_MENU_WND+4, Y_MENU_WND+12, "Повторная инициализация оборудования");
  PushInf (INFMAINMENU);
  while (VertSelect (X_MENU_WND+3, Y_MENU_WND+9, Y_MENU_WND+9, TestFunc, NULL) != UINT_MAX);
  RestoreWnd (ptr_test, ARG_WND_TEST);
  PopInf ();
 }

/*[]****************************************************[]*/
/*[]                 Проверка  адаптера			[]*/
/*[]****************************************************[]*/
void TestAdap (void)
 {
  uchar cur_sec;

  PushInf (INFTESTADAP);
  outp (0x225, 0);		/* внутренний тест адаптера	*/
  _AH = 0x2c;  Intr (0x21);	/* получение текущего времени	*/
  if ((cur_sec = _DH) == 59)  cur_sec = 0;
  while (inp (0x227) & BIT_TEST_ADAP)
   {
    _AH = 0x2c;  Intr (0x21);
    if (_DH > cur_sec+1)
     {
      Error (ERRADAP);  PopInf ();  _DL = FALSE;  return;
     }
   }
  PopInf ();
  _DL = TRUE;
 }

/*[]****************************************************[]*/
/*[]            Проверка  телеграфного  модема		[]*/
/*[]****************************************************[]*/
void TestCodec (void)
 {
  uint i = 0;
  uchar com_test [] = {"LSU\r\n"}, c;
  ulong end_ticks;

  if (sign.errinsstub)
   {
    Error (ERRINSSTUB);  return;
   }
  PushInf (INFTESTCODEC);
  WaitCodec (ON);
  c = db->status.speed;  db->status.speed = 3;		/*	тест при 200 бит/сек	*/
  InitEquip (INITFORTEST);
  db->status.speed = c;					/* восст-ние первонач. скорости	*/
  delay (2000);						/* ожидание срабатывания модема	*/
  while ((inp (0x3f9) & BIT_RXRDY)) (void) inp (0x3f8);	/*         сброс RXRDY		*/
  do
   {
    end_ticks = *ptr_ticks + 18 * TIMEOUT_CODEC;	/* получение конечн. значения	*/
    while (!(inp (0x3f9) & BIT_TXRDY))			/*      ожидание TXRDY = 1	*/
     {
      if (*ptr_ticks > end_ticks)			/* превышено значение таймаута?	*/
       {
	Error (ERRCODEC);  PopInf ();  WaitCodec (OFF);
       _DL = FALSE;  return;
       }
     }
    outp (0x3f8, com_test [i++]);			/*    команда -> LSU (тест)	*/
   }
  while (i < 5);
  i = 0;
  end_ticks = *ptr_ticks + 18 * TIMEOUT_CODEC;		/* получение конечн. значения	*/
  do
   {
    while (!(inp (0x3f9) & BIT_RXRDY))
     {
      if (*ptr_ticks > end_ticks)			/* превышено значение таймаута?	*/
       {
	Error (ERRCODEC);  PopInf ();  WaitCodec (OFF);
	_DL = FALSE;  return;
       }
     }
    com_test [i++] = inp (0x3f8);			/* получение символа из модема	*/
   }
  while (i < 3);
  if (memcmp ((char *) &com_test, "VLM", 3))
   {
    Error (ERRCODEC);  PopInf ();  WaitCodec (OFF);
    _DL = FALSE;  return;
   }
  InitEquip (INITFORWORK);
  outp (0x3f9, 0x37);					/*	108 = ON, 106 = ON	*/
  PopInf ();  WaitCodec (OFF);  _DL = TRUE;
 }

/*[]****************************************************[]*/
/*[] Вывод  информации  из  файла  временного  хранения	[]*/
/*[]                 в  канал  связи			[]*/
/*[]***************************	*************************[]*/
void DrivOutChann (void)
 {
  uint i, j;
  uchar far *ptr_tmp;

  PushInf (INFTOTMPDRVCHAN);
  if (!db->status.chann)
   {
    Error (ERRINSSPEED);  PopInf ();  return;
   }
  if (sign.errinsstub)
   {
    Error (ERRINSSTUB);  PopInf ();  return;
   }
  outp (0x3f9, 0x37);
  sleep (1);
  if (((sign.alien = inp (0x3fa)) & (BIT_CTS || BIT_DCD)) ||
      (inp (0x3f9) & BIT_GAP))				/* разрыв посл-ти перед. кодов?	*/
   {
    Error (ERROFFLINE);  PopInf ();  outp (0x3f9, 0x35);
    return;
   }
  _DL = cur_drive;  _AH = 0x0e;  Intr (0x21);		/*     установка устройства 	*/
  if ((fl = open (FILE_INF, O_RDWR | O_BINARY, S_IREAD | S_IWRITE)) == -1)
   {
    Error (ERRINF);  PopInf ();   outp (0x3f9, 0x35);
    return;
   }
  read (fl, name, sizeof (MSGTMPINF));
  if (memcmp (name, MSGTMPINF, sizeof (MSGTMPINF)))
   {
    close (fl);  Error (ERRINF);  PopInf ();  outp (0x3f9, 0x35);
    return;
   }
  ptr_tmp = SaveWnd (ARG_WND_OPER);
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_OPER);
  permit = (uchar) ((sign.alien & BIT_ALIEN) >> 2);
  memset (&ptr_ch->inf [0].Ntty [0], NULL, 2 * sizeof (ptr_ch->inf [0]));
  Old_0x0c = getvect (0x0c);			/* сохранение старого вектора  0Ch	*/
  setvect (0x0c, SendChan);
  outp (0x21, (inp (0x21) & 0xef));		/*      разрешение IRQ4 для 8259	*/
  if (SetComm ())
   {
    outp (0x21, (inp (0x21) | 0x10));		/* 		запрещение IRQ4		*/
    outp (0x3f9, 0x35);
    setvect (0x0c, Old_0x0c);			/* восстановление старого вектора 0Ch	*/
    Error (ERRNOCOMM);
    RestoreWnd (ptr_tmp, ARG_WND_OPER);
    PopInf ();
    close (fl);
    return;
   }
  do
   {
    outp (0x3fe, 0);				/* запрещение прерываний от ВВ51	*/
    read (fl, (ch_s *) &ptr_ch->inf [0].msg.info [0], LENGTH_REC);
    SearchNTlph (atol ((uchar *) &ptr_ch->inf [0].msg.info [ADAP_POS_N_TLPH]), (uint *) &j);
    if (!_DL)
     {
      Error (ERRINVINF);  break;
     }
    i = db->tlph [j].ind_addr;			/*	 индекс текущей гостиницы	*/
    ltoa (db->addr [i].Ntty, (uchar *) &ptr_ch->inf [0].Ntty [0], 10);
    ptr_ch->inf [0].type = db->addr [i].type;
    ltoa (db->tlph [j].Nroom, (uchar *) &ptr_ch->inf [0].room [0], 10);
    memcpy (&ptr_ch->inf [0].town [0], &db->addr [i].town, 15);
    memcpy (&ptr_ch->inf [0].addr [0], &db->addr [i].addr, 15);
    count_ch = hopping = 1;
    i = tail_ch = count_wait = pos_buff = flag_send = 0;
    (void) inp (0x3f8);
    outp (0x3fe, 1);				/*  разрешить-передачу, запретить-прием	*/
    if (inp (0x3fa) & BIT_CTS)			/*   106: готов модем к передаче?	*/
     {
      Error (ERROFFLINE);  break;
     }
    while (!flag_send)
     {
      outp (0x3fe, 0);  outp (0x3fe, 1);
      sleep (5);
      if (i++ == 12)
       {
	i = 0xff;  break;
       }
     }
    if (i == 0xff)
     {
      Error (ERROFFLINE);  break;
     }
   }
  while (!eof (fl));				/*    достигнут конец файла, ошибка?	*/
  count_ch   = tail_ch  =
  count_wait = pos_buff = hopping = 0;
  outp (0x21, (inp (0x21) | 0x10));		/* 		запрещение IRQ4		*/
  outp (0x3f9, 0x35);
  setvect (0x0c, Old_0x0c);			/* восстановление старого вектора 0Ch	*/
  if (eof (fl) == -1)  Error (ERRINF);		/* ошибка при работе с файлом TLT.INF?  */
  RestoreWnd (ptr_tmp, ARG_WND_OPER);
  PopInf ();
  Ask (UNLINK);
  close (fl);
 }

/*[]****************************************************[]*/
/*[]       Удаление  файла  временного  хранения	[]*/
/*[]****************************************************[]*/
void UnlinkInfFile (void)
 {
  unlink (FILE_INF);
 }

/*[]****************************************************[]*/
/*[]     Ожидание  подготовки  телеграфного  модема	[]*/
/*[]****************************************************[]*/
void WaitCodec (char reg)
 {
  uchar far *ptr_key;
  static uchar s [] [9] = {"отпущена", "нажата"};
  static uchar *msg     = {"Кнопка \"P/A\" на передней панели модема должна быть"};

  ptr_key = SaveWnd (ARG_WND_KEYCODEC);
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_KEYCODEC);
  ToXY (VP_WORK, LINE, X_KEYCODEC_WND+5, Y_KEYCODEC_WND+1, msg);
  ToXY (VP_WORK, LINE, X_KEYCODEC_WND+56, Y_KEYCODEC_WND+1, (uchar *) &s [reg]);
  PushInf (INFANYKEY);
  (void) GetKey (NO_KEYS);
  RestoreWnd (ptr_key, ARG_WND_KEYCODEC);
  PopInf ();
 }

/*[]****************************************************[]*/
/*[] Проверка  адаптера  последовательной  коммуникации	[]*/
/*[]		   внутренним  шлейфом			[]*/
/*[]****************************************************[]*/
uchar TestInsideStub (void)
 {
  uint i;

  outp (0x3fc, 0x1c);				/*  разрешение внутрен. шлейфа	*/
  outp (0x3f9, 0x35);				/*     108 - OFF & 105 - ON	*/
  outp (0x3f8, 0xfa);				/*   выдача контрольного байта	*/
  delay (200);					/*       ожидание символа	*/
  i = inp (0x3f8);
  outp (0x3fc, 0x0c);				/* отключение внутрен. шлейфа	*/
  outp (0x3f9, 0x35);				/* 	подсоединение к АПК	*/

/*
  outp (0x3f9, 0x37);				/* 	подсоединение к АПК	*/
*/

  if (i == 0x1a)  return (TRUE);		/*   получено верное значение?	*/
  else            return (FALSE);
 }

/*[]****************************************************[]*/
/*[]           Отображение  символа  операции		[]*/
/*[]****************************************************[]*/
void PrintLett (char *lett)
 {
  ToXY (VP_WORK, SYMB, 3, 2, (char *) lett);
 }
