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

int first = 0;

char plaintext[128] = "hello";
int firstkey[128] = {0};

char tempkeychar[] = "";

char enctext[128] = "";
char enctextp[128] = "";

char secondenctext[128] = "";

char thirdenctext[128] = "";
char thirdenctextp[128] = "";


void dec() {
    for (int i = 0; i < strlen(secondenctext); i++)
    {
        
        if ((int)secondenctext[i] != '\0')
        {

            thirdenctext[i] = (char)(((int)(secondenctext[i]))-(firstkey[i]));
        }

    }

    for (int i = 0; i < strlen(secondenctext); i++)
    {

        if ((int)thirdenctext[i] > 31)
        {

            thirdenctextp[i-1] = thirdenctext[i];
        }

    }


}


void enc() {
    int randval = 0;
    srand(time(NULL));

    for (int i = 0; i < sizeof(plaintext); i++) 
    {

        if ((int)plaintext[i] != '\0') {

            randval = (rand()%9)+1;

            firstkey[i] = randval;

            enctext[i] = (char)(((int)(plaintext[i]))+(randval));

        }
        
    }


    for (int i = 0; i < sizeof(enctext); i++) 
    {

        if ((int)enctext[i] != '\0') 
        {

            enctextp[i] = enctext[i];
        }
    }

}


int EstblashConnection()
{
    int iResult;
	WSADATA wsaData;

	SOCKET ConnectSocket = INVALID_SOCKET;
	struct sockaddr_in clientService;
	int recvbuflen = DEFAULT_BUFLEN;
	char recvbuf[DEFAULT_BUFLEN] = "";

    do
	{
	ConnectToServer:

		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != NO_ERROR) {

			wprintf(L"WSAStartup failed with error: %d\n", iResult);
			goto ConnectToServer;
		}


		ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (ConnectSocket == INVALID_SOCKET) {

			wprintf(L"socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			goto ConnectToServer;
		}


		clientService.sin_family = AF_INET;
		clientService.sin_addr.s_addr = inet_addr("127.0.0.1");
		clientService.sin_port = htons(27015);


		iResult = connect(ConnectSocket, (SOCKADDR*)&clientService, sizeof(clientService));
		if (iResult == SOCKET_ERROR) {

			wprintf(L"connect failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
		}

		Sleep(1000);

	} while (iResult == SOCKET_ERROR);


    enc();



    iResult = send(ConnectSocket, enctextp, (int)strlen(enctextp)+1, 0);
		if (iResult == SOCKET_ERROR) {

			wprintf(L"send failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();

			goto ConnectToServer;
        }


    do {

        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
        {
            Sleep(3000);

            for (int i = 0; i < sizeof(recvbuf); i++) 
            {

                if ((int)recvbuf[i] != '\0') 
                    secondenctext[i] = recvbuf[i];
            }



            dec();


            iResult = send(ConnectSocket, thirdenctext, (int)strlen(thirdenctext), 0);
                if (iResult == SOCKET_ERROR) {

                    wprintf(L"send failed with error: %d\n", WSAGetLastError());
                    closesocket(ConnectSocket);
                    WSACleanup();
                }
        }
		
        else if (iResult == 0)
		{
			wprintf(L"Connection closed\n");

			iResult = closesocket(ConnectSocket);
			if (iResult == SOCKET_ERROR) {

				wprintf(L"close failed with error: %d\n", WSAGetLastError());
				WSACleanup();
			}

			WSACleanup();

			goto ConnectToServer;
		}
		
        else
		{


			wprintf(L"recv failed with error: %d\n", WSAGetLastError());

			iResult = closesocket(ConnectSocket);

			if (iResult == SOCKET_ERROR) {

				wprintf(L"close failed with error: %d\n", WSAGetLastError());
				WSACleanup();
			}

			WSACleanup();

			goto ConnectToServer;
		}

		for (int i = 0; i <= DEFAULT_BUFLEN - 1; i++)
		{
			if (recvbuf[i] == ' ' || recvbuf[i] == '\n')
			{
				
                recvbuf[i] = '\0';

			}

		}


    } while (iResult > 0);

    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {

        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }


    closesocket(ConnectSocket);
    WSACleanup();
}


int main() {
    char temp[] = "Hello";
    for (int i = 0; i < sizeof(plaintext); i++)
    {
        if (i <= sizeof(temp)-1)
        {
           
            plaintext[i] = temp[i];

        }

        else
        {
            
            plaintext[i] = '\0';

        }

    }


    int randval = 0;
    srand(time(NULL));

    for (int i = 0; i < 20; i++)
    {

        randval = (rand()%93)+33;

        plaintext[i] = (char)randval;

    }


    char temp2[] = "H";
    for (int i = 0; i < sizeof(thirdenctextp); i++)
    {
        if (i <= sizeof(temp2)-1)
        {
            thirdenctextp[i] = temp2[i];
        }
        else
        {
            thirdenctextp[i] = '\0';
        }
    }


    EstblashConnection();

}