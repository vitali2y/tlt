/*[]****************************************************[]*/
/*[]  	    Междугородные  телефонные  разговоры	[]*/
/*[]		         Версия  2.4			[]*/
/*[]							[]*/
/*[] InpAdapt.c: модуль  ввода  информации  с  адаптера	[]*/
/*[]****************************************************[]*/

/*--------------------------------------------------------*/
/*        Внимание !!!  Ошибка  BIOS  ЕС  1840 !!!	  */
/*--------------------------------------------------------*/
/* Не используйте вектор прерывания 1Ch в малых моделях   */
/*     памяти по причине некорректного сохранения и 	  */
/*          восстановления регистров SS и SP!		  */
/*--------------------------------------------------------*/

/*[]****************************************************[]*/
/*[]       INT  08h:  новый  обработчик  IRQ 0		[]*/
/*[]          (опрос  адаптера  по таймеру)	 	[]*/
/*[]****************************************************[]*/
void interrupt InpAdap (void)
 {
  register uint i;
  static   uchar c;

#ifdef WORK_WITH_ADAP				/* 	    работа с адаптером или в отладочном режиме?		*/

  while ((!(inp (0x220) & BIT_INP_FORBID)) &&	/*		          ввод запрещен или			*/
	   (inp (0x220) & BIT_BUFF_MODIF))	/* 	      не был модифицирован буфер - возврат		*/
   {
    outp (0x220, ADAP_CLR_MAIN_STAT);		/* 		команда: сброс бит основного состояния		*/
    outp (0x223, BIT_BUFF_MODIF);		/* 		   сброс бита: буфер модифицирован		*/
    ExecComm;
    outp (0x220, ADAP_READ_FIRST);		/*                   команда: чтение байта			*/
    outp (0x221, 0);
    outp (0x222, 0);
    ExecComm;
    arr_info [0] = inp (0x223);
    i = 0;
    while (++i != LENGTH_REC)			/* 	    чтение строки информации из адаптера		*/
     {
      outp (0x220, ADAP_READ_NEXT);		/* 		команда: чтение байта + 1			*/
      ExecCommVerify;				/*	был обмен с УУ во время чтения информации - возврат	*/
      arr_info [i] = inp (0x223);
     }
    arr_info [LENGTH_REC-1] = NULL;
    if ((inp (0x220) & BIT_INP_FORBID) ||	/*    перезаписана или переписывается информация из адаптера?	*/
	(inp (0x220) & BIT_BUFF_MODIF))  break;

#else

  static uchar count_attempt = 0;

  while (TRUE)
   {
    memcpy ((char *) &arr_info, "  6  111    2222222    1111111  23. 5  555  66  111.22  9", sizeof (arr_info));
    if (count_attempt == 0x7b) count_attempt = count_attempt + 5;	/* "{" --> хитрый символ !!!	*/
    arr_info [2]  = count_attempt;
    arr_info [23] = count_attempt;
    arr_info [29] = count_attempt;
    arr_info [53] = count_attempt++;
    if (arr_info [18] == '9')  arr_info [18] = ' ';

#endif

    Ntlph = atol ((uchar *) &arr_info [ADAP_POS_N_TLPH]);
    if (count_tmp_inf <= MAX_TMP_INF)		/*        подготовить инф-цию к выдаче в верхнее окно?		*/
     {
      memcpy (&arr_tmp_inf [head_tmp_inf], &arr_info, sizeof (arr_info));
      if (++head_tmp_inf == MAX_TMP_INF)  head_tmp_inf = 0;
      count_tmp_inf++;
     }
    if ((count_dst >= MAX_REC_DST) ||		/*      заполнен любой массив: arr_dst, arr_ch - возврат	*/
	(count_ch  >= MAX_REC_CH ))
     {
      c = 0x13;  PrintLett (&c);		/*           сообщение: переполнение рабочих массивов		*/
      break;
     }
    (*FindFunc) ();				/* поиск номера телефона: по категории вызова или двоичный поиск*/
    if (!_DL)  break;				/*          DL = OFF - не гостиничн. номер - возврат		*/


    i = 0;
    arr_info [POS_TYPE_MONTH] = type_month;
    do						/*      передача приемникам (dst) требуемой информации		*/
     {
      ptr_ch->inf [head_ch].msg.info [i] = ptr_dst->msg [head_dst] [i] = arr_info [i];
     }
    while (++i != LENGTH_REC);
    c = db->tlph [pos].ind_addr;
    ltoa (db->addr [c].Ntty, (char *) &ptr_ch->inf [head_ch].Ntty [0], 10);
    ptr_ch->inf [head_ch].type = db->addr [c].type;
    ltoa (db->tlph [pos].Nroom, (char *) &ptr_ch->inf [head_ch].room [0], 10);
    memcpy (&ptr_ch->inf [head_ch].town [0], &db->addr [c].town, 15);
    memcpy (&ptr_ch->inf [head_ch].addr [0], &db->addr [c].addr, 15);
    ++count_dst;				/*        увеличение указателей head и счетчиков count		*/
    if (++head_dst == MAX_REC_DST)  head_dst = 0;
    if (db->status.chann)
     {
      ++count_ch;
      if (++head_ch == MAX_REC_CH)  head_ch = 0;
     }
    arr_info [ADAP_POS_SUM+3] = arr_info [ADAP_POS_SUM+4];
    arr_info [ADAP_POS_SUM+4] = arr_info [ADAP_POS_SUM+5];
    arr_info [ADAP_POS_SUM+5] = NULL;
    ptr_total->quan [c]++;
    ptr_total->sum  [c] += atol ((uchar *) &arr_info [ADAP_POS_SUM]);
    break;
   }
  if (db->status.chann && count_wait)		/*  закончена ответная передача из канала - разрешить передачу	*/
    --count_wait;
  time_rest++;					/*  mikle:      инкремент счетчика до ЗЗ "пробел"		*/

/*--------------------------------------------------------*/
/*        		  Внимание !!!			  */
/*--------------------------------------------------------*/
/*  В TLT.EXE не использован оригинальный вектор прерыва- */
/* ния 8h ЕС 1840  (0AD2:00AB устанавливает MS DOS 3.30,  */
/* F000:FEA5 - BIOS ПЗУ) по причине "ненормальной" рабо-  */
/* ты программы в максимально нагруженном режиме!	  */
/*--------------------------------------------------------*/

  if (!(--*ptr_time_motor))			/* 	   подошло время до выключения мотора дисковода?	*/
   {
    *ptr_motor_off &= 0xf0;			/* 		выключение моторов дисковых устройств		*/
    outp (0x3f2, 0x0c);				/* 		     сброс контроллера дисковода		*/
   }
  disable ();
  outp (0x20, 0x20);				/*  			конец простого прерывания		*/
 }

/*[]****************************************************[]*/
/*[] 	      Быстрый  поиск  номера  телефона		[]*/
/*[]              Метод:  двоичный  поиск		[]*/
/*[] Возвращаемый параметр: DL = TRUE - телефон найден,	[]*/
/*[]   DL = FALSE - телефон по базе данных не найден	[]*/
/*[]****************************************************[]*/
void SearchNTlph (ulong ts, uint *curr_pos)
 {
  register uint rel_pos;
  uchar zero = OFF;

  *curr_pos = rel_pos = beg_pos_tlph; /* MAX_TLPH / 2; */
  while (TRUE)
   {
    if (!rel_pos)  zero = ON;
    rel_pos >>= 1;
    if (db->tlph [*curr_pos].Ntlph == ts)
     {
      _DL = TRUE;  return;	/*    найден номер телефона	*/
     }
    if (zero)  break;
    if (db->tlph [*curr_pos].Ntlph < ts)
      *curr_pos += (rel_pos + 1);
    else
      *curr_pos -= (rel_pos + 1);
   }
  _DL = FALSE;			/*   не найден номер телефона	*/
 }

/*[]****************************************************[]*/
/*[]      "Стандартный"  поиск  номера  телефона	[]*/
/*[]****************************************************[]*/
void BinarySearchFunc (void)
 {
  SearchNTlph (Ntlph, (uint *) &pos);
 }

/*[]****************************************************[]*/
/*[]  Поиск  номера  телефона  по  категории  абонента	[]*/
/*[]****************************************************[]*/
void CategFunc (void)
 {
  if (arr_info [ADAP_POS_CATEG] == db->status.categ)	/* гостиничный номер телефона?	*/
	SearchNTlph (Ntlph, (uint *) &pos);
  else  _DL = OFF;
 }
