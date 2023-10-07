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

	.section .rodata
fileRead: 		.string "r"									/* read only string literal for fopen */
fileOpenError: 	.string "Error opening file\n"				/* error message string literal for fopen */
argvError: 		.string "Wrong number of arguments.\n"		/* error message string literal for invalid argv */

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

.L_readFile:
	movq $0, %rbx 					/* curr hash# */
	cmpq $HASHTABLE_SIZE, %rbx		/* if less than continue, if not return*/
	jge .L_end
	
	incq %rbx 
	
	cmpq $0, 8(%rbx)				/* if curr word is null*/
	je .L_readFile					/*next word */
	jne .L_stat_counter

.L_stat_counter:
	incq %r11						/*unique word ++*/
	

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
