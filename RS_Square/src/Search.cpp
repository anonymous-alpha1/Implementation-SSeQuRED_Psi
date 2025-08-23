#include "RS_Square.h"

void DSSE::Search_client(const int& m, tuple<int, int>& range_query, vector<tuple<string, string, int>> &t_q)
{
    vector<string> CSet = GetBRCm(range_query, m);
    t_q.clear();

    int decision;

    tuple<string, int, int> map1_W_value;
    get<1>(map1_W_value) = 0;
    get<2>(map1_W_value) = 0;

    string k_w;
    string nym_w;
    int c = 0;
    

    for (const auto &tau : CSet)
    {
        decision = Retrive_tupple_DB(this->Data.map1_W, tau, map1_W_value);                                                     // Retrieveing the value stored (st, c) corresponding to the "keyword" in the database map1 if exists

        if (decision == 0)
        {
            get<1>(map1_W_value) = histUptCnt(this->Data.map1_W, m, tau);                                // c_h

            if (get<1>(map1_W_value) == 0)                                                               // c_h == 0 
            {
                continue;
            }
            encryptAES(this->Data.client_sk1, tau, get<0>(map1_W_value));
            get<2>(map1_W_value) = get<1>(map1_W_value);

        }

        if (get<2>(map1_W_value) != 0)
        {
            k_w = get<0>(map1_W_value);
            get<0>(map1_W_value) = generate128BitString();
            c = get<2>(map1_W_value);
        }
        else
        {
            k_w = "";
            c = 0;
        }
        encryptAES(this->Data.client_sk2, tau, nym_w);

        Store_tupple_DB(this->Data.map1_W, tau, make_tuple(get<0>(map1_W_value), get<1>(map1_W_value), 0));
        t_q.emplace_back(nym_w, k_w, c);
    }
    // Send t_q to server
}

void DSSE::Search_server(vector<tuple<string, string, int>>& t_q, vector<string>& R)
{
    vector<string> RSet;
    string addr, ind_op;
    string T_e_value;
    int decision = 0;

    R.clear();
    
    for (const auto& stk : t_q)
    {
        RSet.clear();
        
        decision = Retrive_tupple_T_c_DB(this->Data.map3_T_c, get<0>(stk), RSet);
        if (decision == 0)
        {
            //cout << "T_c: First time retrival: "<< endl;
        }

        if (!get<1>(stk).empty())
        {
            for (int i = 1; i <= get<2>(stk); i++)
            {
                addr = sha256(get<1>(stk) + to_string(i));

                rocksdb::Status status = this->Data.map2_T_e->Get(rocksdb::ReadOptions(), addr, &T_e_value);
                if (!status.ok())
                {
                    cout << "T_e: Keyword: " << addr << " & Value: "<< status.ToString() << endl;
                }

                ind_op = xorStrings(T_e_value, hashSHAKE(get<1>(stk) + to_string(i), 16));

                if (ind_op.substr(15, 1) == "0")
                {
                    RSet.erase(find(RSet.begin(), RSet.end(), ind_op.substr(0, ind_op.size() - 1)));
                }
                else if (ind_op.substr(15, 1) == "1")
                {
                    RSet.push_back(ind_op.substr(0, ind_op.size() - 1));
                }
                rocksdb::Status status_ = this->Data.map2_T_e->Delete(rocksdb::WriteOptions(), addr);
                if (!status_.ok())
                {
                    std::cerr << "Error deleting key" << addr << ":" << status_.ToString() << std::endl;
                    delete this->Data.map2_T_e;
                }
            }
        }
        Store_tupple_T_c_DB(this->Data.map3_T_c, get<0>(stk), RSet);

        R.insert(R.end(), RSet.begin(), RSet.end());
    }
}