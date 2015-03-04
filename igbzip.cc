#include <cstdlib>
#include <iostream>

#include <zlib.h>
#include <bzlib.h>

#include <boost/filesystem.hpp>

#include "igbzip.opts.h"
#include "short_float.h"

using namespace std;
using namespace boost::filesystem;

bool compressFile(const path infilename, const path outfilename, const bool force, const float roundFactor) {
  bool didEverythingGoOk = false;
  BZFILE* bz_outfile;
  FILE* outfile;

  gzFile file;
  file = gzopen(infilename.string().c_str(), "r");
  if (file == NULL) {
    perror(("Can't open " + infilename.string() + " for reading").c_str());
    return false;
  }  
  if (!force && exists(outfilename)) {
    cerr << "File " << outfilename << " already exists, not overwriting.  Override with -f\n";
    goto clean_up_infile;
  }
  outfile = fopen(outfilename.string().c_str(), "w");
  if (outfile == NULL) {
    perror(("Can't open " + outfilename.string() + " for writing").c_str());
    goto clean_up_infile;
  }

  int bzerror;
  bz_outfile = BZ2_bzWriteOpen(
     &bzerror,
     outfile,
     9, //compression level, [0,9]
     0, //verbosity, [0,4]
     0  //workFactor, [1,250], 0 => 30 (default)
     );
  if (bzerror != BZ_OK) {
    cerr << "Error initializing BZ library for " << infilename.string() << ".\n";
    goto clean_up_outfile;
  }

  char header[1024];
  if (gzread(file, header, 1024) != 1024) {
    perror(("Error reading from " + infilename.string()).c_str());
    goto clean_up_bzfile;
  }
  if (fwrite(header, sizeof(char), 1024, outfile) != 1024) {
    perror(("Error writing to " + outfilename.string()).c_str());
    goto clean_up_bzfile;
  }
  
  float ff;
  do {
    int numRead = gzread(file, &ff, sizeof(ff));
    if (numRead == 0) {
      break;
    } else if (numRead == -1) {
      perror(("Error reading from " + infilename.string()).c_str());
      goto clean_up_bzfile;
    } else {
      if (isfinite(ff) && ff != 0) {
        // Make the float positive and strip the mantissa.
        union {
          float as_float;
          uint32_t as_uint32;
        } c;
        c.as_float = ff;
        c.as_uint32 &= 0x7F800000;
        float ffMag = c.as_float;

        float half_prec = ffMag * SHORT_FLOAT_MACHINE_EPS;
        float desired_prec=roundFactor;
        if (half_prec > desired_prec) {
          desired_prec = half_prec;
        }
        ff = roundf(ff/desired_prec)*desired_prec;
      }

      short_float half = shortFromFloat(ff);
      //convert -0 to 0
      if (half == 0x8000) {
        half = 0;
      }
      BZ2_bzWrite(&bzerror, bz_outfile, &half, sizeof(half));
      if (bzerror != BZ_OK) {
        perror(("Error writing to " + outfilename.string()).c_str());
        goto clean_up_bzfile;
      }
    }
  } while (1);

  didEverythingGoOk = true;

 clean_up_bzfile:
  { 
    unsigned int nbytes_in;
    unsigned int nbytes_out;
    BZ2_bzWriteClose(&bzerror, bz_outfile, 0, &nbytes_in, &nbytes_out);
  }
 clean_up_outfile:
  fclose(outfile);
  //delete the outfile if things messed up.
  if (!didEverythingGoOk) {
    remove(outfilename);
  }
 clean_up_infile:
  gzclose(file);
  if (didEverythingGoOk) {
    remove(infilename);
  }

  return didEverythingGoOk;
}

bool checkInputFile(const path infilename, const struct gengetopt_args_info& params, path& outfilename, float& roundFactor) {
  path thisfilename(infilename);
  if (thisfilename.extension() == ".gz") {
    thisfilename.replace_extension();
  }
  if (thisfilename.extension() == ".igb") {
    thisfilename.replace_extension("zgb");
    outfilename = thisfilename;

    string stem(thisfilename.stem().string());
    
    if (params.round_to_nearest_given) {
      roundFactor = params.round_to_nearest_arg;
    } else if (stem.length() >= 2 && 
               ((stem.substr(0,2) == "vm")
                ||
                (stem.substr(stem.length()-2,2) == "vm"))) {
      roundFactor = 0.1;
    } else {
      roundFactor = 0;
    }

    return true;
  }
  return false;
}

void processPath(const path infilename, const bool warnWrongType, const struct gengetopt_args_info& params) {
  if (params.recursive_flag && is_directory(infilename)) {
    for (directory_iterator diter(infilename); diter != directory_iterator(); diter++) {
      processPath(diter->path(), false, params);
    }
  } else {
    path outfilename;
    //if infilename doesn't match the pattern
    float roundFactor;
    if (!checkInputFile(infilename, params, outfilename, roundFactor)) {
      if (warnWrongType) {
        cerr << "Skipping non-IGB file " << infilename << "\n";
      }
      return;
    }
    compressFile(infilename, outfilename, params.force_flag, roundFactor);
  }
}

int main (int argc, char* argv[])
{
  struct gengetopt_args_info params;

  if ( cmdline_parser(argc, argv, &params) != 0 ) {
    return 1;
  }

  if (params.inputs_num == 0) {
    cmdline_parser_print_help();
    return 1;
  }

  for (int ii=0; ii<params.inputs_num; ii++) {
    processPath(params.inputs[ii], true, params);
  }

  return 0;
}
