#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

void main() {
	int number;
	scanf("%d", &number);
	long output = 1;
	for(int i = 1; i <= number; i++){
		output *= i;
	}
	printf("%ld",output);
}
