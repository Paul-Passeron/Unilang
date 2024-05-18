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
void print_num(i32 n) {
  i32 a = (n / 10);
  if ((a != 0)) {
    print_num(a);
  }
  putchar(((n % 10) + '0'));
}
void entry() {
  print_num(12345);
  print("\n");
}
