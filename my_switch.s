.section .text
.global context_switch
context_switch:
//默认支持AT&T
.intel_syntax noprefix
	push ebp
	mov ebp,esp

	//保存现场
	push eax
	push ebx
	push ecx
	push edx
	push esi
	push edi
	pushfd

	//切换

	//当前线程地址放入ebx
	mov ebx,current_task
	//保存当前栈顶到current_task.esp中,ebx+8正好是esp的地址
	mov [ebx+8],esp
	//要切换到的task,ebp+4是函数返回时下一条指令地址，ebp+8正好是函数的入参
	mov ebx,[ebp+8]
	mov current_task,ebx
	//切换栈顶
	mov esp,[ebx+8]

	//恢复
	popfd
	pop edi
	pop esi
	pop edx
	pop ecx
	pop ebx
	pop eax

	pop ebp

	ret


