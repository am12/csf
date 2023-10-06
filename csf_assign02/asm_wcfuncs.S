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
	movq $5381, %r10
	subq $8, %rsp

.L_ifhash:
	movq $0, %r11		/* reset character value */
	movb (%rdi), %r11b	/* move current character value into %r11b */
	cmpb $0, %r11b		/* if curr char is null */
	je .L_hashd			/* return current hash if null */
						/* calculate if not null */
	imulq $33, %r10		/* multiply hash by 33 */
	addq %r11, %r10		/* add calculated hash to current hash */
	incq %rdi			/* move to next char */
	jmp .L_ifhash

.L_hashd:
	movq %r10, %rax
	addq $8, %rsp
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
	je .L_lhs_longer		/* if null, then lhs is longer */

	cmpb %r10b, %r11b		/* compare char value */
	jne .L_compare			/* if not equal, jump to compare */

	incq %rdi				/* look at the next chars since curr chars are equal */
	incq %rsi					
	jmp .L_loop_comp		/* move to restart loop */

.L_lhs_null:						
	cmpb $0, %r11b			/* check if rhs character is null */
	je .L_equal				/* if true, both lhs and rhs are null */
	jne .L_rhs_longer		/* if not, then rhs is longer */
	
.L_rhs_longer:				/* rhs is longer, return -1 */
	movl $-1, %eax
	addq $8, %rsp
	ret

.L_lhs_longer:				/* lhs is longer, return 1 */
	movl $1, %eax
	addq $8, %rsp
	ret

.L_equal:					/* if strings are the same */
	movl $0, %eax
	addq $8, %rsp
	ret

.L_compare:					
	subb %r11b, %r10b		/* comparing individual chars, rhs - lhs = ? */
	js .L_rhs_longer		/* if negative, then rhs > lhs */
	jns .L_lhs_longer		/* if positive, lhs > rhs */


/*
 * Copy NUL-terminated source string to the destination buffer.
 *
 * C function prototype:
 *    void wc_str_copy(unsigned char *dest, const unsigned char *source);
 */
	.globl wc_str_copy
wc_str_copy:
	subq $8, %rsp			/* align stack */

.L_loop_copy:
    movb (%rsi), %al     	/* load the character from source to al */
    test %al, %al     		/* check if it's the null terminator */
    je .L_done_copy        	/* if it is, done */

    movb %al, (%rdi)    	/* otherwise, store the character in dest */
    incq %rdi            	/* increment dest pointer */
    incq %rsi            	/* increment source pointer */

	jmp .L_loop_copy      	/* repeat the loop */

.L_done_copy:
    movq $0, (%rdi)         /* null-terminate dest */
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
	subq $8, %rsp
	movb %dil, %r10b		/* move a single char into r10b */

	cmpb $65, %r10b			/* checks if c is possibly anything below 'A' */
	jl .L_isNot				/* if its lower, then it is not alphabetic */

	cmpb $90, %r10b			/* checks if c is possibly anything above 'Z' */
	jle .L_isAlpha			/* if its lower or equal, then it is an uppercase letter (alphabetic) */

	cmpb $97, %r10b			/* checks if c is possibly anything below 'a' */
	jl .L_isNot				/* if its lower, then it is not alphabetic */

	cmpb $122, %r10b		/* checks if c is possibly anything above 'z' */
	jle .L_isAlpha 			/* if its lower or equal, it is a lowercase letter (alphabetic) */

.L_isNotA:
	xorq %rax, %rax
	ret						/* return 0 */
.L_isNot:
	xorq %rax, %rax 		/* not alpha, return 0 */
	addq $8, %rsp
	ret

.L_isAlpha:
	movq $1, %rax			/* alpha, return 1 */
	addq $8, %rsp
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
	subq $8, %rsp

	pushq %r12					/* save r12 */
	pushq %r13					/* save r13 */
	pushq %r14					/* save r14 */
	pushq %rbx					/* save rbx */

	movq %rdi, %r12				/* r12 is file pointer */
	movq %rsi, %r13 			/* r13 is the string containing the returned word */
	movq $0, %rbx      			/* rbx is length of word read, initialize to 0 */

	//xorq %r13, %r13				/* clear value at r13 */

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

	popq %rbx					/* restore rbx */
	popq %r14					/* restore r14 */
	popq %r13					/* restore r13 */
	popq %r12					/* restore r12 */
	addq $8, %rsp				/* restore stack */
    ret
    
.L_word_found:
	//incq %rbx					/* inc length to reach null term position */
	movb $0, (%r13, %rbx)  		/* null-terminate the word */
    movq $1, %rax          		/* set return value to 1 (successful) */
	movq %r13, %rsi 			/* move string in r13 back to rsi */

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
	subq $8, %rsp

.L_ifnull:
	movb (%rdi), %r10b	/* move current character value into %r11b */
	cmpb $0, %r10b		/* if curr char is null*/
	je .L_retlow		/* return current hash if null*/

	cmpb $65, %r10b		/* if greater or equal, it might be upper*/
	jl .L_next			/* if less than, it is not upper*/

	cmpb $90, %r10b		/* if less than or equal, it is upper*/
	jg .L_next			/* if greater, it is not upper*/

	addb $32, %r10b		/* change char to lowercase */
	movb %r10b, (%rdi)	/* move char back*/
	jmp .L_ifnull

.L_next:
	incq %rdi			/*go to next*/
	jmp .L_ifnull

.L_retlow:
	addq $8, %rsp
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
	subq $8, %rbx				/* align stack */
	movq %rdi, %rbx				/* move curr char into %r11b */
 	cmpq $0, %rdi				/* if curr char is null*/
	je .L_removed
	incq %rbx					/* go next */

.L_find_end:
 	movb (%rbx), %r11b			/* move current character value into %r11b */
 	cmpb $0, %r11b				/* if curr char is null*/
 	je .L_loopalpha				/* found null*/

 	incq %rbx					/*keep going if not found*/
 	jmp .L_find_end

.L_loopalpha:
	cmpq %rdi, %rbx				/* if curr is at front of string */
	je .L_back_alpha			/* if yes, we trim*/

	movb (%rbx), %dil			/* move curr char into func input */
	call wc_isalpha				/*  check if is alpha */

	cmpq $1, %rax				/*  check if is alpha */
	je .L_back_alpha			/* if yes we go trim */

	decq %rbx					/* go to previous */
	jmp .L_loopalpha			/* restart loop */

.L_back_alpha:
	movb (%rbx), %dil			/* move curr char into func input */
 	call wc_isalpha				/* check if is alpha*/
 	cmpq $1, %rax				/*if char, we go next */
 	je .L_setNull
 	movb $0, (%rbx)				/* if not we set curr to null */
	jmp .L_removed				/* jump to .Lreturn */

.L_setNull:
	incq %rbx					/* go to next char*/
	movb $0, (%rbx)				/* set curr to null */

.L_removed:
    addq $8, %rbx				/* restore stack */
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
	/* TODO: implement */
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
	/* TODO: implement */
	ret


/*
 * Free all of the nodes in given linked list of WordEntry objects.
 *
 * C function prototype:
 *    void wc_free_chain(struct WordEntry *p);
 */
	.globl wc_free_chain
wc_free_chain:
	
	subq $8, %rbx								/* align stack*/
	movq %rdi, %rbx								/* move pointer to head into r10 */

.L_looptofree:
	cmpq $0, %rbx								/* if curr is null, end of list*/
	je .L_finishedfree							/*return if null */

	movq %rbx, %rdi								/* store curr obj */
	movq WORDENTRY_NEXT_OFFSET(%rbx), %rbx		/* store next obj */
	call free									/* free curr obj */
	jmp .L_looptofree							/* move to next obj */

.L_finishedfree:
	addq $8, %rbx								/* restore stack*/
	ret



/*
vim:ft=gas:
*/
