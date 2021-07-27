/* 
 * a simple calculator that uses a shunting yard algorithm,
 * made by btugu
 *
 * */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "btcalc.h"

#define MAXOP 64
#define MAXNUM 64

#ifndef BUFSIZE
#define BUFSIZE MAXOP + MAXNUM
#endif

uint8_t validate_input(char*);
double calculate(char*);
void exec_params(void);

// Infix expression interpretation

enum {ASSOC_N = 0, ASSOC_L, ASSOC_R};

typedef struct {

	char op;
	uint8_t prec, assoc;
	_Bool unary;
	double (*func)(double num1, double num2);

} operator;

operator ops[] = {

	{'_', 4, ASSOC_R, 1, muminus},
	{'^', 3, ASSOC_R, 0, mpow},
	{'*', 2, ASSOC_L, 0, mmul},
	{'/', 2, ASSOC_L, 0, mdiv},
	{':', 2, ASSOC_L, 0, mdiv},
	{'%', 2, ASSOC_L, 0, mmod},
	{'+', 1, ASSOC_L, 0, madd},
	{'-', 1, ASSOC_L, 0, msub},
	{'(', 0, ASSOC_N, 0, NULL},
	{')', 0, ASSOC_N, 0, NULL},

};

operator* opstack[MAXOP];
uint8_t n_opstack = 0;

double numstack[MAXNUM];
uint8_t n_numstack = 0;

operator* getop(uint8_t);
void push_opstack(operator*);
operator* pop_opstack(void);
void push_numstack(double);
double pop_numstack(void);
void process_op(operator*, char*);


_Bool Param_found;
_Bool Help_queued;
_Bool Quit_queued;

int main(int argc, char* argv[])
{

	_Bool valid;

	if(argc == 1) {

		system("clear");
		printf("btugu's calculator\n\n");
		printf("Enter 'h' or \"help\" for help.\n");
		printf("Enter 'q' or \"quit\" to exit the program.\n\n");

		while(1) {

			printf("> ");
			char* in = get_input(BUFSIZE);
			in = rm_space(in);
			valid = validate_input(in);

			if(valid) {

				exec_params();

				if(Help_queued){

					Help_queued = 0;
					n_numstack = 0;
					n_opstack = 0;
					free(in);

				} else {

					printf("%f\n", calculate(in));
					n_numstack = 0;
					n_opstack = 0;
					free(in);

				}

			}

		}

	}  

	if(!argv[2]) {

		if(strlen(argv[1]) > BUFSIZE) {
			fprintf(stderr, "ERROR: Input has more characters than the maximum allowed.\n");
			exit(EXIT_FAILURE);
		}

		switch(strasciivalsum(argv[1])) { // switch statement checks if argv[1] matches any of the valid parameters: '-h' and '--help'

			case 149:
			case 515:
				
				Help_queued = 1;
				exec_params();
				exit(EXIT_SUCCESS);
			
		}

	}

	if(argc > 2) {

		printf("Inline usage example:\n\n");
		printf("btcalc \"32 * 2 - 5\"\n\n");
		exit(EXIT_SUCCESS);

	}

	char* in = rm_space(argv[1]);

	valid = validate_input(in);

	if(valid) {

		exec_params();

		if(!Help_queued) {

			printf("%f\n", calculate(in));
			n_numstack = 0;
			n_opstack = 0;
			free(in);
			exit(EXIT_SUCCESS);

		}

	}

	exit(EXIT_FAILURE);

}

uint8_t validate_input(char* in)
{

	_Bool valid = 1;
	uint8_t i;  

	char* valid_nonalpha_chars = "^%+-/:*().,";
	char* temp = malloc(sizeof(char) * 4);

	if(Help_queued)
		exit(EXIT_SUCCESS);

	for(i = 0; in[i] != '\0'; i++) {

		if(isalpha(in[i])) {

			for(uint8_t j = 0; i == 0 && j < 4; j++)
				temp[j] = in[j];

			if(!strcmp(temp, "help")) {

				Help_queued = 1;
				free(temp);
				return valid;

			} else if (!strcmp(temp, "quit")) {

				Quit_queued = 1;
				free(temp);
				return valid;

			}

			if(i == 0) {

				if(in[i] == 'q')
					Quit_queued = 1;
				else if(in[0] == 'h')
					Help_queued = 1;

			} else {

				valid = 0;

				fprintf(stderr, "ERROR: Invalid characters were used in input.\n");

			}

			return valid;

		} else if(!(isdigit(in[i]) || ismemberof(valid_nonalpha_chars, in[i]))) {

			valid = 0;

			fprintf(stderr, "ERROR: Invalid characters were used in input.\n");

			return valid;

		}

	}

	return valid;

}

double calculate(char* in)
{

	double num1, num2;

	operator* lastop = NULL;
	operator* op = NULL;
	operator* nextop = NULL;

	for(uint8_t i = 0; in[i] != '\0'; i++) {

		if(op = getop(in[i])) {

			if(in[i + 1] == '\0') {
				
				if(op->op != ')') {

					fprintf(stderr, "ERROR: Syntax error\n");
					n_numstack = 0;
					n_opstack = 0;
					return EXIT_FAILURE;

				}

			} else if(nextop = getop(in[i + 1])) {

				if((nextop->op != '(') && (nextop->op != ')') && (nextop->op != '-')) {

					if(op->op != ')') {

						fprintf(stderr, "ERROR: Syntax error\n");
						n_numstack = 0;
						n_opstack = 0;
						return EXIT_FAILURE;

					}

				}

			}

			if(op->op == '-' && !n_numstack)
				op = getop('_');

			if(!lastop) {

				switch(op->op) {

					case '(':

						if(i == 0)
							break;

					case ')':

						fprintf(stderr, "ERROR: Syntax error\n");
						n_numstack = 0;
						n_opstack = 0;
						return EXIT_FAILURE;

					case '%':
					case '/':
					case ':':
					case '*':
					case '^':

						if(n_numstack == 0) {

							fprintf(stderr, "ERROR: Syntax error\n");
							n_numstack = 0;
							n_opstack = 0;
							return EXIT_FAILURE;

						}

						break;

				}

			}

			process_op(op, in);
			lastop = op;

		} else {

			uint8_t j = i;

			while((getop(in[j]) == NULL) && (in[j] != '\0'))
				j++;

			push_numstack(strtodec(cut_str(in, i, j - 1)));
			i = j - 1;

		}

	}

	while(n_numstack > 1) {
		
		operator* popped_op = pop_opstack();
		double num1 = pop_numstack();

		if(popped_op->unary) {

			push_numstack(popped_op->func(num1, 0));

		} else {

			double num2 = pop_numstack();
			push_numstack(popped_op->func(num2, num1));

		}

	}

	return numstack[0];

}

void exec_params(void)
{

	if(Help_queued) {

		printf("\nValid inline arguments: \"-h\", \"--help\"\n\n");
		printf("Valid continuous mode arguments: \"help\", \"h\", \"q\"\n\n");
		printf("Valid characters are: Binary operators, digits, comma and dot\
(both used to represent decimal numbers)\n\n");
		printf("Example inline usage: btcalc \"32 * 2 - 5\"\n\n");

	} else if(Quit_queued) {
		
		exit(EXIT_SUCCESS);

	}

}

// Infix interpretation

operator* getop(uint8_t c)
{

	for(uint8_t i = 0; i < (sizeof(ops) / sizeof(ops[0])); i++) {

		if(ops[i].op == c)
			return ops + i;

	}

	return NULL;

}

void push_opstack(operator* op)
{

	if(n_opstack > MAXOP - 1) {
		
		fprintf(stderr, "ERROR: Operator stack overflow\n");
		exit(EXIT_FAILURE);

	}

	opstack[n_opstack++] = op;

}

operator* pop_opstack()
{

	if(!n_opstack) {
	
		fprintf(stderr, "ERROR: Operator stack empty\n");
		exit(EXIT_FAILURE);

	}

	--n_opstack;
	return opstack[n_opstack];

}


void push_numstack(double num)
{

	if(n_numstack > MAXNUM - 1) {

		fprintf(stderr, "ERROR: Number stack overflow\n");
		exit(EXIT_FAILURE);

	}

	numstack[n_numstack++] = num;

}

double pop_numstack()
{

	if(!n_numstack) {

		fprintf(stderr, "ERROR: Number stack empty\n");
		exit(EXIT_FAILURE);

	}

	--n_numstack;
	return numstack[n_numstack];

}

void process_op(operator* op, char* in)
{

	operator* popped_op;
	double num1, num2;

	if(op->op == '(') {

		push_opstack(op);
		return;

 	} else if(op->op == ')') {

		while(n_opstack > 0 && opstack[n_opstack - 1]->op != '(') {

			popped_op = pop_opstack();
			num1 = pop_numstack();

			if(popped_op->unary) {

				push_numstack(popped_op->func(num1, 0));

			} else {

				num2 = pop_numstack();
				push_numstack(popped_op->func(num2, num1));

			}

		}

		if(!(popped_op = pop_opstack()) || popped_op->op != '(') {
			
			fprintf(stderr, "ERROR: Unmatched Parenthesis\n");
			exit(EXIT_FAILURE);

		}

		return;

	}

	if(op->assoc == ASSOC_R) {

		while(n_opstack && op->prec < opstack[n_opstack - 1]->prec) {

			popped_op = pop_opstack();
			num1 = pop_numstack();

			if(popped_op->unary) {

				push_numstack(popped_op->func(num1, 0));

			}	else {

				num2 = pop_numstack();
				push_numstack(popped_op->func(num2, num1));

			}

		}

	} else {

		while(n_opstack && op->prec <= opstack[n_opstack - 1]->prec) {

			popped_op = pop_opstack();
			num1 = pop_numstack();

			if(popped_op->unary) {

				push_numstack(popped_op->func(num1, 0));

			} else {

				num2 = pop_numstack();
				push_numstack(popped_op->func(num2, num1));

			}

		}

	}

	push_opstack(op);
	
}
