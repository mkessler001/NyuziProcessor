
					.text
					.globl _start
					.align 4
					.type _start,@function
_start:				move s0, 15
					setcr s0, 30		; Start all threads

					load.32 sp, stacks_base
					getcr s0, 0			; get my strand ID
					shl s0, s0, 12		; 4096 bytes per stack
					add.i sp, sp, s0	; Compute stack address

					call main
					setcr s0, 29		; Stop thread
done:				goto done

stacks_base:		.word 0x1013C000

