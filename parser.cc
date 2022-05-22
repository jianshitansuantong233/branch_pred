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
    std::ofstream csvFile;
    csvFile.open(trace_path + ".csv");

    csvFile << "PC,branchTaken,branchTarget,history1,history2,history3,nonBranchInstrCount,observedTraversCount\n";
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

    OpType opType; // didn't use, may be added?
    UINT64 PC;
    bool branchTaken;
    UINT64 branchTarget;
    UINT64 nbr_instr_count;
    UINT64 obs_trav_count;
    UINT64 numIter = 0;
    std::map<UINT64, vector<int>> history;

    for (auto it = bt9_reader.begin(); it != bt9_reader.end(); ++it) {
        PC = it->getSrcNode()->brVirtualAddr();
        branchTarget = it->getEdge()->brVirtualTarget();
        branchTaken = it->getEdge()->isTakenPath();

#ifdef SAVE_CSV
        csvFile << std::to_string(PC) + ","
            << std::to_string(branchTaken) + ","
            << std::to_string(branchTarget) + ",";
#endif  

        if(history.find(PC) != history.end()){
            // Not first time to visit
            // maintain history vector
#ifdef SAVE_CSV
        csvFile << history[PC][0]<<","<<history[PC][1]<<","<<history[PC][2]<<",";
#endif 
            // too lazy to optimize
            if(history[PC][1] == -1){
                history[PC][1] = history[PC][0];
            }
            else if(history[PC][2] == -1){
                history[PC][2] = history[PC][1];
                history[PC][1] = history[PC][0];
            }
            else if(history[PC][2] != -1){
                history[PC][2] = history[PC][1];
                history[PC][1] = history[PC][0];
            }

            history[PC][0] = branchTaken?1:0;
        }
        else{
            // First time to see
#ifdef SAVE_CSV
        csvFile << "-1,-1,-1,";
#endif 
            vector<int> tmp(3, -1);
            tmp[0] = branchTaken?1:0;
            history.insert(std::make_pair(PC, tmp));
        }
        nbr_instr_count = it->getEdge()->nonBrInstCnt();
        obs_trav_count = it->getEdge()->observedTraverseCnt();
#ifdef SAVE_CSV
        csvFile << std::to_string(nbr_instr_count) +","
            << std::to_string(obs_trav_count)+"\n";
#endif 
    }

#ifdef SAVE_CSV
    csvFile.close();
#endif
    return 0;

}