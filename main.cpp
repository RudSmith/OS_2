#include <iostream>
#include <windows.h>

void GetSystemInfoFunction() 
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);

    std::cout << "Page size: " << si.dwPageSize << " bytes\n";
    std::cout << "Minimum application address: " << si.lpMinimumApplicationAddress << "\n";
    std::cout << "Maximum application address: " << si.lpMaximumApplicationAddress << "\n";
    std::cout << "Number of processors: " << si.dwNumberOfProcessors << "\n";
}

void GetMemoryStatusFunction() 
{
    MEMORYSTATUS memStatus;
    memStatus.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(&memStatus);

    std::cout << "Total physical memory: " << memStatus.dwTotalPhys / (1024 * 1024) << " MB\n";
    std::cout << "Available physical memory: " << memStatus.dwAvailPhys / (1024 * 1024) << " MB\n";
    std::cout << "Total virtual memory: " << memStatus.dwTotalVirtual / (1024 * 1024) << " MB\n";
    std::cout << "Available virtual memory: " << memStatus.dwAvailVirtual / (1024 * 1024) << " MB\n";
}

void QueryMemoryRegion() 
{
    void* address;
    std::cout << "Enter memory address (hex): ";
    std::cin >> std::hex >> address;

    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(address, &mbi, sizeof(mbi))) 
    {
        std::cout << "Base address: " << mbi.BaseAddress << "\n";
        std::cout << "Region size: " << mbi.RegionSize << " bytes\n";
        std::cout << "State: " << ((mbi.State == MEM_COMMIT) ? "Committed" : "Reserved") << "\n";
    }
    else 
    {
        std::cerr << "Failed to query memory region!\n";
    }
}

void ReserveAndCommitMemory(bool manual) 
{
    void* address = nullptr;
    if (manual)
    {
        std::cout << "Enter memory address (hex): ";
        std::cin >> std::hex >> address;
    }

    address = VirtualAlloc(address, 4096, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (address)
        std::cout << "Memory allocated at: " << address << "\n";
    else
        std::cerr << "Memory allocation failed!\n";
}

void FreeMemory() 
{
    void* address;
    std::cout << "Enter allocated memory address (hex): ";
    std::cin >> std::hex >> address;

    if (VirtualFree(address, 0, MEM_RELEASE))
        std::cout << "Memory freed successfully.\n";
    else
        std::cerr << "Failed to free memory!\n";
}

void WriteToMemory()
{
    void* address;
    int value;

    std::cout << "Enter memory address (hex): ";
    std::cin >> std::hex >> address;
    std::cout << "Enter integer value: ";
    std::cin >> value;

    // Проверяем информацию о памяти 03AF0000
    MEMORY_BASIC_INFORMATION mbi;
    if (VirtualQuery(address, &mbi, sizeof(mbi)) == 0) {
        std::cerr << "Invalid memory address!\n";
        return;
    }

    // Проверяем, доступна ли память для записи
    if (mbi.Protect & (PAGE_READONLY | PAGE_NOACCESS)) {
        std::cerr << "Memory is not writable!\n";
        return;
    }

    DWORD oldProtect;
    if (!VirtualProtect(address, sizeof(int), PAGE_READWRITE, &oldProtect)) {
        std::cerr << "Failed to change memory protection!\n";
        return;
    }

    __try {
        *(int*)address = value;
        std::cout << "Value written successfully.\n";
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        std::cerr << "Access violation! Cannot write to this memory address.\n";
    }

    VirtualProtect(address, sizeof(int), oldProtect, &oldProtect);
}

void ChangeMemoryProtection() 
{
    void* address;
    DWORD protectionFlag;
    std::cout << "Enter memory address (hex): ";
    std::cin >> std::hex >> address;
    std::cout << "Enter new protection flag (1 - READONLY, 2 - READWRITE, 3 - EXECUTE): ";
    std::cin >> protectionFlag;

    DWORD oldProtect;
    DWORD newProtect = (protectionFlag == 1) ? PAGE_READONLY :
        (protectionFlag == 2) ? PAGE_READWRITE : PAGE_EXECUTE;

    if (VirtualProtect(address, 4096, newProtect, &oldProtect)) 
    {
        std::cout << "Memory protection changed.\n";
    }
    else 
    {
        std::cerr << "Failed to change memory protection!\n";
    }
}

int main() 
{
    int choice;
    while (true) 
    {
        std::cout << "\nMENU:\n"
            "1. Get System Info\n"
            "2. Get Memory Status\n"
            "3. Query Memory Region\n"
            "4. Reserve and Commit Memory in manual mode\n"
            "5. Reserve and Commit Memory in automatic mode\n"
            "6. Free Memory\n"
            "7. Write to Memory\n"
            "8. Change Memory Protection\n"
            "0. Exit\n"
            "Enter your choice: ";
        std::cin >> choice;

        switch (choice)
        {
        case 1: GetSystemInfoFunction(); break;
        case 2: GetMemoryStatusFunction(); break;
        case 3: QueryMemoryRegion(); break;
        case 4: ReserveAndCommitMemory(true); break;
        case 5: ReserveAndCommitMemory(false); break;
        case 6: FreeMemory(); break;
        case 7: WriteToMemory(); break;
        case 8: ChangeMemoryProtection(); break;
        case 0: return 0;
        default: std::cout << "Invalid choice!\n";
        }

        system("pause");
        system("cls");
    }
}