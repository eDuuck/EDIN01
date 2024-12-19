#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define maxZLen 1000

void printVec(uint *vec, uint len) {
  for (int i = 0; i < len; i++) {
    printf("%d", (uint)vec[i]);
  }
  printf("\n");
}

void gStrim(uint *C, uint Clen, uint *S, uint Slen, uint *state) {
  uint sum, i, l;
  for (i = 0; i < Clen; i++)
    S[i] = state[i];
  for (i = Clen; i < Slen; i++) {
    sum = 0;
    for (l = 0; l < Clen; l++) {
      sum += C[l] * S[i - l + 1];
    }
    S[i] = sum % 2;
  }
}
void generateStream(uint *C, uint Clen, uint *S, uint Slen, uint *state) {
  uint sum;
  for (int i = 0; i < Slen; i++) {
    S[i] = state[Clen - 1]; // Set output to state of final D-element.
    sum = 0;
    for (int j = 0; j < Clen; j++) { // Perform the state of LFSR.
      sum += state[j] & C[j];
    }
    sum = sum % 2;
    for (int j = Clen - 1; j > 0; j--) {
      state[j] = state[j - 1];
    }
    state[0] = sum;
  }
}

int main(int argc, char *argv[]) {
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

  printVec(zOut, zLen);
  // printVec(Cvals[0], LFSRSizes[0]);
  // printVec(Cvals[1], LFSRSizes[1]);
  // printVec(Cvals[2], LFSRSizes[2]);

  // I know the code above works, it prints out the correct values for CVals and
  // zOut.
  uint *S = calloc(zLen, sizeof(uint));
  uint *bestCorr = calloc(nbrOfLFSR, sizeof(uint));
  float p, bestP;
  for (i = 0; i < nbrOfLFSR; i++) {
    uint *state = calloc(LFSRSizes[i], sizeof(uint));

    uint maxDiff = UINT_MAX;
    bestP = 0.5;
    for (j = 0; j < (1 << LFSRSizes[i]); j++) {
      for (k = 0; k < LFSRSizes[i]; k++)
        state[k] = (j >> k) & 0b1; // Set the state;
      // printVec(state, LFSRSizes[i]);

      generateStream(Cvals[i], LFSRSizes[i], S, zLen, state);
      diff = 0;
      for (k = 0; k < zLen; k++) {
        diff += zOut[k] ^ S[k];
      }
      float p = 1 - (float)diff / zLen;
      // printf("%d ", diff);
      if (fabs(0.5 - p) > fabs(0.5 - bestP)) {
        // printf("Found new best state, p = %.3f, state = %d \n", p, j);
        bestP = p;
        maxDiff = diff;
        bestCorr[i] = j;
      }
    }
    // printf("\n");
    free(state);
  }

  uint *output = calloc(zLen, sizeof(uint));
  for (i = 0; i < nbrOfLFSR; i++) {
    uint *state = calloc(LFSRSizes[i], sizeof(int));
    for (j = 0; j < LFSRSizes[i]; j++)
      state[j] = (bestCorr[i] >> j) & 0b1; // Set the state;
    generateStream(Cvals[i], LFSRSizes[i], S, zLen, state);
    // printVec(S, zLen);
    for (j = 0; j < zLen; j++)
      output[j] += S[j];
  }
  for (j = 0; j < zLen; j++)
    output[j] = (output[j] < 2) ? 0 : 1;
  printVec(output, zLen);

closing:
  for (i = 0; i < nbrOfLFSR; i++) {
    free(Cvals[i]);
  }
  free(output);
  free(S);
  free(zOut);
  free(Cvals);
  free(LFSRSizes);
  return 0;
}
