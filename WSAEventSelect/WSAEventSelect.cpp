// WSAEventSelect.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "winsock2.h"

int main()
{
	std::cout << "Hello World!\n";
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN addr;
	addr.sin_port = htonl(9000);

	bind(listener, (SOCKADDR *)&addr, sizeof(addr));
	listen(listener, 5);

	SOCKET sockets[64];
	WSAEVENT events[64];
	int numEvents = 0;

	WSAEVENT newEvent = WSACreateEvent();
	WSAEventSelect(listener, newEvent, FD_ACCEPT);

	sockets[numEvents];
	events[numEvents] = newEvent;

	int ret;
	char buf[256];
	while (true) {
		ret = WSAWaitForMultipleEvents(numEvents, events, FALSE, WSA_INFINITE, FALSE);
		int index = ret - WSA_WAIT_EVENT_0;
		for (int i = index; i < numEvents; i++)
		{
			ret = WSAWaitForMultipleEvents(1, &events[i], TRUE, 0, FALSE);
			if (ret == WSA_WAIT_FAILED)
				continue;
			if (ret == WSA_WAIT_TIMEOUT)
				continue;

			WSANETWORKEVENTS networkEvents;
			WSAEnumNetworkEvents(sockets[i], events[i], &networkEvents);
			WSAResetEvent(events[index]);
			if (networkEvents.lNetworkEvents & FD_ACCEPT) {
				if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0) {
					printf("FD_ACCEPT failed with error %d\n",
						networkEvents.iErrorCode[FD_ACCEPT_BIT]);
					break;
				}
				SOCKET client = accept(sockets[index], NULL, NULL);
				newEvent = WSAEventSelect();
			}
			if (networkEvents.lNetworkEvents & FD_READ) {
				if (networkEvents.iErrorCode[FD_READ_BIT] != 0) {
					printf("FDREAD failed");
					continue;
				}
			}


		}
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
