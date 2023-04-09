#if 0
gcc -O3 gen_vocab.c -o gen_vocab && ./gen_vocab
exit
#endif

//  tokens in vocab.json
//  50257 tokens total

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { VOCAB_SIZE = 50257, BUF_SIZE = 260 };

char VOCAB[VOCAB_SIZE][BUF_SIZE];

int   line   = 0;
int   c_prev = 0;
int   c      = 0;
FILE *f      = NULL;
FILE *out    = NULL;

void fail(char const *const error) {
  printf("Error: %s  (line %d, parsing %c%c%c).\n", error, line + 2,
         c_prev, c, fgetc(f));
  fclose(f);
  exit(1);
}

int next() {
  if (feof(f))
    fail("Unexpected EOF");

  c_prev = c;
  c      = fgetc(f);
  return c;
}

int main(int argc, char **argv) {
  char buf[BUF_SIZE];
  int  offset;
  int  index;

  f = fopen("vocab.json", "rt");

  memset(*VOCAB, 0, VOCAB_SIZE * BUF_SIZE);

  //  skip first line
  while (next() != '\n')
    ;

  //  read lines
  for (line = 0; line < VOCAB_SIZE; line++) {
    //  skip spaces
    while (next() == ' ') { }

    if (c != '\"')
      fail("Expected '\"'");

    //  read token
    offset = 0;

    int esc = 0;

    for (;;) {
      next();

      if (esc)
        esc = 0;
      else if (c == '\\')
        esc = 1;
      else if (c == '\"')
        break;

      if (c == '?')
        //  avoid trigraphs
        buf[offset++] = '\\';

      if (offset + 1 >= BUF_SIZE)
        fail("Token too long");

      buf[offset++] = c;
    }

    buf[offset] = '\0';

    if (next() != ':')
      fail("Expected ':'");

    //  skip spaces
    while (next() == ' ') { }

    //  parse int
    index = 0;
    for (;;) {
      if (c < '0' || c > '9')
        break;

      index = index * 10 + (c - '0');

      next();
    }

    if (index < 0 || index >= VOCAB_SIZE)
      fail("Invalid index");

    if (c == ',')
      next();

    if (c != '\n')
      fail("Expected EOL");

    if (VOCAB[index][0] != '\0')
      fail("Index duplicate");

    memcpy(VOCAB[index], buf, BUF_SIZE);
  }

  fclose(f);

  out = fopen("vocab.inl.h", "wt");
  fprintf(out, "enum { VOCAB_SIZE = %d };\n", VOCAB_SIZE);
  fprintf(out, "char const *const VOCAB[VOCAB_SIZE] = {\n");
  for (ptrdiff_t i = 0; i < VOCAB_SIZE; i++)
    fprintf(out, "  \"%s\",\n", VOCAB[i]);
  fprintf(out, "};\n");
  fclose(out);
  return 0;
}
