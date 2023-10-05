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

.L_loop_comp: 					/* looping through the string */
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
	jmp .L_loop_comp				/* move to restart loop */

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

    movb %al, (%rdi)    		/* otherwise, store the character in dest */
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
	/* TODO: implement */
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
	incq %rdi			/* go to next */
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
	subq $8, %rsp

    testq   %rdi, %rdi         # Check if w is NULL
    je      .L_done_trim              # If it is, return

    movb    (%rdi), %al        # Load the first character of w to AL
    testb   %al, %al           # Check if it's the null terminator
    je      .L_done_trim              # If it is, return

    xor     %rcx, %rcx         # Initialize len to 0

.L_loop1_trim:  				# searching forward until the end of the string
    movb    (%rdi, %rcx), %al  # Load the character at w[len] to AL
    testb   %al, %al           # Check if it's the null terminator
    jz      .L_loop2_trim             # If it is, jump to loop2
    inc     %rcx               # Increment len
    jmp     .L_loop1_trim             # Repeat loop1

.L_loop2_trim: 					# mid point check on the length 
    testq   %rcx, %rcx         # Check if len is 0
    jz      .L_done_trim              # If it is, return

.L_loop3_trim:
    decq     %rcx               # Decrement len
    movb    (%rdi, %rcx), %al  # Load the character at w[len-1] to AL
	pushq 	(%rdi, %rcx)		
	movb	%al, %dil
    call    wc_isalpha         # Call wc_isalpha to check if it's an alpha char
	popq 	(%rdi, %rcx)
	testq  	%rax, %rax          # Check the result
    jnz     .L_done_trim              # If it's alpha, return
    movb    $0, (%rdi, %rcx)   # Otherwise, set w[len-1] to null terminator
    testq   %rcx, %rcx         # Check if len is now 0
    jnz     .L_loop3_trim             # If not, repeat loop3

.L_done_trim:
	addq $8, %rsp
    ret

; .L_find_null:
; 	movb (%rdi), %r10b	/* move current character value into %r11b */
; 	cmpb $0, %r10b		/* if curr char is null*/
; 	je .L_found_null	/* found null*/

; 	incq %rdi 			/*keep going if not found*/
; 	jmp .L_find_null

; .L_found_null:
; 	dec %rdi
; 	call wc_isalpha
; 	cmpb $1, %al
; 	je .L_removed
; 	jne .L_remove_non

; .L_remove_non:
; 	movb %r10b, (%rdi)
; 	jmp .L_found_null

; .L_removed:
; 	addq $8, %rsp
; 	ret


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
	/* TODO: implement */
	ret


/*
vim:ft=gas:
*/
