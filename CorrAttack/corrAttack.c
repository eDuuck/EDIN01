#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

#define maxZLen 1000

void printVec(uint *vec, uint len) {
  for (int i = 0; i < len; i++) {
    printf("%d", (uint)vec[i]);
  }
  printf("\n");
}

void generateStream(uint *C, uint Clen, uint *S, uint Slen, uint *state) {
  int sum, i, l;
  for (i = Clen - 1; i >= 0; i--)
    S[i] = state[i];
  for (i = Clen - 1; i < Slen - 1; i++) {
    sum = 0;
    for (l = 0; l < Clen; l++) {
      sum += C[l] & S[i - l];
    }
    S[i + 1] = sum & 1;
  }
}

int main(int argc, char *argv[]) {
  clock_t startTime = clock();
  if (argc < 2) {
    printf("Please feed call the program with the inputs: ./corrAttack LFSRs "
           "z-String");
    return 0;
  }
  uint i, j, k, diff;
  uint zLen = 0;
  char ch;
  FILE *LFSRS = fopen(argv[1], "rb");
  uint nbrOfLFSR;
  fscanf(LFSRS, "%d", &nbrOfLFSR);
  uint *LFSRSizes = (uint *)malloc(nbrOfLFSR * sizeof(int));
  uint **Cvals = (uint **)malloc(nbrOfLFSR * sizeof(int *));
  for (i = 0; i < nbrOfLFSR; i++) {
    fscanf(LFSRS, "%d", &LFSRSizes[i]);
    Cvals[i] = (uint *)malloc(LFSRSizes[i] * sizeof(int));
    for (j = 0; j < LFSRSizes[i]; j++)
      fscanf(LFSRS, "%d", &Cvals[i][j]);
  }
  fclose(LFSRS);

  uint *zOut = (uint *)malloc(maxZLen * sizeof(int));
  FILE *zFile = fopen(argv[2], "rb");
  while ((ch = fgetc(zFile)) != EOF && zLen < maxZLen) {
    if (ch == '1')
      zOut[zLen++] = 1;
    if (ch == '0')
      zOut[zLen++] = 0;
  }

  fclose(zFile);
  if (zLen == maxZLen) {
    printf("z-String too long, aborting.");
    goto closing;
  }
  zOut = realloc(zOut, zLen * sizeof(int));

  printf("Input string: \n");
  printVec(zOut, zLen);
  uint *S = calloc(zLen, sizeof(uint));
  uint *bestCorr = calloc(nbrOfLFSR, sizeof(uint));
  float p;
  float *bestP = malloc(nbrOfLFSR * sizeof(float));
  for (i = 0; i < nbrOfLFSR; i++) {
    printf("Searching best state for LFSR %d.\n", i + 1);
    uint *state = calloc(LFSRSizes[i], sizeof(uint));

    bestP[i] = 0.5;
    for (j = 0; j < (1 << LFSRSizes[i]); j++) {
      for (k = 0; k < LFSRSizes[i]; k++)
        state[k] = (j >> k) & 0b1; // Set the state;

      generateStream(Cvals[i], LFSRSizes[i], S, zLen, state);
      diff = 0;
      for (k = 0; k < zLen; k++) {
        diff += zOut[k] ^ S[k];
      }
      float p = 1 - (float)diff / zLen;
      if (fabs(0.5 - p) > fabs(0.5 - bestP[i])) {
        // printf("Found new best state, p = %.3f, state = %d \n", p, j);
        bestP[i] = p;
        bestCorr[i] = j;
      }
    }
    free(state);
  }

  uint *output = calloc(zLen, sizeof(uint));
  printf("\nFound the keys:\n\n");
  for (i = 0; i < nbrOfLFSR; i++) {
    uint *state = calloc(LFSRSizes[i], sizeof(int));
    for (j = 0; j < LFSRSizes[i]; j++)
      state[j] = (bestCorr[i] >> j) & 0b1; // Set the state;
    printf("p = %.3f, k%d: ", bestP[i], i + 1);
    printVec(state, LFSRSizes[i]);
    generateStream(Cvals[i], LFSRSizes[i], S, zLen, state);
    for (j = 0; j < zLen; j++)
      output[j] += S[j];
  }

  // Convert to majority function and check if correct output.
  diff = 0;
  for (j = 0; j < zLen; j++) {
    output[j] = (output[j] < 2) ? 0 : 1; // Majority function.
    diff += output[j] ^ zOut[j];
  }

  printf("\nThis yields the output: \n");
  printVec(output, zLen);
  if (diff > 0)
    printf("Didn't find the correct key. Sad noises...");
  else
    printf("This matches the input exactly.");

  printf("\nFinished in %.6f seconds. \n",
         (float)(clock() - startTime) / CLOCKS_PER_SEC);
closing:
  for (i = 0; i < nbrOfLFSR; i++) {
    free(Cvals[i]);
  }
  free(bestP);
  free(output);
  free(S);
  free(zOut);
  free(Cvals);
  free(LFSRSizes);
  return 0;
}
