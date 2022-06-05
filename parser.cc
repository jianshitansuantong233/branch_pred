#include <iostream>
#include <fstream>

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <math.h> 
using namespace std;

#include "utils.h"
#include "bt9_reader.h"
// #include "Pickle-in-Cpp/pickle.h"
#define SAVE_CSV 1
// #define SAVE_PKL 1
struct BinDataPoint {
    bool branchTaken;
    bool predDir;
    bool conditional;
    OpType opType;
    UINT64 branchTarget;
    UINT64 PC;
};

string getLSB(UINT64 PC, int digits){
    string res="";
    uint16_t lsb = PC >> 3;
    for(int i=0;i<digits;i++){
        res = to_string(1 & lsb) + "," + res;
        lsb = lsb >> 1;
    }
    return res;
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("usage: %s <trace>\n", argv[0]);
        exit(-1);
    }
    // TODO: batch process all traces
    std::string trace_path;
    trace_path = argv[1];

    bt9::BT9Reader bt9_reader(trace_path);

    std::string key = "total_instruction_count:";
    std::string value;
    bt9_reader.header.getFieldValueStr(key, value);
    // UINT64 total_instruction_counter = std::stoull(value, nullptr, 0);
    key = "branch_instruction_count:";
    bt9_reader.header.getFieldValueStr(key, value);
    // UINT64 branch_instruction_counter = std::stoull(value, nullptr, 0);
    // UINT64 numMispred = 0;

    // OpType opType; // didn't use, may be added?
    UINT64 PC;
    bool branchTaken;
    UINT64 branchTarget;
    // UINT64 nbr_instr_count;
    // UINT64 obs_trav_count;
    UINT64 numIter = 0;
    std::map<UINT64, vector<int>> history;

#ifdef SAVE_CSV
    std::ofstream csvFile;
    csvFile.open("/CBP-16-Simulation/cbp2016.eval/parsed_traces/"+trace_path + std::to_string(numIter) + ".csv");

    // csvFile << "PC,branchTaken,branchTarget,history1,history2,history3\n";
#endif

#ifdef SAVE_PKL
#endif
    for (auto it = bt9_reader.begin(); it!=bt9_reader.end(); ++it) {
        if(numIter%1024==0 && numIter!=0){
#ifdef SAVE_CSV

            csvFile.close();
            csvFile.open("/CBP-16-Simulation/cbp2016.eval/parsed_traces/"+trace_path + std::to_string(int(numIter/1024)) + ".csv");

            // csvFile << "PC,branchTaken,branchTarget,history1,history2,history3\n";
#endif
        }
        PC = it->getSrcNode()->brVirtualAddr();
        if(PC==0) continue;
        branchTarget = it->getEdge()->brVirtualTarget();
        branchTaken = it->getEdge()->isTakenPath();
        numIter++;
#ifdef SAVE_CSV
        csvFile << getLSB(PC, 16)
            << std::to_string(branchTaken) + ","
            << getLSB(branchTarget, 16);
#endif  
#ifdef SAVE_PKL
#endif
        if(history.find(PC) != history.end()){
            // Not first time to visit
            // maintain history vector
#ifdef SAVE_CSV
        csvFile << history[PC][0]<<","<<history[PC][1]<<","<<history[PC][2]<<"\n";
#endif 
#ifdef SAVE_PKL
#endif
            // too lazy to optimize
            if(history[PC][1] == 0){
                history[PC][1] = history[PC][0];
            }
            else if(history[PC][2] == 0){
                history[PC][2] = history[PC][1];
                history[PC][1] = history[PC][0];
            }
            else if(history[PC][2] != 0){
                history[PC][2] = history[PC][1];
                history[PC][1] = history[PC][0];
            }

            history[PC][0] = branchTaken?2:1;
        }
        else{
            // First time to see
#ifdef SAVE_CSV
        csvFile << "0,0,0\n";
#endif 
#ifdef SAVE_PKL
#endif
            vector<int> tmp(3, 0);
            tmp[0] = branchTaken?2:1;
            history.insert(std::make_pair(PC, tmp));
        }
        // nbr_instr_count = it->getEdge()->nonBrInstCnt();
        // obs_trav_count = it->getEdge()->observedTraverseCnt();
#ifdef SAVE_CSV
        // csvFile << "\n";
        // csvFile << std::to_string(nbr_instr_count) +","
            // << std::to_string(obs_trav_count)+"\n";
#endif
#ifdef SAVE_PKL
#endif 
    }

#ifdef SAVE_CSV
    csvFile.close();
#endif
#ifdef SAVE_PKL
#endif
    return 0;

}