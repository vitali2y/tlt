/*[]************************************************************[]*/
/*[]      	Междугородные  телефонные  разговоры		[]*/
/*[]			      Версия  2.4			[]*/
/*[]								[]*/
/*[]           Window.c:  модуль  оконного  интерфейса		[]*/
/*[]************************************************************[]*/

#include			<alloc.h>
#include			<dos.h>
#include			<limits.h>
#include			"TLT.h"

extern char arr_harderr [];
extern struct
	{
	 uchar		harderr;
	 uchar		err;
	 uchar		kbhit;
	 uchar		exit;
	 uchar		hlp;
	 uchar		end_help;
	 uchar		change;
	 uchar		next_line;
	 uchar		ctrl_end_key;
	 uchar		wnd_process;
	 uchar		wnd_setregime;
	 uchar		wnd_processout;
	 uchar		wnd_tlphdb;
	 uchar		wnd_listaddrdb;
	 uchar		wnd_addchangdel;
	 uchar		wnd_listtlphdb;
	 uchar		out_prn;
	 uchar		alien;
	 uchar		errinsstub;
	 uint		count_comm;
	 uchar		sum_file;
	 uchar		find;
	} sign;		/*             структура признаков		*/
extern struct
	{
	 uchar  err_pos;
	 char  *err_ptr;
	} err_msg [];	/* структура сообщений об ошибках		*/
extern struct
	{
	 uchar  inf_pos;
	 char  *inf_ptr;
	} inf_msg [];	/*        структура сообщений 			*/
extern uint  cur_x_pos, cur_y_pos, count_tmp_inf, far *ptr_kbd_head;
extern uchar frame [] [7], permit, cur_attr, *tmp_ptr, tmp_c,
	     *cur_num_inf;
extern tdb *db;

/*[]************************************************************[]*/
/*[]         		Установка  рабочего  окна		[]*/
/*[]************************************************************[]*/
void InstallWnd (void)
 {
  ClrWnd (VP_WORK, DOUBLE_FRAME, SHADOW_OFF, X_WORK_WND, Y_WORK_WND, WIDTH_WORK_WND, HEIGTH_WORK_WND);
  cur_attr |= HIGH_ATTR;
  ToXY (VP_WORK, LINE, 22, Y_WORK_WND, " Междугородные телефонные разговоры ");
  cur_attr &= ~HIGH_ATTR;
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, X_MENU_WND, Y_MENU_WND, 47, 6);
  ToXY (VP_WORK, LINE, X_MENU_WND+10, Y_MENU_WND, " Основное меню программы ");
  ToXY (VP_WORK, LINE, X_MENU_WND+2, Y_MENU_WND+1, "Информация для пользователя            {F1}");
  ToXY (VP_WORK, LINE, X_MENU_WND+2, Y_MENU_WND+2, "Меню обработки информации                  \x10");
  ToXY (VP_WORK, LINE, X_MENU_WND+2, Y_MENU_WND+3, "Меню работы с телефонной базой данных      \x10");
  tmp_c = '├';  ToXY (VP_WORK, SYMB, X_MENU_WND, Y_MENU_WND+4, (char *) &tmp_c);
  tmp_c = '─';  ToXY (VP_WORK, 46, X_MENU_WND+1, Y_MENU_WND+4, (char *) &tmp_c);
  tmp_c = '┤';  ToXY (VP_WORK, SYMB, X_MENU_WND+47, Y_MENU_WND+4, (char *) &tmp_c);
  ToXY (VP_WORK, LINE, X_MENU_WND+2, Y_MENU_WND+5, "Возврат в DOS                          {F10}");
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, X_INFO_WND, Y_INFO_WND, 73, 2);
  ToXY (VP_WORK, LINE, X_INFO_WND+28, Y_INFO_WND, " Помощь/Состояние ");
 }

/*[]************************************************************[]*/
/*[]             Функция  получения  кода  клавиши		[]*/
/*[]************************************************************[]*/
uint GetKey (uchar reg)
 {
  if (reg == NO_KEYS)
   {
    cur_attr |= HIGH_ATTR;
    reg = '═';  ToXY (VP_WORK, 34, 42, 24, (uchar *) &reg);
    cur_attr &= ~HIGH_ATTR;
   }
  else ToXY (VP_WORK, LINE, 42, 24, " {F1  F2  F3  F4  F5  F6  F10  Esc} ");
  while (*ptr_kbd_head == *(ptr_kbd_head + 1))	/*           ожидание нажатия клавиши		*/
   {
    if (sign.wnd_processout && db->status.chann)/*    ожидание нажатия во время обработки?	*/
       ReceiveMain ();				/*   		работа с каналом                */
    if (sign.wnd_processout && count_tmp_inf)	/* имеется информация для выдачи в верхнее окно?*/
       PrepareUpperWnd ();
   }
  _AH = 0;  Intr (0x16);			/*   		  читать символ			*/
  if (!_AL)  return (uint) (_AH + 256);
  else       return (uint) (_AL);
 }

/*[]************************************************************[]*/
/*[]              Изменение  номера  видеостраницы		[]*/
/*[]************************************************************[]*/
void SetVideoPage (char page)
 {
  _AL = page;  _AH = 5;  Intr (0x10);	/* установка видеостраницы	*/
 }

/*[]************************************************************[]*/
/*[]                 Установка  позиции  курсора		[]*/
/*[]************************************************************[]*/
void CursorXY (int vpage, uint x, uint y)
 {
  _DX = (y << 8) | x;  _BH = vpage;  _AH = 2;
  Intr (0x10);
 }

/*[]************************************************************[]*/
/*[]                Отображение  и  очистка  окна		[]*/
/*[]************************************************************[]*/
void ClrWnd (uint v_page, char num_frame, char shadow, uint x, uint y, uint w, uint h)
 {
  register int i;

  tmp_c = NULL;
  for (i = 0; i <= h; i++)
   {
    ToXY (v_page, w+1, x, y+i, (char *) &tmp_c);
   }
  if (num_frame == DOUBLE_FRAME)  cur_attr |= HIGH_ATTR;
  ToXY (v_page, SYMB, x, y, (char *) &frame [num_frame] [0]);
  ToXY (v_page, w-1, x+1, y, (char *) &frame [num_frame] [1]);
  ToXY (v_page, SYMB, x+w, y, (char *) &frame [num_frame] [2]);
  ToXY (v_page, SYMB, x, y+h, (char *) &frame [num_frame] [3]);
  ToXY (v_page, w-1, x+1, y+h, (char *) &frame [num_frame] [1]);
  ToXY (v_page, SYMB, x+w, y+h, (char *) &frame [num_frame] [4]);
  for (i = y+1; i < y+h; i++)
   {
    ToXY (v_page, SYMB, x, i, (char *) &frame [num_frame] [5]);
    ToXY (v_page, SYMB, x+w, i, (char *) &frame [num_frame] [5]);
   }
  if (num_frame == DOUBLE_FRAME)  cur_attr &= ~HIGH_ATTR;
  if (shadow == SHADOW_ON)	/*   отобразить "тень"	*/
   {
    tmp_c = '░';
    for (i = y+1; i < y+h+1; i++)
     {
      ToXY (v_page, 2, x+w+1, i, (char *) &tmp_c);
     }
    ToXY (v_page, w+1, x+2, y+h+1, (char *) &tmp_c);
   }
 }

/*[]************************************************************[]*/
/*[]                 Ввод / редактирование  строки.		[]*/
/*[]     Дополнительное возвращаемое значение (выборочно):	[]*/
/*[]   		     CL - код выполняемой операции		[]*/
/*[]************************************************************[]*/
int EditStr (char *str, uint cur_x, uint cur_y)
 {
  char exit = OFF, *old_str;
  uint i = 0, j, k, l = cur_x + MAX_STR - 2, ret, old_cur_x = cur_x;

  old_str = str;			/* сохранение указ-ля на исходн. массив	*/
  ToXY (VP_WORK, LINE, cur_x, cur_y, old_str);
  cur_x = l;
  do
   {
    FromXY (VP_WORK, SYMB, --cur_x, cur_y, NULL);
   }
  while (_AL == NULL);
  ++cur_x;
  do
   {
    if (sign.exit)  break;
    CursorXY (WORK_WND, cur_x, cur_y);
    if (sign.wnd_processout)  i = NO_KEYS;
    else                      i = GLOBAL_KEYS;
    i = GetKey (i);
    switch (i)
     {
      case F1_KEY:
       Help ();
       break;
      case F2_KEY:
       SetRegime ();
       break;
      case F3_KEY:
       ProcessOut ();
       break;
      case F4_KEY:
       ListAddrDB ();
       break;
      case F5_KEY:
       ListTlphDB ();
       break;
      case F6_KEY:
       if (!sign.wnd_addchangdel)
	{
	 _DL = ADD;  AddChangDelNTlph ();
	}
       break;
      case F10_KEY:
       ExitToDos ();
       break;
      case HOME_KEY:
       do
	{
	 FromXY (VP_WORK, SYMB, --cur_x, cur_y, NULL);
	}
       while (_AL != (uchar) '│');
       ++cur_x;
       break;
      case END_KEY:
       cur_x = l;
       do
	{
	 FromXY (VP_WORK, SYMB, --cur_x, cur_y, NULL);
	}
       while (_AL == NULL);
       ++cur_x;
       break;
      case BACKSPACE_KEY:				/*        забой одного символа		*/
       FromXY (VP_WORK, SYMB, --cur_x, cur_y, NULL);
       if (_AL == (uchar) '│')  ++cur_x;
       j = cur_x;
       while (TRUE)
	{
	 k = FromXY (VP_WORK, SYMB, j+1, cur_y, NULL);
	 if (_AL == (uchar) '│')
	  {
	   k = NULL;  ToXY (VP_WORK, SYMB, j, cur_y, (char *) &k);
	   break;
	  }
	 ToXY (VP_WORK, SYMB, j, cur_y, (char *) &k);
	 ++j;
	}
       break;
      case LEFT_KEY:
       FromXY (VP_WORK, SYMB, --cur_x, cur_y, NULL);
       if (_AL == (uchar) '│')  ++cur_x;
       break;
      case RIGHT_KEY:
       FromXY (VP_WORK, SYMB, ++cur_x, cur_y, NULL);
       if (_AL == (uchar) '│')  --cur_x;
       break;
      case ESC_KEY:
      case ENTER_KEY:
      case TAB_KEY:
       exit = ON;  ret = i;
       break;
      default:
       if (((i >= 0x20) && (i <= 0xaf)) ||		/* символы & цифры & англ. & русск. буквы*/
	   ((i >= 0xe0) && (i <= 0xef)))
	{
	 j = l;
	 if (cur_x > j)  break;
	 while (j-- != cur_x)
	  {
	   FromXY (VP_WORK, SYMB, j, cur_y, NULL);
	   k = _AL;  ToXY (VP_WORK, SYMB, j+1, cur_y, (uchar *) &k);
	  }
	 ToXY (VP_WORK, SYMB, cur_x++, cur_y, (char *) &i);
	}
      else Squeak ();
     }
   }
  while (!exit);
  i = 0;  str = old_str;
  while (TRUE)
   {
    FromXY (VP_WORK, SYMB, old_cur_x+i++, cur_y, NULL);
    if (_AL == (uchar) '│')  break;
    else (*str++ = _AL);				/* сохранение редактированного массива	*/
   }
  str = old_str;
  CursorXY (WORK_WND, 0, INVISIBLE);
  return (ret);
 }

/*[]************************************************************[]*/
/*[]               Выбоp  из  веpтикального  меню.		[]*/
/*[]     Дополнительное возвращаемое значение (выборочно):	[]*/
/*[]   		     DH - текущая x позиция,			[]*/
/*[]   		     DL - текущая y позиция,			[]*/
/*[]   		     CL - код выполняемой операции		[]*/
/*[]************************************************************[]*/
uint VertSelect (uint beg_x, uint beg_y, uint y, void (*FuncFunc []) (), void PosFunc ())
 {
  char exit = OFF;
  uint j, ret = UINT_MAX;

  do
   {
    if (sign.exit)  break;
    ReversStr (beg_x, y);
    if (sign.wnd_processout)  j = NO_KEYS;
    else                      j = GLOBAL_KEYS;
    j = GetKey (j);
    ReversStr (beg_x, y);
    switch (j)
     {
      case ESC_KEY:
       exit = ON;  ret = UINT_MAX;
       break;
      case ENTER_KEY:
       ret = ENTER_KEY;  exit = ON;
       if (FuncFunc)			/*  FuncFunc != NULL - функция вызова	*/
	{
	 (*FuncFunc [y-beg_y]) ();
	}
       else				/* возвращение текущих (x, y) координат	*/
	{
         _DH = beg_x+4;
	 if (beg_x != X_MENU_WND+3) ++_DH;
	 _DL = y-beg_y;			/*   DH, DL - позиции x, y		*/
	 if (sign.wnd_listtlphdb == ON)
	   _CL = CHANG;			/*   CL - код выполняем. операции	*/
	 PosFunc ();
	 if (sign.wnd_listtlphdb == ON)
	  {
	   beg_x = X_MENU_WND+3;  y = beg_y;
	  }
	}
       break;
      case DEL_KEY:
       ret = NULL;
       if (sign.wnd_listtlphdb == ON)
	{
         _DH = beg_x+4;
	 if (beg_x != X_MENU_WND+3) ++_DH;
	 _DL = y-beg_y;			/*   DH, DL - позиции x, y		*/
	 _CL = DEL; 			/*   CL - код операции			*/
	 PosFunc ();
	 if (sign.wnd_listtlphdb == ON)
	  {
	   beg_x = X_MENU_WND+3;  y = beg_y;
	  }
	}
       break;
      case TAB_KEY:
       if (sign.wnd_listtlphdb == ON)
	{
	 if (beg_x == X_MENU_WND+3)  cur_x_pos = beg_x = X_MENU_WND+2+35;
	 else                        cur_x_pos = beg_x = X_MENU_WND+3;
	}
       break;
      case SPACE_KEY:
       _DL = y-beg_y;			/*   DL - текущая y позиция		*/
      case HOME_KEY:
      case PG_UP_KEY:
      case PG_DN_KEY:
       cur_x_pos = X_MENU_WND+3;  cur_y_pos = Y_MENU_WND+7;
       exit = ON;  ret = j;
       break;
      case F1_KEY:
       Help ();
       break;
      case F2_KEY:
       SetRegime ();
       break;
      case F3_KEY:
       ProcessOut ();
       break;
      case F4_KEY:
       ListAddrDB ();
       break;
      case F5_KEY:
       ListTlphDB ();
       break;
      case F6_KEY:
       if (!sign.wnd_addchangdel)
	{
	 _DL = ADD;			/*       DL - y позиция операции	*/
	 AddChangDelNTlph ();
	}
       break;
      case F10_KEY:
       ExitToDos ();
       break;
      case UP_KEY:
       FromXY (VP_WORK, SYMB, beg_x-1, --y, NULL);
       if (_AL == (uchar) '├')
	{
	 --y;  --beg_y;  break;
	}
       if (_AL == (uchar) '┌')
	{
	 ++y;
	 if (sign.wnd_listtlphdb == ON)
	  {
	   if (beg_x == X_MENU_WND+3)
	    {
	     cur_y_pos = Y_MENU_WND+7;
	     exit = ON;  ret = UP_KEY;  break;
	    }
	   else
	    {
	     cur_x_pos = X_MENU_WND+3;  cur_y_pos = Y_MENU_WND+7+7;
	    }
	   beg_x = cur_x_pos;  y = cur_y_pos;
	   break;
	  }
	 if (sign.wnd_listaddrdb)
	  {
	   exit = ON;  ret = UP_KEY;
	  }
	}
       break;
      case DOWN_KEY:
       FromXY (VP_WORK, SYMB, beg_x-1, ++y, NULL);
       if (_AL == (uchar) '├')
	{
	 ++y;  ++beg_y;  break;
	}
       if (_AL == (uchar) '└')
	{
	 --y;
	 if (sign.wnd_listtlphdb == ON)
	  {
	   if (beg_x == X_MENU_WND+3)
	    {
	     cur_x_pos = X_MENU_WND+2+35;  cur_y_pos = Y_MENU_WND+7;
	    }
	   else
	    {
	     exit = ON;  ret = DOWN_KEY;
	     cur_x_pos = X_MENU_WND+2+35;  cur_y_pos = Y_MENU_WND+7+7;
	     break;
	    }
	   beg_x = cur_x_pos;  y = cur_y_pos;
	   break;
	  }
	 if (sign.wnd_listaddrdb)
	  {
	   exit = ON;  ret = DOWN_KEY;
	  }
	}
       break;
      default:
       if ((sign.wnd_listtlphdb == ON) && ((j >= 0x30) && (j <= 0x39)))
	{
	 FindNTlph (j);  ret = j;  exit = ON;
	}
       else Squeak ();
     }
   }
  while (!exit);
  return (ret);
 }

/*[]************************************************************[]*/
/*[] Сдвиг  окон  приним.  информации  на  одну  позицию  вверх	[]*/
/*[]************************************************************[]*/
void ScrollWnd (uchar y_top)
 {
  static uchar arr_tmp [LENGTH_REC];
  int i = y_top;

  while (++i != y_top + 5)
   {
    FromXY (VP_WORK, LENGTH_REC, X_MENU_WND+7, i, (uchar *) &arr_tmp);
    ToXY (VP_WORK, LINE, X_MENU_WND+7, i-1, (uchar *) &arr_tmp);
   }
 }

/*[]************************************************************[]*/
/*[]                   Реверсирование  строки			[]*/
/*[]************************************************************[]*/
void ReversStr (uint x, uint y)
 {
  uchar i = cur_attr, cur_symb;

  while (TRUE)
   {
    FromXY (VP_WORK, SYMB, x, y, NULL);
    cur_attr = _AH;  cur_symb = _AL;
    if (cur_symb == (uchar) '│')  break;
    if ((cur_attr & INVERT_ATTR) == INVERT_ATTR)
     {
      cur_attr |= NORM_ATTR;  cur_attr &= ~INVERT_ATTR;
     }
    else
     {
      cur_attr |= INVERT_ATTR;	cur_attr &= ~NORM_ATTR;
     }
    ToXY (VP_WORK, SYMB, x, y, &cur_symb);
    ++x;
   }
  cur_attr = i;
 }

/*[]************************************************************[]*/
/*[]                Сохранение  окна  в  памяти			[]*/
/*[]************************************************************[]*/
char far *SaveWnd (uint beg_x, uint beg_y, uint width, uint height)
 {
  register int i, j;
  char far *ptr;

  width+=2;  height++;
  ptr = (char far *) farmalloc ((width+1) * (height+1) * 2);
  for (i = 0; i <= height; i++)		/* y	*/
   {
    for (j = 0; j <= width; j++)	/* x	*/
     {
      FromXY (VP_WORK, SYMB, beg_x+j, beg_y+i, NULL);
      *(ptr++) = _AL;  *(ptr++) = _AH;
     }
   }
  return (ptr);
 }

/*[]************************************************************[]*/
/*[]              Восстановление  окна  из  памяти		[]*/
/*[]************************************************************[]*/
void RestoreWnd (char far *ptr, uint beg_x, uint beg_y, uint width, uint height)
 {
  register int i, j;

  width+=2;  height++;
  for (i = height; i >= 0; i--)
   {
    for (j = width; j >= 0; j--)
     {
      cur_attr = *(--ptr);  tmp_c = *(--ptr);
      ToXY (VP_WORK, SYMB, beg_x+j, beg_y+i, (char *) &tmp_c);
     }
   }
  farfree (ptr);
 }

/*[]************************************************************[]*/
/*[] Отображение  сообщения  и  загрузка  предыдущего  в  стек	[]*/
/*[]************************************************************[]*/
void PushInf (uchar num)
 {
  ++cur_num_inf;	/*             инкремент указателя		*/
  *cur_num_inf = num;	/*    сохранение текущего сообщения в стеке	*/
  Inform ();
 }

/*[]************************************************************[]*/
/*[] Выгрузка  сообщения  из  стека  и  отображение  сообщения  []*/
/*[]************************************************************[]*/
void PopInf (void)
 {
  --cur_num_inf;
  Inform ();
 }

/*[]************************************************************[]*/
/*[]                 Отображение  сообщения			[]*/
/*[]************************************************************[]*/
void Inform (void)
 {
  tmp_c = ' ';  ToXY (VP_WORK, 72, X_INFO_WND+1, Y_INFO_WND+1, &tmp_c);
  ToXY (VP_WORK, LINE, X_INFO_WND+inf_msg [*cur_num_inf].inf_pos, Y_INFO_WND+1,
	(uchar *) inf_msg [*cur_num_inf].inf_ptr);
 }

/*[]************************************************************[]*/
/*[]                   Опрос  возврата  в  DOS			[]*/
/*[]************************************************************[]*/
void ExitToDos (void)
 {
  char far *ptr_exit;

  if (sign.wnd_processout)  return;
  ptr_exit = SaveWnd (ARG_WND_EXIT);
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_EXIT);
  ToXY (VP_WORK, LINE, 10, 9, "Действительно желаете закончить работу и возвратиться в DOS?");
  PushInf (INFEXIT);
  if (GetKey (NO_KEYS) == END_KEY)  sign.exit = ON;
  RestoreWnd (ptr_exit, ARG_WND_EXIT);
  PopInf ();
 }

/*[]************************************************************[]*/
/*[]             Предупреждающий  звуковой  сигнал  		[]*/
/*[]************************************************************[]*/
void Squeak (void)
 {
  sound (2500);  delay (80);
  sound (1000);  delay (50);
  nosound ();
 }

/*[]************************************************************[]*/
/*[]                Ошибочный  звуковой  сигнал  		[]*/
/*[]************************************************************[]*/
void Signal (void)
 {
  sound (400);  delay (100);  nosound ();  delay (50);
  sound (450);  delay (100);  nosound ();  delay (50);
  sound (500);  delay (100);  nosound ();  delay (50);
  sound (250);  delay (350);  nosound ();
 }

/*[]************************************************************[]*/
/*[]			 Опрос  пользователя			[]*/
/*[]************************************************************[]*/
uint Ask (char reg)
 {
  int i;
  char far *ptr_ask;
  static struct
	  {
	   uchar pos;
	   char *ptr;
	   void (*func) ();
	  } str_msg [] =
	  {
	   {X_ASK_WND+3,  "Желаете сохранить в файле TLT.TDB проведенные изменения?",   SaveTlphDB  },
	   {X_ASK_WND+10, "Загрузить телефонную базу данных из файла?",			LoadTlphDB  },
	   {X_ASK_WND+10, "Действительно желаете очистить базу данных?",                InitTlphDB  },
	   {X_ASK_WND+7,  "Желаете удалить файл временного хранения TLT.INF?",		UnlinkInfFile},
	   {X_ASK_WND+3,  "Файл TLT.INF уже существует! Передать его в канал связи?",	DrivOutChann}
	  };

  ptr_ask = SaveWnd (ARG_WND_ASK);
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_ASK);
  ToXY (VP_WORK, LINE, str_msg [reg].pos, Y_MENU_WND+8, (uchar *) str_msg [reg].ptr);
  PushInf (INFASK);
  while (TRUE)
   {
    i = GetKey (NO_KEYS);
    if (i == INS_KEY)
     {
      (*str_msg [reg].func) ();
      i = _DL;
      switch (reg)
       {
	case SAVE:
	case LOAD:
	 sign.change = OFF;
	 break;
	case INIT:
	 sign.change = ON;
       }
      break;
     }
    if (i == ESC_KEY)  break;
    Squeak ();
   }
  RestoreWnd (ptr_ask, ARG_WND_ASK);
  PopInf ();
  return (i);
 }

/*[]************************************************************[]*/
/*[]                Обработка  ошибочной  ситуации		[]*/
/*[]************************************************************[]*/
void Error (char num_err)
 {
  uint fr = 580;
  uchar far *ptr_err;

  SetVideoPage (WORK_WND);
  ptr_err = SaveWnd (ARG_WND_ERR);
  ClrWnd (VP_WORK, SINGLE_FRAME, SHADOW_ON, ARG_WND_ERR);
  cur_attr = BLINK_ATTR | INVERT_ATTR;
  ToXY (VP_WORK, LINE, 32, Y_ERR_WND, " В н и м а н и е ");
  cur_attr = NORM_ATTR;
  ToXY (VP_WORK, LINE, err_msg [num_err].err_pos, Y_ERR_WND+1, (uchar *) err_msg [num_err].err_ptr);
  if (sign.harderr)
   {
    ToXY (VP_WORK, LINE, 50, Y_ERR_WND+1, (char *) &arr_harderr);
    sign.harderr = OFF;
   }
  PushInf (INFANYKEY);
  if (sign.wnd_processout)
   {
    sign.kbhit = OFF;
    while (!sign.kbhit)
     {
      fr = (fr == 580) ? 780 : 580;
      sound (fr);  delay (400);
     }
    nosound ();
   }
  else
   {
    Signal ();
    _AX = 0x0c07;  Intr (0x21);		/* очистка буфера и ожидание нажатия клавиши	*/
   }
  *(ptr_kbd_head + 1) = *ptr_kbd_head;
  RestoreWnd (ptr_err, ARG_WND_ERR);
  PopInf ();
 }
