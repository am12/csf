 	.section .text

    .globl is_palindrome
/*
 * C function prototype:
 * 	int is_palindrome(const char *s);
 */

is_palindrome:
    /*
     * Notes:
     * Callee-saved registers: rbx, rbp, r12-r15
     * Subroutine arguments:  rdi, rsi, rdx, rcx, r8, r9
     *
     * Register use:
     *    %r12 - saved pointer to beginning of string
     */

    subq $8, %rsp 	/* align the stack */

    /* save values of callee-saved registers! */
   pushq %r12 		/* store pointer to beginning of string in here */
   pushq %r13 		/* store strlen in here */
   movq  %rdi,  %r12 	/* saves pointer to string in %r12 */
    
   call strlen    		/* determine length of string (return is stored in %rax) */
   movq %rax, %r13    /* move result of strlen into %r13 */

	/* note: at this point, r13 is the index of the NUL terminator character */
	dec %r13

   // loop through strlen / 2
	// compare string[i] and string [strlen - i - 1]
		// if not equal, return false
	.LloopTop:
                        leaq (%r12, %r13), %r10  /* %r10 is now pointer to current end for comparison*/
		cmpq %r10, %r12   	/* compare string pointer to length*/
		jge .Le 		/* if front > end, jump to end of program, return true
since we have made all comparisons and non mismatch*/
		movb (%r12), %dil 	/* move current character in %r12 to %dil */
		movb (%r13, %r12), %sil /* move corresponding end character to %sil */
		cmpb %dil, %sil  	/* compare characters */
		jne .Lne		/* if not equal, return false */
		incq %r12		/* increment %r12 */
		decq %r13 		/* decrement %r13 */
		decq %r13 		/* decrement %r13 */
		jmp .LloopTop

	.Le:
                       /* control gets here if we have checked all chars and they all match */
		movl $1, %eax                  /* store 1 in return value register */
		jmp .Lend
	.Lne:
                       /* we get here if we find a mismatched pair of characters */
		movl $0, %eax                 /* store 0 in return value register */
		jmp .Lend 		     /* W best practices */
	.Lend:
		/* restore callee-saved registers and realign stack */
		popq %r12
		popq %r13
		addq $8, %rsp 
		ret     /* return false*/

/*
vim:ft=gas:
*/












