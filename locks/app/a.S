
a.out:     file format elf32-littlearm


Disassembly of section .init:

000083d4 <_init>:
    83d4:	e92d4008 	push	{r3, lr}
    83d8:	eb00002c 	bl	8490 <call_gmon_start>
    83dc:	e8bd8008 	pop	{r3, pc}

Disassembly of section .plt:

000083e0 <.plt>:
    83e0:	e52de004 	push	{lr}		; (str lr, [sp, #-4]!)
    83e4:	e59fe004 	ldr	lr, [pc, #4]	; 83f0 <_init+0x1c>
    83e8:	e08fe00e 	add	lr, pc, lr
    83ec:	e5bef008 	ldr	pc, [lr, #8]!
    83f0:	00008c10 	.word	0x00008c10
    83f4:	e28fc600 	add	ip, pc, #0
    83f8:	e28cca08 	add	ip, ip, #32768	; 0x8000
    83fc:	e5bcfc10 	ldr	pc, [ip, #3088]!	; 0xc10
    8400:	e28fc600 	add	ip, pc, #0
    8404:	e28cca08 	add	ip, ip, #32768	; 0x8000
    8408:	e5bcfc08 	ldr	pc, [ip, #3080]!	; 0xc08
    840c:	e28fc600 	add	ip, pc, #0
    8410:	e28cca08 	add	ip, ip, #32768	; 0x8000
    8414:	e5bcfc00 	ldr	pc, [ip, #3072]!	; 0xc00
    8418:	e28fc600 	add	ip, pc, #0
    841c:	e28cca08 	add	ip, ip, #32768	; 0x8000
    8420:	e5bcfbf8 	ldr	pc, [ip, #3064]!	; 0xbf8
    8424:	e28fc600 	add	ip, pc, #0
    8428:	e28cca08 	add	ip, ip, #32768	; 0x8000
    842c:	e5bcfbf0 	ldr	pc, [ip, #3056]!	; 0xbf0
    8430:	e28fc600 	add	ip, pc, #0
    8434:	e28cca08 	add	ip, ip, #32768	; 0x8000
    8438:	e5bcfbe8 	ldr	pc, [ip, #3048]!	; 0xbe8
    843c:	e28fc600 	add	ip, pc, #0
    8440:	e28cca08 	add	ip, ip, #32768	; 0x8000
    8444:	e5bcfbe0 	ldr	pc, [ip, #3040]!	; 0xbe0
    8448:	e28fc600 	add	ip, pc, #0
    844c:	e28cca08 	add	ip, ip, #32768	; 0x8000
    8450:	e5bcfbd8 	ldr	pc, [ip, #3032]!	; 0xbd8

Disassembly of section .text:

00008454 <_start>:
    8454:	e3a0b000 	mov	fp, #0
    8458:	e3a0e000 	mov	lr, #0
    845c:	e49d1004 	pop	{r1}		; (ldr r1, [sp], #4)
    8460:	e1a0200d 	mov	r2, sp
    8464:	e52d2004 	push	{r2}		; (str r2, [sp, #-4]!)
    8468:	e52d0004 	push	{r0}		; (str r0, [sp, #-4]!)
    846c:	e59fc010 	ldr	ip, [pc, #16]	; 8484 <_start+0x30>
    8470:	e52dc004 	push	{ip}		; (str ip, [sp, #-4]!)
    8474:	e59f000c 	ldr	r0, [pc, #12]	; 8488 <_start+0x34>
    8478:	e59f300c 	ldr	r3, [pc, #12]	; 848c <_start+0x38>
    847c:	ebffffdf 	bl	8400 <_init+0x2c>
    8480:	ebffffdb 	bl	83f4 <_init+0x20>
    8484:	00008734 	.word	0x00008734
    8488:	000085c0 	.word	0x000085c0
    848c:	000086cc 	.word	0x000086cc

00008490 <call_gmon_start>:
    8490:	e59f3014 	ldr	r3, [pc, #20]	; 84ac <call_gmon_start+0x1c>
    8494:	e59f2014 	ldr	r2, [pc, #20]	; 84b0 <call_gmon_start+0x20>
    8498:	e08f3003 	add	r3, pc, r3
    849c:	e7932002 	ldr	r2, [r3, r2]
    84a0:	e3520000 	cmp	r2, #0
    84a4:	012fff1e 	bxeq	lr
    84a8:	eaffffd7 	b	840c <_init+0x38>
    84ac:	00008b60 	.word	0x00008b60
    84b0:	0000002c 	.word	0x0000002c

000084b4 <__do_global_dtors_aux>:
    84b4:	e59f3010 	ldr	r3, [pc, #16]	; 84cc <__do_global_dtors_aux+0x18>
    84b8:	e5d32000 	ldrb	r2, [r3]
    84bc:	e3520000 	cmp	r2, #0
    84c0:	03a02001 	moveq	r2, #1
    84c4:	05c32000 	strbeq	r2, [r3]
    84c8:	e12fff1e 	bx	lr
    84cc:	0001103c 	.word	0x0001103c

000084d0 <frame_dummy>:
    84d0:	e59f0020 	ldr	r0, [pc, #32]	; 84f8 <frame_dummy+0x28>
    84d4:	e92d4008 	push	{r3, lr}
    84d8:	e5903000 	ldr	r3, [r0]
    84dc:	e3530000 	cmp	r3, #0
    84e0:	08bd8008 	popeq	{r3, pc}
    84e4:	e59f3010 	ldr	r3, [pc, #16]	; 84fc <frame_dummy+0x2c>
    84e8:	e3530000 	cmp	r3, #0
    84ec:	08bd8008 	popeq	{r3, pc}
    84f0:	e12fff33 	blx	r3
    84f4:	e8bd8008 	pop	{r3, pc}
    84f8:	00010f0c 	.word	0x00010f0c
    84fc:	00000000 	.word	0x00000000

00008500 <do_thread_fool0>:
#include <stdlib.h>

#define CNT 100000000

void *do_thread_fool0 (void *data)
{
    8500:	e52db004 	push	{fp}		; (str fp, [sp, #-4]!)
    8504:	e28db000 	add	fp, sp, #0
    8508:	e24dd014 	sub	sp, sp, #20
    850c:	e50b0010 	str	r0, [fp, #-16]
	int i;

	for (i = 0; i < CNT; i++) {
    8510:	e3a03000 	mov	r3, #0
    8514:	e50b3008 	str	r3, [fp, #-8]
    8518:	ea000006 	b	8538 <do_thread_fool0+0x38>
		(*(int *)data)++;	
    851c:	e51b3010 	ldr	r3, [fp, #-16]
    8520:	e5932000 	ldr	r2, [r3]
    8524:	e2822001 	add	r2, r2, #1
    8528:	e5832000 	str	r2, [r3]

void *do_thread_fool0 (void *data)
{
	int i;

	for (i = 0; i < CNT; i++) {
    852c:	e51b3008 	ldr	r3, [fp, #-8]
    8530:	e2833001 	add	r3, r3, #1
    8534:	e50b3008 	str	r3, [fp, #-8]
    8538:	e51b2008 	ldr	r2, [fp, #-8]
    853c:	e59f3018 	ldr	r3, [pc, #24]	; 855c <do_thread_fool0+0x5c>
    8540:	e1520003 	cmp	r2, r3
    8544:	dafffff4 	ble	851c <do_thread_fool0+0x1c>
		(*(int *)data)++;	
	}

	return data;
    8548:	e51b3010 	ldr	r3, [fp, #-16]
}
    854c:	e1a00003 	mov	r0, r3
    8550:	e28bd000 	add	sp, fp, #0
    8554:	e8bd0800 	pop	{fp}
    8558:	e12fff1e 	bx	lr
    855c:	05f5e0ff 	.word	0x05f5e0ff

00008560 <do_thread_fool1>:

void *do_thread_fool1 (void *data)
{
    8560:	e52db004 	push	{fp}		; (str fp, [sp, #-4]!)
    8564:	e28db000 	add	fp, sp, #0
    8568:	e24dd014 	sub	sp, sp, #20
    856c:	e50b0010 	str	r0, [fp, #-16]
	int i;

	for (i = 0; i < CNT; i++) {
    8570:	e3a03000 	mov	r3, #0
    8574:	e50b3008 	str	r3, [fp, #-8]
    8578:	ea000006 	b	8598 <do_thread_fool1+0x38>
		(*(int *)data)--;	
    857c:	e51b3010 	ldr	r3, [fp, #-16]
    8580:	e5932000 	ldr	r2, [r3]
    8584:	e2422001 	sub	r2, r2, #1
    8588:	e5832000 	str	r2, [r3]

void *do_thread_fool1 (void *data)
{
	int i;

	for (i = 0; i < CNT; i++) {
    858c:	e51b3008 	ldr	r3, [fp, #-8]
    8590:	e2833001 	add	r3, r3, #1
    8594:	e50b3008 	str	r3, [fp, #-8]
    8598:	e51b2008 	ldr	r2, [fp, #-8]
    859c:	e59f3018 	ldr	r3, [pc, #24]	; 85bc <do_thread_fool1+0x5c>
    85a0:	e1520003 	cmp	r2, r3
    85a4:	dafffff4 	ble	857c <do_thread_fool1+0x1c>
		(*(int *)data)--;	
	}

	return data;
    85a8:	e51b3010 	ldr	r3, [fp, #-16]
}
    85ac:	e1a00003 	mov	r0, r3
    85b0:	e28bd000 	add	sp, fp, #0
    85b4:	e8bd0800 	pop	{fp}
    85b8:	e12fff1e 	bx	lr
    85bc:	05f5e0ff 	.word	0x05f5e0ff

000085c0 <main>:

int main(void)
{
    85c0:	e92d4800 	push	{fp, lr}
    85c4:	e28db004 	add	fp, sp, #4
    85c8:	e24dd010 	sub	sp, sp, #16
	int var = 9527;
    85cc:	e59f30e0 	ldr	r3, [pc, #224]	; 86b4 <main+0xf4>
    85d0:	e50b300c 	str	r3, [fp, #-12]
	int ret;

	pthread_t tid[2];

	ret = pthread_create(&tid[0], NULL, do_thread_fool0, &var);
    85d4:	e24b2014 	sub	r2, fp, #20
    85d8:	e24b300c 	sub	r3, fp, #12
    85dc:	e1a00002 	mov	r0, r2
    85e0:	e3a01000 	mov	r1, #0
    85e4:	e59f20cc 	ldr	r2, [pc, #204]	; 86b8 <main+0xf8>
    85e8:	ebffff8a 	bl	8418 <_init+0x44>
    85ec:	e50b0008 	str	r0, [fp, #-8]
	if (ret != 0) {
    85f0:	e51b3008 	ldr	r3, [fp, #-8]
    85f4:	e3530000 	cmp	r3, #0
    85f8:	0a000008 	beq	8620 <main+0x60>
		fprintf(stderr, "pthread_create failure...\n");
    85fc:	e59f20b8 	ldr	r2, [pc, #184]	; 86bc <main+0xfc>
    8600:	e59f30b8 	ldr	r3, [pc, #184]	; 86c0 <main+0x100>
    8604:	e5933000 	ldr	r3, [r3]
    8608:	e1a00002 	mov	r0, r2
    860c:	e3a01001 	mov	r1, #1
    8610:	e3a0201a 	mov	r2, #26
    8614:	ebffff85 	bl	8430 <_init+0x5c>
		exit(1);
    8618:	e3a00001 	mov	r0, #1
    861c:	ebffff86 	bl	843c <_init+0x68>
	}

	ret = pthread_create(&tid[1], NULL, do_thread_fool1, &var);
    8620:	e24b3014 	sub	r3, fp, #20
    8624:	e2832004 	add	r2, r3, #4
    8628:	e24b300c 	sub	r3, fp, #12
    862c:	e1a00002 	mov	r0, r2
    8630:	e3a01000 	mov	r1, #0
    8634:	e59f2088 	ldr	r2, [pc, #136]	; 86c4 <main+0x104>
    8638:	ebffff76 	bl	8418 <_init+0x44>
    863c:	e50b0008 	str	r0, [fp, #-8]
	if (ret != 0) {
    8640:	e51b3008 	ldr	r3, [fp, #-8]
    8644:	e3530000 	cmp	r3, #0
    8648:	0a000008 	beq	8670 <main+0xb0>
		fprintf(stderr, "pthread_create failure...\n");
    864c:	e59f2068 	ldr	r2, [pc, #104]	; 86bc <main+0xfc>
    8650:	e59f3068 	ldr	r3, [pc, #104]	; 86c0 <main+0x100>
    8654:	e5933000 	ldr	r3, [r3]
    8658:	e1a00002 	mov	r0, r2
    865c:	e3a01001 	mov	r1, #1
    8660:	e3a0201a 	mov	r2, #26
    8664:	ebffff71 	bl	8430 <_init+0x5c>
		exit(1);
    8668:	e3a00001 	mov	r0, #1
    866c:	ebffff72 	bl	843c <_init+0x68>
	}

	pthread_join(tid[0], NULL);
    8670:	e51b3014 	ldr	r3, [fp, #-20]
    8674:	e1a00003 	mov	r0, r3
    8678:	e3a01000 	mov	r1, #0
    867c:	ebffff71 	bl	8448 <_init+0x74>
	pthread_join(tid[1], NULL);
    8680:	e51b3010 	ldr	r3, [fp, #-16]
    8684:	e1a00003 	mov	r0, r3
    8688:	e3a01000 	mov	r1, #0
    868c:	ebffff6d 	bl	8448 <_init+0x74>

	printf("var = %d\n", var);
    8690:	e59f2030 	ldr	r2, [pc, #48]	; 86c8 <main+0x108>
    8694:	e51b300c 	ldr	r3, [fp, #-12]
    8698:	e1a00002 	mov	r0, r2
    869c:	e1a01003 	mov	r1, r3
    86a0:	ebffff5f 	bl	8424 <_init+0x50>

	return 0;
    86a4:	e3a03000 	mov	r3, #0
}
    86a8:	e1a00003 	mov	r0, r3
    86ac:	e24bd004 	sub	sp, fp, #4
    86b0:	e8bd8800 	pop	{fp, pc}
    86b4:	00002537 	.word	0x00002537
    86b8:	00008500 	.word	0x00008500
    86bc:	00008744 	.word	0x00008744
    86c0:	00011038 	.word	0x00011038
    86c4:	00008560 	.word	0x00008560
    86c8:	00008760 	.word	0x00008760

000086cc <__libc_csu_init>:
    86cc:	e92d45f8 	push	{r3, r4, r5, r6, r7, r8, sl, lr}
    86d0:	e59f5050 	ldr	r5, [pc, #80]	; 8728 <__libc_csu_init+0x5c>
    86d4:	e59fa050 	ldr	sl, [pc, #80]	; 872c <__libc_csu_init+0x60>
    86d8:	e1a06000 	mov	r6, r0
    86dc:	e1a07001 	mov	r7, r1
    86e0:	e1a08002 	mov	r8, r2
    86e4:	ebffff3a 	bl	83d4 <_init>
    86e8:	e59f3040 	ldr	r3, [pc, #64]	; 8730 <__libc_csu_init+0x64>
    86ec:	e065a00a 	rsb	sl, r5, sl
    86f0:	e08f3003 	add	r3, pc, r3
    86f4:	e1b0a14a 	asrs	sl, sl, #2
    86f8:	e0835005 	add	r5, r3, r5
    86fc:	08bd85f8 	popeq	{r3, r4, r5, r6, r7, r8, sl, pc}
    8700:	e3a04000 	mov	r4, #0
    8704:	e4953004 	ldr	r3, [r5], #4
    8708:	e1a00006 	mov	r0, r6
    870c:	e1a01007 	mov	r1, r7
    8710:	e1a02008 	mov	r2, r8
    8714:	e2844001 	add	r4, r4, #1
    8718:	e12fff33 	blx	r3
    871c:	e154000a 	cmp	r4, sl
    8720:	3afffff7 	bcc	8704 <__libc_csu_init+0x38>
    8724:	e8bd85f8 	pop	{r3, r4, r5, r6, r7, r8, sl, pc}
    8728:	ffffff04 	.word	0xffffff04
    872c:	ffffff08 	.word	0xffffff08
    8730:	00008908 	.word	0x00008908

00008734 <__libc_csu_fini>:
    8734:	e12fff1e 	bx	lr

Disassembly of section .fini:

00008738 <_fini>:
    8738:	e92d4008 	push	{r3, lr}
    873c:	e8bd8008 	pop	{r3, pc}
