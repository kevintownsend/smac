//TODO: deal with pattern matrices
//TODO: deal with symmetric matrices
//TODO: deal with matrices with empty rows

#ifndef SMAC_HPP
#define SMAC_HPP

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "fzip.hpp"
#include "spm.hpp"

#define SUB_WIDTH 4
#define SUB_HEIGHT 4
#define CONSTANT_DELTAS 32

struct SmacOptions{
    bool compress;
    int subheight = 512;
    FILE* inputFile;
    FILE* outputFile;
    SmacOptions(int argc, char* argv[]){
        cerr << "here" << endl;
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
                    cerr << "opening input file" << endl;
                    inputFile = fopen(arg.c_str(), "r");
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
    ull width;
    ull height;
    ull nnz;
    ull spmCodeStreamBitLength;
    ull spmArgumentStreamBitLength;
    ull fzipCodeStreamBitLength;
    ull fzipArgumentStreamBitLength;
    ull r1[8];
    ull spmCodesPtr;
    ull fzipCodesPtr;
    ull commonDoublesPtr;
    ull spmCodeStreamPtr;
    ull spmArgumentStreamPtr;
    ull fzipCodeStreamPtr;
    ull fzipArgumentStreamPtr;
    ull size;
    ull r2[8];
};

bool smacCompress(vector<ull> &row, vector<ull> &col, vector<double> &val, vector<SpmCode> &spmCodes, vector<FzipCode> &fzipCodes, vector<ull> &commonDoubles, vector<ull> &spmCodeStream, vector<ull> &spmArgumentStream, vector<ull> &fzipCodeStream, vector<ull> &fzipArgumentStream, ull &spmCodeStreamBitLength, ull &spmArgumentStreamBitLength, ull &fzipCodeStreamBitLength, ull &fzipArgumentStreamBitLength);

int smacCompress(SmacOptions options){
    cerr << "i am here" << endl;
    SmacHeader header;
    //TODO: spmCodes
    vector<SpmCode> spmCodes;
    vector<FzipCode> fzipCodes;
    vector<ull> commonDoubles;
    vector<ull> spmCodeStream;
    vector<ull> spmArgumentStream;
    vector<ull> fzipCodeStream;
    vector<ull> fzipArgumentStream;
    ull spmCodeStreamBitLength;
    ull spmArgumentStreamBitLength;
    ull fzipCodeStreamBitLength;
    ull fzipArgumentStreamBitLength;
    cerr << "i am here" << endl;
    char line[1000];
    size_t size;
    cerr << "i am here" << endl;
    //getline(&line, &size, options.inputFile);
    fgets(line, 1000, options.inputFile);
    ull width, height, nnz;
    fscanf(options.inputFile, "%lld%lld%lld", &width, &height, &nnz);
    cerr << "info: " << width << " " << height << " " << nnz << endl;
    cerr << "here" << endl;
    cerr << "line: " << string(line) << endl;
    vector<ull> row;
    vector<ull> col;
    vector<double> val;

    map<ull, map<ull ,map<ull, map<ull, double> > > > rcrMatrixMap;
    for(ull i = 0; i < nnz; i++){
        ull tmp1, tmp2;
        double tmp3;
        fscanf(options.inputFile, "%lld%lld%lf", &tmp1, &tmp2, &tmp3);
        tmp1--; tmp2--;
        rcrMatrixMap[tmp1 / SUB_HEIGHT][tmp2 / SUB_WIDTH][tmp1 % SUB_HEIGHT][tmp2 % SUB_WIDTH] = tmp3;
        row.push_back(tmp1);
        col.push_back(tmp2);
    }
    for(auto i1 = rcrMatrixMap.begin(); i1 != rcrMatrixMap.end(); ++i1){
        for(auto i2 = i1->second.begin(); i2 != i1->second.end(); ++i2){
            for(auto i3 = i2->second.begin(); i3 != i2->second.end(); ++i3){
                for(auto i4 = i3->second.begin(); i4 != i3->second.end(); ++i4){
                    val.push_back(i4->second);
                }
            }
        }
    }

    smacCompress(row, col, val, spmCodes, fzipCodes, commonDoubles, spmCodeStream, spmArgumentStream, fzipCodeStream, fzipArgumentStream, spmCodeStreamBitLength, spmArgumentStreamBitLength, fzipCodeStreamBitLength, fzipArgumentStreamBitLength);

    header.width = width;
    header.height = height;
    header.nnz = nnz;
    header.spmCodeStreamBitLength = spmCodeStreamBitLength;
    header.spmArgumentStreamBitLength = spmArgumentStreamBitLength;
    header.fzipCodeStreamBitLength = fzipCodeStreamBitLength;
    header.fzipArgumentStreamBitLength = fzipArgumentStreamBitLength;

    ull tmp = sizeof(SmacHeader);
    header.spmCodesPtr = tmp;
    //TODO: increase tmp
    tmp += spmCodes.size() * sizeof(SpmCode);
    header.fzipCodesPtr = tmp;
    tmp += fzipCodes.size() * sizeof(FzipCode);
    header.commonDoublesPtr = tmp;
    tmp += commonDoubles.size() * sizeof(ull);
    header.spmCodeStreamPtr = tmp;
    tmp += spmCodeStream.size() * sizeof(ull);
    header.spmArgumentStreamPtr = tmp;
    tmp += spmArgumentStream.size() * sizeof(ull);
    header.fzipCodeStreamPtr = tmp;
    tmp += fzipCodeStream.size() * sizeof(ull);
    header.fzipArgumentStreamPtr = tmp;
    tmp += fzipArgumentStream.size() * sizeof(ull);
    header.size = tmp;
    fwrite(&header, sizeof(SmacHeader), 1, options.outputFile);
    fwrite(&spmCodes[0], sizeof(SpmCode), spmCodes.size(), options.outputFile);
    fwrite(&fzipCodes[0], sizeof(FzipCode), fzipCodes.size(), options.outputFile);
    fwrite(&commonDoubles[0], sizeof(ull), commonDoubles.size(), options.outputFile);
    fwrite(&spmCodeStream[0], sizeof(ull), spmCodeStream.size(), options.outputFile);
    fwrite(&spmArgumentStream[0], sizeof(ull), spmArgumentStream.size(), options.outputFile);
    fwrite(&fzipCodeStream[0], sizeof(ull), fzipCodeStream.size(), options.outputFile);
    fwrite(&fzipArgumentStream[0], sizeof(ull), fzipArgumentStream.size(), options.outputFile);
    return 0;
}

bool smacCompress(vector<ull> &row, vector<ull> &col, vector<double> &val, vector<SpmCode> &spmCodes, vector<FzipCode> &fzipCodes, vector<ull> &commonDoubles, vector<ull> &spmCodeStream, vector<ull> &spmArgumentStream, vector<ull> &fzipCodeStream, vector<ull> &fzipArgumentStream, ull &spmCodeStreamBitLength, ull &spmArgumentStreamBitLength, ull &fzipCodeStreamBitLength, ull &fzipArgumentStreamBitLength){
    fzipCompress(val, commonDoubles, fzipCodes, fzipCodeStream, fzipArgumentStream, fzipCodeStreamBitLength, fzipArgumentStreamBitLength);
    spmCompress(row, col, spmCodes, spmCodeStream, spmArgumentStream, spmCodeStreamBitLength, spmArgumentStreamBitLength, SUB_WIDTH, SUB_HEIGHT, CONSTANT_DELTAS);

}
bool smacDecompress(vector<ull> &row, vector<ull> &col, vector<double> &val, vector<SpmCode> &spmCodes, vector<FzipCode> &fzipCodes, vector<ull> &commonDoubles, vector<ull> &spmCodeStream, vector<ull> &spmArgumentStream, vector<ull> &fzipCodeStream, vector<ull> &fzipArgumentStream, ull &spmCodeStreamBitLength, ull &spmArgumentStreamBitLength, ull &fzipCodeStreamBitLength, ull &fzipArgumentStreamBitLength);

int smacDecompress(SmacOptions options){
    cerr << "decompressing" << endl;
    vector<ull> row;
    vector<ull> col;
    vector<double> val;
    vector<SpmCode> spmCodes;
    vector<FzipCode> fzipCodes;
    vector<ull> commonDoubles;
    vector<ull> spmCodeStream;
    vector<ull> spmArgumentStream;
    vector<ull> fzipCodeStream;
    vector<ull> fzipArgumentStream;
    ull spmCodeStreamBitLength, spmArgumentStreamBitLength, fzipCodeStreamBitLength, fzipArgumentStreamBitLength;
    SmacHeader header;
    fread(&header, 1, sizeof(SmacHeader), options.inputFile);
    cerr << "fzipCodesPtr: " << header.fzipCodesPtr << endl;
    cerr << "commonDoublesPtr: " << header.commonDoublesPtr << endl;
    for(ull i = 0; i < ((header.fzipCodesPtr - header.spmCodesPtr) / sizeof(SpmCode)); ++i){
        SpmCode tmp;
        fread(&tmp, sizeof(SpmCode), 1, options.inputFile);
        spmCodes.push_back(tmp);
    }
    for(ull i = 0; i < ((header.commonDoublesPtr - header.fzipCodesPtr) / sizeof(FzipCode)); ++i){
        FzipCode tmp;
        fread(&tmp, sizeof(FzipCode), 1, options.inputFile);
        fzipCodes.push_back(tmp);
    }
    for(ull i = 0; i < ((header.spmCodeStreamPtr - header.commonDoublesPtr) / sizeof(ull)); ++i){
        ull tmp;
        fread(&tmp, sizeof(ull), 1, options.inputFile);
        commonDoubles.push_back(tmp);
    }
    for(ull i = 0; i < ((header.spmArgumentStreamPtr - header.spmCodeStreamPtr) / sizeof(ull)); ++i){
        ull tmp;
        fread(&tmp, sizeof(ull), 1, options.inputFile);
        spmCodeStream.push_back(tmp);
    }
    for(ull i = 0; i < ((header.fzipCodeStreamPtr - header.spmArgumentStreamPtr) / sizeof(ull)); ++i){
        ull tmp;
        fread(&tmp, sizeof(ull), 1, options.inputFile);
        spmArgumentStream.push_back(tmp);
    }
    for(ull i = 0; i < ((header.fzipArgumentStreamPtr - header.fzipCodeStreamPtr) / sizeof(ull)); ++i){
        ull tmp;
        fread(&tmp, sizeof(ull), 1, options.inputFile);
        fzipCodeStream.push_back(tmp);
    }
    for(ull i = 0; i < ((header.size - header.fzipArgumentStreamPtr) / sizeof(ull)); ++i){
        ull tmp;
        fread(&tmp, sizeof(ull), 1, options.inputFile);
        fzipArgumentStream.push_back(tmp);
    }
    //TODO: decode
    smacDecompress(row, col, val, spmCodes, fzipCodes, commonDoubles, spmCodeStream, spmArgumentStream, fzipCodeStream, fzipArgumentStream, header.spmCodeStreamBitLength, header.spmArgumentStreamBitLength, header.fzipCodeStreamBitLength, header.fzipArgumentStreamBitLength);

    //TODO: write
    fprintf(options.outputFile, "\%\%MatrixMarket matrix coordinate real general\n");
    fprintf(options.outputFile, "%lld %lld %lld\n", header.width, header.height, header.nnz);
    if(header.nnz != val.size()){
        cerr << "ERROR: val.size: " << val.size() << endl;
        return 1;
    }
    if(header.nnz != row.size()){
        cerr << "WARNING: row.size: " << row.size() << endl;
    }
    for(ull i = 0; i < header.nnz; ++i){
        fprintf(options.outputFile, "%lld %lld %lf\n", row[i] + 1, col[i] + 1, val[i]);
    }
    return 0;
}

bool smacDecompress(vector<ull> &row, vector<ull> &col, vector<double> &val, vector<SpmCode> &spmCodes, vector<FzipCode> &fzipCodes, vector<ull> &commonDoubles, vector<ull> &spmCodeStream, vector<ull> &spmArgumentStream, vector<ull> &fzipCodeStream, vector<ull> &fzipArgumentStream, ull &spmCodeStreamBitLength, ull &spmArgumentStreamBitLength, ull &fzipCodeStreamBitLength, ull &fzipArgumentStreamBitLength){
    fzipDecompress(val, commonDoubles, fzipCodes, fzipCodeStream, fzipArgumentStream, fzipCodeStreamBitLength, fzipArgumentStreamBitLength);
    spmDecompress(row, col, spmCodes, spmCodeStream, spmArgumentStream, spmCodeStreamBitLength, spmArgumentStreamBitLength, SUB_WIDTH, SUB_HEIGHT);


}

#endif
