#include "RS_Square.h"


void DSSE::Update_client(const int& m, const bool& op, const string& ind, const int& keyword, vector<tuple<string, string>>& t_u)
{
    vector<tuple<string, int>> SSet;
    t_u.clear();
    int cnt = 0;

    constexpr int max_bits = 32; // ensure this is large enough

    bitset<max_bits> bin_w(keyword);
    string binary_w = bin_w.to_string().substr(max_bits - (m + 1), m + 1);

    tuple<string, int, int> map1_W_value;
    int decision;
    decision = Retrive_tupple_DB(this->Data.map1_W, binary_w, map1_W_value);                                                    // Retrieveing the value stored (st, c) corresponding to the "keyword" in the database map1 if exists

    if (decision == 0)
    {
        encryptAES(this->Data.client_sk1, binary_w, get<0>(map1_W_value));                                                      // creating k_tau
        get<1>(map1_W_value) = 0;                                                                                               // c_h
        get<2>(map1_W_value) = 0;                                                                                               // c_u
    }

    SSet.emplace_back(get<0>(map1_W_value), get<2>(map1_W_value));                                                              // storing (k_tau, c_u)
    cnt = get<1>(map1_W_value);                                                                                                 // storing (c_h)

    Store_tupple_DB(this->Data.map1_W, binary_w, make_tuple(get<0>(map1_W_value), get<1>(map1_W_value) + 1, get<2>(map1_W_value) + 1));

    for (int i = m - 1; i >= 0; --i)
    {
        string w_i = binary_w.substr(0, i + 1);                                                                                 //w_0w_1...w_i
        map1_W_value = {};                                                                                                      // emptying the variable    
        decision = Retrive_tupple_DB(this->Data.map1_W, w_i, map1_W_value);

        if (decision == 0)
        {
            string w_iplus1 = w_i + (binary_w.substr(i+1, 1) == "0" ? "1" : "0");                                               //w_0w_1...w_i||bar(w_{i+1})
            cnt = cnt + histUptCnt(this->Data.map1_W, m, w_iplus1);

            encryptAES(this->Data.client_sk1, w_i, get<0>(map1_W_value));

            get<1>(map1_W_value) = cnt;

            SSet.emplace_back(get<0>(map1_W_value), get<1>(map1_W_value));
        }
        else
        {
            SSet.emplace_back(get<0>(map1_W_value), get<2>(map1_W_value));

            cnt = get<1>(map1_W_value);
        }
    }

    string addr, val;
    int c;
    for (const auto& elt : SSet)
    {
        c = get<1>(elt) + 1;
        addr = sha256(get<0>(elt) + to_string(c));
        val = xorStrings(ind + to_string(op == true ? 1 : 0), hashSHAKE(get<0>(elt) + to_string(c), 16));

        t_u.emplace_back(addr, val);
    }
    // send t_u to server                                   
}

void DSSE::Update_server(vector<tuple<string, string>>& t_u)
{

    for (const auto& elt : t_u)
    {
        rocksdb::Status status = this->Data.map2_T_e->Put(rocksdb::WriteOptions(), get<0>(elt), get<1>(elt));
        if (!status.ok())
        {
            std::cerr << "Error updating T_e map from RocksDB: " << status.ToString() << std::endl;
        }
    }
}