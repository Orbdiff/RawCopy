#include <Windows.h>
#include <winioctl.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

bool GetClusterInfo(HANDLE hFile, std::vector<std::pair<ULONGLONG, ULONGLONG>>& clusters, ULONGLONG& startVCN);

int CopyRawFile(const std::wstring& sourcePath, const std::wstring& outputPath);