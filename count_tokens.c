#if 0
gcc -O3 count_tokens.c -o count_tokens && ./count_tokens sample.jsonl temp.bin out.csv
exit
#endif

//  training data in 00.jsonl / 29.jsonl
//  30 files total

#include "vocab.inl.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

enum { BUF_SIZE = 4096, SPACE_LEN = 2 };
char const SPACE[] = "Ġ";
static_assert(sizeof SPACE == SPACE_LEN + 1, "");

ptrdiff_t quantity[VOCAB_SIZE];

int token_eq(char const *text, char const *token) {
  for (; *text == *token || *token == '\0'; text++, token++)
    if (*token == '\0')
      return 1;
  return 0;
}

ptrdiff_t count_substrings(char const       *text,
                           char const *const token) {
  ptrdiff_t n = 0;

  ptrdiff_t len           = strlen(token);
  int const leading_space = (len >= SPACE_LEN &&
                             memcmp(token, SPACE, SPACE_LEN) == 0);

  if (leading_space)
    len--;

  while (*text != '\0') {
    int         eq = 1;
    char const *p  = token;

    if (leading_space) {
      //  check if text starts with a whitespace character
      if (*text != ' ' && *text != '\t' && *text != '\n')
        eq = 0;
      while (*text == ' ' || *text == '\t' || *text == '\n') text++;
      p = token + SPACE_LEN;
    }

    if (token_eq(text, p))
      //  can skip because tokens don't contain whitespaces
      text += len;
    else {
      text++;
      eq = 0;
    }

    if (eq)
      n++;
  }

  return n;
}

int main(int argc, char **argv) {
  if (argc != 3 && argc != 4) {
    printf(
        "Usage: `count_tokens data.jsonl temp.bin [output.csv]`\n\n");
    return 1;
  }

  char  buf[BUF_SIZE];
  char *text;

  char const *const data_file   = argv[1];
  char const *const temp_file   = argv[2];
  char const *const output_file = argc == 4 ? argv[3] : NULL;

  //  read previous quantity values
  FILE *f = fopen(temp_file, "rb");
  if (f != NULL) {
    if (fread(quantity, 1, sizeof quantity, f) != sizeof quantity) {
      printf("Can't read %s\n", temp_file);
      fclose(f);
      return 1;
    }
    fclose(f);
  } else
    memset(quantity, 0, sizeof quantity);

  f = fopen(data_file, "rt");
  if (f == NULL) {
    printf("Can't read %s\n", data_file);
    return 1;
  }

#define FAIL(x)                      \
  do {                               \
    printf("Line %d " x "\n", line); \
    fclose(f);                       \
    return 1;                        \
  } while (0)

  for (int line = 0; !feof(f); line++) {
    if (fgets(buf, sizeof buf, f) == NULL)
      break;

    int len = strlen(buf);

    if (len >= sizeof buf - 1)
      FAIL("too long");

    char const prefix[] = "{\"text\": \"";

    if (len <= sizeof prefix)
      FAIL("too short");

    if (memcmp(buf, prefix, sizeof prefix - 1) != 0)
      FAIL("wrong beginning");

    text = buf + sizeof prefix - 1;

    char *end = text;

    for (;; end++)
      if (*end == '\0')
        FAIL("wrong tail");
      else if (*end == '\\')
        end++;
      else if (*end == '\"')
        break;

    *end = '\0';

    for (ptrdiff_t token = 0; token < VOCAB_SIZE; token++)
      quantity[token] += count_substrings(text, VOCAB[token]);
  }

  fclose(f);

  //  write quantity values
  f = fopen(temp_file, "wb");
  if (f == NULL) {
    printf("Can't write %s\n", temp_file);
    return 1;
  }
  fwrite(quantity, 1, sizeof quantity, f);
  fclose(f);

  if (output_file != NULL) {
    //  save to csv
    //

    f = fopen(output_file, "wt");
    if (f == NULL) {
      printf("Can't write %s\n", output_file);
      return 1;
    }

    //  all non-zero quantity tokens
    for (ptrdiff_t i = 0; i < VOCAB_SIZE; i++)
      if (quantity[i] > 0)
        fprintf(f, "%lld,%lld\n", (long long) i,
                (long long) quantity[i]);

    //  all zero quantity tokens
    for (ptrdiff_t i = 0; i < VOCAB_SIZE; i++)
      if (quantity[i] == 0)
        fprintf(f, "%lld,%lld\n", (long long) i,
                (long long) quantity[i]);

    fclose(f);
  }

  return 0;
}
