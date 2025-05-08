#pragma once
#include <windows.h>

namespace privilege {
    inline bool IsRunningAsAdmin() {
        BOOL isAdmin = FALSE;
        HANDLE hToken = NULL;
        if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
            TOKEN_ELEVATION elevation;
            DWORD size;
            if (GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &size)) {
                isAdmin = elevation.TokenIsElevated;
            }
            CloseHandle(hToken);
        }
        return isAdmin;
    }
}
