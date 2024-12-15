#include <stdio.h>
#include <stdlib.h>

#define maxZLen 10

void generateStream(int *C, int Clen, int *S, int Slen) {
  int i, j;
  int sum = 0;
  int *state = calloc(Clen, sizeof(int));
  state[0] = 1; // Set the initial state to [0, 0, 0... 0, 1]
  for (i = 0; i < Slen; i++) {
    S[i] = state[Clen - 1];      // Set output to state of final D-element.
    for (j = 0; j < Clen; j++) { // Perform the state of LFSR.
      sum += state[j] * C[j];
    }
    sum = sum % 2;
    for (j = Clen - 1; j > 0; j--) {
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
  int i, j, zLen;
  FILE *LFSRS = fopen(argv[1], "rb");
  int nbrOfLFSR;
  fscanf(LFSRS, "%d", &nbrOfLFSR);
  int *LFSRSizes = (int *)malloc(nbrOfLFSR * sizeof(int));
  int **Cvals = (int **)malloc(nbrOfLFSR * sizeof(int *));
  for (i = 0; i < nbrOfLFSR; i++) {
    fscanf(LFSRS, "%d", &LFSRSizes[i]);
    Cvals[i] = (int *)malloc(LFSRSizes[i] * sizeof(int));
    for (j = 0; j < LFSRSizes[i]; j++)
      fscanf(LFSRS, "%d", &Cvals[i][j]);
  }
  fclose(LFSRS);
  int *zOut = (int *)malloc(maxZLen * sizeof(int));
  FILE *zFile = fopen(argv[2], "rb");
  while (fscanf(zFile, "%d", &zOut[zLen]) != EOF && zLen < maxZLen)
    zLen++;
  fclose(zFile);
  if (zLen == maxZLen) {
    printf("z-String too long, aborting.");
    goto closing;
  }
  zOut = realloc(zOut, zLen * sizeof(int));

closing:
  for (i = 0; i < nbrOfLFSR; i++) {
    free(Cvals[i]);
  }
  free(zOut);
  free(Cvals);
  free(LFSRSizes);
  return 0;
}
