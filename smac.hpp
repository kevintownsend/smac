#ifndef SMAC_HPP
#define SMAC_HPP

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "fzip.hpp"

struct SmacOptions{
    bool compress;
    int subheight = 512;
    FILE* inputFile;
    FILE* outputFile;
    SmacOptions(int argc, char* argv[]){
        int nonOptionArgs = 0;
        for(int i = 1; i != argc; ++i){
            string arg(argv[i]);
            if(arg[0] == '-'){
                if(arg[1] == '-'){
                    if(arg.find("subheight") != string::npos)
                        subheight = atoi(arg.substr(arg.find('='), arg.size()).c_str());
                }else{
                    if(arg.find('c') != string::npos)
                        compress = true;
                    if(arg.find('d') != string::npos)
                        compress = false;
                }
            }else{
                if(nonOptionArgs == 0){
                    inputFile = fopen(arg.c_str(), "rb");
                }else if(nonOptionArgs == 1){
                    outputFile = fopen(arg.c_str(), "wb");
                }else{
                    cerr << "usage: wrong" << endl;
                    exit(1);
                }
                nonOptionArgs++;
            }
        }
        if(nonOptionArgs < 2)
            outputFile = stdout;
        if(nonOptionArgs < 1)
            inputFile = stdin;
    }
};
typedef unsigned long long ull;

struct SmacHeader{
    ull r0;
    ull r1[15];
    ull spmCodesPtr;
    ull fzipCodesPtr;
    ull commonDoublesPtr;
    ull spmCodeStreamPtr;
    ull spmArgumentStreamPtr;
    ull fzipCodesStreamPtr;
    ull fzipArgumentStreamPtr;
    ull r2[9];
};

int smacCompress(SmacOptions options){
    SmacHeader header;
    //TODO: spmCodes
    vector<FzipCode> fzipCodes;
    vector<ull> commonDoubles;
    vector<ull> spmCodeStream;
    vector<ull> spmArgumentStream;
    vector<ull> fzipCodeStream;
    vector<ull> fzipArgumentStream;

    //smacCompress(spmCodes, fzipCodes, commonDoubles, spmCodeSteram, spmArgumentStream, fzipCodeStream, fzipArgumentStream);
    ull tmp = sizeof(SmacHeader);
    header.spmCodesPtr = tmp;
    //TODO: increase tmp
    header.fzipCodesPtr = tmp;
    tmp += fzipCodes.size() * sizeof(FzipCode);
    header.commonDoublesPtr = tmp;
    tmp += commonDoubles.size() * sizeof(ull);
    header.spmCodeStreamPtr = tmp;
    tmp += spmCodeStream.size() * sizeof(ull);
    header.spmArgumentStreamPtr = tmp;
    return 0;
}

int smacDecompress(SmacOptions options){
    return 0;
}

#endif
