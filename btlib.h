#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#ifndef BUFSIZE
#define BUFSIZE 128
#endif

// Function declarations

double muminus(double, double);
double madd(double, double);
double msub(double, double);
double mmul(double, double);
double mdiv(double, double);
double mpow(double, double);
double mmod(double, double);
uint16_t strasciivalsum(char*); 					// Returns the sum of the ascii values of the characters in a string
uint8_t chartoint(char);        					// Converts a char digit into a numeric digit.
char* merge_args(int, char**);                      // Merge args of an arg array into one array, putting no space inbetween args.
char* get_input(size_t);
char* rm_space(char*);                              // Remove spaces from a string
char* cut_str(char*, uint8_t, uint8_t); 	        // Returns the string given in the parameters as a new string but starting from index_start and ending in index_end
char* stob(short);																	// Returns binary representation of a short (16 bit signed integer)
_Bool ismemberof(char*, uint8_t);                  // Checks if a char is an element of a string
_Bool isarrmemberof(char**, char*);				    // Checks if an array is an element of a 2D array
_Bool xor(_Bool, _Bool);
double strtodec(char*); 							// Converts a number in a string into its decimal value.

// Mathematical Functions

double muminus(double num1, double num2) {return -num1;}
double madd(double num1, double num2) {return num1 + num2;}
double msub(double num1, double num2) {return num1 - num2;}
double mmul(double num1, double num2) {return num1 * num2;}

double mdiv(double num1, double num2) {

	if(!num2) {

		fprintf(stderr, "ERROR: Division by zero\n");
		return EXIT_FAILURE;

	}

	return num1 / num2;

}

double mpow(double x, double y)
{

	if(y >= 1) {

		return x * mpow(x, y - 1);

	} else if(y == 0) {

		return 1;

	} else {

		return 1/mpow(x, -y);

	}

}

double mmod(double dividend, double divisor)
{

	_Bool dividend_is_negative = dividend < 0;
	_Bool divisor_is_negative = divisor < 0;

	double result = dividend;

	if(!divisor) {

		fprintf(stderr, "ERROR: Division by zero\n");
		return EXIT_FAILURE;

	}

	if(dividend == 0) {
		return 0;
	}

	if((!dividend_is_negative && !divisor_is_negative) || (dividend_is_negative && divisor_is_negative)) {

		if(!dividend_is_negative && !divisor_is_negative)
			while(result - divisor >= 0)
				result -= divisor;
		else
			while(result - divisor <= 0)
				result -= divisor;

	} else if(xor(dividend_is_negative, divisor_is_negative)) {

		if(dividend_is_negative)
			do {
				result += divisor;
			} while(result < 0);
		else 
			do {
				result += divisor;
			} while(result > 0);

	}

	return result;

}

// Others

// Returns the sum of the ascii values of the characters in a string
uint16_t strasciivalsum(char* str)
{

	uint16_t ret = 0;
	uint8_t i = 0;

	for(; str[i] != '\0'; i++)
		ret += str[i];

	return ret;

}

// Converts a char digit into a numeric digit.
uint8_t chartoint(char c) 
{ 

	return c - 48; 

}

// Merge args of an arg array 
// into one array, putting no space inbetween args.
char* merge_args(int argc, char** args)
{

	char* arr = malloc(sizeof(char) * BUFSIZE);
	uint8_t i = 1;

	while(i < argc)
		strcat(arr, args[i++]);

	arr = realloc(arr, strlen(arr) + 1);
	return arr;

}

char* get_input(size_t bufsize)
{

	char* in = malloc(sizeof(char) * bufsize);
	uint8_t c = 0, i = 0;

	for(; i < bufsize - 1 && (c = getchar()) != EOF && c != '\n'; i++)
		in[i] = c;
	
	if(c == '\n')
		in[i] = '\0';
	
	return in;

}

// Remove spaces from a string
char* rm_space(char* in)
{

	char* out = malloc(sizeof(char) * BUFSIZE);
	uint8_t i = 0, j = 0;

	for(; in[i] != '\0'; i++) {

		if(in[i] != ' ')
			out[j++] = in[i];

	}

	out[j] = '\0';
	out = realloc(out, j + 1);
	return out;
	
}

// Returns the string given in the parameters as a new string but
// starting from index_start and ending in index_end
char* cut_str(char* in, uint8_t index_start, uint8_t index_end)
{

	char* ret = malloc(sizeof(char) * (index_end - index_start + 2));
	uint8_t i = 0, j = 0;

	if(index_start == index_end) {

		ret = realloc(ret, sizeof(char) * 1);
		ret[0] = in[index_start];
		return ret;

	}

	while(1) {

		if(i == index_start) {

			while(i <= index_end)
				ret[j++] = in[i++];

		} else {

			i++;
			continue;

		}

		ret[j] = '\0';
		return ret;

	}

}

// Returns binary representation of a short (16 bit signed integer); 127
char* stob(short num)
{

	char* bin = malloc(sizeof(char) * 16);

	for(uint8_t i = 0; i < 16; i++)
		bin[i] = '0';

	if(num < 0) {

		bin[0] = '1';
		short cur = mpow(-2, 15);

		for(uint8_t i = 15; i > 0; i--) {

			short x = mpow(2.0, (double)i-1);

			if(cur + x <= num) {

				bin[16 - i] = '1';
				cur += x;

			}

		}

	} else if(num > 0) {

		for(uint8_t i = 15; i > 0; i--) {

			short x = mpow(2.0, (double)i-1);

			if(num/x >= 1) {

				bin[16 - i] = '1';
				num -= x;

			}

		}

	}

	return bin;

}

// Checks if a char is an element of a string
_Bool ismemberof(char* str, uint8_t c)
{

	for(uint8_t i = 0; i < strlen(str); i++) {
	
		if(c == str[i])
			return true;

	}

	return false;

}

// Checks if an array is an element of a 2D array
_Bool isarrmemberof(char** arrs, char* arr)
{
	uint8_t j = 0;

	while(arrs[j] != '\0');
		j++;

	for(uint8_t i = 0; i < j; i++) {

		if(!strcmp(arr, arrs[i]))
			return true;

	}

	return false;

}

_Bool xor(_Bool a, _Bool b)
{

	return (a || b) && !(a && b);

}

// Converts a number in a string into its decimal value.
double strtodec(char* str)
{

	double dec = 0;
	uint8_t i = 0, j = strlen(str) - 1, k = 0;

	while(str[i] != '.' && str[i] != ',' && str[i] != '\0')
		i++;
	
	if(str[i] == '\0') {

		for(; k < i; k++) {

			dec += mpow(10, k) * (double)chartoint(str[j - k]);

		}

		return dec;

	} else {

		for(; k < i; k++) {

			dec += mpow(10, k) * (double)chartoint(str[j - k - 1]);
			
		}

	}

	uint8_t dec_places = j - i;

	while(dec_places-- > 1) {

		dec += mpow(10, -dec_places) * (double)chartoint(str[j--]);

	}

	return dec;

}
