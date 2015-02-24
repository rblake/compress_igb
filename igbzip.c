#include <zlib.h>
#include "short_float.h"

int main (int argc, char* argv[])
{
  gzFile file;
  file = gzopen(argv[1], "r");
  gzFile outfile;
  outfile = gzopen("out.blah", "w");

  char header[1024];
  gzread(file, header, 1024);
  gzwrite(outfile, header, 1024);
  
  float ff;
  while (gzread(file, &ff, sizeof(ff)) == sizeof(ff)) {
    short_float half = shortFromFloat(ff);
    gzwrite(outfile, &half, sizeof(half));
  }
  

  gzclose(file);
  gzclose(outfile);


  return 0;
}
