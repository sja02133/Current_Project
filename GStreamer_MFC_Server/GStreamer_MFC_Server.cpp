// GStreamer_MFC_Server.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include "../Common/GStreamer_MFC/_TCP_/tcpSocket.h"

int main()
{
   CTCP_SOCKET sock;
   if (!sock.Initialize(1)) {
       sock.~CTCP_SOCKET();
   }
}