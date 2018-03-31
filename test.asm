ASSUME CS:CODE,DS:DATA

DATA   SEGMENT
    COUNT    DW  36
    OLD1CH   DW  0, 0
	LOOPCOUNT DW 10
    MSG1     DB  'Hello world!',13,10,'$'
DATA   ENDS

CODE   SEGMENT

BEGIN:

	MOV   AX, 351CH     ;读1C中断向量到ES:BX
    INT   21H
    MOV   AX, DATA
    MOV   DS, AX

    MOV   AX, ES
    MOV   OLD1CH[2], AX ;保存ES:BX
    MOV   OLD1CH, BX
	
	MOV AX,251CH
	MOV DX,SEG INT1CH
	MOV DS,DX
	MOV DX,OFFSET INT1CH
	CLI
	INT 21H
	STI
	
    MOV   AX, 0619H     ;清屏－－用AL=0竟然会荡机
    MOV   BX, 0E00H
    MOV   CX, 0000H
    MOV   DX, 184FH
    INT   10H
	
	;MOV AH,2;设置光标位置
	;MOV BH,0;页号
	;MOV DX,0101H;第一行第一列
	;INT 10H
	
M_LOOP:
	NOP
	NOP
	MOV AX,LOOPCOUNT
	CMP AX,0H
	JNZ M_LOOP
	

EXIT:
	CLI
    MOV   AX, OLD1CH[2] ;恢复原来 1CH 的中断向量
    MOV   DS, AX
    MOV   DX, OLD1CH
    MOV   AX, 251CH
    INT   21H
    ;STI
	MOV   AH, 4CH
    INT   21H
INT1CH PROC
	PUSH DS
	PUSH AX
	PUSH BX
	PUSH CX
	PUSH DX
	
	MOV AX,DATA
	MOV DS,AX
	DEC COUNT
	JNZ EXIT_INT1CH
	DEC LOOPCOUNT
	JZ EXIT
	
	CALL PRINT
	ADD COUNT,36
	
EXIT_INT1CH:
    POP   DX
    POP   CX
    POP   BX
    POP   AX
    POP   DS
    IRET
INT1CH  ENDP

PRINT PROC
	PUSH AX
	PUSH DX
	PUSH BX
	
	MOV AX,DATA
	MOV DS,AX
	MOV AX,LOOPCOUNT
	MOV DL,AL
	ADD DL,30H
	MOV   AH, 2
    INT   21H
    MOV DL,':'
    INT 21H
	
	MOV AX,DATA
	MOV DS,AX
	LEA DX,MSG1
	
	MOV AH,9
	INT 21H
	
	POP BX
	POP DX
	POP AX
	
	RET
PRINT ENDP

CODE ENDS
END BEGIN
	




