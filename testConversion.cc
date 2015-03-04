#include <iostream>

#include "short_float.cc"

using namespace std;

void convert(const float orig) {
  cout.precision(32);
  cout << orig << " " << floatFromShort(shortFromFloat(orig)) << endl;
}

int main(void) {

  convert(0);
  convert(1e-14);
  convert(1e-13);
  convert(1e-12);
  convert(1e-11);
  convert(1e-10);
  convert(1e-09);
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
  convert(1e7);
  convert(1e8);
  convert(1e9);
  convert(1e10);
  convert(1e11);
  convert(1e12);
  convert(1e13);
  convert(1e14);

  convert(-0);
  convert(-1e-14);
  convert(-1e-13);
  convert(-1e-12);
  convert(-1e-11);
  convert(-1e-10);
  convert(-1e-09);
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
  convert(-1e7);
  convert(-1e8);
  convert(-1e9);
  convert(-1e10);
  convert(-1e11);
  convert(-1e12);
  convert(-1e13);
  convert(-1e14);


  return 0;
}
