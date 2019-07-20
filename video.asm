		title	Mодуль обмена информацией с видеопамятью CGA

;[]*************************************************************[]
;[]		Междугородные  телефонные  разговоры		[]
;[]			     Версия  2.4			[]
;[]								[]
;[]   Video.asm: модуль  обмена  информацией  с  видеопамятью  	[]
;[]*************************************************************[]

NULL		=	0                       ; символ окончания строки
SET_BRIGHT_SYMB	=	'{'			; символ, огранич. слева сообщение с высок. интен-тью
CLR_BRIGHT_SYMB	=	'}'     		; символ, огранич. справа сообщение с высок. интен-тью
HIGH_ATTR	=	8			; атрибут символа с высокой интенсивностью

		locals
		.model	small
		extrn	_cur_attr: byte
		.code

;[]*************************************************************[]
;[]    ToXY:  отображение  в  видеопамять  одного  символа,	[]
;[]  	     последовательности  символов  или  строки		[]
;[]								[]
;[] Вызов:   void ToXY (unsigned int video_seg,			[]
;[]		 	unsigned int regime,			[]
;[]			unsigned char x,			[]
;[]			unsigned char y,			[]
;[]			unsigned char *ptr);			[]
;[] Входные параметры:	[bp+4]  - видеосегмент video_seg,	[]
;[]			[bp+6] 	- режим работы regime,		[]
;[]			[bp+8] 	- x-координата вывода,		[]
;[]			[bp+10] - y-координата вывода,		[]
;[]			[bp+12]	- указатель на строку.		[]
;[] Возвращ. параметры:	нет.                             	[]
;[]*************************************************************[]

						;----------------> regime <-----------------
SYMB		=	1			; SYMB  - выводится одиночный символ
LINE		=	0	         	; LINE  - выводится строка до NULL
						; иначе - количество повторов отображения
						;-------------------------------------------
		public	_ToXY
_ToXY		proc
		push	bp
		mov	bp, sp
		push	di
		push	si
		mov	ax, [bp+4]		; ax = видеосегмент
		mov	es, ax
		mov	al, [bp+8]		; al = x
		xor	ah, ah
		mov	di, ax			; di = x
		mov	bl, [bp+10]		; bl = y
		xor	bh, bh
		mov	cl, 4
		shl	bx, cl			; bx = 16*y
		add	di, bx
		shl	bx, 1
		shl	bx, 1
		add	di, bx			; di = 16*y+64*y+x
		shl	di, 1			; di = смещение символа
		mov	dx, 3dah
		mov	si, [bp+12]		; si = указатель на текущий символ
		mov	cx, [bp+6]		; cx = pежим вывода
		cmp	cx, LINE		; режим - строка ?
		jz	@@3
		mov	bl, [si]
@@1:		call	PrintSymb		; отобразить символ / последовательность символов
		loop	@@1
@@2:		pop	si
		pop	di
		pop	bp
		ret
@@3:		mov	cl, SYMB		; cl - один отображаемый символ
		mov	bx, si
		mov	bl, [bx]		; получение следующего символа
		cmp	bl, NULL		; конец строки ?
		jz	@@2
		cmp	bl, SET_BRIGHT_SYMB	; отображать с высокой интенсивностью ?
		jnz	@@4
		or	[_cur_attr], HIGH_ATTR
		jmp	short @@6
@@4:		cmp	bl, CLR_BRIGHT_SYMB	; закончить отображение с высокой интенсивностью ?
		jnz	@@5
		and	[_cur_attr], not (HIGH_ATTR)
		jmp	short @@6
@@5:		call	PrintSymb
@@6:		inc	si			; инкремент указателя
		jmp	@@3
_ToXY		endp

;[]*************************************************************[]
;[]   PrintSymb:     процедура  отображения  одного  символа	[]
;[]								[]
;[] Входные параметры:	bl - отображаемый символ		[]
;[]			dx - регистр статуса CGA,		[]
;[]			di - смещение символа,			[]
;[]			es - видеосегмент.			[]
;[] Возвращ. параметры:	нет.                           		[]
;[]*************************************************************[]
PrintSymb	proc
		mov	bh, [_cur_attr]		; bh - текущий атрибут отображаемого символа
		cli
@@1:		in	al, dx
		test	al, 8
		jne	@@3
		shr	al, 1
		jb	@@1
@@2:		in	al, dx
		shr	al, 1
		jnb	@@2
@@3:		mov	ax, bx
		stosw				; запись в видеопамять
		sti
		ret
PrintSymb	endp

;[]*************************************************************[]
;[]         FromXY:    получение  одиночного  символа		[]
;[]  и  его  атрибута  или  строки  символов  из  видеопамяти	[]
;[]								[]
;[] Вызов: unsigned int FromXY (unsigned int video_seg,		[]
;[]		 		unsigned int regime,		[]
;[]				unsigned char x,		[]
;[]				unsigned char y,		[]
;[]				unsigned char *ptr);		[]
;[] Входные параметры:	[bp+4]  - видеосегмент video_seg,	[]
;[]			[bp+6]  - режим работы regime,		[]
;[]			[bp+8]  - начальная x-координата,	[]
;[]			[bp+10] - начальная y-координата,	[]
;[]			[bp+12] - принимаемая строка.		[]
;[] Возвращ. параметры: если regime = SYMB (одиночный символ),	[]
;[]			то: ah - атрибут символа и al - символ, []
;[]			иначе содержимое видеопамяти размером	[]
;[]			regime байт сохраняется начиная с	[]
;[]			указателя  ptr.				[]
;[]*************************************************************[]

		public	_FromXY
_FromXY		proc
		push	bp
		mov	bp, sp
		push	si
		push	di
		push	ds
		mov	di, ds			; сохранение текущего сегмента данных в di
		mov	ax, [bp+4]		; ax = видеосегмент
		mov	ds, ax
		mov	al, [bp+8]		; al = x
		xor	ah, ah
		mov	si, ax			; si = x
		mov	bl, [bp+10]		; bl = y
		xor	bh, bh
		mov	cl, 4
		shl	bx, cl			; bx = 16*y
		add	si, bx
		shl	bx, 1
		shl	bx, 1
		add	si, bx			; si = 16*y+64*y+x
		shl	si, 1			; si = смещение символа
		mov	bx, [bp+12]		; bx = адpес пpинимаемой стpоки
		mov	cx, [bp+6]		; cx = regime
		mov	dx, 3dah
@@1:		in	al, dx			; получение статуса 6845
		rcr	al, 1
		jb	@@1
		cli
@@2:		in	al, dx
		rcr	al, 1
		jnb	@@2
		lodsw				; ah = атрибут символа, al = символ
		sti
		cmp	word ptr [bp+6], SYMB	; прочитать одиночный символ или строку?
		jz	@@3
		push	ds			; сохранение текущего видеосегмента
		mov	ds, di			; установка сегмента данных
		mov	[bx], al		; сохранение символа в строке
		inc	bx
		pop	ds
@@3:		loop	@@1
		pop	ds
		pop	di
		pop	si
		pop	bp
		ret
_FromXY		endp
		end
