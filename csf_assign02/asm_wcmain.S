/*
 * A word count program in both C and x86-64 assembly language.
 * CSF Assignment 2
 * Alan Mao, Lauren Siu
 * smao10@jhu.edu, lsiu1@jhu.edu
 */
 

/*
 * Assembly language main function implementation
 */

#define MAX_WORDLEN 63
#define HASHTABLE_SIZE 13249
#define WORDENTRY_COUNT_OFFSET (MAX_WORDLEN+1)
#define WORDENTRY_NEXT_OFFSET  (MAX_WORDLEN+1+4+4)

	.section .rodata
fileRead: .string "r"									/* read only string literal for fopen */
fileOpenError: .string "Error opening file\n"			/* error message string literal for fopen */
argvError: .string "Wrong number of arguments.\n"		/* error message string literal for invalid argv */
totalWord: .string "Total words read: %u\n"				/* print statement for total word count */
uniqueWord: .string "Unique words read %u\n"			/* print statement for unique word count */
bestWord: .string "Most frequent word: %s (%u)\n" 		/* print statement for best word count & stats */

	.section .text

	.globl main
main:
// SETUP MAIN VARIABLES
	pushq %r12			/* save r12 */	
	pushq %r13			/* save r13 */		
	pushq %r14			/* save r14 */	
	pushq %r15			/* save r15 */	
	pushq %rbx			/* save rbx */		

	movq %rdi, %r12		/* r12 is argc */
	movq %rsi, %r13		/* r13 is argv */

// ERROR HANDLING FOR FILE INPUT
	cmpq $2, %r12				/* check if argc has 2 args */
	je .L_openFileNormal		/* if two arguments, use file pointer */

	cmpq $1, %r12				/* check if argc has 1 arg */
	je .L_openFileStdin			/* if one argument, use standard input */
	
								/* if neither, its wrong */		
	movq stderr, %rdi			/* set output stream to stderr */
	movq $argvError, %rsi		/* load the error message */
	call fprintf				/* print error message */
	movq $1, %rax				/* return w errcode 1 */
	jmp .L_end					/* exit */

// OPEN FILE INPUT (argc == 2)
.L_openFileNormal:
	leaq 8(%r13), %r10				/* load position of first arg (file name) into r10 */
	movq (%r10), %rdi				/* argv[1] to %rdi */
	movq $fileRead, %rsi			/* store "r" in rsi */
	call fopen						/* open file */

	movq %rax, %r14					/* store file pointer in r14 */	
	cmpq $0, %r14					/* if null, error code */
	jne .L_loadTable				/* continue if correctly opened */

	movq stderr, %rdi				/* print in stderr */
	movq $fileOpenError, %rsi		/* error message for file not opened */
	call fprintf					/* print error msg */
	movq $1, %rax					/* return w code 1 */
	jmp .L_end						/* exit */

// OPEN STANDARD INPUT (argc == 1)
.L_openFileStdin:
	movq stdin, %r14				/* load stdin as file pointer in r14 */

// HANDLE FILE READING INTO HASH TABLE
.L_loadTable:
	xorq %r12, %r12 				/* clear r12 to store WordEntry hash table */
	xorq %r13, %r13					/* also clear r13 for the moving pointer */
	xorq %r15, %r15					/* r15 is total word count */

	movq $HASHTABLE_SIZE, %r10		/* store size of hash table */
	movq %r10, %rdi					/* move to first arg */
	movq $8, %rsi 					/* 8 bytes for a pointer */
	call calloc						/* allocate memory for the table */
	movq %rax, %r12					/* move pointer to table to r12 */	

// LOOPING FOR ENTRIES INTO HASH TABLE
.L_loadLoop: 
	// read the next word from file
	movq %r14, %rdi				/* move file pointer as first arg */
	movq $0, (%rbx)				/* clear reserved space */
	movq %rbx, %rsi				/* move reserved space to read */
	call wc_readnext 			/* read next word */
	cmpl $0, %eax 				/* check if something was read */
	je .L_getStats				/* exit the loop once no more to be read */

	incq %r15					/* increase the count of total words */

	// lowercase and trim word
	movq %rsi, %rdi				/* move word to first argument */
	call wc_tolower 			/* lowercase the given word */
	movq %rsi, %rdi				/* move returned word to first arg */
	call wc_trim_non_alpha 		/* trim the given word */
	
	// insert the word to the dictionary
	movq %rdi, %rdx							/* move word to third arg */
	movq $HASHTABLE_SIZE, %rsi				/* move the size of hashtable to second arg */
	movq %r12, %rdi							/* move hashtable to first arg */
	call wc_dict_find_or_insert				/* insert word to hashtable */
	xorq %r10, %r10							/* clear space in temp register r10 to hold return WordEntry */
	movq %rax, %r10 						/* move WordEntry to r10 */
	movq WORDENTRY_COUNT_OFFSET(%r10), %r11	/* r11 is the count of WordEntry */
	incq %r11								/* increment the count */
	movq %r11, WORDENTRY_COUNT_OFFSET(%r10)	/* return count to WordEntry */

	movq %r10, %r13							/* set word to best word, as a dummy */

	jmp .L_loadLoop							/* go back and loop */

// GET STATISTICS
.L_getStats: 			

	movq %r14, %rdi			/* move file pointer to first arg */
	call fclose 			/* close the file */

	movq $0, %rbx			/* rbx is count of the outer loop */
	
	xorq %r14, %r14 		/* initialize r14 (unique word count) */

	pushq %r15				/* save the value of total words */
	subq $8, %rsp			/* align stack */

.L_statsOuterLoop:

	cmpl $HASHTABLE_SIZE, %ebx	/* compare the count of outer loop with size of hash table */
	jge .L_printStats			/* exit the whole set of loops once done iterating */

	leaq (%r12, %rbx, 8), %r15	/* r15 is the pointer to current WordEntry */
	movq (%r15), %r15			/* dereference the WordEntry to get the head */
	
.L_statsInnerLoop:

	testq %r15, %r15			/* check whether current WordEntry pointer is null */
	je .L_statsOuterAfter		/* jump to outer if so */

	incq %r14					/* increase the count of unique words */

	movq WORDENTRY_COUNT_OFFSET(%r15), %r10 	/* r10 holds the current word count */
	movq WORDENTRY_COUNT_OFFSET(%r13), %r11 	/* r11 holds the best word count */
	cmpq %r11, %r10						 		/* compare word counts */
	jl .L_statsInnerAfter						/* iterate inner loop if the count is less than */
	je .L_statsCheckStrings						/* if they were equal, get the string with higher alphabetic order */
	jmp .L_betterWordFound						/* else, a better word was found */

.L_statsCheckStrings:

	movq %r15, %rdi				/* move current word to first arg */
	movq %r13, %rsi				/* move best word to second arg */
	call wc_str_compare 		/* compare the strings */
	cmpl $0, %eax				/* compare output with 0 (equal) */
	jge .L_statsInnerAfter 		/* if current word was not better than best word, continue looping */

// WHEN BETTER WORD IS FOUND, GET THAT WORDENTRY
.L_betterWordFound:

	movq %r15, %r13				/* move the current word to best word position */

.L_statsInnerAfter:

	movq WORDENTRY_NEXT_OFFSET(%r15), %r15	/* move current entry to next */
	jmp .L_statsInnerLoop 					/* go back to the inner loop */

.L_statsOuterAfter:

	incq %rbx 					/* increment the outer loop count */
	jmp .L_statsOuterLoop		/* jump to beginning of outer loop */

// PRINT ALL THE STATISTICS
.L_printStats:

	addq $8, %rsp				/* dealign stack */
	popq %r15					/* retrieve value of total words */

	// total words
	movq $totalWord, %rdi					/* move total word string to first arg */
	movq %r15, %rsi 						/* move count to second arg */
	call printf								/* print to stdout */

	// unique words
	movq $uniqueWord, %rdi					/* move unique word string to first arg */
	movq %r14, %rsi 						/* move count to second arg */
	call printf								/* print to stdout */

	// most frequent word
	movq $bestWord, %rdi					/* move frequent word string to first arg */
	movq %r13, %rsi 						/* move word to second arg */
	movq WORDENTRY_COUNT_OFFSET(%r13), %r10 /* move position of count to r10 */
	movq %r10, %rdx							/* return value at count to third arg */			
	call printf								/* print to stdout */

// FREE ALL ALLOCATED MEMORY WITH FREE CHAIN
.L_cleanup:

	movq $0, %rbx 		/* rbx is count of loop */

.L_freeLoop:

	cmpl $HASHTABLE_SIZE, %ebx	/* compare the count of outer loop with size of hash table */
	jge .L_freeAfter			/* exit the whole set of loops once done iterating */

	leaq (%r12, %rbx, 8), %r15	/* r15 is the pointer to current WordEntry */
	movq (%r15), %r15			/* get the head WordEntry by deferencing */

	movq %r15, %rdi				/* move WordEntry to first arg */
	call wc_free_chain 			/* free the chain of WordEntries */

	incq %rbx					/* increment counter */
	jmp .L_freeLoop				/* start loop again */

.L_freeAfter:

	movq %r12, %rdi		/* move hash table to first arg */
	call free			/* free hash table itself */

	xorq %rax, %rax 	/* return 0 (ran without error) */

// CLEANUP VARIABLES AND RETURN
.L_end:

	popq %rbx			/* restore rbx */
	popq %r15			/* restore r15 */
	popq %r14			/* restore r14 */
	popq %r13			/* restore r13 */
	popq %r12			/* restore r12 */

	ret

/*
vim:ft=gas:
*/