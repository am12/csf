	.section .rodata

sPromptMsg: .string "Enter 10 integers: "
sInputFmt:  .string "%u"
sResultFmt: .string "Sum is %u\n"

	.section .bss
vals: .space (4 * 10)            

	.section .text                /* space for 10 uint32_t elements */

/*
 * Compute sum of array of 32 bit unsigned integer values.
 *
 * Parameters:
 *   array (%rdi) - pointer to first element of array
 *   n (%esi) - number of elements in array
 *
 * Returns:
 *  sum of array elements
 */
sum_elts:
	movl $0, %eax                 /* %eax stores sum */
	movl $0, %r10d                /* %r10d is the array index */

.LsumLoop:
	cmpl %esi, %r10d              /* see if index has reached num elts */
	jae .LsumLoopDone             /* if index >= num elts, done with loop */
	addl (%rdi,%r10,4), %eax      /* add element to sum */
	incl %r10d                    /* increment index */
	jmp .LsumLoop                 /* continue loop */

.LsumLoopDone:
	ret                           /* return sum in %eax */

/*
 * main function (entry point to program)
 */
	.globl main
main:
	pushq %r12                    /* preserve %r12 and align stack */

	movq $sPromptMsg, %rdi
	call printf

	movq $vals, %r12              /* use %r12 as pointer to element */

.LinputLoop:
	movq $sInputFmt, %rdi         /* scanf format for input */
	movq %r12, %rsi               /* address of next array element */
	call scanf                    /* read value into element */

	addq $4, %r12                 /* increment element pointer */
	movq $vals, %r10              /* load array base addr into %r10 */
	addq $40, %r10                /* compute end address */
	cmpq %r10, %r12               /* determine if element ptr not at end */
	jl .LinputLoop                /* if element ptr not at end, continue */

.LinputLoopDone:
	movq $vals, %rdi              /* pass base address of array */
	movl $10, %esi                /* pass number of elements */
	call sum_elts                 /* compute sum of elements */
	movl %eax, %esi               /* pass computed sum as second arg */
	movq $sResultFmt, %rdi        /* pass result fmt string as first arg */
	call printf                   /* print result */

	popq %r12                     /* restore %r12 */
	movl $0, %eax                 /* return 0 from main */
	ret

/*
vim:ft=gas:
*/
