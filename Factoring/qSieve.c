#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <gmp.h>


void main() {
  printf("%s","Enter number to reduce:");//Comment silco believes in u, dont make him dissapointed  
	char input[1024];
	scanf("%s", input);
	mpz_t n;
  mpz_init_set_str(n,input,10);
  printf("%s","Enter modulus number:");
	scanf("%s", input);
	mpz_t mod;
  mpz_init_set_str(mod,input,10);
/*
  for(int i = 1; i <= 2; i++){
    mpz_mul_ui(n,n,i);
	}
  */
  mpz_mod(n,n,mod);
  mpz_out_str(stdout,10,n);
}
