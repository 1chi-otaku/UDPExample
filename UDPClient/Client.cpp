﻿
#define WIN32_LEAN_AND_MEAN

#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <windows.h>
#include <vector>
using namespace std;

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 4096

#define SERVER_IP "127.0.0.1"
#define DEFAULT_PORT "8888"

SOCKET client_socket;

string nickname;
string colour;

bool isNew = true;

HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

DWORD WINAPI Sender(void* param)
{
    while (true) {
        string fullstring;
        if (isNew) {
            fullstring = nickname + " just joined the chat!\n";
            send(client_socket, fullstring.c_str(), fullstring.length(), 0);
            isNew = false;
            continue;
        }
        // cout << "Please insert your query for server: ";
        string query;
        getline(cin,query);
        if (query == "off") {
            fullstring = nickname + " has left the chat!\n";
            send(client_socket, fullstring.c_str(), fullstring.length(), 0);
            exit(10);
        }
        fullstring = nickname + ": " + query + colour;
        send(client_socket, fullstring.c_str(), fullstring.size(), 0);
        // àëüòåðíàòèâíûé âàðèàíò ââîäà äàííûõ ñòðèíãîì
        // string query;
        // getline(cin, query);
        // send(client_socket, query.c_str(), query.size(), 0);
    }
}

DWORD WINAPI Receiver(void* param)
{
    while (true) {
        int colour;
        char response[DEFAULT_BUFLEN];
        int result = recv(client_socket, response, DEFAULT_BUFLEN, 0);
        colour = response[result - 1] - '0';
        response[result-1] = '\0';

        // cout << "...\nYou have new response from server: " << response << "\n";

        SetConsoleTextAttribute(h, colour);
        cout << response << "\n";
        SetConsoleTextAttribute(h, 15);
        // cout << "Please insert your query for server: ";
    }
}

BOOL ExitHandler(DWORD whatHappening)
{
    switch (whatHappening)
    {
    case CTRL_C_EVENT: // closing console by ctrl + c
    case CTRL_BREAK_EVENT: // ctrl + break
    case CTRL_CLOSE_EVENT: {
        string fullstring = "I left";
        send(client_socket, fullstring.c_str(), fullstring.size(), 0);
        Sleep(100);
        break;
    } 
       
    default:
        return FALSE;
    }
}

int main()
{
    // îáðàáîò÷èê çàêðûòèÿ îêíà êîíñîëè
    //SetConsoleCtrlHandler((PHANDLER_ROUTINE)ExitHandler, true);

    system("title Client");

    // initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }
    

    addrinfo hints = {};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // ðàçðåøèòü àäðåñ ñåðâåðà è ïîðò
    addrinfo* result = nullptr;
    iResult = getaddrinfo(SERVER_IP, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 2;
    }

    addrinfo* ptr = nullptr;
    // ïûòàòüñÿ ïîäêëþ÷èòüñÿ ê àäðåñó, ïîêà íå óäàñòñÿ
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        // ñîçäàòü ñîêåò íà ñòîðîíå êëèåíòà äëÿ ïîäêëþ÷åíèÿ ê ñåðâåðó
        client_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

        if (client_socket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 3;
        }

        // connect to server
        iResult = connect(client_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
        cout << "Enter nickname: ";
        getline(cin, nickname);
        system("cls");
        for (int i = 1; i < 10; i++)
        {
            SetConsoleTextAttribute(h, i);
            cout << i << '\t';
            cout << "Sample" << endl;

        }
        SetConsoleTextAttribute(h, 15);
        cout << "Enter colour of nickname ->";
        cin >> colour;
        system("cls");
        if (iResult == SOCKET_ERROR) {
            closesocket(client_socket);
            client_socket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (client_socket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 5;
    }

    CreateThread(0, 0, Sender, 0, 0, 0);
    CreateThread(0, 0, Receiver, 0, 0, 0);

    Sleep(INFINITE);
}