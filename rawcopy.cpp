#include "rawcopy.h"

bool GetClusterInfo(HANDLE hFile, std::vector<std::pair<ULONGLONG, ULONGLONG>>& clusters, ULONGLONG& startVCN) {
    BYTE buffer[4096];
    STARTING_VCN_INPUT_BUFFER input = { 0 };
    DWORD returned = 0;

    if (!DeviceIoControl(hFile, FSCTL_GET_RETRIEVAL_POINTERS,
        &input, sizeof(input),
        buffer, sizeof(buffer),
        &returned, NULL)) {
        std::wcerr << L"FSCTL_GET_RETRIEVAL_POINTERS error: " << GetLastError() << std::endl;
        return false;
    }

    auto* ptrs = reinterpret_cast<RETRIEVAL_POINTERS_BUFFER*>(buffer);
    startVCN = ptrs->StartingVcn.QuadPart;

    for (DWORD i = 0; i < ptrs->ExtentCount; ++i) {
        const auto& ext = ptrs->Extents[i];
        clusters.emplace_back(ext.NextVcn.QuadPart - (i == 0 ? startVCN : ptrs->Extents[i - 1].NextVcn.QuadPart), ext.Lcn.QuadPart);
    }

    return true;
}

int CopyRawFile(const std::wstring& sourcePath, const std::wstring& outputPath) {
    HANDLE hFile = CreateFileW(sourcePath.c_str(), 0,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL, OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        std::wcerr << L"Error opening source file: " << GetLastError() << std::endl;
        return 1;
    }

    std::vector<std::pair<ULONGLONG, ULONGLONG>> clusterInfo;
    ULONGLONG startVCN = 0;

    if (!GetClusterInfo(hFile, clusterInfo, startVCN)) {
        CloseHandle(hFile);
        return 1;
    }

    CloseHandle(hFile);

    HANDLE hVolume = CreateFileW(L"\\\\.\\C:", GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
        OPEN_EXISTING, 0, NULL);

    if (hVolume == INVALID_HANDLE_VALUE) {
        std::wcerr << L"Failed to open the volume: " << GetLastError() << std::endl;
        return 1;
    }

    DWORD bytesPerSector = 512;
    DWORD sectorsPerCluster = 8;
    DWORD clusterSize = bytesPerSector * sectorsPerCluster;
    DWORD bufferSize = clusterSize;

    std::vector<BYTE> buffer(bufferSize);
    std::ofstream out(outputPath, std::ios::binary);

    for (const auto& pair : clusterInfo) {
        ULONGLONG count = pair.first;
        ULONGLONG lcn = pair.second;

        for (ULONGLONG i = 0; i < count; ++i) {
            LARGE_INTEGER offset;
            offset.QuadPart = (lcn + i) * clusterSize;

            SetFilePointerEx(hVolume, offset, NULL, FILE_BEGIN);
            DWORD read = 0;

            if (!ReadFile(hVolume, buffer.data(), bufferSize, &read, NULL)) {
                std::wcerr << L"Error reading from volume: " << GetLastError() << std::endl;
                CloseHandle(hVolume);
                return 1;
            }

            out.write(reinterpret_cast<const char*>(buffer.data()), read);
        }
    }

    out.close();
    CloseHandle(hVolume);

    std::wcout << L"File successfully copied to " << outputPath << std::endl;
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::wcerr << L"Usage: RawCopy.exe <SourcePath> <OutputPath>" << std::endl;
        return 1;
    }

    std::wstring sourcePath = std::wstring(argv[1], argv[1] + strlen(argv[1]));
    std::wstring outputPath = std::wstring(argv[2], argv[2] + strlen(argv[2]));

    return CopyRawFile(sourcePath, outputPath);
}
