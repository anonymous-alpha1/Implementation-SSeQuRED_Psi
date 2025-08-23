#include "MoRSE.h"

#include "../../../FAST/Utilities.cpp"

void ind_Padding_FAST(string &ind)
{
    int padding_size = 15 - ind.size();
        if(padding_size < 0)
        {
            cerr <<"Prefix exceeds the target size for ID" << endl;
        }
    ind = ind + string(padding_size, '_');
}