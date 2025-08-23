#include "MoRSE.h"

void MoRSE_Update_client(DSSE &FAST_, const bool &op, const int &w, string &ind, const int &m, vector<tuple<string, string>> &u_tokens)   // Range D = {0, 1, 2, ... 2^m -1}
{
    string keyword;
    keyword = bitset<32>(w).to_string().substr(32-(m+1));           // this outputs a substring starting at position 32-(m+1) and going to the end.
    ind_Padding_FAST(ind);
    

    for (int i = 0; i <= m; i++)
    {
        tuple<string, string> u_token;                                                          // Update tokens for FAST     
        //cout << "The keyword " << keyword.substr(0, i+1)<< endl;                            //.substr(0, 0) in C++ returns an empty string
        FAST_.Update_client(ind, keyword.substr(0, i+1), op, u_token);     
        u_tokens.push_back(u_token);
    }
}

void MoRSE_Update_server(DSSE &FAST_, const vector<tuple<string, string>> &u_tokens)
{
    // string u, e; u = get<0>(utk), and e = get<1>(utk) 
    for(const auto & utk : u_tokens)
    {
        FAST_.Update_server(utk);
    }   
}