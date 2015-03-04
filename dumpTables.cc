
#include <cstdint>
#include <cstdio>

uint16_t basetable[512];
uint8_t shifttable[512];

uint32_t mantissatable[2048];
uint16_t offsettable[64];
uint32_t exponenttable[64];

void generatetables(){
  unsigned int i;
  int e;
  for(i=0; i<256; ++i){
    e=i-127;
    if(e<-24){ // Very small numbers map to zero
      basetable[i|0x000]=0x0000;
      basetable[i|0x100]=0x8000;
      shifttable[i|0x000]=24;
      shifttable[i|0x100]=24;
    }
    else if(e<-14){ // Small numbers map to denorms
      basetable[i|0x000]=(0x0400>>(-e-14));
      basetable[i|0x100]=(0x0400>>(-e-14)) | 0x8000;
      shifttable[i|0x000]=-e-1;
      shifttable[i|0x100]=-e-1;
    }
    else if(e<=15){ // Normal numbers just lose precision
      basetable[i|0x000]=((e+15)<<10);
      basetable[i|0x100]=((e+15)<<10) | 0x8000;
      shifttable[i|0x000]=13;
      shifttable[i|0x100]=13;
    }
    else if(e<128){ // Large numbers map to Infinity
      basetable[i|0x000]=0x7C00;
      basetable[i|0x100]=0xFC00;
      shifttable[i|0x000]=24;
      shifttable[i|0x100]=24;
    }
    else{ // Infinity and NaN's stay Infinity and NaN's
      basetable[i|0x000]=0x7C00;
      basetable[i|0x100]=0xFC00;
      shifttable[i|0x000]=13;
      shifttable[i|0x100]=13;
    }
  }
}

uint32_t convertmantissa(unsigned int i){
  uint32_t m=i<<13;
  // Zero pad mantissa bits
  uint32_t e=0;
  // Zero exponent
  while(!(m&0x00800000)){
    // While not normalized
    e-=0x00800000;
    // Decrement exponent (1<<23)
    m<<=1;
    // Shift mantissa
  }
  m&=~0x00800000;
  // Clear leading 1 bit
  e+=
    0x38800000;
  // Adjust bias ((127-14)<<23)
  return m | e;
  // Return combined number
}

int main(void) {
  generatetables();
  
  for (int i=0; i<64; i++) {
    offsettable[i] = 1024;
  }
  offsettable[0] = offsettable[32] = 0;

  exponenttable[0] = 0;
  exponenttable[32]= 0x80000000;
  exponenttable[31]= 0x47800000;
  exponenttable[63]= 0xC7800000;

  for (int i=1; i<=30; i++) {
    exponenttable[i] = i<<23;
  }
  for (unsigned int i=33; i<=62; i++) {
    exponenttable[i] = 0x80000000 | ((i-32)<<23);
  }

  mantissatable[0] = 0;
  for (int i=1; i<=1023; i++) {
    mantissatable[i] = convertmantissa(i);
  }
  for (int i=1024; i<=2047; i++) {
    mantissatable[i] = 0x38000000 + ((i-1024)<<13);
  }

  printf(
     "#include \"short_float.h\"\n\n"

     "const uint16_t basetable[512] = {\n"
     );
  for (int ii=0; ii<512; ii++) {
    char comma= (ii==0) ? ' ' : ',';
    printf("  %c 0x%X\n", (int)(comma), (unsigned int)(basetable[ii]));
  }

  printf(
     "};\n\n"
     "const uint16_t shifttable[512] = {\n"
     );
  for (int ii=0; ii<512; ii++) {
    char comma= (ii==0) ? ' ' : ',';
    printf("  %c 0x%X\n", (int)(comma), (unsigned int)(shifttable[ii]));
  }

  printf(
     "};\n\n"
     "const uint32_t mantissatable[2048] = {\n"
     );
  for (int ii=0; ii<2048; ii++) {
    char comma= (ii==0) ? ' ' : ',';
    printf("  %c 0x%X\n", (int)(comma), (unsigned int)(mantissatable[ii]));
  }


  printf(
     "};\n\n"
     "const uint16_t offsettable[64] = {\n"
     );
  for (int ii=0; ii<64; ii++) {
    char comma= (ii==0) ? ' ' : ',';
    printf("  %c 0x%X\n", (int)(comma), (unsigned int)(offsettable[ii]));
  }

  printf(
     "};\n\n"
     "const uint32_t exponenttable[64] = {\n"
     );
  for (int ii=0; ii<64; ii++) {
    char comma= (ii==0) ? ' ' : ',';
    printf("  %c 0x%X\n", (int)(comma), (unsigned int)(exponenttable[ii]));
  }
  printf(
     "};\n\n"
     );

}
