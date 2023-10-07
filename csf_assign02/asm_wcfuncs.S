/*
 * A word count program in both C and x86-64 assembly language.
 * CSF Assignment 2
 * Alan Mao, Lauren Siu
 * smao10@jhu.edu, lsiu1@jhu.edu
 */

/*
 * Assembly language function implementations
 */

#define MAX_WORDLEN 63

/*
 * Offsets for the fields of the struct WordEntry data type
 * (Note that there is 4 bytes of padding between the count and next
 * fields.)
 */
#define WORDENTRY_WORD_OFFSET   (0)
#define WORDENTRY_COUNT_OFFSET  (MAX_WORDLEN+1)
#define WORDENTRY_NEXT_OFFSET   (MAX_WORDLEN+1+4+4)

/* Size of the WordEntry data type */
#define WORDENTRY_SIZE 	(WORDENTRY_NEXT_OFFSET+8)

	.section .bss
	.align 4	

glob_inserted:  /* a pointer to store the value of inserted (int *) */
	.space 4

	.section .rodata
/* Define any string constants or read-only data here */

	.section .text

/*
 * Compute a hash code for the given NUL-terminated
 * character string.
 *
 * The hash algorithm should be implemented as follows:
 *
 * uint32_t hash_code = 5381
 * for each character c of w in order {
 *   hash_code = hash_code * 33 + c
 * }
 *
 * Note that the character values should be treated as
 * being unsigned (in the range 0..255)
 *
 * C function prototype:
 *    uint32_t wc_hash(const unsigned char *w);
 */
	.globl wc_hash
wc_hash:
	subq $8, %rsp			/* align stack */

	pushq %r12				/* save r12 */
	pushq %r13 				/* save r13 */

	movq %rdi, %r12			/* r12 is the string */
	xorq %rax, %rax			/* clear output */
	movl $5381, %eax		/* move hash value into return */
	xorq %r13, %r13			/* reset character value */
	
.L_ifhash:
	movzbl (%r12), %r13d	/* move current character value into r13 */
	testl %r13d, %r13d		/* if curr char is null term */
	jz .L_hashd				/* return current hash if null */
							
	imull $33, %eax			/* otherwise, calculate next hash */
	addl %r13d, %eax		/* add calculated hash to current hash */
	incq %r12				/* move to next char */
	jmp .L_ifhash			/* loop back */

.L_hashd:

	popq %r13				/* restore r13 */
	popq %r12 				/* restore r12 */
	addq $8, %rsp			/* restore stack */
	ret


/*
 * Compare two strings lexicographically. Return
 *
 * - a negative value if lhs string is less than rhs string
 * - 0 if lhs string is identical to rhs string
 * - a positive value if lhs string is greater than rhs string
 *
 * Lexicographical comparison is a generalization of alphabetical
 * order, but using character codes. If one string is a prefix
 * of the other, it is considered as "less than". E.g.,
 * "hi" would compare as less than "high".
 *
 * C function prototype:
 *    int wc_str_compare(const unsigned char *lhs, const unsigned char *rhs);
 */
	.globl wc_str_compare
wc_str_compare: 			/* rdi is lhs, rsi is rhs */
	subq $8, %rsp			/* align stack pointer */	

.L_loop_comp: 				/* looping through the string */
	movb (%rdi), %r10b		/* put character from lhs into register */
	movb (%rsi), %r11b		/* put character from rhs into register */

	cmpb $0, %r10b			/* check if lhs is null */
	je .L_lhs_null			/* if null, jump to L_lhs_null */

	cmpb $0, %r11b			/* check if rhs is null since lhs is NOT null */
	je .L_lhs_greater		/* if null, then lhs is longer */

	cmpb %r10b, %r11b		/* compare char value (rhs - lhs = ?) */
	jg .L_rhs_greater		/* if positive, then rhs > lhs */
	jl .L_lhs_greater		/* if negative, lhs > rhs */

	incq %rdi				/* look at the next chars since curr chars are equal */
	incq %rsi					
	jmp .L_loop_comp		/* move to restart loop */

.L_lhs_null:						
	cmpb $0, %r11b			/* check if rhs character is null */
	je .L_equal				/* if true, both lhs and rhs are null */
	jne .L_rhs_greater		/* if not, then rhs is longer */
	
.L_rhs_greater:				/* rhs is longer, return -1 */
	movl $-1, %eax
	addq $8, %rsp
	ret

.L_lhs_greater:				/* lhs is longer, return 1 */
	movl $1, %eax
	addq $8, %rsp
	ret

.L_equal:					/* if strings are the same */
	movl $0, %eax
	addq $8, %rsp
	ret


/*
 * Copy NUL-terminated source string to the destination buffer.
 *
 * C function prototype:
 *    void wc_str_copy(unsigned char *dest, const unsigned char *source);
 */
	.globl wc_str_copy
wc_str_copy:
	subq $8, %rsp			/* align stack */

	pushq %r12				/* save value at r12 */
	pushq %r13				/* save value at r13 */
	pushq %r14				/* save value at r14 */

	movq %rsi, %r12			/* move argument at rsi to r12 */
	movq %rdi, %r13			/* move argument at rdi to r13 */	

.L_loop_copy:
    movb (%r12), %r14b     	/* load the character from source to al */
    testb %r14b, %r14b    	/* check if it's the null terminator */
    je .L_done_copy        	/* if it is, done */

    movb %r14b, (%r13)    	/* otherwise, store the character in dest */
    incq %r13            	/* increment dest pointer */
    incq %r12            	/* increment source pointer */

	jmp .L_loop_copy      	/* repeat the loop */

.L_done_copy:
    movq $0, (%r13)         /* null-terminate dest */
	movq %r13, %rdi			/* move string to rdi */ 

	popq %r14				/* restore r14 */
	popq %r13				/* restore r13 */
	popq %r12				/* restore r12 */
	addq $8, %rsp			/* restore stack */	
	ret

/*
 * Return 1 if the character code in c is a whitespace character,
 * false otherwise.
 *
 * For the purposes of this function, a whitespace character is one of
 *
 *   ' '
 *   '\t'
 *   '\r'
 *   '\n'
 *   '\f'
 *   '\v'
 *
 * C function prototype:
 *    int wc_isspace(unsigned char c);
 */
	.globl wc_isspace
wc_isspace:
	subq $8, %rsp		/* align stack */
	movb %dil, %r10b	/* move a single char into r10b */

	cmpb $9, %r10b		/* cmp '\t' */
	je .L_space			/* jump if equal */

	cmpb $10, %r10b		/* cmp '\n' */
	je .L_space 		/* jump if equal */
	
	cmpb $11, %r10b		/* cmp '\v' */
	je .L_space 		/* jump if equal */

	cmpb $12, %r10b		/* cmp '\f' */
	je .L_space 		/* jump if equal */

	cmpb $13, %r10b		/* cmp '\r' */
	je .L_space 		/* jump if equal */

	cmpb $32, %r10b		/* cmp ' ' */
	je .L_space 		/* jump if equal */

	xorq %rax, %rax 	/* not a space, return 0 */
	addq $8, %rsp		/* restore stack */
	ret

.L_space:				/* is space */
	movq $1, %rax		/* space, return 1 */
	addq $8, %rsp		/* restore stack */
	ret


/*
 * Return 1 if the character code in c is an alphabetic character
 * ('A' through 'Z' or 'a' through 'z'), 0 otherwise.
 *
 * C function prototype:
 *    int wc_isalpha(unsigned char c);
 */
	.globl wc_isalpha
wc_isalpha:
	subq $8, %rsp			/* align stack */
	movb %dil, %r10b		/* move a single char into r10b */

	cmpb $65, %r10b			/* checks if c is possibly anything below 'A' */
	jl .L_isNot				/* if its lower, then it is not alphabetic */

	cmpb $90, %r10b			/* checks if c is possibly anything above 'Z' */
	jle .L_isAlpha			/* if its lower or equal, then it is an uppercase letter (alphabetic) */

	cmpb $97, %r10b			/* checks if c is possibly anything below 'a' */
	jl .L_isNot				/* if its lower, then it is not alphabetic */

	cmpb $122, %r10b		/* checks if c is possibly anything above 'z' */
	jle .L_isAlpha 			/* if its lower or equal, it is a lowercase letter (alphabetic) */	

.L_isNot:
	xorq %rax, %rax 		/* not alpha, return 0 */
	addq $8, %rsp			/* restore stack */
	ret						

.L_isAlpha:
	movq $1, %rax			/* alpha, return 1 */
	addq $8, %rsp			/* restore stack */
	ret


/*
 * Read the next word from given input stream, storing
 * the word in the array pointed-to by w. (This array should be
 * assumed to be MAX_WORDLEN+1 elements in size.) Return 1
 * if a word is read successfully, 0 otherwise.
 *
 * For the purposes of this function, a word is a sequence of
 * 1 or more non-whitespace characters.
 *
 * If a sequence of non-whitespace characters has more than
 * MAX_WORDLEN characters, then only the first MAX_WORDLEN
 * characters in the sequence should be stored in the array.
 *
 * C function prototype:
 *    int wc_readnext(FILE *in, unsigned char *w);
 */
	.globl wc_readnext
wc_readnext:
	subq $8, %rsp				/* align stack */

	pushq %r12					/* save r12 */
	pushq %r13					/* save r13 */
	pushq %r14					/* save r14 */
	pushq %rbx					/* save rbx */

	movq %rdi, %r12				/* r12 is file pointer */
	movq %rsi, %r13 			/* r13 is the string containing the returned word */
	movq $0, %rbx      			/* rbx is length of word read, initialize to 0 */

.L_loop_read:
	movq %r12, %rdi				/* moves file pointer at r12 into rdi */
    call fgetc            		/* call the fgetc function to read a character into %al */
    cmpb $-1, %al	      		/* check if EOF */
    je .L_done_read       		/* if EOF, done */

	movb %al, %dil				/* copy read char to rdi */
	movq %rax, %r14				/* save the value of rax in */
    call wc_isspace      		/* call whitespace checker */
	cmpq $1, %rax				/* check if output was whitespace */
	je .L_confirm_word			/* if whitespace, check if word was encountered */

    cmpq $MAX_WORDLEN, %rbx 	/* compare length of word with MAX_WORDLEN */
	jge .L_word_found			/* if number exceeds MAX_WORDLEN, return word */

	movq %r14, %rax 			/* retrieve value of rax (the character) */
    movb %al, (%r13, %rbx)		/* store character in returned string */
	incq %rbx              		/* increment length */
    jmp .L_loop_read        	/* repeat the loop */

.L_confirm_word:  
	xorq %rax, %rax 			/* clears rax pointer */
	cmpq $0, %rbx				/* check if length of word is nonzero */
    ja .L_word_found     		/* if nonzero, word was found */
	jmp .L_loop_read			/* if zero, keep looking for a word */

.L_done_read:
	xorq %rax, %rax 			/* clears rax pointer */
	cmpq $0, %rbx 				/* check if length of word is nonzero */
    ja .L_word_found     		/* if nonzero, word was found */
	jmp .L_cleanup_readnext 	/* otherwise, cleanup and return */
    
.L_word_found:
	movb $0, (%r13, %rbx)  		/* null-terminate the word */
    movq $1, %rax          		/* set return value to 1 (successful) */
	movq %r13, %rsi 			/* move string in r13 back to rsi */

.L_cleanup_readnext:
	popq %rbx					/* restore rbx */
	popq %r14					/* restore r14 */
	popq %r13					/* restore r13 */
    popq %r12					/* restore r12 */
	addq $8, %rsp				/* restore stack */
    ret


/*
 * Convert the NUL-terminated character string in the array
 * pointed-to by w so that every letter is lower-case.
 *
 * C function prototype:
 *    void wc_tolower(unsigned char *w);
 */
	.globl wc_tolower
wc_tolower:
	subq $8, %rsp		/* align stack */

.L_ifnull:
	movb (%rdi), %r10b	/* move current character value into r10 */
	cmpb $0, %r10b		/* if curr char is null */
	je .L_retlow		/* return if null */

	cmpb $65, %r10b		/* if greater or equal, it might be upper */
	jl .L_next			/* if less than, it is not upper */

	cmpb $90, %r10b		/* if less than or equal, it is upper */
	jg .L_next			/* if greater, it is not upper */

	addb $32, %r10b		/* change char to lowercase */
	movb %r10b, (%rdi)	/* move char back */

.L_next:
	incq %rdi			/* go to next */
	jmp .L_ifnull		/* go back to loop */

.L_retlow:
	addq $8, %rsp 		/* restore stack */
	ret


/*
 * Remove any non-alphaabetic characters from the end of the
 * NUL-terminated character string pointed-to by w.
 *
 * C function prototype:
 *    void wc_trim_non_alpha(unsigned char *w);
 */
	.globl wc_trim_non_alpha
wc_trim_non_alpha:
	subq $8, %rsp				/* align stack */

	pushq %rbx 					/* save rbx */
	pushq %r12 					/* save r12 */

	movq %rdi, %rbx				/* move string into rbx, a roving cur pointer */
	movq %rbx, %r12				/* also save head into r12 */
 	cmpq $0, %rbx				/* check if pointer is null */
	je .L_finish_trim			/* exit if pointer is null */

.L_find_end:
 	movb (%rbx), %r11b			/* move current character value into r11 */
 	cmpb $0, %r11b				/* check if curr char is null */
 	je .L_find_start			/* if null, go to next loop */

 	incq %rbx					/* if not found, move to next char */
 	jmp .L_find_end				/* keep going */

.L_find_start:
	cmpq %r12, %rbx				/* check if cur pointer is past front of string */
	jl .L_finish_trim			/* if yes, we end */

	movb (%rbx), %dil			/* move curr char into func input */
	call wc_isalpha				/* check if is alpha */
	cmpq $1, %rax				/* check if output was alpha */
	je .L_finish_trim			/* if yes, we end */

	movb $0, (%rbx) 			/* terminate the curr char */
	decq %rbx					/* go to previous */
	jmp .L_find_start			/* restart loop */

.L_finish_trim:
	movq %r12, %rdi				/* restore rdi */

	popq %r12 					/* restore r12 */
	popq %rbx					/* restore rbx */

    addq $8, %rsp				/* restore stack */
 	ret


/*
 * Search the specified linked list of WordEntry objects for an object
 * containing the specified string.
 *
 * If a matching object is found, set the int variable pointed-to by
 * inserted to 0 and return a pointer to the matching object.
 *
 * If a matching object is not found, allocate a new WordEntry object,
 * set its next pointer to point to what head points to (i.e., so the
 * new object is at the head of the list), set the variable pointed-to
 * by inserted to 1, and return a pointer to the new node. Note that
 * the new node should have its count value set to 0. (It is the caller's
 * job to update the count.)
 *
 * C function prototype:
 *    struct WordEntry *wc_find_or_insert(struct WordEntry *head, const unsigned char *s, int *inserted);
 */
	.globl wc_find_or_insert
wc_find_or_insert:
	subq $8, %rsp				/* align stack */

	pushq %r12					/* save r12 */
	pushq %r13					/* save r13 */
	pushq %r14					/* save r14 */
	pushq %rbx					/* save rbx */

	movq %rdi, %r12				/* r12 is head WordEntry */
	movq %rsi, %r13 			/* r13 is the specified string */
	movq %rdx, %r14      		/* r14 is the int representing inserted */

	movq %r12, %rbx				/* rbx is the current WordEntry */

.L_loop_search_match:
	
	cmpq $0, %rbx							/* check if current is null */
	je .L_not_found_match					/* if reached end, did not find */

	movq %rbx, %rdi 						/* move word into LHS input */
	movq %r13, %rsi 						/* move target string into RHS input */
	call wc_str_compare 					/* compare strings */
	cmpq $0, %rax  							/* check if equal */
	je .L_found_match						/* if equal, found match */

	movq WORDENTRY_NEXT_OFFSET(%rbx), %rbx 	/* move current to next */
	jmp .L_loop_search_match 				/* loop back to top */
	
.L_found_match:

	movl $0, (%r14)							/* inserted is 0 */
	movq %rbx, %rax							/* return the found WordEntry */
	jmp .L_dict_find_cleanup				/* jump to cleanup */

.L_not_found_match: 

	movq $WORDENTRY_SIZE, %rdi 				/* pass size of WordEntry to malloc */
	call malloc 							/* call malloc */
	movq $0, (%rax) 						/* clear the values in the malloc'd area */
	movq %rax, %rbx							/* move returned new WordEntry to rbx */
	movq $0, WORDENTRY_COUNT_OFFSET(%rbx) 	/* initialize count to 0 */
	movq %r12, WORDENTRY_NEXT_OFFSET(%rbx)	/* prepend the new WordEntry to head WordEntry */

	movq %rbx, %rdi 						/* move word section to destination position */
	movq %r13, %rsi 						/* move source string to source position */
	call wc_str_copy						/* call string copy to copy string into rdi */

	movl $1, (%r14) 						/* inserted is 1 */
	movq %rbx, %rax 						/* return inserted WordEntry */

.L_dict_find_cleanup:
	
	popq %rbx					/* restore rbx */
	popq %r14					/* restore r14 */
	popq %r13					/* restore r13 */
    popq %r12					/* restore r12 */

	addq $8, %rsp				/* restore stack */
    ret


/*
 * Find or insert the WordEntry object for the given string (s), returning
 * a pointer to it. The head of the linked list which contains (or should
 * contain) the entry for s is the element of buckets whose index is the
 * hash code of s mod num_buckets.
 *
 * Returns a pointer to the WordEntry object in the appropriate linked list
 * which represents s.
 *
 * C function prototype:
 *    struct WordEntry *wc_dict_find_or_insert(struct WordEntry *buckets[], unsigned num_buckets, const unsigned char *s);
 */
	.globl wc_dict_find_or_insert
wc_dict_find_or_insert:
	
	subq $8, %rsp				/* align stack */

	pushq %r12					/* save r12 */
	pushq %r13					/* save r13 */
	pushq %r14					/* save r14 */
	pushq %r15					/* save r15 */
	pushq %rbx					/* save rbx */

	movq %rdi, %r12				/* r12 is hash table of WordEntries */
	movq %rsi, %r13 			/* r13 is number of buckets */
	movq %rdx, %r14      		/* r14 is the string to add */

	movq %r14, %rdi 			/* move the string to argument for hash */
	call wc_hash				/* call hash function, result in rax */
	movq %r13, %rcx				/* move num buckets as dividend */
	xorq %rdx, %rdx				/* clear rdx (where remainder goes) */
	idivq %rcx 					/* integer divide, remainder in rdx */
	movq %rdx, %rbx 			/* move hash value into rbx */
	
	xorq %rdx, %rdx 			/* clear third arg (inserted) to call find or insert */
	movq $glob_inserted, %rdx	/* move pointer to int as third arg */	
	movq %r12, %r15				/* move table into r15 */
	imulq $8, %rbx				/* multiply offset by 8 bytes (for pointer) */
	addq %rbx, %r15				/* offset the table by the index */
	movq (%r15), %rdi			/* move the specific WordEntry in hash table to first arg */
	movq %r14, %rsi 			/* move the string to add as second arg */
	call wc_find_or_insert		/* call find or insert */
	
	movl $glob_inserted, %r10d	/* move value stored in inserted */
	cmpl $0, (%r10d)			/* check if new WordEntry was inserted */
	je .L_return_word			/* jump to return if not inserted */
	
	movq %rax, (%r12, %rbx)		/* if inserted, set the head of list to new WordEntry */

.L_return_word: 

	movq %r12, %rdi				/* move hash table back to arg it was passed in */

	popq %rbx					/* restore rbx */
	popq %r15					/* restore r15 */
	popq %r14					/* restore r14 */
	popq %r13					/* restore r13 */
    popq %r12					/* restore r12 */

	addq $8, %rsp				/* restore stack */
    ret


/*
 * Free all of the nodes in given linked list of WordEntry objects.
 *
 * C function prototype:
 *    void wc_free_chain(struct WordEntry *p);
 */
	.globl wc_free_chain
wc_free_chain:
	subq $8, %rbx								/* align stack */
	movq %rdi, %rbx								/* move pointer to head into r10 */

.L_looptofree:
	cmpq $0, %rbx								/* if curr is null, end of list */
	je .L_finishedfree							/* return if null */

	movq %rbx, %rdi								/* store curr obj */
	movq WORDENTRY_NEXT_OFFSET(%rbx), %rbx		/* store next obj */
	call free									/* free curr obj */
	jmp .L_looptofree							/* move to next obj */

.L_finishedfree:
	addq $8, %rbx								/* restore stack */
	ret

/*
vim:ft=gas:
*/
