#include <gmp.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_SMOOTHNESS 10
#define K_DEF 1000
#define J_DEF 1000
#define L_DEF 10

// quadSieve nbrToFactor |factorBase| K_range J_range L_limit.

struct vector {
  int *array;
  int size;
};

struct guess {
  mpz_t *r;
  mpz_t *rSquared;
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
    free(factors);
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
  int l_size = L_DEF;
  int k_range = K_DEF;
  int j_range = J_DEF;
  int boundness = DEFAULT_SMOOTHNESS;
  if (argc < 3) {
    printf("No smoothness specified. Set to %d.\n", DEFAULT_SMOOTHNESS);
  } else {
    boundness = atoi(argv[2]);
  }
  if (argc > 3)
    k_range = atoi(argv[3]);
  if (argc > 4)
    j_range = atoi(argv[4]);
  if (argc > 5)
    l_size = atoi(argv[5]);
  mpz_t N;
  mpz_init_set_str(N, argv[1], 10);

  struct guess *guesses[l_size];
  int i, j, k, L_found;
  L_found = 0;

  struct vector F = factorBase(boundness);

  mpz_t sqr_kn, r, Y;
  mpz_init(sqr_kn);
  mpz_init(r);
  mpz_init(Y);
  for (k = 1; k <= k_range; k++) {
    mpz_set(sqr_kn, N);
    mpz_mul_si(sqr_kn, sqr_kn, k);
    mpz_sqrt(sqr_kn, sqr_kn);

    for (j = 1; j <= j_range; j++) {
      // printf("Testing k=%d and j=%d\n", k, j);
      mpz_set(r, sqr_kn);
      mpz_add_ui(r, r, j);
      mpz_powm_ui(Y, r, 2, N);
      struct vector factors = mod2bFact(Y, F);
      if (factors.size != 1) {
        guesses[L_found] = malloc(sizeof(struct guess));
        guesses[L_found]->r = malloc(sizeof(mpz_t));
        guesses[L_found]->rSquared = malloc(sizeof(mpz_t));
        mpz_init_set(*guesses[L_found]->r, r);
        mpz_init_set(*guesses[L_found]->rSquared, Y);
        guesses[L_found]->factors = factors;
        // printf("Success! Found %d candidates.\n", L_found + 1);
        L_found++;
        if (L_found == l_size)
          break;
      } else {
        free(factors.array);
      }
    }
    if (L_found == l_size)
      break;
  }
  printf("Done searching.\n");
  mpz_clear(sqr_kn);
  mpz_clear(r);
  mpz_clear(Y);

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
  // system("rm binInput");

  FILE *binOutput = fopen("binOutput", "rb");
  int binSolutions;
  fscanf(binOutput, "%d", &binSolutions);

  if (binSolutions > 0) {
    mpz_t diff, X, rSquared;
    int l;
    mpz_init(X);
    mpz_init(Y);
    mpz_init(diff);
    for (i = 0; i < binSolutions; i++) {
      mpz_set_si(X, 1);
      mpz_set_si(Y, 1);
      for (j = 0; j < L_found; j++) {
        fscanf(binOutput, "%d", &k);
        if (k) {
          mpz_mul(X, X, *guesses[j]->r);
          mpz_mul(Y, Y, *guesses[j]->rSquared);
          /*
          for (l = 0; l < F.size; l++) {
            int p = F.array[l] * guesses[j]->factors.array[l];
            if (p != 0) {
              mpz_mul_si(Y, Y, p);
            }
          }
          */
        }
      }
      mpz_sqrt(Y, Y);
      mpz_mod(Y, Y, N);
      mpz_mod(X, X, N);
      j = mpz_cmp(X, Y);
      if (j != 0) {
        mpz_sub(diff, Y, X);
        mpz_mod(diff, diff, N);
        gcd(diff, diff, N);
        if (mpz_cmp_ui(diff, 1) != 0) {
          printf("Try X = %ld and Y = %ld. GCD = %ld \n", mpz_get_si(X),
                 mpz_get_si(Y), mpz_get_si(diff));
        }
      }
    }
    mpz_clear(X);
    mpz_clear(Y);
    mpz_clear(diff);
  } else {
    printf("No solutions found, try different parameters.");
  }
  // system("rm binOutput");

  for (i = 0; i < L_found; i++) {
    mpz_clear(*guesses[i]->r);
    mpz_clear(*guesses[i]->rSquared);
    free(guesses[i]->factors.array);
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
