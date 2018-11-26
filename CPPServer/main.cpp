#include <iostream>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std; 

void main(){
	
	// Initialize winsock

	WSADATA wsData; 
	WORD ver = MAKEWORD(2, 2);

	int ws0k = WSAStartup(ver, &wsData);
	
	if (ws0k != 0) {
		cerr << "Winsock failed to initialize! Quitting" << endl;
		return; 
	}

	// Create a socket

	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0); 
	if (listening == INVALID_SOCKET)
	{
		cerr << "Can't create socket! Quitting" << endl; 
		return; 
	}

	// Bind the socket an IP&port

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	// Tell winsock the socket is for listening

	listen(listening, SOMAXCONN);

	// Wait for connection

	sockaddr_in client; 
	int clientSize = sizeof(client);
	
	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
	if (clientSocket == INVALID_SOCKET)
	{
		cerr << "Client socket invalid! Quitting" << endl; 
		return;
	}

	char host[NI_MAXHOST]; // Client's remote name
	char service[NI_MAXHOST]; // Service the client is connected on

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXHOST);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		cout << host << "Connected on port " << service << endl; 
	}
	else 
	{
			inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
			cout << host << " host connected on port " <<
			ntohs(client.sin_port) << endl;
	}

	// Close listening socket
	
	closesocket(listening);

	// While loop: accept and echo message back to client

	char buf[4096]; 

	while (true) 
	{
		ZeroMemory(buf, 4096); 

			// Wait for client to send data

		int bytesReceived = recv(clientSocket, buf, 4096, 0); 
		
		if (bytesReceived != SOCKET_ERROR)
		{
			cout << buf << endl;
		}

		
		if (bytesReceived == SOCKET_ERROR)
		{
			cerr << "recv() error. Quitting" << endl; 
			break;
		}
		

		if (bytesReceived == 0)
		{
			cout << "Client Disconnected" << endl;
			break;
		}

		// Echo message back to client

//		send(clientSocket, buf, bytesReceived + 1, 0); 
		send(clientSocket, "message sent", 0, 0); 

	}

	// Close socket

	closesocket(clientSocket);

	// Shudown winsock

	WSACleanup();
}