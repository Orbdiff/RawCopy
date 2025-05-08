#include "privilege.hpp"
#include "rawcopy.hpp"

int wmain(int argc, wchar_t* argv[]) {
    if (!privilege::IsRunningAsAdmin()) {
        std::wcerr << L"This program must be run as administrator." << std::endl;
        return 1;
    }

    if (argc != 3) {
        std::wcerr << L"Usage: RawCopy.exe <SourcePath> <OutputPath>" << std::endl;
        return 1;
    }

    RawCopier copier;
    return copier.Run(argv[1], argv[2]);
}
