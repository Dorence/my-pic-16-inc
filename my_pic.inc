#ifndef _MY_PIC_INC_
#define	_MY_PIC_INC_

; version 0.02.4 2020.08.13 11pm

#ifdef BOOTLOADER
    #define BLOFFSET 0x200
#else
    #define BLOFFSET 0
    __CONFIG _CONFIG3, _WDTCPS_WDTCPS_31 & _WDTE_OFF & _WDTCWS_WDTCWS_7 & _WDTCCS_SC
#endif

;<editor-fold defaultstate="collapsed" desc="var">
; shared varibales
.var.mypic.shr udata_shr
    A0 res 1h
    A1 res 1h
if 0
    A2 res 1h
    A3 res 1h
    V0 res 1h
    V1 res 1h
    S0 res 1h
    S1 res 1h
    T0 res 1h
    T1 res 1h
endif
    TMP res 1h ; used by lib

if 0
; lib used variables
.var.mypic.lib udata
    z0 res 1h
    z1 res 1h
    z2 res 1h
    z3 res 1h
    z4 res 1h
    z5 res 1h
    z6 res 1h
    z7 res 1h

; user used variables
.var.mypic.usr udata
    t2 res 1h ; T0, T1 for udata_shr
    t3 res 1h
    t4 res 1h
    t5 res 1h
    t6 res 1h
    t7 res 1h
    t8 res 1h
    t9 res 1h
endif
;</editor-fold>

;<editor-fold defaultstate="collapsed" desc="movff, movff_w, movff_b">
; mov file to file
movff macro x, y
    movf x, w
    movwf y
endm

; sav file to file, W safe
movff_w macro x, y
    movwf TMP ; store W
    movf x, w
    movwf y
    movfw TMP
endm

; mov file to file, with banksel
movff_b macro x, y
    banksel x
    movf x, w
    banksel y
    movwf y
endm
;</editor-fold>

;<editor-fold defaultstate="collapsed" desc="movlf, movlf_w, movlf_b">
; mov literal to file
movlf macro x, y
    movlw x
    movwf y
endm

; mov literal to file, W safe
movlf_w macro x, y
    movwf TMP ; store W
    movlw x
    movwf y
    movf TMP, w
endm

; mov literal to file, with banksel
movlf_b macro x, y
    movlw x
    banksel y
    movwf y
endm
;</editor-fold>

;<editor-fold defaultstate="collapsed" desc="jeq, jeql, jez">
; skip next instruction when x == y
jeq macro x, y
    movf x, w ; w = x
    xorwf y, w ; w ^= y, Z = 1 if w == y
    btfss STATUS, Z ; skip when Z == 1
endm

; skip next instruction when x == yi, yi is literal
jeql macro x, yi
    movf x, w; w = x
    xorlw yi ; w ^= yi, Z = 1 if w == yi
    btfss STATUS, Z ; skip when Z == 1
endm

; skip next instruction when x == 0
jez macro x
    movf x, w ; w = x, Z = 1 if x == 0
    btfss STATUS, Z ; skip when Z == 1
endm
;</editor-fold>

;<editor-fold defaultstate="collapsed" desc="jne, jnel, jnz">
; skip next instruction when x != y
jne macro x, y
    movf y, w ; w = y
    subwf x, w ; x - w, Z = 0 if w != x
    btfsc STATUS, Z ; skip when Z == 0
endm

; skip next instruction when x != yi, yi is literal
jnel macro x, yi
    movlw yi ; w = yi
    subwf x, w ; x - w, Z = 0 if w != x
    btfsc STATUS, Z ; skip when Z == 0
endm

; skip next instruction when x != 0
jnz macro x
    movf x, w ; w = x, Z = 0 if x != 0
    btfsc STATUS, Z ; skip when Z == 0
endm
;</editor-fold>

;<editor-fold defaultstate="collapsed" desc="jlt, jltl, jlz_s">
; @note 0 is smaller than any other number
; skip next instruction when x < y
jlt macro x, y
    movf y, w ; w = y
    subwf x, w ; x - w, C = 0 if w > x
    btfsc STATUS, C ; skip when C = 0
endm

; skip next instruction when x < yi, yi is literal
jltl macro x, yi
    movlw yi ; w = yi
    subwf x, w ; x - w, C = 0 if w > x
    btfsc STATUS, C ; skip when C == 0
endm

; skip next instruction when 0x80 <= x <= 0xff
jlz_s macro x
    btfss x, d'7'
endm
;</editor-fold>

;<editor-fold defaultstate="collapsed" desc="jgt, jgtl, jgz, jgz_s">
; @note 0xff is greater than any other number
; skip next instruction when x > y
jgt macro x, y
    jlt y, x
endm

; skip next instruction when x > yi, yi is literal
jgtl macro x, yi
    movf x, w ; w = x
    sublw yi ; w = yi - w, C = 0 if w > yi
    btfsc STATUS, C ; skip when C == 0
endm

; skip next instruction when x > 0
; @note unsigned x > 0 means x != 0
jgz macro x
    jnz x
endm

; skip next instruction when 0x00 < x <= 0x7f
jgz_s macro x
    btfsc x, d'7' ; skip if 0x00 <= x <= 0x7f
	goto $ + 3 ; not skip
    movf x, w ; w = x, Z = 0 if x != 0
    btfsc STATUS, Z ; skip when Z = 0
endm
;</editor-fold>

;<editor-fold defaultstate="collapsed" desc="jge, jgel">
; skip next instruction when x >= y
jge macro x, y
    movf y, w ; w = y
    subwf x, w ; x - w, C = 1 if x >= w
    btfss STATUS, C ; skip when C = 1
endm

; skip next instruction when x >= yi, yi is literal
jgel macro x, yi
    movlw yi ; w = yi
    subwf x, w ; x - w, C = 1 if x >= w
    btfss STATUS, C ; skip when C == 1
endm
;</editor-fold>

;<editor-fold defaultstate="collapsed" desc="pow2, pow2_f, pow2com, pow2com_f">
.code.math.pow2 code
; w = 1 << w, where 0 <= w <= 7
pow2: brw
POW2 dt 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80

; w = 1 << x , where 0 <= x <= 7
pow2_f macro x
    movf x, w
    call pow2
endm

; w = ~(1 << w), where 0 <= w <= 7
pow2com macro
    call pow2
    movwf TMP
    comf TMP, w
endm

; w = ~(1 << x) , where 0 <= x <= 7
pow2com_f macro x
    movf x, w
    pow2com
endm
;</editor-fold>

;<editor-fold defaultstate="collapsed" desc="swapwf, inctol, dectol">
; swap w and x(reg)
swapwf macro x
    xorwf x, f
    xorwf x, w
    xorwf x, f
endm

; x = (x < yi) ? (x + 1) : 0, where 0 <= x < yi, yi is literal
inctol macro x, yi
    incf x, f ; x ++
    jnel x, yi
	clrf x
endm

; x = (x > 0) ? (x - 1) : (yi - 1), yi is literal
dectol macro x, yi
    jez x
	goto $ + 3
    movlw yi
    movwf x
    decf x, f
endm
;</editor-fold>

;<editor-fold defaultstate="collapsed" desc="SEG.TABLE SEG.x">
; decimal
SEG.0 equ b'0111111'
SEG.1 equ b'0000110'
SEG.2 equ b'1011011'
SEG.3 equ b'1001111'
SEG.4 equ b'1100110'
SEG.5 equ b'1101101'
SEG.6 equ b'1111101'
SEG.7 equ b'0000111'
SEG.8 equ b'1111111'
SEG.9 equ b'1101111'

; other decimal
SEG.tail7 equ b'0100111' ; 7 with tail

; uppercase, use lower if no upper
SEG.A equ b'1110111'
SEG.b equ b'1111100'
SEG.C equ b'0111001'
SEG.d equ b'1011110'
SEG.E equ b'1111001'
SEG.F equ b'1110001'
SEG.G equ b'0111001'
SEG.H equ b'1110110'
SEG.I equ b'0110000'
SEG.J equ b'0011110'
; no K/k
SEG.L equ b'0111000'
; no M/m
SEG.N equ b'0110111'
SEG.o equ b'1011100'
SEG.P equ b'1110011'
SEG.q equ b'1100111'
SEG.r equ b'1010000'
; S/s <= 5
SEG.t equ b'1111000'
SEG.U equ b'0111110'
; no V/v, W/w, X/x
SEG.y equ b'1101110'
; no Z/z

; other lowercase
SEG.c equ b'1011000'
SEG.h equ b'1110100'
SEG.i equ b'0001000'
SEG.n equ b'1010100'
SEG.u equ b'0011100'

; letter
SEG.Gamma  equ b'0110001' ; \Gamma
SEG.gamma  equ b'1000100' ; \gamma
SEG.lambda equ b'1010111' ; \lambda
SEG.mu     equ b'1110010' ; \mu


; glyph
SEG.null equ b'0000000' ; blank
SEG.tbar equ b'0000001' ; top bar
SEG.cbar equ b'1000000' ; center bar
SEG.bbar equ b'0001000' ; bottom bar
SEG.teq  equ b'1000001' ; top equal
SEG.beq  equ b'1001000' ; bottom equal
SEG.seq  equ b'0001001' ; seperated equal
SEG.trib equ b'1001001' ; triple bar
SEG.deg  equ b'1100011' ; degree
SEG.squo equ b'0100000' ; single quote
SEG.lquo equ b'0000010' ; single left quote (dash on right)
SEG.dquo equ b'0100010' ; double quote
SEG.rpar equ b'0001111' ; right parenthesis
SEG.ques equ b'1010011' ; question mark

SEG.TABLE: brw
_SEGTABLE_1 dt SEG.0, SEG.1, SEG.2, SEG.3, SEG.4, SEG.5, SEG.6, SEG.7
_SEGTABLE_2 dt SEG.8, SEG.9, SEG.A, SEG.b, SEG.C, SEG.d, SEG.E, SEG.F
;</editor-fold>

#endif ; end _MY_PIC_INC_
