// We are implementing our proposed DSSE scheme supporting range query.
#include "../src/Lambda.h"

#include <bitset>
const int Size_ID = 100000;

const int Size_KS = 10000;

// void ind_Depadding_FAST(std::string &ind)
// {
//     size_t pos = ind.find_last_not_of('_');
//     if (pos != std::string::npos)
//         ind.erase(pos + 1);  // Keep characters up to the last non-'_'
//     else
//         ind.clear();  // All characters were '_'
// }


// bitset<Size_ID> build_bitmap(vector<string> inds)
// {
//     bitset<Size_ID> bitmap;                                                     // All bits are initialized to 0

//     for(auto id : inds)
//     {
//         ind_Depadding_FAST(id);
//         int index  = stoi(id);
//         bitmap.set(Size_ID - 1 - index);                                        // Set the corresponding bit to 1. Note: most-significant bit need to be set 1
//     }
//     return bitmap;
// }


void Lambda_Setup(DSSE &FAST_)
{
    cout << "============================= PSI: Setup =============================" << endl; 
    
    Setup(FAST_);
    
}

void Lambda_Update(DSSE &FAST_, vector<int> &Keyword_Space)
{
    cout << "============================= PSI: Update =============================" << endl; 
    
    long double avg_uc = 0;
    long double avg_us = 0;


    // Use a random device to seed the random number engine
    std::random_device rd; 
    std::mt19937 gen(rd());                             // Mersenne Twister engine

    // Create a uniform distribution in the given range
    std::uniform_int_distribution<> distrib(0, Size_KS-1);

    for(int i = 0; i < Size_ID; i++)
    {   
        string ind = "ID" + to_string(i);
        int w = distrib(gen);

        vector<tuple<string, string>> U_list;                                                       // Lambda Update Token List

        auto u_c0 = chrono::high_resolution_clock::now();
        Update_client(FAST_, true, ind, i, w, Keyword_Space, U_list);
        auto u_c1 = chrono::high_resolution_clock::now();
        long double u_cd = chrono::duration_cast<chrono::milliseconds>(u_c1 - u_c0).count();
        

        auto u_s0 = chrono::high_resolution_clock::now();
        Update_server(FAST_, U_list);
        auto u_s1 = chrono::high_resolution_clock::now();
        long double u_sd = chrono::duration_cast<chrono::milliseconds>(u_s1 - u_s0).count();

        avg_uc = avg_uc + u_cd;
        avg_us = avg_us + u_sd; 
    }

    long double avg_client_time = avg_uc / Size_ID;
    long double avg_server_time = avg_us / Size_ID;

    cout << "Average client-side update time (ms): " << avg_client_time << endl;
    cout << "Average server-side update time (ms): " << avg_server_time << endl;

}   


// void Lambda_Search_result(const tuple<int, int> &range_query, vector<string> &res1_l, vector<string> &res1_e, vector<string> &res2_l, vector<string> &res2_e)
// {
    
//     // ####################### Client Side: Bitmap Generation and result interpretation #####################################################

//     bitset<Size_ID> a_lbm, a_ebm;
//     bitset<Size_ID> b_lbm, b_ebm;


//     // As this instant, we are considering only a <= x <= b type range query. To answer such queries, we only required l-bitmap of a, and l and e bitmap of b.

//     a_lbm = build_bitmap(res1_l);
//     //cout << "l-bitmap(" << get<0>(range_query) << ")= "<<  a_lbm << " and ";
    

//     // a_ebm = build_bitmap(bitmap_length, res1_e);
//     // cout << "e-bitmap(" << get<0>(range_query) << ") = " << a_ebm << endl;

//     b_lbm = build_bitmap(res2_l);
//     //cout << "l-bitmap(" << get<1>(range_query) <<")= " << b_lbm << " and ";

//     b_ebm = build_bitmap(res2_e);
//     //cout << "e-bitmap(" << get<1>(range_query) << ")= " << b_ebm << endl;

//     //cout << "Search Result for the query q: [" << get<0>(range_query) << ", " << get<1>(range_query) << "]" << endl;
    
//     auto result = ((~a_lbm) & (b_lbm | b_ebm));
//   //cout << "Result: " << result << endl;                                                         // Equivalent to NOT(l-bm(a)) AND (l-bm(b) AND e-bm(b))
    
//     for (int i = Size_ID - 1; i >= 0; --i) 
//     {
//         if (result.test(i)) 
//         {
//             cout << "ID" << Size_ID - 1 - i << endl;
//         }
//     }

// }



tuple<int, int> generate_range_query(const int &Size_KS, int &range_size) 
{

    if (range_size >= Size_KS) 
    {
        throw std::invalid_argument("Range size too large for given domain.");
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, Size_KS - range_size - 1);

    int a = dist(gen);
    int b = a + range_size;

    return make_tuple(a, b);
}


int main()
{
    DSSE FAST_;

    Lambda_Setup(FAST_);

    //============================= PSI: Keyword Space =============================    

    vector<int> Keyword_Space;
    for(int i = 0; i < Size_KS; i++)
    {
        Keyword_Space.push_back(i);
    }

    Lambda_Update(FAST_, Keyword_Space);

    tuple<int, int> range_query;
    vector<int> Sizes_KS = {10, 100, 500, 1000, 2000};
    for (auto &ks: Sizes_KS)
    {
        long double avg_sc = 0;
        long double avg_ss = 0;

        cout << "============================= PSI: Search for query of size" << ks << "=============================" << endl;

        for (int i = 0; i < 100; i++)
        {
            range_query = generate_range_query(Size_KS, ks);
            
            cout << "q = [" << get<0>(range_query) << ", " << get<1>(range_query) << "]" << endl;

            vector<string> res1_l, res1_e, res2_l, res2_e;
            tuple<string, string, int> stk1_l, stk1_e, stk2_l, stk_2_e;                                                   // Lambda's Search Tokens

            auto s_c0 = chrono::high_resolution_clock::now();
            Search_client(FAST_, range_query, Keyword_Space, stk1_l, stk1_e, stk2_l, stk_2_e);
            auto s_c1 = chrono::high_resolution_clock::now();
            auto s_cd = chrono::duration_cast<chrono::microseconds>(s_c1 - s_c0).count();


            auto s_s0 = chrono::high_resolution_clock::now();
            Search_server(FAST_, stk1_l, stk1_e, stk2_l, stk_2_e, res1_l, res1_e, res2_l, res2_e);
            auto s_s1 = chrono::high_resolution_clock::now();
            auto s_sd = chrono::duration_cast<chrono::milliseconds>(s_s1 - s_s0).count();

            avg_sc = avg_sc + s_cd;
            avg_ss = avg_ss + s_sd; 
        }

        long double avg_client_time = avg_sc / 100;
        long double avg_server_time = avg_ss / 100;

        cout << "Average client-side Search Time (μs): " << avg_client_time << endl;
        cout << "Average server-side Search Time (ms): " << avg_server_time << endl;
        
    }
    
    return 0;
}


    