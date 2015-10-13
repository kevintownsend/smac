#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "smac.hpp"

using namespace std;

int main(int argc, char* argv[]){
    SmacOptions options(argc, argv);
    cerr << "here" << endl;
    cerr << "compress: " << options.compress << endl;
    if(options.compress)
        return smacCompress(options);
    else
        return smacDecompress(options);
}
