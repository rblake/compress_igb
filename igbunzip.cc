#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>

#include <bzlib.h>

#include <boost/filesystem.hpp>

#include "igbunzip.opts.h"
#include "short_float.h"

using namespace std;
using namespace boost::filesystem;

bool uncompressFile(const path infilename, const path outfilename) {
  bool didEverythingGoOk = false;
  int numWritten, numRead;

  FILE* file = fopen(infilename.c_str(), "r");
  if (file == NULL) {
    perror(("Can't open " + infilename.string() + " for reading").c_str());
    return false;
  }
  int bzerror;
  BZFILE *bz_file = BZ2_bzReadOpen( 
     &bzerror, 
     file, 
     0, //verbosity, [0,4]
     0, // small, boolean
     NULL, 0);
  if (bzerror != BZ_OK) {
    cerr << "Error initializing BZ library for " << infilename.string() << ".\n";
    goto clean_up_infile;
  }

  FILE* outfile;
  outfile = fopen(outfilename.c_str(), "w");
  if (outfile == NULL) {
    perror(("Can't open " + outfilename.string() + " for writing").c_str());
    goto clean_up_bzfile;
  }

  char header[1024];
  numRead = fread(header, sizeof(char), 1024, file);
  if (numRead != 1024) {
    perror(("Error reading from " + infilename.string()).c_str());
    goto clean_up_outfile;
  }
  numWritten = fwrite(header, sizeof(char), 1024, outfile);
  if (numWritten != 1024) {
    perror(("Error writing to " + outfilename.string()).c_str());
    goto clean_up_outfile;
  }

  short_float half;
  do {
    numRead = BZ2_bzRead(&bzerror, bz_file, &half, sizeof(half));
    if (bzerror == BZ_OK || bzerror == BZ_STREAM_END) {
      if (numRead == sizeof(half)) {
        float ff = floatFromShort(half);
        if (fwrite(&ff, sizeof(ff), 1, outfile) != 1) {
          perror(("Error writing to " + outfilename.string()).c_str());
          goto clean_up_outfile;
        }
      }
    } else {
      cerr << "Error reading from " << infilename << "\n";
      goto clean_up_outfile;
    }
    if (bzerror == BZ_STREAM_END) {
      break;
    } 
  } while(1);

  didEverythingGoOk = true;

 clean_up_outfile:
  fclose(outfile);
  //delete the outfile.
 clean_up_bzfile:
  BZ2_bzReadClose(&bzerror, bz_file);
 clean_up_infile:
  fclose(file);

  return didEverythingGoOk;

}

bool checkInputFile(const path infilename, path& outfilename) {
  if (infilename.extension() == ".zgb") {
    outfilename = infilename;
    outfilename.replace_extension("igb");
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
    if (!checkInputFile(infilename, outfilename)) {
      if (warnWrongType) {
        cerr << "Skipping non-ZGB file " << infilename << "\n";
      }
      return;
    }
    if (!params.force_flag && exists(outfilename)) {
      cerr << "File " << outfilename << " already exists, not overwriting.  Override with -f\n";
      return;
    }
    if (uncompressFile(infilename, outfilename)) {
      if (!params.keep_flag) {
        remove(infilename);
      }
    } else {
      remove(outfilename);
    }
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
