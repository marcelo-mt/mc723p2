#include<stdio.h>

int fib (int n) {
	if (n<2) {
		return 1;
	}
	return fib(n-1)+fib(n-2);
}

int main () {
	printf("Fibonacci de 25: %d\n", fib(25));
	return 0;
}
