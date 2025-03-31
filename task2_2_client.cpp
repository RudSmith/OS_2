#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <sys/select.h>

#define FILENAME "shared_memory_file"
#define FILESIZE 1024

void mapFile(int& fd, void*& ptr) {
    fd = open(FILENAME, O_RDWR);
    if (fd == -1) {
        perror("Error opening file");
        exit(1);
    }

    ptr = mmap(nullptr, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("Error mapping file");
        exit(1);
    }
    std::cout << "File mapped successfully.\n";
}

void readData(void* ptr, int fd) {
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);
    
    timeval timeout = {5, 0}; // 5 секунд
    int result = select(fd + 1, &read_fds, nullptr, nullptr, &timeout);

    if (result > 0) {
        std::cout << "Client received: " << (char*)ptr << "\n";
    } else {
        std::cerr << "Timeout: No data received!\n";
    }
}

void cleanup(int fd, void* ptr) {
    munmap(ptr, FILESIZE);
    close(fd);
    std::cout << "Resources released.\n";
}

int main() {
    int fd;
    void* ptr;
    
    while (true) {
        std::cout << "\nClient Menu:\n"
                     "1. Map file to memory\n"
                     "2. Read data\n"
                     "3. Exit\n"
                     "Enter choice: ";
        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1:
                mapFile(fd, ptr);
                break;
            case 2:
                readData(ptr, fd);
                break;
            case 3:
                cleanup(fd, ptr);
                return 0;
            default:
                std::cout << "Invalid choice!\n";
        }
    }
}
