// s411Meta.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
//#include "S411Metadata.h"
#include "MD_S411.h"
#include <iostream>

int main()
{
    {
        //S411Metadata MyMeta;
        std::wstring XML_path = L"C:\\projects\\DkProj\\PortMon\\dKPortMonitoring_Current_16\\dkt\\dKPortMonitoring\\s411Meta\\Prog.txt";

        //bool bRes = MyMeta.MakeMeta(XML_path);
        MD_S411 S411Meta(L"");
        S411Meta.MakeMeta(XML_path);
        _wsystem(XML_path.c_str());
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
