#include <gmp.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_SMOOTHNESS 100

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

// Calculates the factorization of N with a given factor base. Returns
// the array {0} if the number is not factorizable with the factor base.
struct vector baseFactorize(mpz_t N, struct vector factorBase) {
  int *factors = (int *)calloc(factorBase.size, sizeof(int));
  mpz_t temp, q, r;
  mpz_init(temp);
  mpz_init(q);
  mpz_init(r);
  mpz_set(temp, N);
  for (int i = 0; i < factorBase.size; i++) {
    mpz_fdiv_qr_ui(q, r, temp,
                   factorBase.array[i]); // Perform division of prime.
    while (mpz_get_si(r) == 0) {     // If remainder = 0, then it's divisible.
      factors[i] = (factors[i] + 1); // Add one prime.
      mpz_divexact_ui(temp, temp, factorBase.array[i]); // Perform division.
      mpz_fdiv_qr_ui(q, r, temp, factorBase.array[i]);
    }
    if (mpz_get_si(temp) == 1)
      break;
  }
  mpz_clear(q);
  mpz_clear(r);
  if (mpz_get_si(temp) != 1) {
    printf("%ld", mpz_get_si(temp));
    mpz_clear(temp);
    return (struct vector){(int *)calloc(1, sizeof(int)), 1};
  }
  mpz_clear(temp);
  return (struct vector){factors, factorBase.size};
}

/*
// Returns a vector containing [GCD(A,B), X, Y] such that GCD(A,B) = XA + YB.
struct vector gcd(mpz_t A, mpz_t B) { return 0; }
*/

struct vector factorBase(int baseSize) {
  int *primeList = (int *)calloc(baseSize, sizeof(int));
  primeList[0] = 2;
  int nbrOfPrimes = 1;
  for (int i = 3; nbrOfPrimes < baseSize; i++) {
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
      // printf("%d is a prime\n", i);
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
  }
  int boundness = DEFAULT_SMOOTHNESS;
  if (argc < 3) {
    printf("No smoothness specified. Set to %d.\n", DEFAULT_SMOOTHNESS);
  } else {
    boundness = atoi(argv[2]);
  }
  mpz_t N;
  mpz_init_set_str(N, argv[1], 10);

  struct vector F = factorBase(boundness);

  // printArray(F);
  struct vector base = baseFactorize(N, F);
  if (base.size != 1) {
    printf("Factorized the number.");
  } else {
    printf("Gee dang, didn't factorize.");
  }
  //  Comment silco believes in u, dont make him dissapointed
  return 0;
}
