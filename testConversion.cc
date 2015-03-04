#include <iostream>
#include <cstdio>

#include "short_float.cc"

using namespace std;

void convert(const float orig) {
  union {
    float as_float;
    uint32_t as_uint32;
  } c_orig, c_converted;
  c_orig.as_float = orig;
  short_float half = shortFromFloat(orig);
  c_converted.as_float = floatFromShort(half);
  
  printf("%22.16g %08X %04hX %08X %.16g\n", c_orig.as_float, c_orig.as_uint32, half, c_converted.as_uint32, c_converted.as_float);
}

int main(void) {

  convert(0);
  convert(-1*(+0./+0.));
  convert(1e-08);
  convert(1e-07);
  convert(1e-06);
  convert(1e-05);
  convert(1e-04);
  convert(1e-03);
  convert(1e-02);
  convert(0.1);
  convert(1);
  convert(10);
  convert(1e2);
  convert(1e3);
  convert(1e4);
  convert(1e5);
  convert(1e6);

  convert(-0);
  convert(-0./0);
  convert(-1e-08);
  convert(-1e-07);
  convert(-1e-06);
  convert(-1e-05);
  convert(-1e-04);
  convert(-1e-03);
  convert(-1e-02);
  convert(-0.1);
  convert(-1);
  convert(-10);
  convert(-1e2);
  convert(-1e3);
  convert(-1e4);
  convert(-1e5);
  convert(-1e6);


  return 0;
}
