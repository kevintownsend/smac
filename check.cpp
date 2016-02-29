#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>

typedef unsigned long long ull;

char line[1001];

using namespace std;

int main(int argc, char* argv[]){
    if(argc != 3){
        cerr << "usage: " << argv[0] << " first.mtx second.mtx" << endl;
        exit(1);
    }
    FILE* firstFile = fopen(argv[1], "r");
    fgets(line, 1000, firstFile);
    //TODO: pattern matrices
    bool pattern = false;
    int M, N, nnz;
    fscanf(firstFile, "%d%d%d", &M, &N, &nnz);
    vector<ull> row;
    vector<ull> col;
    vector<double> val;
    for(int i = 0; i < nnz; ++i){
        ull tmp1, tmp2;
        double tmp3;
        if(pattern){}
        else
            fscanf(firstFile, "%lld%lld%lf", &tmp1, &tmp2, &tmp3);
        row.push_back(tmp1-1);
        col.push_back(tmp2-1);
        val.push_back(tmp3);
    }
    fclose(firstFile);
    FILE* secondFile = fopen(argv[2], "r");
    fgets(line, 1000, secondFile);
    fscanf(secondFile, "%d%d%d", &M, &N, &nnz);
    map<ull, map<ull, double> > secondMatrixMap;
    for(int i = 0; i < nnz; ++i){
        ull tmp1, tmp2;
        double tmp3;
        fscanf(secondFile, "%lld%lld%lf", &tmp1, &tmp2, &tmp3);
        tmp1--; tmp2--;
        secondMatrixMap[tmp1][tmp2] = tmp3;
    }
    fclose(secondFile);
    int mismatches = 0;
    for(int i = 0; i < row.size(); ++i){
        if(secondMatrixMap[row[i]].count(col[i])){
            if(secondMatrixMap[row[i]][col[i]] != val[i]){
                cerr << "values not equal at [" << row[i] << "][" << col[i] << "] first: " << val[i] << " second: " << secondMatrixMap[row[i]][col[i]] << endl;
                mismatches++;
            }
        }else{
            cerr << "No value in second matrix at [" << row[i] << "][" << col[i] << "] value: " << val[i] << endl;
            mismatches++;
        }
    }
    if(!mismatches)
        cerr << "no discrepencies found\n";
}
