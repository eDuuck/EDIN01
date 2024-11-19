#include <gmp.h>
#include <limits.h>
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
  mpz_t n, q, r;
  mpz_inits(n);
  mpz_init(q);
  mpz_init(r);
  mpz_set(n, N);
  for (int i = 0; i < factorBase.size; i++) {
    mpz_fdiv_qr_ui(q, r, n, factorBase.array[i]); // Perform division of prime.
    while (mpz_cmp_ui(r, 0) == 0) {  // If remainder = 0, then it's divisible.
      factors[i] = (factors[i] + 1); // Add one prime.
      mpz_divexact_ui(n, n, factorBase.array[i]); // Perform division.
      mpz_fdiv_qr_ui(q, r, n, factorBase.array[i]);
    }
    if (mpz_get_si(n) == 1)
      break;
  }
  mpz_clear(q);
  mpz_clear(r);
  if (mpz_get_si(n) != 1) {
    printf("%ld", mpz_get_si(n));
    mpz_clear(n);
    return (struct vector){(int *)calloc(1, sizeof(int)), 1};
  }
  mpz_clear(n);
  return (struct vector){factors, factorBase.size};
}

// Returns GCD(A,B).
void gcd(mpz_t res, mpz_t A, mpz_t B) {
  int cmp = mpz_cmp(A, B);
  if (cmp == 0) {
    mpz_set(res, A);
    return;
  }
  mpz_t a, b, r;
  if (cmp < 0) {
    mpz_init_set(b, A);
    mpz_init_set(a, B);
  } else {
    mpz_init_set(a, A);
    mpz_init_set(b, B);
  }
  mpz_init(r);
  mpz_mod(r, a, b);
  while (mpz_cmp_ui(r, 0) != 0) {
    mpz_set(a, b);
    mpz_set(b, r);
    mpz_mod(r, a, b);
  }
  mpz_set(res, b);
  mpz_clear(a);
  mpz_clear(b);
  mpz_clear(r);
}

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

  /*
  mpz_t A,B,res;
  mpz_init_set_str(A, argv[3], 10);
  mpz_init_set_str(B, argv[4], 10);
  mpz_init(res);
  gcd(res, A, B);
  printf("GCD(%s,%s) = %ld", argv[3], argv[4], mpz_get_si(res));
*/
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
