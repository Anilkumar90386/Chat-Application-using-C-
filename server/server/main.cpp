#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<tchar.h>
#include<thread>
#include<vector>
using namespace std;
#pragma comment(lib,"ws2_32.lib")
/*
  // Intitialize winsock library

   //create the socket

   //get ip and port

   //bind the ip/port with the socket

   //listen on the socket

   //accept

   //receive and send

   //clean up the library
*/
bool Initialize()
{
	WSADATA data;
	return WSAStartup(MAKEWORD(2,2),&data)==0;
}
void InteractWithClient(SOCKET clientSocket,vector<SOCKET>&clients)
{
	//send and recive
	cout << "Client connected successfully" << endl;
	while (1)
	{
		char buffer[4096];
		int bytesrecvd = recv(clientSocket, buffer, sizeof(buffer), 0);
		if (bytesrecvd <= 0)
		{
			cout << "client disonnected";
		}
		string message(buffer, bytesrecvd);
		cout << "Message from client:" << message << endl;
		for (auto client : clients)
		{
			if (client != clientSocket)
			{
				send(client, message.c_str(), message.length(), 0);
			}
		}
	}
	auto it = find(clients.begin(), clients.end(), clientSocket);
	if (it != clients.end())
	{
		clients.erase(it);
	}
	closesocket(clientSocket);
}
int main()
{
	if (!Initialize())
	{
		cout << "Winsock initialize failed"<<endl;
		return 1;
	}
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM , 0);
	if (listenSocket == INVALID_SOCKET)
	{
		cout << "Socket creation failed" << endl;
		return 1;
	}
	//create address structure
	int port = 12345;
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	//convert the ip address (0.0.0.0) to binary
	if (InetPton(AF_INET, _T("0.0.0.0"), &serveraddr.sin_addr) != 1) {
		cout << "setting address structure is failed" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
	//bind

	if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR)
	{
		cout << "bind failed" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
	//listen
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		cout << "listen failed" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
	cout << "server has started listening on port :" <<port<< endl;
	vector<SOCKET>clients;
	//accept

	while (1)
	{
		SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
		if (clientSocket == INVALID_SOCKET)
		{
			cout << "Invalid socket" << endl;
		}
		clients.push_back(clientSocket);
		thread t1(InteractWithClient, clientSocket,std::ref(clients));
		t1.detach();
	}
	
	closesocket(listenSocket);



	WSACleanup();
	return 0;
}