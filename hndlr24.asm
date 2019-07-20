		title	Hовый обработчик критических ошибок hardware

;[]****************************************************[]
;[]        Междугородные  телефонные  разговоры        []
;[]			 Версия  2.4		       []
;[]						       []
;[]   Hndlr24.asm:  обработчик  критических  ошибок    []
;[]****************************************************[]

NULL		equ	0				; символ заканчивающий строку
ON		equ	1
ERRFATAL	equ	13				; номер фатальной ошибки в err_msg
BIT_TYPE_DEVICE	equ	80h				; бит, указывающий тип устройства
NAME_DRV	equ	10				; смещение имени драйвера в станд. заголовке
SPACE		equ	20h				; символ, заканчивающий имя драйвера послед. устр-ва

		public	_New_0x24
		extrn	_Error:near			; функция сообщения об ошибке
		extrn	_arr_harderr:byte		; массив для хранения имени устр-ва, вызвавшего ошибку
		extrn	_sign:byte			; структура признаков

		model	small
		.code

;[]*****************************************************[]
;[]  INT 24h:  новый  обработчик  критических  ошибок	[]
;[]  Вызов: 	        вызов DOS			[]
;[]  Входные параметры: ah    - тип устройства,		[]
;[]		        al    - номер устройства,	[]
;[]		        di    - код ошибки,		[]
;[]	 	        bp:si - заголовок драйвера	[]
;[]  Возвращ. значение: нет (возврат в программу)	[]
;[]*****************************************************[]
_New_0x24	proc
		mov	di, @data
		mov	ds, di
		xor	di, di
		and	ah, BIT_TYPE_DEVICE		; ошибка в устр-ве прямого доступа или в послед. устр-ве?
		jz	BlockDevice			; устройство прямого доступа
		mov	es, bp
NextChar:	cmp	byte ptr es:[si+NAME_DRV], SPACE; символ Space?
		jz	SignalError
		mov	bl, es:[si+NAME_DRV]
		mov	_arr_harderr [di], bl
		inc	si
		inc	di
		jmp	NextChar
BlockDevice:	mov	_arr_harderr, al		; сохранение номера устройства прямого доступа
		add	_arr_harderr, 'A'
		inc	di
SignalError:    mov	byte ptr _arr_harderr   [di], ':'
		mov	byte ptr _arr_harderr+1 [di], NULL
		mov	byte ptr [_sign], ON		; установка пpизнака ошибки sign.harderr
		mov	al, ERRFATAL
		push	ax
		call    _Error				; сигнализация об ошибке
		add	sp, 8				; удаление со стека входного параметра и адреса возврата в DOS
		pop	ax				; восстановление регистров
		pop     bx
		pop     cx
		pop     dx
		pop     si
		pop     di
		pop     bp
		pop     ds
		pop     es
		iret
_New_0x24	endp

		end
