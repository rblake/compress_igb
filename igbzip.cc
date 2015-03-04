#include <zlib.h>
#include <bzlib.h>
#include <cstdlib>
#include "igbzip.opts.h"
#include "short_float.h"

int main (int argc, char* argv[])
{
  struct gengetopt_args_info params;

  if ( cmdline_parser(argc, argv, &params) != 0 )
    exit(1);

  gzFile file;
  file = gzopen(argv[1], "r");
  FILE* outfile = fopen("out.zgb", "w");

  int bzerror;
  BZFILE* bz_outfile = BZ2_bzWriteOpen(
     &bzerror,
     outfile,
     9, //compression level, [0,9]
     0, //verbosity, [0,4]
     0  //workFactor, [1,250], 0 => 30 (default)
     );

  char header[1024];
  gzread(file, header, 1024);
  fwrite(header, sizeof(char), 1024, outfile);

  
  float ff;
  while (gzread(file, &ff, sizeof(ff)) == sizeof(ff)) {
    short_float half = shortFromFloat(ff);
    BZ2_bzWrite(&bzerror, bz_outfile, &half, sizeof(half));
  }

  gzclose(file);
  
  unsigned int nbytes_in;
  unsigned int nbytes_out;
  BZ2_bzWriteClose(&bzerror, bz_outfile, 0, &nbytes_in, &nbytes_out);
  fclose(outfile);

  return 0;
}
