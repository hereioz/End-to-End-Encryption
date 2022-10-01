#define _WIN32_WINNT 0x0501
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#undef UNICODE
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <Wininet.h>
#pragma comment(lib, "Wininet.lib")
#include <Psapi.h>
#pragma comment(lib, "Psapi.lib")
#include <Ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"



char firstenctext[128] = "";
int firstkey[128] = {0};

char firstenctextp[128] = "";
int first = 0;

char secondenctext[128] = "";
char secondenctextp[128] = "";

char thirdenctext[128] = "";
char thirdenctextp[128] = "";

char forthdectext[128] = "";



void dec() {

    for (int i = 0; i < strlen(thirdenctext); i++)
    {

        if ((int)thirdenctext[i] != '\0')
        {
            
            forthdectext[i] = (int)((thirdenctext[i])-(firstkey[i]));

        }
    }


}

void enc() {

    int randval = 0;
    srand(time(NULL));


    for (int i = 0; i < strlen(firstenctext); i++) 
    {

        if ((int)firstenctext[i] != '\0') {
        
            randval = (rand()%9)+1;
        
            firstkey[i] = randval;
        
            secondenctext[i] = (char)(((int)(firstenctext[i]))+randval);
        
        }
    }



    for (int i = 0; i < strlen(firstenctext); i++) 
    {

        if ((int)secondenctext[i] > 31) 
        {

            secondenctextp[i] = secondenctext[i];

        }
    }

}


int EstiblashConnection()
{
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {

        printf("WSAStartup failed with error: %d\n", iResult);

        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;


    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {

        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();

        return 1;
    }


    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {

        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();

        return 1;
    }


    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {

        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();

        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {

        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {

        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();

        return 1;
    }


    closesocket(ListenSocket);


    do {

        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            Sleep(3000);

            if (first == 0)
            {
                for (int i = 0; i < sizeof(recvbuf); i++) {
                    if ((int)recvbuf[i] != '\0') 
                        firstenctext[i] = recvbuf[i];
                }


                enc();


                iResult = send(ClientSocket, (char*)secondenctextp, (int)strlen((char*)secondenctext), 0);
                if (iResult == SOCKET_ERROR) {

                    wprintf(L"send failed with error: %d\n", WSAGetLastError());
                    closesocket(ClientSocket);
                    WSACleanup();
                }


                first = 1;
            }
            
            
            else
            {

                for (int i = 0; i < sizeof(recvbuf); i++) 
                {

                    if ((int)recvbuf[i] != '\0') 
                        thirdenctext[i] = recvbuf[i];

                }


                dec();
                
                return 1;
            }

            

        }
        else if (iResult == 0)
            printf("Connection closing...\n");
        else  {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }



    } while (iResult > 0);


    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }


    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}

int main() {
    char temp[] = "Hello";
    for (int i = 0; i < sizeof(firstenctext); i++)
    {
        if (i <= sizeof(temp)-1)
        {
            firstenctext[i] = temp[i];
        }
        else
        {
            firstenctext[i] = '\0';
        }
    }
    char temp2[] = "H";
    for (int i = 0; i < sizeof(secondenctext); i++)
    {
        if (i <= sizeof(temp2)-1)
        {
            secondenctext[i] = temp2[i];
        }
        else
        {
            secondenctext[i] = '\0';
        }
    }


    EstiblashConnection();
    

    if (strlen(thirdenctext) > 1)
    {
        printf("Gotcha!\n");
    }
    else
    {
        printf("Didn't collect the text ):\n");
        exit(0);
    }

    char answer[21] = "";
    printf("what do you think the server sent? [only 20 chars]: ");
    scanf("%s", &answer);

    if (strlen(answer) > 20)
    {
        exit(0);
    }

    int correctcounter = 0;

    for (int i = 0; i < 20; i++)
    {
        if (answer[i] == forthdectext[i])
        {
            correctcounter += 1;
        }
    }

    if (correctcounter == 20)
    {
        printf("You won ):\n");
    }
    else
    {
        printf("Try again xD\n");
    }

}