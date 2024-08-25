#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <wininet.h>
#include <shellapi.h>
#include "apikey.h" // File containig the API key

#pragma comment(lib, "wininet.lib")

#define BUFFER_SIZE 4096
#define WHITE_SPACE " \t\r\n"
#define BTC_API_ENDP_USD "https://rest.coinapi.io/v1/exchangerate/BTC/USD?apikey=" API_KEY

// Shows a notification with a given message
static void ShowNotification(double value)
{
    NOTIFYICONDATA nid;
    memset(&nid, 0, sizeof(NOTIFYICONDATA));

    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = NULL;
    nid.uID = 1001;
    nid.uFlags = NIF_INFO;
    nid.hIcon = LoadIcon(NULL, IDI_INFORMATION);

    char message[BUFFER_SIZE];
    snprintf(message, sizeof(message), "USD: %.2f", value);
    strcpy_s(nid.szInfoTitle, sizeof(nid.szInfoTitle) / sizeof(TCHAR), "Bitcoin value");
    strcpy_s(nid.szInfo, sizeof(nid.szInfo) / sizeof(TCHAR), message);

    nid.dwInfoFlags = NIIF_INFO;

    Shell_NotifyIcon(NIM_ADD, &nid);
    Sleep(5000);
    Shell_NotifyIcon(NIM_DELETE, &nid);
}

// Fetch the crypto data
static void FetchBTCData(char* buffer, DWORD size)
{
    HINTERNET hSession = InternetOpen("HTTPGET", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);

    if (hSession) {
        HINTERNET hConnect = InternetOpenUrl(hSession,
            BTC_API_ENDP_USD,
            NULL, 0,
            INTERNET_FLAG_RELOAD | INTERNET_FLAG_SECURE, 0);

        if (hConnect) {
            DWORD bytesRead;
            DWORD totalBytesRead = 0;

            while (InternetReadFile(hConnect, buffer + totalBytesRead, size - totalBytesRead - 1, &bytesRead) && bytesRead) {
                totalBytesRead += bytesRead;
                buffer[totalBytesRead] = '\0';  // Null-termina a string a cada iteração
            }

            InternetCloseHandle(hConnect);
        }
        
        InternetCloseHandle(hSession);
    }
}

int main(void)
{
    char buffer[BUFFER_SIZE] = {0};
    FetchBTCData(buffer, sizeof(buffer));

    char* begpos = strstr(buffer, "\"rate\": ") + strlen("\"rate\": ");
    size_t endpos = strcspn(begpos, WHITE_SPACE);

    char rate[BUFFER_SIZE] = { 0 };
    strncpy(rate, begpos, endpos);

    char* endptr;
    double value = strtod(rate, &endptr);
    printf("USD: %.2f", value);
    ShowNotification(value);

	return EXIT_SUCCESS;
}