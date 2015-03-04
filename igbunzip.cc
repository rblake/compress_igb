#include <bzlib.h>
#include <cstdlib>
#include <cstdio>
#include "igbunzip.opts.h"
#include "short_float.h"

int main (int argc, char* argv[])
{
  struct gengetopt_args_info params;

  if ( cmdline_parser(argc, argv, &params) != 0 )
    exit(1);

  FILE* file = fopen(argv[1], "r");
  int bzerror;
  BZFILE *bz_file = BZ2_bzReadOpen( 
     &bzerror, 
     file, 
     0, //verbosity, [0,4]
     0, // small, boolean
     NULL, 0);
  FILE* outfile;
  outfile = fopen("out.blah.igb", "w");

  char header[1024];
  fread(header, sizeof(char), 1024, file);
  fwrite(header, sizeof(char), 1024, outfile);
  
  short_float half;
  while (BZ2_bzRead(&bzerror, bz_file, &half, sizeof(half)) == sizeof(half)) {
    float ff = floatFromShort(half);
    fwrite(&ff, sizeof(ff), 1, outfile);
  }

  BZ2_bzReadClose(&bzerror, bz_file);
  fclose(file);
  fclose(outfile);


  return 0;
}
