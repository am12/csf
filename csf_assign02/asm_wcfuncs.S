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
	/* TODO: implement */
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
wc_str_compare:
	/* rdi is lhs, rsi is rhs */

	.L_loop: 						/*looping through the string*/
		movb (%rdi), %r12b			/* Put character from lhs into register */
    	movb (%rsi), %r13b			/* Put character from rhs into register */

		cmp $0, %r12b				/*check if lhs is null*/
		je .L_lhs_null				/* if null, jump to L_lhs_null */

		cmp $0, %r13b				/*check if rhs is null since lhs is NOT null*/
		je .L_lhs_longer			/* if null, then lhs is longer*/

		cmp %r12b, %r13b			/*compare char value*/
		jne .L_compare				/* If not equal, jump to compare */

		inc %rdi					/* look at the next chars since curr chars are equal*/
		inc %rsi					
		jmp .L_loop					/* move to restart loop*/
	
	.L_lhs_null:						
		cmp $0, %r13b				/* Check if rhs character is null */
		je .L_equal					/*if true, both lhs and rhs are null*/
		jne .L_rhs_longer			/* if not, then rhs is longer*/
		
	.L_rhs_longer:					/*rhs is longer, return -1*/
		mov $-1, %eax
		ret

	.L_lhs_longer:					/*lhs is longer, return 1*/
		mov $1, %eax
		ret

	.L_equal:						/* if strings are the same */
		mov $0, %eax
		ret

	.L_compare:					
		sub %r13b, %r12b			/*comparing individual chars, rhs - lhs = ? */
		js .L_rhs_longer			/*if negative, then rhs > lhs*/
		jns .L_lhs_longer			/* if positive, lhs > rhs*/

/*
 * Copy NUL-terminated source string to the destination buffer.
 *
 * C function prototype:
 *    void wc_str_copy(unsigned char *dest, const unsigned char *source);
 */
	.globl wc_str_copy
wc_str_copy:
	/* TODO: implement */
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
	cmpq $9, %rdi		/*cmp '\t'*/
	je .L_space			/*jump if equal*/

	cmpq $10, %rdi		/*cmp '\n'*/
	je .L_space 		/*jump if equal*/
	
	cmpq $11, %rdi		/*cmp '\v'*/
	je .L_space 		/*jump if equal*/

	cmpq $12, %rdi		/*cmp '\f'*/
	je .L_space 		/*jump if equal*/

	cmpq $13, %rdi		/*cmp '\r'*/
	je .L_space 		/*jump if equal*/

	cmpq $32, %rdi		/*cmp ' '*/
	je .L_space 		/*jump if equal*/
	jne .L_not_space	/*jump if not equal*/

	
	.L_not_space:		/*is not space*/
		xor %rax, %rax 	/*clear eax*/
		ret

	.L_space:			/*is space*/
		xor %rax, %rax
		mov $1, %rax	/*move 1 to return*/
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
	cmpq $65, %rdi			/*checks if c is possibly anything below 'A'*/
	jl .L_isNot				/*if its lower, then it is not alphabetic*/
	jge .L_highA			/*if its higher it can be alphabetic or a symbol*/

	.L_highA:
		cmpq $90, %rdi
		jle .L_isAlpha		/*if its lower, then it is an uppercase letter*/
		jge .L_checklower	/*if its greater, it can be alphabetic or a lowercase*/

	.L_checklower:
		cmpq $97, %rdi
		jl .L_isNot			/*if its lower, then it is a symbol*/
		jge .L_highlowZ		/*if its higher, it can be a lowercase letter*/
	
	.L_highlowZ:
		cmpq $122, %rdi
		jle .L_isAlpha		/*if its lower, it is a lowercase letter*/
		jg .L_isNot			/*if its greater, then it is not alphabetic*/

	.L_isNot:
		xor %rax, %rax		/*return 0*/
		ret
	
	.L_isAlpha:
		xor %rax, %rax
		mov $1, %rax		/*return 1*/
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
	/* TODO: implement */
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
	/* TODO: implement */
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
	/* TODO: implement */
	ret

/*
vim:ft=gas:
*/
