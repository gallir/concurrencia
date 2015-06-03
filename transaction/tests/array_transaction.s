	.file	"array_transaction.c"
	.comm	counter,16,16
	.section	.rodata
.LC0:
	.string	"End %d counter: %d\n"
	.text
	.globl	count
	.type	count, @function
count:
.LFB496:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movq	%rdi, -40(%rbp)
	movq	$25000000, -8(%rbp)
	movq	-40(%rbp), %rax
	movl	(%rax), %eax
	movl	%eax, -20(%rbp)
	movq	$0, -16(%rbp)
	jmp	.L2
.L9:
	movl	$43, %edi
	movl	$0, %eax
	call	_ITM_beginTransaction
	andl	$2, %eax
	testl	%eax, %eax
	je	.L3
	movl	$0, -24(%rbp)
.L5:
	cmpl	$3, -24(%rbp)
	jg	.L4
	movl	-24(%rbp), %eax
	cltq
	movl	counter(,%rax,4), %eax
	leal	1(%rax), %edx
	movl	-24(%rbp), %eax
	cltq
	movl	%edx, counter(,%rax,4)
	addl	$1, -24(%rbp)
	jmp	.L5
.L4:
	call	_ITM_commitTransaction
	jmp	.L6
.L3:
	movl	$0, -24(%rbp)
	jmp	.L7
.L8:
	movl	-24(%rbp), %eax
	cltq
	salq	$2, %rax
	addq	$counter, %rax
	movq	%rax, %rdi
	call	_ITM_RU4
	addl	$1, %eax
	movl	-24(%rbp), %edx
	movslq	%edx, %rdx
	salq	$2, %rdx
	addq	$counter, %rdx
	movl	%eax, %esi
	movq	%rdx, %rdi
	call	_ITM_WU4
	addl	$1, -24(%rbp)
.L7:
	cmpl	$3, -24(%rbp)
	jle	.L8
	call	_ITM_commitTransaction
.L6:
	addq	$1, -16(%rbp)
.L2:
	movq	-16(%rbp), %rax
	cmpq	-8(%rbp), %rax
	jl	.L9
	movl	counter(%rip), %edx
	movl	-20(%rbp), %eax
	movl	%eax, %esi
	movl	$.LC0, %edi
	movl	$0, %eax
	call	printf
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE496:
	.size	count, .-count
	.section	.rodata
	.align 8
.LC1:
	.string	"Counter value: %d Expected: %d\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB497:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$80, %rsp
	movl	%edi, -68(%rbp)
	movq	%rsi, -80(%rbp)
	movl	$0, -56(%rbp)
	jmp	.L11
.L12:
	movl	-56(%rbp), %eax
	cltq
	movl	-56(%rbp), %edx
	movl	%edx, -48(%rbp,%rax,4)
	leaq	-48(%rbp), %rax
	movl	-56(%rbp), %edx
	movslq	%edx, %rdx
	salq	$2, %rdx
	addq	%rax, %rdx
	leaq	-32(%rbp), %rax
	movl	-56(%rbp), %ecx
	movslq	%ecx, %rcx
	salq	$3, %rcx
	addq	%rcx, %rax
	movq	%rdx, %rcx
	movl	$count, %edx
	movl	$0, %esi
	movq	%rax, %rdi
	call	pthread_create
	movl	%eax, -52(%rbp)
	addl	$1, -56(%rbp)
.L11:
	cmpl	$3, -56(%rbp)
	jle	.L12
	movl	$0, -56(%rbp)
	jmp	.L13
.L14:
	movl	-56(%rbp), %eax
	cltq
	movq	-32(%rbp,%rax,8), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	pthread_join
	addl	$1, -56(%rbp)
.L13:
	cmpl	$3, -56(%rbp)
	jle	.L14
	movl	counter(%rip), %eax
	movl	$100000000, %edx
	movl	%eax, %esi
	movl	$.LC1, %edi
	movl	$0, %eax
	call	printf
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE497:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 4.8.2-19ubuntu1) 4.8.2"
	.section	.note.GNU-stack,"",@progbits
