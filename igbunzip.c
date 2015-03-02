#include <zlib.h>
#include <stdlib.h>
#include <stdio.h>
#include "igbunzip.opts.h"
#include "short_float.h"

int main (int argc, char* argv[])
{
  struct gengetopt_args_info params;

  if ( cmdline_parser(argc, argv, &params) != 0 )
    exit(1);

  gzFile file;
  file = gzopen(argv[1], "r");
  FILE* outfile;
  outfile = fopen("out.blah.igb", "w");

  char header[1024];
  gzread(file, header, 1024);
  fwrite(header, sizeof(char), 1024, outfile);
  
  short_float half;
  while (gzread(file, &half, sizeof(half)) == sizeof(half)) {
    float ff = floatFromShort(half);
    fwrite(&ff, sizeof(ff), 1, outfile);
  }
  

  gzclose(file);
  fclose(outfile);


  return 0;
}
