#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define len 4
const int C2[len] = {1, 0, 0, 1};
const int C5[len] = {1, 2, 0, 2};

struct Z {
  int *S;
  int length;
  int base;
};

void Z2Shift(int *P) {
  int sum = 0;
  int state = 0b0;
  for (int i = 0; i < len; i++) {
    sum += P[i] * C2[i];
    state += P[i] << i;
  }
  sum += state == 0b1000 || state == 0b0000;
  sum = sum % 2;
  for (int i = len - 1; i > 0; i--) {
    P[i] = P[i - 1];
  }
  P[0] = sum;
}

void Z5Shift(int *P) {
  int sum = 0;
  int state = 0;
  for (int i = 0; i < len; i++) {
    sum += P[i] * (C5[i]);
    state += P[i] * pow(5, i);
  }
  sum = sum % 5;
  if (state == 3 * pow(5, 3))
    sum = 0;
  if (state == 0)
    sum = 1;
  for (int i = len - 1; i > 0; i--) {
    P[i] = P[i - 1];
  }
  P[0] = sum;
}

int main(int argc, char *argv[]) {
  int P5[len] = {0, 0, 0, 0};
  int P2[len] = {0, 0, 0, 0};
  int loop = atoi(argv[1]);
  FILE *shiftOutput = fopen("out", "wb");
  for (int i = 0; i < loop; i++) {
    fprintf(shiftOutput, "%d", P5[len - 1] + P2[len - 1] * 5);
    // printf("%d", P5[len - 1] + P2[len - 1] * 5);
    Z2Shift(P2);
    Z5Shift(P5);
  }
  fclose(shiftOutput);
  return 0;
}
