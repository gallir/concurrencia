	.align	4
	.global	push
	.type	push, %function
push:
	push	{ip, lr}
1:	
	ldr	r3, [r0]
	str	r3, [r1] @@ To avoid storing e->next inside ldrex/strex
	@ mcr	p15, 0, r0, c7, c10, 5 @@ It shouldn't be needed
	ldrex	r2, [r0]
	cmp	r2, r3
	bne	1b
	strex	ip, r1, [r0]
	cmp	ip, #0
	bne	1b
	mcr	p15, 0, r0, c7, c10, 5

	pop	{ip, pc}

	.align	4
	.global	pop
	.type	pop, %function
pop:
	push	{ip, lr}
1:	
	ldrex	r1, [r0]
	cmp	r1, #0
	beq	2f
	ldr	r2, [r1]
	strex	ip, r2, [r0]
	cmp	ip, #0
	bne	1b
	mcr	p15, 0, r0, c7, c10, 5
2:	
	mov	r0, r1
	pop	{ip, pc}

