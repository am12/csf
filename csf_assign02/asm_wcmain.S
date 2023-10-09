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
	subq $8, %rsp		/* align stack */

	pushq %r12			/* save r12 */	
	pushq %r13			/* save r13 */		
	pushq %r14			/* save r14 */	
	pushq %r15			/* save r15 */	
	pushq %rbx			/* save rbx */	
	pushq %rbp			/* save rbp */	

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
	imulq $8, %r10					/* calculate overall size of table */
	movq %r10, %rdi					/* move to first arg */
	call malloc						/* allocate memory for the table */
	movq %rax, %r12					/* move pointer to table to r12 */	

	//movq %rsp, %rbx					/* save current position of stack pointer to rbx */
	//addq $64, %rsp 					/* reserve 64 bytes on stack for the word from readnext */

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
	call wc_trim_non_alpha 		/* trim the given word */
	
	// insert the word to the dictionary
	movq %rdi, %rdx						/* move word to third arg */
	movq $HASHTABLE_SIZE, %rsi			/* move the size of hashtable to second arg */
	movq %r12, %rdi						/* move hashtable to first arg */
	call wc_dict_find_or_insert			/* insert word to hashtable */
	xorq %r10, %r10						/* clear space in temp register r10 to hold return WordEntry */
	movq %rax, %r10 					/* move WordEntry to r10 */
	incq WORDENTRY_COUNT_OFFSET(%r10)	/* increment the count of WordEntry */

	jmp .L_loadLoop						/* go back and loop */

// GET STATISTICS
.L_getStats: 

	movq $0, %r11			/* r11 is count of the outer loop */
	

//.L_statsOuterLoop:

//.L_statsInnerLoop:

	// iterate over the whole table
	







// print all the statistics


.L_cleanup:

	//subq $64, %rsp		/* reset space allocated for word read input */

// FREE ALL ALLOCATED MEMORY WITH FREE CHAIN

	movq %r12, %rdi		/* move hash table to first arg */
	call free			/* free hash table itself */


// MAKE SURE FILE IS CLOSED


	xorq %rax, %rax 	/* return 0 (ran without error) */

// CLEANUP VARIABLES AND RETURN
.L_end:

	popq %rbp			/* restore rbp */
	popq %rbx			/* restore rbx */
	popq %r15			/* restore r15 */
	popq %r14			/* restore r14 */
	popq %r13			/* restore r13 */
	popq %r12			/* restore r12 */

	addq $8, %rsp		/* restore stack */

	ret

/*
vim:ft=gas:
*/
