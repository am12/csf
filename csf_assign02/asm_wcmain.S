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
#define WORDENTRY_COUNT_OFFSET  (MAX_WORDLEN+1)

	.section .rodata
fileRead: .string "r"									/* read only string literal for fopen */
fileOpenError: .string "Error opening file\n"			/* error message string literal for fopen */
argvError: .string "Wrong number of arguments.\n"		/* error message string literal for invalid argv */
totalWord: .string "Total words read: %u\n"				/* print statement for total word count */
uniqueWord: .string "Unique words read %u\n"			/* print statement for unique word count */
bestWord: .string "Most frequent word: %s (%u)\n" 		/* print statement for best word count & stats*/


	.section .text

	.globl main
main:
	subq $8, %rsp
							/*  r10 is total words count */
							/*  r11 unique words count */
	pushq %r12				/* best word pointer */
	pushq %r13				/* best word count */
	pushq %r14				/* file pointer */
	pushq %r15				/* words hashtable */

	cmpq $2, %rdi
	je .L_openFileNormal		/* if two arguements */
	cmpq $1, %rdi
	je .L_openFileStdin			/* if one argument */
								/* if neither, its wrong*/
	movq $2, %rdi				/* set filestream to rdi */
	movq $argvError, %rsi		/* print error message */
	call fprintf
	movq $1, %rax				/* return w code 1*/
	jmp .L_end

.L_openFileNormal:
	mov 8(%rsi), %rdi				/* argv[1] to %rdi */
	movq $fileRead, %rsi			/* store "r" in rsi */
	call fopen						/* open file */
	movq %rax, %r14					/* store file pointer */
	cmpq $0, %r14					/* if null, error code */
	jne .L_correctOpen
	jmp .L_errorFile			

.L_openFileStdin:
	movq stdin, %rdi				/* load stdin to %rdi */
	movq $fileRead, %rsi			/* store "r" in rsi */
	call fopen						/* open file */
	movq %rax, %r14					/* store file pointer */
	cmpq $0, %r14
	jne .L_openFileNormal
	jmp .L_errorFile				/* if null, handle as an error file */

	/* if not opened file properly */
.L_errorFile:
	movq $2, %rdi				
	movq $fileOpenError, %rsi	/* error message */
	call fprintf
	movq $1, %rax				/* return w code 1 */
	jmp .L_end

	/* if did open file properly */
.L_correctOpen:
	movq $1, %rdi						/* size of struct */
	addq $MAX_WORDLEN, %rdi	
	addq $4, %rdi						/* uint32 count */
	addq $8, %rdi						/* pointer to next entry */
	imulq $HASHTABLE_SIZE, %rdi			/* malloc memory for hashtable */
	call malloc
	movq %rax, %r15						/* pointer to hashtable to r15 */
	movq $0, %r15 						/* clear hash# */

	jmp .L_fillHash

.L_fillHash:
	movq %rdi, %dil						/* move file pointer for readnext */
	call wc_readnext
	cmpq $1, %rax						/* if 1, then next exists*/ 	
	jne .L_readFile




/* */
.L_readFile:
	cmpq $HASHTABLE_SIZE, %r15			/*rbx is pointer to hash, if less than continue, if not return*/
	jge .L_printStat

	incq %r15 
	
	cmpq $0, (%r15)					/* if curr word is null*/
	je .L_readFile						/*next word */
	jne .L_stat_counter

.L_stat_counter:
	incq %r11												/*unique word count ++*/
	cmpq %r13, WORDENTRY_COUNT_OFFSET(%r15)					/*compare best word count w curr count*/ 
	jg .L_switchBestWord									/*if greater than, switch word */
	je .L_checkWordLex										/* if its equal than, check word length; if less than move on */
	jl .L_readFile	

.L_checkWordNull:
	cmpq $0, (%r15)
	je .L_readFile							/*if null, continue readFile*/
	jne .L_checkWordLex

.L_checkWordLex:
	cmpq %r12, (%r15)						/* compare best word and curr word */
	jge .L_switchBestWord					/* if greater than or equal, switch word */
	jl .L_readFile							/* if less than, continue readFile*/

.L_switchBestWord:
	movq (%r15), %r12														/* set curr word = best word */
	movq WORDENTRY_COUNT_OFFSET(%r15), %r13									/* set curr count = best count */
	jmp .L_readFile															/* continue to readFile*/

.L_printStat:
	/* PRINT STAT */
	call fprintf
	jmp .L_end

.L_end:
	popq %r15
	popq %r14
	popq %r13
	popq %r12
	addq $8, %rsp
	ret

/*
vim:ft=gas:
*/
