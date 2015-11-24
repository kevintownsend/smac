
#ifndef SMAC_HPP
#define SMAC_HPP

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include "fzip.hpp"
#include "spm.hpp"
#include "constants.hpp"

#define SUB_WIDTH 8
#define SUB_HEIGHT 512
#define CONSTANT_DELTAS 32

using namespace std;

struct SmacOptions{
    bool compress;
    bool pattern;
    bool symmetric;
    int multipleFiles;
    int subheight;
    FILE* inputFile;
    FILE* outputFile;
    string outputFilename;
    SmacOptions(int argc, char* argv[]){
        pattern = false;
        symmetric = false;
        multipleFiles = -1;
        subheight = 512;
        cerr << "here" << endl;
        int nonOptionArgs = 0;
        for(int i = 1; i != argc; ++i){
            string arg(argv[i]);
            if(arg[0] == '-'){
                if(arg[1] == '-'){
                    if(arg.find("subheight") != string::npos)
                        subheight = atoi(arg.substr(arg.find('='), arg.size()).c_str());
                    if(arg.find("pattern") != string::npos)
                        pattern = true;
                    if(arg.find("multipleFiles") != string::npos)
                        multipleFiles = atoi(arg.substr(arg.find("=")+1,arg.size()).c_str());
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
                    outputFilename = arg;
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

bool smacCompress(vector<ull> &row, vector<ull> &col, vector<double> &val, vector<SpmCode> &spmCodes, vector<FzipCode> &fzipCodes, vector<ull> &commonDoubles, vector<ull> &spmCodeStream, vector<ull> &spmArgumentStream, vector<ull> &fzipCodeStream, vector<ull> &fzipArgumentStream, ull &spmCodeStreamBitLength, ull &spmArgumentStreamBitLength, ull &fzipCodeStreamBitLength, ull &fzipArgumentStreamBitLength, bool staticCommon = false);

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
    cerr << "i am here 3" << endl;
    //getline(&line, &size, options.inputFile);
    fgets(line, 1000, options.inputFile);
    cerr << "i am here 4" << endl;
    if(string(line).find("pattern") != string::npos)
        options.pattern = true;
    if(string(line).find("symmetric") != string::npos)
        options.symmetric = true;
    cerr << "i am here 5" << endl;
    ull width, height, nnz;
    fscanf(options.inputFile, "%lld%lld%lld", &height, &width, &nnz);
    cerr << "info: " << height << " " << width << " " << nnz << endl;
    cerr << "here" << endl;
    cerr << "line: " << string(line) << endl;
    vector<ull> row;
    vector<ull> col;
    vector<double> val;
    cerr << "mapping to matrix map" << endl;
    map<ull, map<ull ,map<ull, map<ull, double> > > > rcrMatrixMap;
    for(ull i = 0; i < nnz; i++){
        ull tmp1, tmp2;
        double tmp3;
        if(options.pattern){
            fscanf(options.inputFile, "%lld%lld", &tmp1, &tmp2);
            tmp3 = 1.0;
        }else{
            fscanf(options.inputFile, "%lld%lld%lf", &tmp1, &tmp2, &tmp3);
        }
        tmp1--; tmp2--;
        rcrMatrixMap[tmp1 / SUB_HEIGHT][tmp2 / SUB_WIDTH][tmp1 % SUB_HEIGHT][tmp2 % SUB_WIDTH] = tmp3;
        row.push_back(tmp1);
        col.push_back(tmp2);
        if(options.symmetric && tmp1 != tmp2){
            swap(tmp1, tmp2);
            rcrMatrixMap[tmp1 / SUB_HEIGHT][tmp2 / SUB_WIDTH][tmp1 % SUB_HEIGHT][tmp2 % SUB_WIDTH] = tmp3;
            row.push_back(tmp1);
            col.push_back(tmp2);
        }
    }
    cerr << "done mapping to matrix map" << endl;
    vector<bool> rowHasValues;
    rowHasValues.resize(height);
    //for(int i = 0; i < rowHasValues.size(); ++i)
    //    rowHasValues[i] = false;
    for(auto i1 = rcrMatrixMap.begin(); i1 != rcrMatrixMap.end(); ++i1){
        for(auto i2 = i1->second.begin(); i2 != i1->second.end(); ++i2){
            for(auto i3 = i2->second.begin(); i3 != i2->second.end(); ++i3){
                for(auto i4 = i3->second.begin(); i4 != i3->second.end(); ++i4){
                    rowHasValues[i1->first * SUB_HEIGHT + i3->first] = true;
                }
            }
        }
    }
    for(int i = 0; i < rowHasValues.size(); ++i){
        if(!rowHasValues[i]){
            row.push_back(i);
            col.push_back(0);
            rcrMatrixMap[i / SUB_HEIGHT][0][i % SUB_HEIGHT][0] = 0.0;
        }
    }
    vector<ull> newRow;
    vector<ull> newCol;
    for(auto i1 = rcrMatrixMap.begin(); i1 != rcrMatrixMap.end(); ++i1){
        for(auto i2 = i1->second.begin(); i2 != i1->second.end(); ++i2){
            for(auto i3 = i2->second.begin(); i3 != i2->second.end(); ++i3){
                for(auto i4 = i3->second.begin(); i4 != i3->second.end(); ++i4){
                    val.push_back(i4->second);
                    newRow.push_back(i1->first * SUB_HEIGHT + i3->first);
                    newCol.push_back(i2->first * SUB_WIDTH + i4->first);
                }
            }
        }
    }
    row = newRow;
    col = newCol;
    nnz = row.size();
    cerr << "calling smacCompress" << endl;
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

    //TODO: write to multiple files
    if(options.multipleFiles != -1){
        string prefix = options.outputFilename.substr(0,options.outputFilename.find("."));
        ull currentIndex = 0;
        for(int i = 0; i < options.multipleFiles; ++i){
            char buffer[100];
            snprintf(buffer, 100, "%d", i);
            FILE* subOutputFile = fopen((prefix + string(buffer) + ".smac").c_str(), "wb");
            //TODO: set row col and val
            //TODO: clear arguments
            vector<ull> subRow;
            vector<ull> subCol;
            vector<double> subVal;
            ull rowStart = row[currentIndex] / SUB_HEIGHT * SUB_HEIGHT;
            while(1){
                if(currentIndex >= nnz)
                    break;
                if(currentIndex != 0)
                    if(currentIndex > nnz / options.multipleFiles * (i + 1) - SUB_HEIGHT && row[currentIndex] / SUB_HEIGHT != row[currentIndex - 1] / SUB_HEIGHT)
                        break;
                subRow.push_back(row[currentIndex] - rowStart);
                subCol.push_back(col[currentIndex]);
                subVal.push_back(val[currentIndex]);
                currentIndex++;
            }
            cerr << "subSize: " << subRow.size() << endl;
            cerr << "before: " << endl;
            cerr << "fzipCodes.size: " << fzipCodes.size() << endl;
            cerr << "fzipCodeStreamBitLength: " << fzipCodeStreamBitLength << endl;
            smacCompress(subRow, subCol, subVal, spmCodes, fzipCodes, commonDoubles, spmCodeStream, spmArgumentStream, fzipCodeStream, fzipArgumentStream, spmCodeStreamBitLength, spmArgumentStreamBitLength, fzipCodeStreamBitLength, fzipArgumentStreamBitLength, true);
            cerr << "after: " << endl;
            cerr << "fzipCodes.size: " << fzipCodes.size() << endl;
            cerr << "fzipCodeStreamBitLength: " << fzipCodeStreamBitLength << endl;

            header.width = width;
            header.height = (row[currentIndex - 1] / SUB_HEIGHT + 1) * SUB_HEIGHT - rowStart;
            header.nnz = subRow.size();
            header.spmCodeStreamBitLength = spmCodeStreamBitLength;
            header.spmArgumentStreamBitLength = spmArgumentStreamBitLength;
            header.fzipCodeStreamBitLength = fzipCodeStreamBitLength;
            header.fzipArgumentStreamBitLength = fzipArgumentStreamBitLength;

            tmp = sizeof(SmacHeader);
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
            fwrite(&header, sizeof(SmacHeader), 1, subOutputFile);
            fwrite(&spmCodes[0], sizeof(SpmCode), spmCodes.size(), subOutputFile);
            fwrite(&fzipCodes[0], sizeof(FzipCode), fzipCodes.size(), subOutputFile);
            fwrite(&commonDoubles[0], sizeof(ull), commonDoubles.size(), subOutputFile);
            fwrite(&spmCodeStream[0], sizeof(ull), spmCodeStream.size(), subOutputFile);
            fwrite(&spmArgumentStream[0], sizeof(ull), spmArgumentStream.size(), subOutputFile);
            fwrite(&fzipCodeStream[0], sizeof(ull), fzipCodeStream.size(), subOutputFile);
            fwrite(&fzipArgumentStream[0], sizeof(ull), fzipArgumentStream.size(), subOutputFile);
            fclose(subOutputFile);
        }
    }
    return 0;
}

bool smacCompress(vector<ull> &row, vector<ull> &col, vector<double> &val, vector<SpmCode> &spmCodes, vector<FzipCode> &fzipCodes, vector<ull> &commonDoubles, vector<ull> &spmCodeStream, vector<ull> &spmArgumentStream, vector<ull> &fzipCodeStream, vector<ull> &fzipArgumentStream, ull &spmCodeStreamBitLength, ull &spmArgumentStreamBitLength, ull &fzipCodeStreamBitLength, ull &fzipArgumentStreamBitLength, bool staticCommon){
    cerr << "calling fzip compress" << endl;
    fzipCompress(val, commonDoubles, fzipCodes, fzipCodeStream, fzipArgumentStream, fzipCodeStreamBitLength, fzipArgumentStreamBitLength, staticCommon);
    cerr << "calling spm compress" << endl;
    spmCompress(row, col, spmCodes, spmCodeStream, spmArgumentStream, spmCodeStreamBitLength, spmArgumentStreamBitLength, SUB_HEIGHT, SUB_WIDTH, CONSTANT_DELTAS);

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
    fprintf(options.outputFile, "%lld %lld %lld\n", header.height, header.width, header.nnz);
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
    spmDecompress(row, col, spmCodes, spmCodeStream, spmArgumentStream, spmCodeStreamBitLength, spmArgumentStreamBitLength, SUB_HEIGHT, SUB_WIDTH);


}

#endif
