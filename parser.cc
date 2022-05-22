#include <iostream>
#include <fstream>

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <map>
using namespace std;

#include "utils.h"
#include "bt9_reader.h"

#define SAVE_CSV 1

struct BinDataPoint {
    bool branchTaken;
    bool predDir;
    bool conditional;
    OpType opType;
    UINT64 branchTarget;
    UINT64 PC;
};

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("usage: %s <trace>\n", argv[0]);
        exit(-1);
    }
    // TODO: batch process all traces
    std::string trace_path;
    trace_path = argv[1];

#ifdef SAVE_CSV
    // std::ofstream csvFile;
    // csvFile.open(trace_path + ".csv");

    // csvFile << "PC,branchTaken,history1,history2,history3, predDir,opType,branchTarget\n";
#endif

    bt9::BT9Reader bt9_reader(trace_path);

    std::string key = "total_instruction_count:";
    std::string value;
    bt9_reader.header.getFieldValueStr(key, value);
    UINT64 total_instruction_counter = std::stoull(value, nullptr, 0);
    key = "branch_instruction_count:";
    bt9_reader.header.getFieldValueStr(key, value);
    UINT64 branch_instruction_counter = std::stoull(value, nullptr, 0);
    UINT64 numMispred = 0;

    UINT64 cond_branch_instruction_counter = 0;
    UINT64 uncond_branch_instruction_counter = 0;
    OpType opType;
    UINT64 PC;
    bool branchTaken;
    UINT64 branchTarget;
    UINT64 numIter = 0;

    for (auto it = bt9_reader.begin(); it != bt9_reader.end(); ++it) {
        bt9::BrClass br_class = it->getSrcNode()->brClass();
        std::cout << br_class.directness << endl;
    }

#ifdef SAVE_CSV
    // csvFile.close();
#endif
    return 0;

}