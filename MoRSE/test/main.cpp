#include "../src/MoRSE.h"

#define Size_ID 100
#define Size_KS 100

          
/* Range parameter: keyword space calW = {0, 1, 2, ... , 2^m - 1}. It is infact the size of the keyword Space
 * m = 17 if |calW| = 10^5, 
 * m = 14 if |calW| = 10^4,  
 * m = 10 if |calW| = 10^3, 
 * m = 7 if |calW| = 10^2,
 * m = 4 if |calW| = 10            
*/

#define m 7

void MoRSE_Update(DSSE &FAST_)
{
    cout << "============================= MoRSE: Update Begins =============================" << endl; 

    bool op = true;

    int avg_uc = 0;
    int avg_us = 0;


    for(int i = 0; i < Size_ID; i++)
    {
        string ind = to_string(i);
        int w = i;

        vector<tuple<string, string>> u_tokens;
        
        auto u_c0 = chrono::high_resolution_clock::now();
        MoRSE_Update_client(FAST_, op, w, ind, m, u_tokens);
        auto u_c1 = chrono::high_resolution_clock::now();

        auto u_cd = chrono::duration_cast<chrono::microseconds>(u_c1 - u_c0).count();

        auto u_s0 = chrono::high_resolution_clock::now();
        MoRSE_Update_server(FAST_, u_tokens);
        auto u_s1 = chrono::high_resolution_clock::now();

        auto u_sd = chrono::duration_cast<chrono::microseconds>(u_s1 - u_s0).count();

        avg_uc = avg_uc + u_cd;
        avg_us = avg_us + u_sd; 
    }                                      

    cout << "Avg time to update a keyword/identifier pair at Client side : " << avg_uc/Size_ID << endl;
    cout << "Avg time to update a keyword/identifier pair at Server side : " << avg_us/Size_ID << endl;

    cout << "============================= MoRSE: Update Ends =============================" << endl; 
    
}

void MoRSE_Search(DSSE &FAST_, tuple<int, int> &range_query)
{
    cout << "####################### MoRSE: Search Protocol Begins #######################" << endl;

    vector<tuple<string, string, int>> s_tokens;
    vector<string> search_result;

    auto s_c0 = chrono::high_resolution_clock::now();
    MoRSE_Search_client(FAST_, range_query, m, s_tokens);
    auto s_c1 = chrono::high_resolution_clock::now();

    auto s_cd = chrono::duration_cast<chrono::microseconds>(s_c1 - s_c0).count();

    auto s_s0 = chrono::high_resolution_clock::now();
    MoRSE_Search_server(FAST_, s_tokens, search_result);
    auto s_s1 = chrono::high_resolution_clock::now();

    auto s_sd = chrono::duration_cast<chrono::microseconds>(s_s1 - s_s0).count();

    
    for(const auto& id : search_result)
    {
        cout << "ind = " << id << endl;
    }

    cout << "avg time to search a query of length 100 at client side : " << s_cd << endl;
    cout << "avg time to search a query of length 100 at server side : " << s_sd << endl;

    cout << "####################### MoRSE: Search Protocol Ends #######################" << endl;
}

int main()
{
    DSSE FAST_;

    cout << "============================= MoRSE: Setup Begins =============================" << endl; 
    MoRSE_Setup(FAST_);
    cout << "============================= MoRSE: Setup Ends =============================" << endl; 


    int expression;
    char choice = 'y';

    tuple <int, int> range_query; 

    while (choice == 'y' || choice  == 'Y')
    {
        cout << "MoRSE: Enter choice (Update/Search):(1/2)" << endl;

        cin >> expression;
        switch (expression)
        {
            case 1:
                MoRSE_Update(FAST_);
                break;
            
            case 2:
                cout << "Enter the range query of the form [int a, int b]: a  = " << endl;
                cin >> get<0>(range_query);

                cout << "Enter the range query of the form [int a, int b]: b  = " << endl;
                cin >> get<1>(range_query);

                MoRSE_Search(FAST_, range_query);
                break;
            
            default:
                cout << "The entered option is not correct" << endl;
                break;
        }
        cout << "Do you want to continue? (y/n): ";
        cin >> choice;  
    }
    
    return 0;
}