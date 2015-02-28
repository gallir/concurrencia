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
	.file	"lib.s"
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
	str	r0, [fp, #-8] @ 0
	str	r1, [fp, #-12] @20

	ldr	r4, [fp, #-8]		@ Store the address of head
	ldr	r5, [fp, #-12]		@ Store the addess of e

.PUSH_BEGIN:
	mcr	p15, 0, r0, c7, c10, 5
	ldrex	r1, [r4]		@ Load content of head
	str	r1, [r5] 		@ Store e->next
	strex	lr, r5, [r4]		@ Store in head
	cmp	lr, #0
	bne	.PUSH_BEGIN		@ Failed, restart

	mcr	p15, 0, r0, c7, c10, 5
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
	@ Store the address of head in r4
	ldr	r4, [fp, #-16]

.POP_BEGIN:
	ldr	r2, [fp, #-16]
	mcr	p15, 0, r0, c7, c10, 5
	ldrex	r1, [r2]
	ldr	r3, [r4]
	str	r3, [fp, #-8] @store in old_head
.IF_NULL:
	ldr	r3, [fp, #-8]
	cmp	r3, #0
	bne	.NOT_NULL
	mov	r3, #0
	b	.POP_RETURN
.NOT_NULL:
	ldr	r3, [fp, #-8]
	ldr	r3, [r3]
	mov	ip, r3
	strex	lr, ip, [r2]
	cmp	lr, #0
	bne	.POP_BEGIN
	mcr	p15, 0, r0, c7, c10, 5
	ldr	r3, [fp, #-8]
.POP_RETURN:
	mov	r0, r3
	sub	sp, fp, #4
	@ sp needed
	ldmfd	sp!, {fp, pc}
	.size	pop, .-pop
	.ident	"GCC: (Raspbian 4.9.1-19) 4.9.1"
	.section	.note.GNU-stack,"",%progbits
