#include "MoRSE.h"

void MoRSE_Search_client(DSSE &FAST_, const tuple<int, int> &range_query, const int &m, vector<tuple<string, string, int>> &s_tokens)
{
    vector <string> RSet;
    vector <string> CSet =  GetBRCm(range_query, m);

    for (const auto & w : CSet)
    {
        //Search token of FAST search_client algorithms
        tuple<string, string, int> s_token; 

        FAST_.Search_client(w, s_token);
        s_tokens.push_back(s_token);                   // Storing all search tokens of FAST.Search()
    }
}

void MoRSE_Search_server(DSSE &FAST_, const vector<tuple<string, string, int>> &s_tokens, vector<string> &search_result)
{

    // variables for storing search tokens of FAST search algorithms

    for (const auto& stk : s_tokens)
    {
        vector<string> s_res;
        FAST_.Search_server(stk, s_res);
        search_result.insert(search_result.end(), s_res.begin(), s_res.end());
    }
}