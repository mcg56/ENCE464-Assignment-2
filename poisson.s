	.file	"poisson.c"
	.text
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 8
.LC0:
	.string	"Starting solver with:\nn = %i\niterations = %i\nthreads = %i\ndelta = %f\n"
	.align 8
.LC1:
	.string	"Error: ran out of memory when trying to allocate %i sized cube\n"
	.section	.rodata.str1.1,"aMS",@progbits,1
.LC2:
	.string	"Finished solving."
	.text
	.p2align 4
	.globl	poisson_neumann
	.type	poisson_neumann, @function
poisson_neumann:
.LFB50:
	.cfi_startproc
	endbr64
	cmpb	$0, debug(%rip)
	pushq	%r13
	.cfi_def_cfa_offset 16
	.cfi_offset 13, -16
	pushq	%r12
	.cfi_def_cfa_offset 24
	.cfi_offset 12, -24
	movl	%edi, %r12d
	pushq	%rbp
	.cfi_def_cfa_offset 32
	.cfi_offset 6, -32
	jne	.L9
.L2:
	movl	%r12d, %ebp
	movl	$8, %esi
	imull	%r12d, %ebp
	imull	%r12d, %ebp
	movslq	%ebp, %rbp
	movq	%rbp, %rdi
	call	calloc@PLT
	movq	%rbp, %rdi
	movl	$8, %esi
	movq	%rax, %r13
	call	calloc@PLT
	movq	%rax, %rdi
	testq	%r13, %r13
	je	.L6
	testq	%rax, %rax
	je	.L6
	call	free@PLT
	cmpb	$0, debug(%rip)
	jne	.L10
	movq	%r13, %rax
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%r12
	.cfi_def_cfa_offset 16
	popq	%r13
	.cfi_def_cfa_offset 8
	ret
	.p2align 4,,10
	.p2align 3
.L9:
	.cfi_restore_state
	movl	%ecx, %r8d
	movl	$1, %eax
	movl	%edx, %ecx
	movl	%edi, %edx
	leaq	.LC0(%rip), %rsi
	movl	$1, %edi
	cvtss2sd	%xmm0, %xmm0
	call	__printf_chk@PLT
	jmp	.L2
	.p2align 4,,10
	.p2align 3
.L10:
	leaq	.LC2(%rip), %rdi
	call	puts@PLT
	movq	%r13, %rax
	popq	%rbp
	.cfi_remember_state
	.cfi_def_cfa_offset 24
	popq	%r12
	.cfi_def_cfa_offset 16
	popq	%r13
	.cfi_def_cfa_offset 8
	ret
.L6:
	.cfi_restore_state
	movq	stderr(%rip), %rdi
	movl	%r12d, %ecx
	movl	$1, %esi
	xorl	%eax, %eax
	leaq	.LC1(%rip), %rdx
	call	__fprintf_chk@PLT
	movl	$1, %edi
	call	exit@PLT
	.cfi_endproc
.LFE50:
	.size	poisson_neumann, .-poisson_neumann
	.section	.rodata.str1.1
.LC3:
	.string	"--help"
	.section	.rodata.str1.8
	.align 8
.LC4:
	.string	"Usage: poisson [-n size] [-i iterations] [-t threads] [--debug]"
	.align 8
.LC5:
	.string	"Error: expected size after -n!\n"
	.align 8
.LC6:
	.string	"Error: expected iterations after -i!\n"
	.align 8
.LC7:
	.string	"Error: expected threads after -t!\n"
	.section	.rodata.str1.1
.LC8:
	.string	"--debug"
	.section	.rodata.str1.8
	.align 8
.LC9:
	.string	"Error: n should be an odd number!\n"
	.align 8
.LC10:
	.string	"Error: failed to allocated source term (n=%i)\n"
	.section	.rodata.str1.1
.LC13:
	.string	"%0.5f "
	.section	.text.startup,"ax",@progbits
	.p2align 4
	.globl	main
	.type	main, @function
main:
.LFB51:
	.cfi_startproc
	endbr64
	pushq	%r15
	.cfi_def_cfa_offset 16
	.cfi_offset 15, -16
	pushq	%r14
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -24
	pushq	%r13
	.cfi_def_cfa_offset 32
	.cfi_offset 13, -32
	pushq	%r12
	.cfi_def_cfa_offset 40
	.cfi_offset 12, -40
	pushq	%rbp
	.cfi_def_cfa_offset 48
	.cfi_offset 6, -48
	pushq	%rbx
	.cfi_def_cfa_offset 56
	.cfi_offset 3, -56
	subq	$40, %rsp
	.cfi_def_cfa_offset 96
	cmpl	$1, %edi
	jle	.L32
	movl	$10, (%rsp)
	movl	%edi, %r13d
	movq	%rsi, %rbx
	leal	-1(%rdi), %ebp
	movl	$1, 12(%rsp)
	movl	$1, %r15d
	movl	$5, %r12d
	jmp	.L27
	.p2align 4,,10
	.p2align 3
.L33:
	movl	$7, %ecx
	movq	%rax, %rsi
	leaq	.LC3(%rip), %rdi
	repz cmpsb
	seta	%dl
	sbbb	$0, %dl
	testb	%dl, %dl
	je	.L14
	cmpl	$45, %r10d
	jne	.L18
	cmpb	$110, 1(%rax)
	jne	.L18
	cmpb	$0, 2(%rax)
	jne	.L18
	cmpl	%r15d, %ebp
	je	.L40
	leaq	8(%rbx,%r9), %r14
	movl	$10, %edx
	xorl	%esi, %esi
	addl	$1, %r15d
	movq	(%r14), %rdi
	call	strtol@PLT
	movl	%eax, %r12d
	movq	(%r14), %rax
.L18:
	cmpb	$45, (%rax)
	jne	.L21
	cmpb	$105, 1(%rax)
	jne	.L21
	cmpb	$0, 2(%rax)
	jne	.L21
	cmpl	%r15d, %ebp
	je	.L41
	addl	$1, %r15d
	movl	$10, %edx
	xorl	%esi, %esi
	movslq	%r15d, %rax
	leaq	(%rbx,%rax,8), %r14
	movq	(%r14), %rdi
	call	strtol@PLT
	movl	%eax, (%rsp)
	movq	(%r14), %rax
.L21:
	cmpb	$45, (%rax)
	jne	.L24
	cmpb	$116, 1(%rax)
	jne	.L24
	cmpb	$0, 2(%rax)
	jne	.L24
	cmpl	%r15d, %ebp
	je	.L42
	addl	$1, %r15d
	movl	$10, %edx
	xorl	%esi, %esi
	movslq	%r15d, %rax
	leaq	(%rbx,%rax,8), %r14
	movq	(%r14), %rdi
	call	strtol@PLT
	movl	%eax, 12(%rsp)
	movq	(%r14), %rax
.L24:
	movq	%rax, %rsi
	movl	$8, %ecx
	leaq	.LC8(%rip), %rdi
	repz cmpsb
	seta	%al
	sbbb	$0, %al
	testb	%al, %al
	jne	.L26
	movb	$1, debug(%rip)
.L26:
	addl	$1, %r15d
	cmpl	%r15d, %r13d
	jle	.L43
.L27:
	movslq	%r15d, %rax
	leaq	0(,%rax,8), %r9
	movq	(%rbx,%rax,8), %rax
	movzbl	(%rax), %r10d
	cmpl	$45, %r10d
	jne	.L33
	cmpb	$104, 1(%rax)
	jne	.L33
	cmpb	$0, 2(%rax)
	jne	.L33
.L14:
	leaq	.LC4(%rip), %rdi
	call	puts@PLT
	xorl	%eax, %eax
.L11:
	addq	$40, %rsp
	.cfi_remember_state
	.cfi_def_cfa_offset 56
	popq	%rbx
	.cfi_def_cfa_offset 48
	popq	%rbp
	.cfi_def_cfa_offset 40
	popq	%r12
	.cfi_def_cfa_offset 32
	popq	%r13
	.cfi_def_cfa_offset 24
	popq	%r14
	.cfi_def_cfa_offset 16
	popq	%r15
	.cfi_def_cfa_offset 8
	ret
.L43:
	.cfi_restore_state
	testb	$1, %r12b
	je	.L44
.L12:
	movl	%r12d, %ebx
	movl	$8, %esi
	imull	%r12d, %ebx
	imull	%r12d, %ebx
	movslq	%ebx, %rdi
	call	calloc@PLT
	movq	%rax, 16(%rsp)
	testq	%rax, %rax
	je	.L45
	movl	%ebx, %eax
	movl	$2, %ecx
	movq	16(%rsp), %rbx
	movl	%r12d, %edi
	cltd
	movss	.LC12(%rip), %xmm0
	idivl	%ecx
	movq	.LC11(%rip), %rcx
	movl	(%rsp), %edx
	movq	%rbx, %rsi
	cltq
	movq	%rcx, (%rbx,%rax,8)
	movl	12(%rsp), %ecx
	call	poisson_neumann
	movq	%rax, 24(%rsp)
	movq	%rax, %rbx
	testl	%r12d, %r12d
	jle	.L29
	movl	%r12d, %eax
	xorl	%r15d, %r15d
	leaq	.LC13(%rip), %rbp
	sarl	%eax
	imull	%r12d, %eax
	imull	%r12d, %eax
	cltq
	leaq	(%rbx,%rax,8), %rax
	movslq	%r12d, %rbx
	movq	%rax, (%rsp)
	salq	$3, %rbx
	.p2align 4,,10
	.p2align 3
.L30:
	movq	(%rsp), %r14
	xorl	%r13d, %r13d
	.p2align 4,,10
	.p2align 3
.L31:
	movsd	(%r14), %xmm0
	movq	%rbp, %rsi
	movl	$1, %edi
	addq	%rbx, %r14
	movl	$1, %eax
	call	__printf_chk@PLT
	movl	%r13d, %esi
	addl	$1, %r13d
	cmpl	%r13d, %r12d
	jne	.L31
	movl	$10, %edi
	movl	%esi, 12(%rsp)
	call	putchar@PLT
	movl	12(%rsp), %esi
	addq	$8, (%rsp)
	leal	1(%r15), %eax
	cmpl	%esi, %r15d
	je	.L29
	movl	%eax, %r15d
	jmp	.L30
.L44:
	movq	stderr(%rip), %rcx
	movl	$34, %edx
	movl	$1, %esi
	leaq	.LC9(%rip), %rdi
	call	fwrite@PLT
	movl	$1, %eax
	jmp	.L11
.L29:
	movq	16(%rsp), %rdi
	call	free@PLT
	movq	24(%rsp), %rdi
	call	free@PLT
	xorl	%eax, %eax
	jmp	.L11
.L40:
	movq	stderr(%rip), %rcx
	movl	$31, %edx
	movl	$1, %esi
	leaq	.LC5(%rip), %rdi
	call	fwrite@PLT
	movl	$1, %eax
	jmp	.L11
.L41:
	movq	stderr(%rip), %rcx
	movl	$37, %edx
	movl	$1, %esi
	leaq	.LC6(%rip), %rdi
	call	fwrite@PLT
	movl	$1, %eax
	jmp	.L11
.L42:
	movq	stderr(%rip), %rcx
	movl	$34, %edx
	movl	$1, %esi
	leaq	.LC7(%rip), %rdi
	call	fwrite@PLT
	movl	$1, %eax
	jmp	.L11
.L32:
	movl	$1, 12(%rsp)
	movl	$5, %r12d
	movl	$10, (%rsp)
	jmp	.L12
.L45:
	movq	stderr(%rip), %rdi
	movl	%r12d, %ecx
	movl	$1, %esi
	leaq	.LC10(%rip), %rdx
	call	__fprintf_chk@PLT
	movl	$1, %eax
	jmp	.L11
	.cfi_endproc
.LFE51:
	.size	main, .-main
	.local	debug
	.comm	debug,1,1
	.section	.rodata.cst8,"aM",@progbits,8
	.align 8
.LC11:
	.long	0
	.long	1072693248
	.section	.rodata.cst4,"aM",@progbits,4
	.align 4
.LC12:
	.long	1065353216
	.ident	"GCC: (Ubuntu 9.4.0-1ubuntu1~20.04.1) 9.4.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	 1f - 0f
	.long	 4f - 1f
	.long	 5
0:
	.string	 "GNU"
1:
	.align 8
	.long	 0xc0000002
	.long	 3f - 2f
2:
	.long	 0x3
3:
	.align 8
4:
