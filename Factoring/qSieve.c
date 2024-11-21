#include <gmp.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_SMOOTHNESS 10
#define K_RANGE 1000
#define J_RANGE 1000
#define L_SIZE 100

struct vector {
  int *array;
  int size;
};

struct guess {
  mpz_t *r;
  struct vector factors;
};

void printArray(struct vector vector) {
  for (int i = 0; i < vector.size; i++) {
    printf("%d ", vector.array[i]);
  }
  printf("\n");
}

// Calculates the factorization of N with a given factor base. Returns
// the array {0} if the number is not factorizable with the factor base.
struct vector mod2bFact(mpz_t N, struct vector factorBase) {
  int *factors = (int *)calloc(factorBase.size, sizeof(int));
  if (mpz_cmp_ui(N, 1) == 0)
    return (struct vector){(int *)calloc(1, sizeof(int)), 1};
  mpz_t n, q, r;
  mpz_init(n);
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
    if (mpz_cmp_ui(n, 1) == 0)
      break;
  }
  mpz_clear(q);
  mpz_clear(r);
  if (mpz_cmp_ui(n, 1) != 0) {
    // printf("%ld", mpz_get_si(n));
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
  return (struct vector){primeList, nbrOfPrimes};
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

  struct guess *guesses[L_SIZE];
  int i, j, k, L_found;
  L_found = 0;

  struct vector F = factorBase(boundness);

  mpz_t sqr_kn, r, Y;
  mpz_init(sqr_kn);
  mpz_init(r);
  mpz_init(Y);
  for (k = 1; k <= K_RANGE; k++) {
    mpz_set(sqr_kn, N);
    mpz_mul_si(sqr_kn, sqr_kn, k);
    mpz_sqrt(sqr_kn, sqr_kn);

    for (j = 1; j <= J_RANGE; j++) {
      // printf("Testing k=%d and j=%d\n", k, j);
      mpz_set(r, sqr_kn);
      mpz_add_ui(r, r, j);
      mpz_powm_ui(Y, r, 2, N);
      struct vector factors = mod2bFact(Y, F);
      if (factors.size != 1) {
        guesses[L_found] = malloc(sizeof(struct guess));
        guesses[L_found]->r = malloc(sizeof(mpz_t));
        mpz_init(*guesses[L_found]->r);
        mpz_set(*guesses[L_found]->r, r);
        guesses[L_found]->factors = factors;
        printf("Success! Found %d candidates.\n", L_found + 1);
        // printArray(factors);
        // printf("\n");
        L_found++;
        if (L_found == L_SIZE)
          break;
      }
    }
    if (L_found == L_SIZE)
      break;
  }
  printf("Done searching.\n");
  /*
  for (i = 0; i < L_found; i++)
    printArray(guesses[i]->factors);
  */
  FILE *binInput = fopen("binInput", "wb");

  fprintf(binInput, "%d %d \n", L_found, F.size);
  for (i = 0; i < L_found; i++) {
    for (j = 0; j < F.size; j++) {
      fprintf(binInput, "%d ", guesses[i]->factors.array[j]);
    }
    fprintf(binInput, " ");
  }
  fclose(binInput);
  system("./GaussBin binInput binOutput");

  FILE *binOutput = fopen("binOutput", "rb");
  int binSolutions;
  fscanf(binOutput, "%d", &binSolutions);
  if (!(binSolutions > 0)) {
    printf("No solutions found, try different parameters.");
    exit(EXIT_FAILURE);
  }
  mpz_t X;
  mpz_init(X);
  // mpz_init(Y);
  for (i = 0; i < binSolutions; i++) {
    mpz_set_si(X, 1);
    mpz_set_si(Y, 1);
    for (j = 0; j < L_found; j++) {
      fscanf(binOutput, "%d", &k);
      if (k) {
        mpz_mul(X, X, *guesses[j]->r);
        mpz_mod(X, X, N);
        for (int l = 0; l < F.size; l++) {
          int p = F.array[l] * guesses[j]->factors.array[l];
          if (p != 0) {
            mpz_mul_si(Y, Y, p);
          }
        }
        // mpz_sqrt(Y, Y);
        // mpz_mod(Y, Y, N);
      }
    }
    mpz_sqrt(Y, Y);
    mpz_mod(Y, Y, N);
    j = mpz_cmp(X, Y);
    if (j != 0) {
      if (j < 0) {
        mpz_sub(r, Y, X);
      } else {
        mpz_sub(r, X, Y);
      }
      gcd(r, r, N);
      if (mpz_cmp_ui(r, 1) != 0) {
        printf("Try X = %ld and Y = %ld. GCD = %ld \n", mpz_get_si(X),
               mpz_get_si(Y), mpz_get_si(r));
      }
    }
    //}
  }
  /*
  mpz_t A,B,res;
  mpz_init_set_str(A, argv[3], 10);
  mpz_init_set_str(B, argv[4], 10);
  mpz_init(res);
  gcd(res, A, B);
  printf("GCD(%s,%s) = %ld", argv[3], argv[4], mpz_get_si(res));
*/ //  Comment silco believes in u, dont make him dissapointed
  return 0;
}
