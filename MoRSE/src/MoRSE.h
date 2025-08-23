#ifndef MoRSE
#define MoRSE

#include <bitset>

#include "../../FAST/FAST.h"

void ind_Padding_FAST(string &ind);

vector<string> GetBRCm(const tuple<int, int> &range_query, const int &height);

void MoRSE_Setup(DSSE &FAST_);

void MoRSE_Update_client(DSSE &FAST_, const bool &op, const int &w, string &ind, const int &m, vector<tuple<string, string>> &u_tokens);                   // Range D = {0, 1, 2, ... 2^m -1}
void MoRSE_Update_server(DSSE &FAST_, const vector<tuple<string, string>> &u_tokens);

void MoRSE_Search_client(DSSE &FAST_, const tuple<int, int> &range_query, const int &m, vector<tuple<string, string, int>> &s_tokens);
void MoRSE_Search_server(DSSE &FAST_, const vector<tuple<string, string, int>> &s_tokens, vector<string> &search_result);

#endif