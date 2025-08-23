#include "../src/RS_Square.h"
#include <chrono>


#define  Size_ID 10000


/* Range parameter: keyword space calW = {0, 1, 2, ... , 2^m - 1}. It is infact the size of the keyword Space
 * m = 17 if |calW| = 10^5, 
 * m = 14 if |calW| = 10^4,  
 * m = 10 if |calW| = 10^3, 
 * m = 7 if |calW| = 10^2,
 * m = 4 if |calW| = 10            
*/

#define  m  14                                         
   


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
    //############################## RS^2: Update Protocol ################################################
    cout << "####################### RS^2: Update Protocol Begins #######################" << endl;


    bool op = true;
   
    int avg_uc = 0;
    int avg_us = 0;

                                                                              
    for (int i = 0; i < Size_ID; i++)
    {
        string ind = "ID" + to_string(i);
        ind = padded_ind(ind);

        int keyword = i;

        cout << "Updating " << ind << " " << keyword << endl;

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
    cout << "Avg time to update a keyword/identifier pair at Client side : " << avg_uc/Size_ID << endl;
    cout << "Avg time to update a keyword/identifier pair at Server side : " << avg_us/Size_ID << endl;

    cout << "####################### RS^2: Update Protocol Ends #######################" << endl;
}


void RS_2_Search(DSSE &RS_2, tuple<int, int> &range_query)
{
    
    //############################## RS^2: Search Protocol ################################################
    cout << "####################### RS^2: Search Protocol Starts #######################" << endl;

    vector<tuple<string, string, int>> t_q;
    vector<string> search_result;


    auto s_c0 = chrono::high_resolution_clock::now();
    RS_2.Search_client(m, range_query, t_q);
    auto s_c1 = chrono::high_resolution_clock::now();

    auto s_cd = chrono::duration_cast<chrono::microseconds>(s_c1 - s_c0).count();

    auto s_s0 = chrono::high_resolution_clock::now();
    RS_2.Search_server(t_q, search_result);
    auto s_s1 = chrono::high_resolution_clock::now();

    auto s_sd = chrono::duration_cast<chrono::microseconds>(s_s1 - s_s0).count();

    for (const auto& id : search_result)
    {
        cout << id;
    }

    cout << " Search time  for a query of a fixed length at Client side : " << s_cd << endl;
    cout << " Search time a query of a fixed length at Server side : " << s_sd << endl;

    cout << "####################### RS^2: Search Protocol Ends #######################" << endl;

}


int main()
{
    DSSE RS_2;
    
    RS_2_Setup(RS_2);

    int expression;
    char choice = 'y';

    tuple <int, int> range_query; 

   

    while (choice == 'y' || choice  == 'Y')
    {
        cout << "RS^2: Enter choice (Update/Search):(1/2)" << endl;

        cin >> expression;
        switch (expression)
        {
            case 1:
                RS_2_Update(RS_2);
                break;
            
            case 2:
                cout << "Enter the range query of the form [int a, int b]: a  = " << endl;
                cin >> get<0>(range_query);

                cout << "Enter the range query of the form [int a, int b]: b  = " << endl;
                cin >> get<1>(range_query);

                RS_2_Search(RS_2, range_query);
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