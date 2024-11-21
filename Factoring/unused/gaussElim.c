#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int GET(unsigned long **mas, int i, int j) {
  return ((mas[i][j >> 5] >> (j & 31)) & 1);
}

int PUT(unsigned long **mas, int i, int j, int k) {
  return mas[i][j >> 5] = (k) ? (mas[i][j >> 5] | (1 << (j & 31)))
                              : (mas[i][j >> 5] & ((1 << (j & 31)) ^ -1));
}

void GaussBin_Elimination(char *inputname, char *outputname) {
  FILE *in = fopen(inputname, "rb");
  long m, n, i, j, k, l, cols, sum, old, rem = 0;
  fscanf(in, "%ld %ld", &n, &m);
  cols = 1 + (n >> 5);
  unsigned long **mas = (unsigned long **)malloc(m * sizeof(unsigned long *));
  for (i = 0; i < m; ++i)
    mas[i] = (unsigned long *)malloc(cols * sizeof(unsigned long));
  for (i = 0; i < m; ++i)
    memset(mas[i], 0, sizeof(unsigned long) * cols);

  for (i = 0; i < n; i++)
    for (j = 0; j < m; j++) {
      fscanf(in, "%ld", &k);
      k %= 2;
      if (k)
        PUT(mas, j, i, k);
    }

  long *vert = (long *)malloc(m * sizeof(long));
  long *horz = (long *)malloc(n * sizeof(long));
  long *uncl = (long *)malloc(n * sizeof(long));
  long *solv = (long *)malloc(n * sizeof(long));
  memset(vert, 0, sizeof(long) * m);
  memset(horz, 0, sizeof(long) * n);
  memset(uncl, 0, sizeof(long) * n);

  for (i = 0; i < n; ++i)
    for (j = i + 1; j < n; ++j) {
      for (k = 0; k < m; k++)
        if (GET(mas, k, i) != GET(mas, k, j))
          break;
      if (k == m)
        horz[j] = -1, ++rem;
    }
  printf("Rejected rows: %ld\n", rem);
  for (i = 0; i < n; ++i)
    if (!horz[i]) {
      for (j = 0; j < m; ++j)
        if (vert[j] == 0 && GET(mas, j, i))
          break;
      if (j == m)
        continue;
      vert[j] = 1;
      horz[i] = j + 1;
      l = j;
      for (j = 0; j < m; ++j)
        if (GET(mas, j, i) && j != l)
          for (k = 0; k < cols; ++k)
            mas[j][k] ^= mas[l][k];
    }
  for (i = sum = 0; i < n; ++i)
    if (--horz[i] == -1)
      uncl[sum++] = i;

  FILE *out = fopen(outputname, "wb");
  old = sum;
  if (sum > 12) {
    printf(
        "The number of solutions is too many (2^%ld) ... truncated to 2^12!\n",
        sum);
    sum = 12;
  }
  fprintf(out, "%d\n", (1 << sum) - 1);
  for (i = 1; i < (1 << sum); ++i) {
    memset(solv, 0, sizeof(long) * n);
    for (j = sum; j < old; ++j)
      solv[uncl[j]] = rand() % 2;
    for (j = 0; j < sum; ++j)
      solv[uncl[j]] = (i >> j) & 1;

    for (j = 0; j < n; ++j)
      if (horz[j] >= 0) {
        for (l = k = 0; l < old; ++l)
          if (GET(mas, horz[j], uncl[l]))
            k ^= solv[uncl[l]];
        solv[j] = k;
      }

    for (j = 0; j < n; j++)
      fprintf(out, "%ld ", solv[j]);
    fprintf(out, "\n");
  }
  fclose(out);
  for (i = 0; i < m; ++i)
    free(mas[i]);
  free(mas);
  free(vert);
  free(horz);
  free(uncl);
  fclose(in);
}
