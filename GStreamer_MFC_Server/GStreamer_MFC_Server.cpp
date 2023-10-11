// GStreamer_MFC_Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

//#include "PODBC.h";
//#include "POLEDB.h"
//#include "DBStructure.h"
#include "tcp/tcpSocket.h"

int main()
{
   CTCP_SOCKET sock;
   if (!sock.Initialize(1)) {
       sock.~CTCP_SOCKET();
   }




    //CODBC podbc;
    //if (podbc.AllocateHandles()) {
    //    if (podbc.ConnectDataSource()) {
    //        if (podbc.ExecuteStatementDirect((SQLWCHAR*)"SELECT * FROM TB_LOGIN")) {
    //            if (podbc.RetrieveResult()) {
    //                podbc.DisconnectDataSource();
    //            }
    //        }
    //    }
    //}
    


   


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
