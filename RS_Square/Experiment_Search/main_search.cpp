#include "../src/RS_Square.h"
#include <chrono>


#define  Size_ID 100000
#define Size_KS 10000

/* Range parameter: keyword space calW = {0, 1, 2, ... , 2^m - 1}. It is infact the size of the keyword Space
 * m = 17 if |calW| = 10^5, 
 * m = 14 if |calW| = 10^4,  
 * m = 10 if |calW| = 10^3, 
 * m = 7 if |calW| = 10^2,
 * m = 4 if |calW| = 10            
*/

#define m 14

string padded_ind(string &id)
{
    string ind;
    int padding_size = 15 - id.size();
    if (padding_size < 0)
    {
        cerr << "Prefix exceeds the target size for ID" << endl;
        exit(1);
    }

    ind = id + string(padding_size, '_');

    return ind;
}

void RS_2_Setup(DSSE &RS_2)
{
    cout << "####################### RS^2: Setup Protocol Starts ################" << endl;

    RS_2.Setup();

    cout << "####################### RS^2: Setup Protocol Ends #######################" << endl;
}

void RS_2_Update(DSSE &RS_2)
{
    cout << "####################### RS^2: Update Protocol Begins #######################" << endl;

    bool op = true;
   
    long double avg_uc = 0;
    long double avg_us = 0;

    std::random_device rd; 
    std::mt19937 gen(rd());                             // Mersenne Twister engine

    // Create a uniform distribution in the given range
    std::uniform_int_distribution<> distrib(0, Size_KS-1);            
                                                                              
    for (int i = 0; i < Size_ID; i++)
    {
        string ind = "ID" + to_string(i);
        ind = padded_ind(ind);

        int keyword = distrib(gen);

        vector<tuple<string, string>> utoken;

        auto u_c0 = chrono::high_resolution_clock::now();
        RS_2.Update_client(m, op, ind, keyword, utoken);
        auto u_c1 = chrono::high_resolution_clock::now();

        auto u_cd = chrono::duration_cast<chrono::microseconds>(u_c1 - u_c0).count();
 
        auto u_s0 = chrono::high_resolution_clock::now();
        RS_2.Update_server(utoken);
        auto u_s1 = chrono::high_resolution_clock::now();

        auto u_sd = chrono::duration_cast<chrono::microseconds>(u_s1 - u_s0).count();

        avg_uc = avg_uc + u_cd;
        avg_us = avg_us + u_sd; 
    }

    long double avg_client_time = avg_uc/Size_ID;
    long double avg_server_time = avg_us/Size_ID;

    cout << "Avg time to update a keyword/identifier pair at Client side : " << avg_client_time << endl;
    cout << "Avg time to update a keyword/identifier pair at Server side : " <<  avg_server_time << endl;
    cout << "####################### RS^2: Update Protocol Ends #######################" << endl;
}

tuple<int, int> generate_range_query(int &range_size) 
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
    DSSE RS_2;
    
    RS_2_Setup(RS_2);

    RS_2_Update(RS_2);

    tuple <int, int> range_query; 
    vector<int> Sizes_Q = {10, 100, 500, 1000, 2000};

    for (auto &ks: Sizes_Q)
    {
        long double avg_sc = 0;
        long double avg_ss = 0;

        double avg_stk = 0;

        cout << "============================= RS_Square: Search for query of size" << ks << "=============================" << endl;

        for (int i = 0; i < 100; i++)
        {
            range_query = generate_range_query(ks);
            
            cout << "q = [" << get<0>(range_query) << ", " << get<1>(range_query) << "]" << endl;

            
            int stk_num = 0;

            vector<tuple<string, string, int>> t_q;
            vector<string> search_result;


            auto s_c0 = chrono::high_resolution_clock::now();
            RS_2.Search_client(m, range_query, t_q);
            auto s_c1 = chrono::high_resolution_clock::now();
            auto s_cd = chrono::duration_cast<chrono::microseconds>(s_c1 - s_c0).count();

            stk_num = t_q.size();

            auto s_s0 = chrono::high_resolution_clock::now();
            RS_2.Search_server(t_q, search_result);
            auto s_s1 = chrono::high_resolution_clock::now();

            auto s_sd = chrono::duration_cast<chrono::microseconds>(s_s1 - s_s0).count();

            avg_stk = avg_stk + stk_num;

            avg_sc = avg_sc + s_cd;
            avg_ss = avg_ss + s_sd;
        } 
        
        long double avg_client_time = avg_sc / 100;
        long double avg_server_time = avg_ss / 100;
        
        cout << "Average number of search tokens: " << avg_stk / 100 << endl;
        
        cout << "Average client-side Search Time (μs): " << avg_client_time << endl;
        cout << "Average server-side Search Time (ms): " << avg_server_time << endl;
              
    }
    
    return 0;
}