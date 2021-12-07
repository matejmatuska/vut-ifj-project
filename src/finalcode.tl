.IFJcode21
DEFVAR GF@tmp1
DEFVAR GF@tmp2
DEFVAR GF@tmp3
JUMP __MAIN__

LABEL error_label
CLEARS
EXIT int@8

#start of the function write
LABEL write
POPS GF@tmp1
LABEL loop2
GT GF@tmp2 GF@tmp1 int@0
JUMPIFEQ end_loop2 GF@tmp2 bool@false 
POPS GF@tmp2
SUB GF@tmp1 GF@tmp1 int@1
WRITE GF@tmp2
JUMP loop2
LABEL end_loop2
RETURN
#end of the function write

#start of the function reads
LABEL reads
PUSHFRAME
DEFVAR LF@retval1
MOVE LF@retval1 string@
READ LF@retval1 string
POPFRAME
RETURN
#end of the function reads

#start of the function readi
LABEL readi
PUSHFRAME
DEFVAR LF@retval1
MOVE LF@retval1 int@0
READ LF@retval1 int
POPFRAME
RETURN
#end of the function readi

#start of the function readn
LABEL readn
PUSHFRAME
DEFVAR LF@retval1
MOVE LF@retval1 float@0x1.2666666666666p+0
READ LF@retval1 float
POPFRAME
RETURN
#end of the function readn

#start of the function tointeger
LABEL tointeger
PUSHFRAME
DEFVAR LF@retval1
MOVE LF@retval1 nil@nil
DEFVAR LF@f
MOVE LF@f  LF@%1
FLOAT2INT LF@retval1 LF@f
POPFRAME
RETURN
#end of the function tointeger

#start of the function substr
LABEL substr
PUSHFRAME
DEFVAR LF@retval1
MOVE LF@retval1 string@
DEFVAR LF@s
MOVE LF@s  LF@%1
DEFVAR LF@i
MOVE LF@i  LF@%2
DEFVAR LF@j
MOVE LF@j  LF@%3
DEFVAR LF@length
DEFVAR LF@tmp
DEFVAR LF@sign
STRLEN LF@length LF@s
GT LF@tmp LF@i LF@length
JUMPIFEQ return_label3 LF@tmp bool@true
GT LF@tmp LF@j LF@length
JUMPIFEQ return_label3 LF@tmp bool@true
GT LF@tmp LF@i LF@j
JUMPIFEQ return_label3 LF@tmp bool@true
SUB LF@i LF@i int@1
SUB LF@j LF@j int@1
LABEL loop
GETCHAR LF@sign LF@s LF@i
CONCAT LF@retval1 LF@retval1 LF@sign
ADD LF@i LF@i int@1
GT LF@tmp LF@i LF@j
JUMPIFEQ return_label3 LF@tmp bool@true
JUMP loop
LABEL return_label3
POPFRAME
RETURN
#end of the function substr

#start of the function ord
LABEL ord
PUSHFRAME
DEFVAR LF@retval1
MOVE LF@retval1 nil@nil
DEFVAR LF@s
MOVE LF@s  LF@%1
DEFVAR LF@i
MOVE LF@i  LF@%2
DEFVAR LF@length
DEFVAR LF@tmp
STRLEN LF@length LF@s
GT LF@tmp LF@i LF@length
JUMPIFEQ return_label2 LF@tmp bool@true
SUB LF@i LF@i int@1
STRI2INT LF@retval1 LF@s LF@i
LABEL return_label2
POPFRAME
RETURN
#end of the function ord

#start of the function chr
LABEL chr
PUSHFRAME
DEFVAR LF@retval1
MOVE LF@retval1 nil@nil
DEFVAR LF@i
MOVE LF@i  LF@%1
DEFVAR LF@tmp
LT LF@tmp LF@i int@0
JUMPIFEQ return_label1 LF@tmp bool@true
GT LF@tmp LF@i int@255
JUMPIFEQ return_label1 LF@tmp bool@true
INT2CHAR LF@retval1 LF@i
LABEL return_label1
POPFRAME
RETURN
#end of the function chr

#start of the function factorial
LABEL factorial
PUSHFRAME
DEFVAR LF@n
MOVE LF@n  LF@%1
DEFVAR LF@retval1
MOVE LF@retval1 int@0
DEFVAR LF@n1
MOVE LF@n1 int@0
PUSHS LF@n
PUSHS int@1
POPS GF@tmp1
TYPE GF@tmp3 GF@tmp1
JUMPIFEQ error_label GF@tmp3 string@nil
POPS GF@tmp2
TYPE GF@tmp3 GF@tmp2
JUMPIFEQ error_label GF@tmp3 string@nil
PUSHS GF@tmp2
PUSHS GF@tmp1
SUBS

POPS LF@n1
PUSHS LF@n
PUSHS int@2
POPS GF@tmp1
TYPE GF@tmp3 GF@tmp1
JUMPIFEQ error_label GF@tmp3 string@nil
POPS GF@tmp2
TYPE GF@tmp3 GF@tmp2
JUMPIFEQ error_label GF@tmp3 string@nil
PUSHS GF@tmp2
PUSHS GF@tmp1
LTS
POPS GF@tmp1
JUMPIFEQ end_if1 GF@tmp1 bool@false
PUSHS int@1
POPS LF@retval1
DEFVAR LF@tmp
MOVE LF@tmp int@0
CREATEFRAME
PUSHS LF@n1
DEFVAR TF@%1
POPS TF@%1
CALL factorial
MOVE LF@tmp TF@retval1
PUSHS LF@n
PUSHS LF@tmp
POPS GF@tmp1
TYPE GF@tmp3 GF@tmp1
JUMPIFEQ error_label GF@tmp3 string@nil
POPS GF@tmp2
TYPE GF@tmp3 GF@tmp2
JUMPIFEQ error_label GF@tmp3 string@nil
PUSHS GF@tmp2
PUSHS GF@tmp1
MULS

POPS LF@retval1
JUMP end_else1
LABEL end_if1
LABEL end_else1
POPFRAME
RETURN
#end of the function factorial

#start of the function main
LABEL main
PUSHFRAME
CREATEFRAME
PUSHS string@Zadejte\032cislo\032pro\032vypocet\032faktorialu:\032
PUSHS int@1
CALL write
DEFVAR LF@a
MOVE LF@a int@0
CREATEFRAME
CALL readi
MOVE LF@a TF@retval1
PUSHS LF@a
EQS

NOTS

POPS GF@tmp1
JUMPIFEQ end_if2 GF@tmp1 bool@false
PUSHS LF@a
PUSHS int@0
POPS GF@tmp1
TYPE GF@tmp3 GF@tmp1
JUMPIFEQ error_label GF@tmp3 string@nil
POPS GF@tmp2
TYPE GF@tmp3 GF@tmp2
JUMPIFEQ error_label GF@tmp3 string@nil
PUSHS GF@tmp2
PUSHS GF@tmp1
LTS
POPS GF@tmp1
JUMPIFEQ end_if3 GF@tmp1 bool@false
CREATEFRAME
PUSHS string@Faktorial\032nejde\032spocitat!
PUSHS string@\010
PUSHS int@2
CALL write
DEFVAR LF@vysl
MOVE LF@vysl int@0
CREATEFRAME
PUSHS LF@a
DEFVAR TF@%1
POPS TF@%1
CALL factorial
MOVE LF@vysl TF@retval1
CREATEFRAME
PUSHS string@Vysledek\032je\032
PUSHS LF@vysl
PUSHS string@\010
PUSHS int@3
CALL write
JUMP end_else3
LABEL end_if3
LABEL end_else3
CREATEFRAME
PUSHS string@Chyba\032pri\032nacitani\032celeho\032cisla!\010
PUSHS int@1
CALL write
JUMP end_else2
LABEL end_if2
LABEL end_else2
POPFRAME
RETURN
#end of the function main

LABEL __MAIN__
CREATEFRAME
PUSHFRAME
CREATEFRAME
CALL main
POPFRAME
CLEARS
EXIT int@0
