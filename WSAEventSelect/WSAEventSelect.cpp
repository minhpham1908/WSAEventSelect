// WSAEventSelect.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib")

int main() {
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(9000);

	bind(listener, (SOCKADDR*)&addr, sizeof(addr));
	listen(listener, 5);

	SOCKET sockets[64];
	WSAEVENT events[64];
	int numEvents = 0;
	WSAEVENT newEvent = WSACreateEvent();
	WSAEventSelect(listener, newEvent, FD_ACCEPT);
	sockets[numEvents] = listener;
	events[numEvents] = newEvent;
	numEvents++;

	int ret;
	char buf[512];

	while (true) {
		ret = WSAWaitForMultipleEvents(numEvents, events, FALSE, 0, FALSE);
		if (ret == WSA_WAIT_FAILED)
			break;

		int index = ret - WSA_WAIT_EVENT_0;
		for (int i = index; i < numEvents; i++) {
			ret = WSAWaitForMultipleEvents(1, &events[i], TRUE, 0, FALSE);
			if (ret == WSA_WAIT_FAILED)
				continue;
			if (ret == WSA_WAIT_TIMEOUT)
				continue;

			WSANETWORKEVENTS networkEvents;
			WSAEnumNetworkEvents(sockets[i], events[i], &networkEvents);
			WSAResetEvent(events[i]);

			if (networkEvents.lNetworkEvents & FD_ACCEPT) {
				if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0) {
					printf("FD_ACCEPT failed\n");
					continue;
				}
				if (numEvents == WSA_MAXIMUM_WAIT_EVENTS) {
					printf("Too many connection\n");
					continue;
				}

				SOCKET client = accept(sockets[i], NULL, NULL);

				newEvent = WSACreateEvent();
				WSAEventSelect(client, newEvent, FD_READ | FD_CLOSE);

				events[numEvents] = newEvent;
				sockets[numEvents] = client;
				numEvents++;
				printf("New client connected %d\n", (int)client);
			}

			if (networkEvents.lNetworkEvents & FD_READ) {
				if (networkEvents.iErrorCode[FD_READ_BIT] != 0)
				{
					printf("FD_READ failed\n");
					continue;
				}

				ret = recv(sockets[i], buf, sizeof(buf), 0);
				if (ret <= 0) {
					printf("FD_READ failed\n");
					continue;
				}

				buf[ret] = 0;
				if (buf[ret - 1] = '\n') {
					buf[ret - 1] = 0;
				}
				printf("Received: %s\n", buf);
			}

			if (networkEvents.lNetworkEvents & FD_CLOSE) {
				if (networkEvents.iErrorCode[FD_CLOSE_BIT] != 0) {
					printf("FD_CLOSE failed\n");
					continue;
				}

				closesocket(sockets[i]);

				printf("Client disconnected\n");
			}
		}
	}
}