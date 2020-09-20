#include <string.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include "gzstream.h"
#include <sstream>
#include <stdlib.h>
#include <cstdlib>

using namespace std;
//get the id hash table and
//iterate over the fastq file
// determine which sequence to print out

string fixfilenum(int filenum)
{
	string out;
	ostringstream convert;
	if (filenum >= 100)
	{
		convert << filenum;
		out = convert.str();
	}
	else if(filenum >= 10)
	{
		convert << filenum;
		out = "0" + convert.str();
	}
	else
	{
		convert << filenum;
		out = "00" + convert.str();
	}
	return out;
}

void splitFastq(char *fqFile, string filePrefix, int recordNum, string suffix)
{
	// open fastq file for kseq parsing
	cerr << "From " << fqFile << "...." << endl;
	cerr << "Splitting " << recordNum << " records per file" << endl;
	int maxLine = recordNum * 4;
	int lineCount = 0, filenum = 1;

	string filename;

	igzstream in(fqFile);
	ogzstream outFile;
	for (string line; getline(in,line);)
	{
		if (lineCount == 0)
		{
			filename = filePrefix + "." + fixfilenum(filenum) + suffix;
			cerr << "writing " << filename << " ..." << endl;
			outFile.open(filename.c_str());
			outFile << line << '\n';
		}
		else if (lineCount == maxLine)
		{
			outFile.close();
			lineCount = 0;
			filenum ++;
			filename = filePrefix + "." + fixfilenum(filenum) + suffix;
			cerr << "writing " << filename << " ..." << endl;
			outFile.open(filename.c_str());
			outFile << line << '\n';
		}
		else
		{
			outFile << line << '\n';
		}
		lineCount ++;
	}
	outFile.close();

	cerr << "done." << endl;
}

// print usage
void usage(string programname)
{
	cerr << "usage: "<< programname << " -i <fqfile> -n <# of record per file> -o <prefix> -s <suffix>" << endl;
	cerr << "[options]" << endl;
	cerr << "-i    <fastq file>"  << endl;
	cerr << "-n    <number of record in each splitted file> default: 10000000"  << endl;
	cerr << "-o    <prefix>"  << endl;
	cerr << "-s    <suffix>"  << endl;
}

// main function
int main(int argc, char **argv){
	char *fqFile;
	int c, recordNum = 10000000;

	string programname = argv[0];
	if (argc == 1){
		usage(programname);
		return 1;
	}
	string filePrefix = "";
	string suffix = "";

	opterr = 0;
	// print usage if not enough argumnets
	while ((c = getopt(argc, argv, "i:n:o:s:z")) != -1){
		switch (c){
			case 'i':
				fqFile = optarg;
				cerr << "i=" << fqFile << endl;
				break;
			case 'n':
				recordNum = atoi(optarg);
				cerr << "n=" << recordNum << endl;
				break;
			case 'o':
				filePrefix = optarg;
				cerr << "o=" << filePrefix << endl;
				break;
			case 's':
				suffix = optarg;
				cerr << "s=" << suffix << endl;
				break;
			case '?':
  			cerr << "???" << endl;
				if (optopt == 'n' || optopt == 'i' || optopt== 'o' || optopt == 's'){
					cerr << "option n, i, o, s need arguments!" << endl;
					usage(programname);
				}
				else {
					cerr << "error" << endl;
					usage(programname);
				}
				return 1;
			default:
  			cerr << "???" << endl;
				abort();
		}
  }
	if (filePrefix == "" || strcmp(fqFile,"") == 0 || suffix == "")
	{
		usage(programname);
		return 1;
	}

	splitFastq(fqFile, filePrefix, recordNum, suffix);
	return 0;
}
