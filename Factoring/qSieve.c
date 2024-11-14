#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <gmp.h>

void printArray(int* array){
  for(int i = 0;
      i < sizeof(array) / sizeof(array[0]);
      i++){
    printf("%d ", array[i]);
  }
  printf("\n");
}

int* factorBase(int bound){
  int *primeList = (int *)calloc(bound,sizeof(int));
  primeList[0] = 2;
  int nbrOfPrimes = 1;
  for(int i = 3; i <= bound; i++){
    int j = 0;
    bool isPrime = true;
    while(primeList[j] != 0 && isPrime){
      if(i%primeList[j] == 0) isPrime = false;
      j++;
    }
    if(isPrime){
      primeList[nbrOfPrimes]=i;
      nbrOfPrimes++;
      printf("%d is a prime\n", i);
    }
  }
  int* F = (int*)malloc(nbrOfPrimes * sizeof(int)); //Allocate space for returned array.
  for(int i = 0; i<nbrOfPrimes;i++){
    F[i] = primeList[i];
  }
  return F;
}


int main(int argc, char *argv[]) { 
  
  if(argc < 2) {
    printf("Please input a number to factor.\n");
    return 0;
  }else if (argc < 3) {
    printf("No smoothness specified. Set to XXX\n");
    int boundness = 12;
  }
  int boundness = atoi(argv[2]);

  int* F = factorBase(boundness);
  
  printArray(F);

  printf("Enter number to reduce:");//Comment silco believes in u, dont make him dissapointed  
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
  gmp_printf("The value is: %Zd\n",n);
  return 0;
}
