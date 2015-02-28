	.arch armv6
	.eabi_attribute 27, 3
	.eabi_attribute 28, 1
	.fpu vfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 6
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.file	"stack_cas_freelist.c"
	.global	head
	.bss
	.align	2
	.type	head, %object
	.size	head, 4
head:
	.space	4
	.global	free_nodes
	.align	2
	.type	free_nodes, %object
	.size	free_nodes, 4
free_nodes:
	.space	4
	.text
	.align	2
	.global	push
	.type	push, %function
push:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #8
	str	r0, [fp, #-8]
	str	r1, [fp, #-12]
	ldr	r3, [fp, #-12]
	ldr	r2, [fp, #-8]
	ldr	r2, [r2]
	str	r2, [r3]
	mov	r0, r0	@ nop
.L3:
	mcr	p15, 0, r0, c7, c10, 5
	ldr	r4, [fp, #-8]
	ldrex	r1, [r4]

	ldr	r3, [fp, #-12]
	mov	r2, r3
	ldr	r3, [fp, #-12]
	mov	ip, r3
	ldr	r0, [r2]
.L4:
	cmp	r1, r0
	bne	.L5
	strex	lr, ip, [r4]
	cmp	lr, #0
	bne	.L3
.L5:
	mcr	p15, 0, r0, c7, c10, 5
	moveq	r3, #1
	movne	r3, #0
	cmp	r3, #0
	bne	.L2
	str	r1, [r2]
.L2:
	eor	r3, r3, #1
	uxtb	r3, r3
	cmp	r3, #0
	bne	.L3
	sub	sp, fp, #4
	@ sp needed
	ldmfd	sp!, {fp, pc}
	.size	push, .-push
	.align	2
	.global	pop
	.type	pop, %function
pop:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #16
	str	r0, [fp, #-16]
	ldr	r3, [fp, #-16]
	ldr	r3, [r3]
	str	r3, [fp, #-8]
.L10:
	ldr	r3, [fp, #-8]
	cmp	r3, #0
	bne	.L7
	mov	r3, #0
	b	.L11
.L7:
	mcr	p15, 0, r0, c7, c10, 5
	ldr	r2, [fp, #-16]
	ldrex	r1, [r2]

	ldr	r3, [fp, #-8]
	ldr	r3, [r3]
	mov	ip, r3
	sub	r3, fp, #8
	ldr	r0, [r3]
.L12:
	cmp	r1, r0
	bne	.L13
	strex	lr, ip, [r2]
	cmp	lr, #0
	bne	.L7
.L13:
	mcr	p15, 0, r0, c7, c10, 5
	moveq	r2, #1
	movne	r2, #0
	cmp	r2, #0
	bne	.L9
	str	r1, [r3]
.L9:
	mov	r3, r2
	eor	r3, r3, #1
	uxtb	r3, r3
	cmp	r3, #0
	bne	.L10
	ldr	r3, [fp, #-8]
.L11:
	mov	r0, r3
	sub	sp, fp, #4
	@ sp needed
	ldmfd	sp!, {fp, pc}
	.size	pop, .-pop
	.section	.rodata
	.align	2
.LC0:
	.ascii	"%d malloc\012\000"
	.align	2
.LC1:
	.ascii	"Error in %d it shouldn't be empty\012\000"
	.align	2
.LC2:
	.ascii	"End %d\012\000"
	.text
	.align	2
	.global	add_elements
	.type	add_elements, %function
add_elements:
	@ args = 0, pretend = 0, frame = 24
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #24
	str	r0, [fp, #-24]
	ldr	r3, .L20
	str	r3, [fp, #-16]
	ldr	r3, [fp, #-24]
	ldr	r3, [r3]
	str	r3, [fp, #-20]
	mov	r3, #0
	str	r3, [fp, #-8]
	b	.L15
.L19:
	ldr	r0, .L20+4
	bl	pop
	str	r0, [fp, #-12]
	ldr	r3, [fp, #-12]
	cmp	r3, #0
	bne	.L16
	mov	r0, #12
	bl	malloc
	mov	r3, r0
	str	r3, [fp, #-12]
	ldr	r0, .L20+8
	ldr	r1, [fp, #-20]
	bl	printf
.L16:
	ldr	r3, [fp, #-12]
	ldr	r2, [fp, #-20]
	str	r2, [r3, #4]
	ldr	r3, [fp, #-12]
	ldr	r2, [fp, #-8]
	str	r2, [r3, #8]
	ldr	r0, .L20+12
	ldr	r1, [fp, #-12]
	bl	push
	ldr	r0, .L20+12
	bl	pop
	str	r0, [fp, #-12]
	ldr	r3, [fp, #-12]
	cmp	r3, #0
	beq	.L17
	ldr	r3, [fp, #-12]
	mov	r2, #0
	str	r2, [r3]
	ldr	r0, .L20+4
	ldr	r1, [fp, #-12]
	bl	push
	b	.L18
.L17:
	ldr	r0, .L20+16
	ldr	r1, [fp, #-20]
	bl	printf
.L18:
	ldr	r3, [fp, #-8]
	add	r3, r3, #1
	str	r3, [fp, #-8]
.L15:
	ldr	r2, [fp, #-8]
	ldr	r3, [fp, #-16]
	cmp	r2, r3
	blt	.L19
	ldr	r0, .L20+20
	ldr	r1, [fp, #-20]
	bl	printf
	mov	r0, #0
	bl	pthread_exit
.L21:
	.align	2
.L20:
	.word	2500000
	.word	free_nodes
	.word	.LC0
	.word	head
	.word	.LC1
	.word	.LC2
	.size	add_elements, .-add_elements
	.section	.rodata
	.align	2
.LC3:
	.ascii	"ERROR; return code from pthread_create() is %d\012\000"
	.text
	.align	2
	.global	main
	.type	main, %function
main:
	@ args = 0, pretend = 0, frame = 48
	@ frame_needed = 1, uses_anonymous_args = 0
	stmfd	sp!, {fp, lr}
	add	fp, sp, #4
	sub	sp, sp, #48
	str	r0, [fp, #-48]
	str	r1, [fp, #-52]
	mov	r3, #0
	str	r3, [fp, #-8]
	b	.L23
.L25:
	ldr	r3, [fp, #-8]
	mov	r3, r3, asl #2
	sub	r2, fp, #4
	add	r3, r2, r3
	ldr	r2, [fp, #-8]
	str	r2, [r3, #-40]
	sub	r2, fp, #28
	ldr	r3, [fp, #-8]
	mov	r3, r3, asl #2
	add	r1, r2, r3
	sub	r2, fp, #44
	ldr	r3, [fp, #-8]
	mov	r3, r3, asl #2
	add	r3, r2, r3
	mov	r0, r1
	mov	r1, #0
	ldr	r2, .L28
	bl	pthread_create
	str	r0, [fp, #-12]
	ldr	r3, [fp, #-12]
	cmp	r3, #0
	beq	.L24
	ldr	r0, .L28+4
	ldr	r1, [fp, #-12]
	bl	printf
	mvn	r0, #0
	bl	exit
.L24:
	ldr	r3, [fp, #-8]
	add	r3, r3, #1
	str	r3, [fp, #-8]
.L23:
	ldr	r3, [fp, #-8]
	cmp	r3, #3
	ble	.L25
	mov	r3, #0
	str	r3, [fp, #-8]
	b	.L26
.L27:
	ldr	r3, [fp, #-8]
	mov	r3, r3, asl #2
	sub	r2, fp, #4
	add	r3, r2, r3
	ldr	r3, [r3, #-24]
	mov	r0, r3
	mov	r1, #0
	bl	pthread_join
	ldr	r3, [fp, #-8]
	add	r3, r3, #1
	str	r3, [fp, #-8]
.L26:
	ldr	r3, [fp, #-8]
	cmp	r3, #3
	ble	.L27
	mov	r0, r3
	sub	sp, fp, #4
	@ sp needed
	ldmfd	sp!, {fp, pc}
.L29:
	.align	2
.L28:
	.word	add_elements
	.word	.LC3
	.size	main, .-main
	.ident	"GCC: (Raspbian 4.9.1-19) 4.9.1"
	.section	.note.GNU-stack,"",%progbits
