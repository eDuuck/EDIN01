#include <gmp.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct vector {
  int *array;
  int size;
};

void printArray(struct vector vector) {
  for (int i = 0; i < vector.size; i++) {
    printf("%d ", vector.array[i]);
  }
  printf("\n");
}

vector array isBSmooth(struct vector factorBase, mpz_t N) {
  bool isSmooth = false;
  for (i = 0; i < factorBase.size; i++) {
    if(mpz_divmod_ui()
  }
  return 0;
}

struct vector factorBase(int bound) {
  int *primeList = (int *)calloc(bound, sizeof(int));
  primeList[0] = 2;
  int nbrOfPrimes = 1;
  for (int i = 3; i <= bound; i++) {
    int j = 0;
    bool isPrime = true;
    while (primeList[j] != 0 && isPrime) {
      if (i % primeList[j] == 0)
        isPrime = false;
      j++;
    }
    if (isPrime) {
      primeList[nbrOfPrimes] = i;
      nbrOfPrimes++;
      printf("%d is a prime\n", i);
    }
  }
  primeList = (int *)realloc(
      primeList,
      nbrOfPrimes *
          sizeof(int)); // Resize the primeList to be the correct size.
  struct vector F = {primeList, nbrOfPrimes};
  return F;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Please input a number to factor.\n");
    return 0;
  } else if (argc < 3) {
    printf("No smoothness specified. Set to XXX\n");
    int boundness = 12;
  }
  int boundness = atoi(argv[2]);

  mpz_t n;
  mpz_init_set_str(n, argv[1], 10);
  struct vector F = factorBase(boundness);

  printArray(F);

  // Comment silco believes in u, dont make him dissapointed
  char input[1024];
  printf("%s", "Enter modulus number:");
  scanf("%s", input);
  mpz_t mod;
  mpz_init_set_str(mod, input, 10);
  /*
  for(int i = 1; i <= 2; i++){
    mpz_mul_ui(n,n,i);
        }
  */
  mpz_mod(n, n, mod);
  gmp_printf("The value is: %Zd\n", n);
  return 0;
}
