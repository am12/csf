/* addLongs.S */

.section .rodata

sPromptMsg: .string "Enter two integers: "
sInputFmt:  .string "%ld %ld"
sResultMsg: .string "Sum is %ld\n"

.section .text

	.globl main
	.align 16
main:
	pushq %rbp                /* save original %rbp value */
	movq %rsp, %rbp           /* %rbp points to top of local var area */
	subq $16, %rsp            /* alloc space for two long local vars */

	movl $0, %eax             /* no vector args */
	movq $sPromptMsg, %rdi    /* pass ptr to input prompt */
	call printf               /* print input prompt */
	
	movl $0, %eax             /* no vector args */
	movq $sInputFmt, %rdi     /* pass ptr to scanf format string */
	leaq -16(%rbp), %rsi      /* pass address of 1st var */
	leaq -8(%rbp), %rdx       /* pass address of 2nd var */
	call scanf                /* read the input values */

	movq -16(%rbp), %rdi      /* pass first value */
	movq -8(%rbp), %rsi       /* pass second value */
	call addLongs             /* call addLongs to compute sum */

	movq $sResultMsg, %rdi    /* pass ptr to output message */
	movq %rax, %rsi           /* pass computed sum */
	call printf               /* print output message */

	addq $16, %rsp            /* deallocate space for local vars */
	popq %rbp                 /* restore original value of %rbp */
	ret

	.align 16
addLongs:
	movq %rdi, %rax
	addq %rsi, %rax
	ret

/*
vim:ft=gas:
*/
