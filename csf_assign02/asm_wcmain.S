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
	fileRead: .string "r"		/* read only string literal for fopen */
	fileOpenError: .string "Error opening file\n"	/* error message string literal for fopen */
	argvError: .string "Wrong number of arguments.\n"	/* error message string literal for invalid argv */

	.section .text

	.globl main
main:
	subq $8, %rsp
	pushq %r10				/* total words count */
	pushq %r11				/* unique words count */
	pushq %r12				/* best word pointer */
	pushq %r13				/* best word count */
	pushq %r14				/* file pointer */
	pushq %r15				/* words hashtable */

	cmpq $2, %rdi
	je .L_openFileNormal			/* if two arguements */
	cmpq $1, %rdi
	je .L_openFileStdin			/* if one argument */
								/* else, wrong number of arguments */
	movq $2, %rdi				/* set filestream to stderr (2) */
	movq $argvError, %rsi		/* load error message */
	call fprintf
	movq $1, %rax				/* return exit code 1 */
	jmp .L_endMain

.L_openFileNormal:
	mov 8(%rsi), %rdi		/* load argv[1] to %rdi */
	movq $fileRead, %rsi			/* store str literal "r" as 2nd arg */
	call fopen
	movq %rax, %r14					/* store file pointer to allocated register */
	cmpq $0, %r14
	jne .L_OpenedFileProperly
	jmp .L_errorOpeningFile			/* jump if opened file ptr is null */

.L_openFileStdin:
	movq stdin, %rdi				/* load stdin to %rdi */
	movq $fileRead, %rsi			/* store str literal "r" as 2nd arg */
	call fdopen
	movq %rax, %r14					/* store file pointer to allocated register */
	cmpq $0, %r14
	jne .L_OpenedFileProperly
	jmp .L_errorOpeningFile			/* jump if opened file ptr is null */

	/* if not opened file properly */
.L_errorOpeningFile:
	movq $2, %rdi				/* set filestream to stderr (2) */
	movq $fileOpenError, %rsi	/* load error message */
	call fprintf
	movq $1, %rax				/* return exit code 1 */
	jmp .LendMain

	/* if did open file properly */
.LOpenedFileProperly:
	movq $1, %rdi				/* calculate size of WordEntry struct */
	addq $MAX_WORDLEN, %rdi		/* unsigned char array of size MAX_WORDLEN + 1 */
	addq $4, %rdi				/* uint32 count */
	addq $8, %rdi				/* pointer to next WordEntry */
	
	imulq $HASHTABLE_SIZE, %rdi	/* total memory needed for hashtable */
	call malloc
	movq %rax, %r15				/* store pointer to hashtable to %r15 */
	
	/* TODO: initialize malloc'd space to null and read through file */

.L_endMain:
	popq %r15
	popq %r14
	popq %r13
	popq %r12
	popq %r11
	popq %r10
	ret

/*
vim:ft=gas:
*/
