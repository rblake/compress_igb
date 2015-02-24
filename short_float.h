//// HEADER GUARD ///////////////////////////
// If automatically generated, keep above
// comment as first line in file.
#ifndef __SHORT_FLOAT_H__
#define __SHORT_FLOAT_H__
//// HEADER GUARD ///////////////////////////

#include <stdint.h>

extern const uint16_t basetable[512];
extern const uint16_t shifttable[512];
extern const uint32_t mantissatable[2048];
extern const uint16_t offsettable[64];
extern const uint32_t exponenttable[64];

typedef uint16_t short_float;

static inline short_float shortFromFloat(const float external_ff) {
  union {
    float ff;
    uint32_t f;
  } c;
  c.ff = external_ff;
  return basetable[(c.f>>23)&0x1ff]+((c.f&0x007fffff)>>shifttable[(c.f>>23)&0x1ff]);
}

static inline float floatFromShort(const short_float h) {
  union {
    float ff;
    uint32_t f;
  } c;
  c.f = mantissatable[offsettable[h>>10]+(h&0x3ff)]+exponenttable[h>>10];
  return c.ff;
}

//// HEADER GUARD ///////////////////////////
#endif
//// HEADER GUARD ///////////////////////////
