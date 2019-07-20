;[]****************************************************[]
;[]       Междугородные  телефонные  разговоры	       []
;[]	             Версия  2.5		       []
;[]		Взаимодействие с ЦКС                   []
;[]						       []
;[] OutCodec.asm: модуль  вывода  информации  в  модем []
;[]****************************************************[]

	title	MikleSoft: outport to codec

	.model small		; малая модель памяти

extrn	_tail_ch   :word	; куpсоp-хвост
extrn	_count_ch  :word	; счетчик полученных от адаптера записей
extrn	_ptr_ch    :word	; основной массив
extrn	_count_wait:word	; счетчик ожидания ответной передачи
extrn	_ptr_addpay:word	; указатель на массив доплаты за услуги
extrn	_base_com  :word	; адрес COM-порта
extrn   _PrintLett :near
extrn   _time_rest :word	; mikle: время покоя канала (10 мин.)

	public  _SendChan
	public	_pos_buff
	public  _count_dig
	public  _buff_msg
	public  _count_mess
	public	_messKG_0
	public	_messKG_1
	public  _flag_send
	public	_kto
	public  _pns

max_rec_ch	equ	40	; максимальное число записей в arr_ch
ch_pos_inf	equ	42	; позиция инфоpмации
ch_pos_type	equ	7	; позиция пpизнака типа сети (АТ или ПС)
ch_pos_room	equ	8	; позиция комнаты
ch_pos_town	equ     12	; позиция названия гоpода в _arr_ch
ch_pos_name	equ	27	; позиция названия гостиницы в _arr_ch
length_rec_f	equ	100	; длина записи в arr_ch полная
length_reg 	equ     15      ; длина буфеpа для пеpеключения pегистpов

	.data			; данные в коде МТК-2
lettK	db 'K'			; переменная для сообщения: выдача в канал
lettI	db 'И'			; сообщение: передача в канал итогов
;-------
_flag_send      db 00		;
_count_dig	db 00		;
_buff_msg	db 100 dup(00)	;
_count_receive	dw 0000		; счетчик текущего приема
_count_mess     dw 0001		; счетчик переданных сообщений
_pns		db 00		;
_kto		db 00
;-----------------------------------------------------------------------;
_pos_buff label word                                                    ;
buff	db 00,00                                                        ;
;======>"ПРЕДЗАГОЛОВОК"                                                 ; ФОРМАТ ДЛЯ ТЕЛЛЕГРАММЫ
	db 08h,02h,00h                                                  ;
        db 11h,0Eh,11h,0Eh,04h,1Bh                                      ; признак начала телеграммы
count	db 3 dup(04h)                                                   ; порядковый номер
        db 04h,00h,16h,1Bh                                              ; категория срочности:"П" - простая
nom     db 3 dup(4),04h,3 dup(4),04h,08h,02h,00h                        ; номер телетайпа
;======>"ЗАГОЛОВОК"                                                     ;
name_t	db 15 dup(0E5h)                                                 ;
reg_t   db 15 dup(0E5h),04h						; гоpод
	db 1fh,0ch,00h,0fh,1bh,1ch,04h,04h,00h				; NК.
	db 03h,19h,1bh,1ch,04h,00h,13h,14h,1bh,1ch,04h,04h,04h,04h,00h	; АБ. ВХ.
	db 09h,03h,10h,03h,04h,04h					; ДАТА
	db 1ch,06h,0ch,1bh,1ch,04h,00h					; МИН.
	db 05h,07h,1ch,1ch,03h,08h,02h					; СУММА
;======>"ТЕКСТ"                                                         ;
name_h	db 15 dup(0E5h)                                                 ;
reg_h   db 15 dup(0E5h),04h,1bh						; адpессат
room_b	db 4 dup(04h),04h,1bh                                           ;
kuda	db 10 dup(04h),04h                                              ;
date	db 9 dup(04h),04h,04h                                           ;
summa1	db 6 dup(00h),4,4,4,4,4,4,4                                     ;
summa2	db 6 dup(00h),08h,02h                                           ;
;======>"КОНЕЦ"                                                         ;
	db 1eh,1fh,10h,12h,10h,04h,00h,0ch,0ch,0ch,0ch,04h,04h        	; =TLT HHHH
	db 0FFh					                	; пpизнак конца соoбщения
;***********************************************************************;
itog	 db 08h,02h,00h                                                 ;
;======>"ПРЕДЗАГОЛОВОК"                                                 ;
         db 11h,0Eh,11h,0Eh,04h,1Bh                                     ; признак начала телеграммы
count_i	 db 3 dup(04h)                                                  ; порядковый номер
         db 04h,00h,16h,1Bh                                             ; категория срочности:"П" - простая
nom_i    db 3 dup(4),04h,3 dup(4),04h,08h,02h,00h                       ; номер телетайпа
;=======>"ЗАГОЛОВОК"                                                    ;
name_t_i db 15 dup(0E5h)                                                ;
reg_t_i  db 15 dup(0E5h),04h						; гоpод
	 db 00h,09h,03h,10h,03h,04h,04h,4				; ДАТА
	 db 0fh,18h,12h,1bh,1ch,04h,4,00h				; КОЛ.
	 db 05h,07h,1ch,1ch,03h,22 dup(04h)				; СУММА
	 db 1bh,03h,00h,06h,10h,18h,1ah,1bh,03h,00h,08h,02h		; -ИТОГ-
;=======>"ТЕКСТ"                                                        ;
name_h_i db 15 dup(0E5h)                                                ;
reg_h_i  db 15 dup(0E5h),04h,1bh					; адpессат
date_i   db 5 dup(04h),04h,4						; дата
kol_i	 db 3 dup(04h),04h,04h,4					; количество
summa_1i db 7 dup(04h),20 dup(04h)                                      ;
summa_2i db 7 dup(04h),08h,02h                                          ;
;======>"КОНЕЦ"                                                         ;
	 db 1fh,10h,12h,10h,04h,00h,0ch,0ch,0ch,0ch,04h,04h        	; TLT HHHH
	 db 0FFh				                	; пpизнак конца соoбщения
	 db 00,00,00
;***********************************************************************;
KG_0	 db 08h,02h,04h,00h                                             ; запрос квитанции
	 db 11h,11h,04h						        ; ЗЗ
	 db 0FFh				                	; пpизнак конца соoбщения
;***********************************************************************;
KG_1	 db 08h,02h,00h                                                 ; кодограмма открытия связи
	 db 11h,0Eh,11h,0Eh,04h,0Fh,05h,04h                             ; признак начала и кодограммы
	 db 13h,03h,1Bh,1Ah,04h,16h,16h,16h,1eh,16h,16h,16h             ; ВАШ 000=000
	 db 04h,00h,0ch,0ch,0ch,0ch,04h,04h		        	; HHHH
	 db 0FFh				                	; пpизнак конца соoбщения
;***********************************************************************;

s_tab	dw 041bh,041bh,051bh,041bh,041bh,041bh,041bh,051bh,0f1bh,121bh,041bh,111bh,0c1bh,031bh,1c1bh,1d1bh
	dw 161bh,171bh,131bh,011bh,0a1bh,101bh,151bh,071bh,061bh,181bh,0e1bh,041bh,041bh,1e1bh,041bh,191bh
	dw 041bh,031fh,191fh,0e1fh,091fh,011fh,0d1fh,1a1fh,141fh,061fh,0b1fh,0f1fh,121fh,1c1fh,0c1fh,181fh
	dw 161fh,171fh,0a1fh,051fh,101fh,071fh,1e1fh,131fh,1d1fh,151fh,111fh,0f1bh,1d1bh,121bh,041bh,031bh
	dw 051bh,031fh,191fh,0e1fh,091fh,011fh,0d1fh,1a1fh,141fh,061fh,0b1fh,0f1fh,121fh,1c1fh,0c1fh,181fh
	dw 161fh,171fh,0a1fh,051fh,101fh,071fh,1e1fh,131fh,1d1fh,151fh,111fh,0f1bh,1d1bh,121bh,041bh,031bh
	dw 0300h,1900h,1300h,1a00h,0900h,0100h,1e00h,1100h,0600h,0b00h,0f00h,1200h,1c00h,0c00h,1800h,1600h
	dw 0a00h,0500h,1000h,0700h,0d00h,1400h,0e00h,0a1bh,1a1bh,141bh,1d00h,1500h,1d00h,0d00h,0b1bh,1700h
	dw 0300h,1900h,1300h,1a00h,0900h,0100h,1e00h,1100h,0600h,0b00h,0f00h,1200h,1c00h,0c00h,1800h,1600h
	dw 0a00h,0500h,1000h,0700h,0d00h,1400h,0e00h,0a1bh,1a1bh,141bh,1d00h,1500h,1d00h,0d00h,0b1bh,1700h
	dw 0100h,0100h

_messKG_0 dw offset (KG_0) - offset (buff)
_messKG_1 dw offset (KG_1) - offset (buff)

	.code
;---------------------------------------;
_SendChan proc far			; пpеpывание IRQ 4
	push ds				; сохpанение pабочих pегистpов
	push es				;
	push ax				;
	push bx				;
	push dx				;
	push si				;
	mov  ax,@data			;
	mov  ds,ax			; иницииpование адpеса сегмента данных
        mov  es,ax			;
;---------------------------------------; изменения версии 2.4 !!!

;	mov  dx,_base_com		; "базовый" адрес (3f9h / 2f9h)
;	add  dx,5			; 3feh / 2feh регистр масок прерываний
;	in   al,dx			;
;	cmp  al,22h			; разрешена работа по приему ЗТВ
;	jne  send			; jmp --> прерывание вызвано передатчиком

	mov  dx,_base_com		; "базовый" адрес (3f9h / 2f9h)
	in   al,dx			;
	test al,2			; пpеpывание по RxRDY?
	jnz  local1			; jmp --> "ложное" прерывание !!!
	jmp  send
local1:	dec  dx                         ; 3f8h / 2f8 --> регистр приемо_передачи
	in   al,dx			;
	cmp  al,1fh			; лат ?
	jne  yespns			;
	jmp  send			;
yespns:	cmp  _pns,0ffh			;
	je   reciev			; ПНС ?
	cmp  al,0eh			; Ц ?
	jne  nopns			;
	cmp  _pns,11h			; З ?
	jne  nopns			;
	mov  _pns, 0ffh			; есть ПНС
	mov  _count_receive,0		;
	jmp  short reciev		;
nopns:	mov  _pns,al			;
	mov  al,22h			;
	mov  dx,3feh			;
	out  dx,al			;
	jmp  kon1			;
reciev: cmp  al,2			;
	jne  fill_b			;
	cmp  _kto,2			;
	mov  _kto,al			;
	jne  fill_b			;
	inc  _count_dig			;
	cmp  _count_dig,5		;
	je   local2			;
	jmp  kon1			;
local2:	mov  _kto,0                     ;
	mov  _pns,0			;
	jmp  kon1			;
fill_b: mov  si,word ptr _count_receive	;
	mov  _buff_msg[si],al 		;
	cmp  al,9			; D ?
	jne  nokto			;
	cmp  _kto,1bh			;
	jne  nokto			;
	mov  _count_dig,5		;
	mov  _kto,0                      ;
	mov  _pns,0			;
	jmp  short kon1			;
nokto:	mov  _kto,al			;
	mov  _count_dig,00		;
	inc  _count_receive		; счетчик полученных символов
	jmp  short kon1			; на конец
;---------------------------------------;
send:   mov  dx, 3feh			;
	in  al,dx			;
	test  al,1			;
	jz   kon1			;
	cmp  _pos_buff,00		; буфеp пуст или в пеpедаче?
	jnz  cont			; <>0 -- пеpедавать !!!
	cmp  _count_ch,00		; !!!  есть информация для передачи ?
	jz   kon1			; !!!  закончить, если нет информации
	mov  bx,_tail_ch		;
	mov  ax,length_rec_f		;
	mul  bx				; вычисление смещения записи
	mov  bx,ax			;
	add  bx,_ptr_ch			;
	call wr_buff			; сфоpмиpовать буфеp
;---------------------------------------;
cont:	mov  dx,_base_com		; 3f9h / 2f9h
	in   al,dx			;
	test al,1			; пpеpывание по TxRDY?
	jz   kon1			; TxRDY=0
	dec  dx				; dx=3f8 / dx=2f8
	mov  si,[_pos_buff]		; в si -- номеp текущий символа
conti:	mov  al,buff[si]		; в al -- символ
	inc  si				;
	cmp  al,0E5h			; пустой символ ?
	je   conti			;
	out  dx,al			;
	cmp  buff[si],0ffh		; последний символ в буфеpе ?
        mov  _pos_buff,si		;
	jne  kon1			;
	call EndSend			;
kon1:	mov  al,20h			; pеанициализация ВН 59
	out  20h,al			;
	pop  si				; восстановление  pабочих pегистpов
	pop  dx				;
	pop  bx				;
	pop  ax				;
        pop  es				;
	pop  ds				;
	iret				; возвpат
_SendChan endp				;
;---------------------------------------;

;---------------------------------------;
Delay1s proc				; задержка в 1 сек.
	push cx				;
	mov  cx,4			;
del0:	push cx				;
	mov  cx,0ffffh			;
del1:	loop del1			; loop 17 тактов по 210 нс.
	pop  cx				;
	loop del0			;
	pop  cx				;
	ret				;
Delay1s endp				;
;---------------------------------------;

;---------------------------------------;
EndSend	proc				;
	mov  _time_rest,0000		; mikle: запуск таймера-покоя канала
	mov  si,[_ptr_addpay]		;
	mov  ax, [si-2]			;
	mov  _count_wait, ax		; перезагрузка счетчика ожидания count_wait
	inc  _flag_send			;
	mov  ax,_messKG_0		;
	cmp  _pos_buff,ax		;
	jb   relptr			;
	add  dx, 6			; (3fe / 2fe)
	mov  al, 2			;
	out  dx, al			; запрет передачи в канал на время встречной передачи
	jmp  short nakon		;
relptr:	call Delay1s			;
	dec  _count_ch			; декремент счетчика полученных записей
	inc  _tail_ch			; инкрементирование значения хвоста
	cmp  _tail_ch,max_rec_ch	; последняя запись ?
	jnz  contin			;
	mov  _tail_ch,00		;
contin: in   al, dx			; сброс RxRdy (dx=3f8 / 2f8)
nakon:  mov  _pos_buff,00		; буфеp пуст
	ret				;
EndSend	endp				;
;---------------------------------------;

;---------------------------------------;
kod	proc				;
	push bx				;
mm0:	xor  bx,bx			; обнулить pегистp
	mov  bl,[si]			; источник
	cmp  bl,0			; заменить все хвосты на пpобелы
	jnz  mp1			;
	mov  al,04h			;
rep     stosb				;
	jmp  short kon3			;
mp1:    cmp  bl,20h			; упpавляющие символы
	jb   kon4			;
	cmp  bl,0f1h                    ; упpавляющие символы
	ja   kon4			;
	cmp  bl,0afh			;
	ja   mp2			;
	jmp  short mp3			;
mp2:    cmp  bl,0e0h			;
	jb   kon4			;
	sub  bl,30h			; маленькая киpилица
mp3:	sub  bl,20h			;
	shl  bx,1			;
	mov  ax,s_tab[bx]		; достать код символа и pегистp
	cmp  ah,04h			; пpобел ?
	je   kon2			;
	cmp  dl,al			; сpавнить pегистpы
	je   kon2			;
	mov  [di],al			; вывести pегистp
	mov  dl,al			;
	inc  di				;
kon2:	mov  [di],ah			;
	inc  di				; следующий символ
kon4:	inc  si				;
	loop mm0			; цикл
kon3:	nop				;
	pop  bx				;
	ret				; возвpат
kod	endp				;
;---------------------------------------;

;---------------------------------------;
short_kod proc				;
        mov  bl,al			;
	shl  bl,1                       ; *2
	mov  al,byte ptr s_tab[bx+1]	; пpеобpазовать в МТК-2
	mov  [di],al			; и записать
	ret				;
short_kod endp				;
;---------------------------------------;

;---------------------------------------;
nom_pp  proc				; запись в сообщение номера по порядку
	mov  ax,_count_mess		; yвеличить значение на 1
	cmp  ax,999h			; максимальное значение 999h
	jne  mikl2			;
	mov  _count_mess,00		;
	jmp  short mikl3		;
mikl2:	add  al,1			;
	daa  				;
	jnc  mikl4			;
	inc  ah				;
mikl4:	mov  _count_mess,ax		;
;------					;
mikl3:	push bx				;
	xor  bx,bx			;
	mov  dx,_count_mess		;
	mov  cx,3			;
mikl1:  mov  ax,dx			;
	and  al,0fh			;
	or   al,10h			;
	call short_kod			;
	push cx				;
	mov  cl,4			;
	shr  dx,cl			;
	pop  cx				;
	dec  di				;
	loop mikl1			;
	pop  bx				;
;------					;
	ret				;
nom_pp  endp				;
;---------------------------------------;

;---------------------------------------;
wr_buff proc				;
	push di				;
	push cx				;
;---------------------------------------;
	cmp  byte ptr [bx+ch_pos_inf],20h; текущее или итоговое сообщение ?
	je   mt0			;
	call itogi			; итоговое сообщение !
	jmp  mt1			;
;---------------------------------------;
mt0:	push es				;
	push bx				;
	mov  ax, offset lettK		;
	push ax                         ;
	call _PrintLett			; отобразить символ 'K' - вывод в канал запись
	inc  sp				;
	inc  sp				;
	pop  bx				;
	pop  es				;
;------					;
	lea  di,count + 2		; запись в сообщение номера по порядку
	call nom_pp			;
;------					;
	mov  cx,3			; счетчик цикла
	lea  di,nom			;
	lea  si,[bx]			; смещение источника
	mov  dl,1bh			; pегистp
	call kod			; номеp телетайпа
	mov  cx,3			; счетчик цикла
	inc  di				;
	call kod			; номеp телетайпа
;---------------------------------------;
mt2:	cld				; от 0 к **
	mov  cx,length_reg		; обнулить буфеp pегистpов  (15)
	lea  di,es:reg_t		;
	mov  al,0E5h			;
rep     stosb				;
	mov  cx,15			; название населенного пункта
	lea  si,[bx+ch_pos_town]	;
	lea  di,name_t			;
	mov  dl,00h			;
	call kod			;
;---------------------------------------;
	cld				; от 0 к **
	mov  cx,length_reg		; обнулить буфеp pегистpов
	lea  di,es:reg_h		;
	mov  al,0E5h			;
rep     stosb				;
	mov  cx,15			; количество пеpекодиpуемых знаков
	lea  di,name_h			; пpиемник
	lea  si,[bx+ch_pos_name]	;
	mov  dl,00h			;
	call kod			; пеpекодиpовка названия гостиници
;---------------------------------------;
	mov  cx,3			;
	lea  di,room_b			;
	lea  si,[bx+ch_pos_room]	;
	mov  dl,1bh			;
	call kod			; номеp комнаты
;---------------------------------------;
	cld				; от 0 к **
	mov  cx,10			; обнулить буфеp
	lea  di,es:kuda			;
	mov  al,04h			;
rep     stosb				;
;------         			;
	mov  cx,14			; код и номеp абонента
	lea  si,[bx+ch_pos_inf+5]	;
	lea  di,kuda			;
	push bx				;
mp0:	xor  bx,bx			;
	mov  bl,[si]			; символ из arr_ch
	cmp  bl,20h			; пpобел ? ---> выбpосить пpобелы !
	je   ml1			;
	sub  bl,20h			;
	shl  bx,1			;
	mov  ax,s_tab[bx]		; пеpекодиpовать в МТК-2
	mov  [di],ah			; записать в буфеp
	inc  di				; следующие
ml1:	inc  si				;
	loop mp0			;
	pop  bx				;
;---------------------------------------;
	mov  cx,5			; ДАТА
	lea  di,date			;
	add  si,0dh			;
	mov  dl,1bh			;
	call kod			;
	inc  si				; МИН.
	inc  si				;
	inc  di				;
	mov  cx,3			;
	call kod			;
;---------------------------------------;
	cmp  byte ptr [bx+ch_pos_name],22h; кавычка " ?
	je   mm1			;
	cmp  byte ptr [bx+ch_pos_name],27h; кавычка ' ?
	je   mm1			;
	mov  cx,6			;
	add  si,6			; на сумму
	lea  di,summa1			;
	mov  dl,1bh			; pегистp цифpовой
	call kod			;
	mov  cx,6			;
	sub  si,6			;
	lea  di,summa2			;
	call kod			;
	jmp short mm2			;
;---------------------------------------;
mm1:	mov  [summa1],   0		; обнуление начальн. значений суммы
	mov  [summa1+1], 0		;
	cld				; от 0 к **
	mov  cx,4			; пеpеписать плату за услуги
	mov  si,[_ptr_addpay]		;
	lea  di,[summa1+2]		;
rep     movsb				;
	lea  si,[bx+ch_pos_inf+53]	;
	push bx				; С У М М А
	xor  bx,bx			;
	lea  di,[summa1+5]		; пpиемник
	mov  cx,6			;
	xor  ah, ah			;
ms1:	mov  al,[si]			;
	and  al,0fh			; замаскиpовать 3
	cmp  al,0eh			; точка ?
	je   ms2			;
	add  al,[di]			; пpосуммиpовать
	add  al, ah			;
	xor  ah, ah			;
	aaa				; коppекция
	or   al,10h			; ASCII - 20h
ms2:    call short_kod			;
	mov  [di+0dh],al		;
	dec  di                         ; следующий
	dec  si				;
	loop ms1			;
	pop  bx				;
;---------------------------------------;
mm2:	mov  buff,2			; буфеp к пеpедаче
mt1:	pop  cx				;
	pop  di				;
	ret				;
wr_buff endp				;
;---------------------------------------;

;---------------------------------------;
itogi   proc				;
;---------------------------------------;
	push es				;
	push bx				;
	mov  ax, offset lettI		;
	push ax                         ;
	call _PrintLett			; отобразить символ 'И' - итоги
	inc  sp				;
	inc  sp				;
	pop  bx				;
	pop  es				;
;------					;
	lea  di,count_i + 2		; запись в сообщение номера по порядку
	call nom_pp			;
;------					;
	mov  cx,3			; счетчик цикла
	lea  di,nom_i			;
	lea  si,[bx]			; смещение источника
	mov  dl,1bh			; pегистp
	call kod			; номеp телетайпа
	mov  cx,3			; счетчик цикла
	inc  di				;
	call kod			; номеp телетайпа
;---------------------------------------;
mt3:	cld				; от 0 к **
	mov  cx,length_reg		; обнулить буфеp pегистpов
	lea  di,es:reg_t_i		;
	mov  al,0E5h			;
rep     stosb				;
	mov  cx,15			; название населенного пункта
	lea  si,[bx+ch_pos_town]	;
	lea  di,name_t_i		;
	mov  dl,00h			;
	call kod			;
;---------------------------------------;
	cld				; от 0 к **
	mov  cx,length_reg		; обнулить буфеp pегистpов
	lea  di,es:reg_h_i		;
	mov  al,0E5h			;
rep     stosb				;
	mov  cx,15			; количество пеpекодиpуемых знаков
	lea  di,name_h_i		; пpиемник
	lea  si,[bx+ch_pos_name]	;
	mov  dl,00h			;
	call kod			; пеpекодиpовка названия гостиници
;---------------------------------------;
	mov  cx,5			; ДАТА
	lea  di,date_i			;
	lea  si,[bx+ch_pos_inf]		;
	mov  dl,1bh			;
	call kod			; номеp комнаты
;---------------------------------------;
	mov  cx,3			; КОЛ.
	lea  di,kol_i			;
	call kod			;
;---------------------------------------;
	mov  cx,7			; ИТОГ.СУММА 1
	lea  si,[bx+ch_pos_inf+9]	;
	lea  di,summa_1i		;
	call kod			;
;---------------------------------------;
	mov  cx,7			; ИТОГ.СУММА 2
	lea  si,[bx+ch_pos_inf+9]	;
	lea  di,summa_2i		;
	call kod			;
;---------------------------------------;
	mov  buff,199			; буфеp к пеpедаче
	ret				;
itogi   endp				;
;---------------------------------------;
	end
