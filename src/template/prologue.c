#include <stdint.h>
#include <stdio.h>

typedef unsigned char uintt;
typedef char i8;
typedef unsigned short u16;
typedef short i16;
typedef unsigned int u32;
typedef int i32;

void print(const char *str) {
  // temporary
  printf("%s", str);
}

void entry();

int main(void) {
  entry();
  return 0;
}

// End of Prologue
