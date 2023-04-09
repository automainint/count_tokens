#if 0
gcc -O3 count_tokens.c -o count_tokens && ./count_tokens
exit
#endif

//  training data in 00.jsonl / 29.jsonl
//  30 files total

#include <stdio.h>
#include "vocab.inl.h"

int main(int argc, char **argv) {
  if (argc != 3) {
    printf("Usage: `count_tokens data.jsonl output.csv`\n\n");
    return 1;
  }

  char const *const data_file = argv[1];
  char const *const output_file = argv[2];

  return 0;
}
