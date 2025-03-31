#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>

#define FILENAME "shared_memory_file"
#define FILESIZE 1024

void mapFile(int& fd, void*& ptr) {
    fd = open(FILENAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("Error opening file");
        exit(1);
    }

    ftruncate(fd, FILESIZE);
    ptr = mmap(nullptr, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("Error mapping file");
        exit(1);
    }
    std::cout << "File mapped successfully.\n";
}

void writeData(void* ptr) {
    std::cout << "Enter message to write: ";
    std::string message;
    std::cin.ignore();
    std::getline(std::cin, message);
    strncpy((char*)ptr, message.c_str(), FILESIZE);
    std::cout << "Data written: " << message << "\n";
}

void cleanup(int fd, void* ptr) {
    munmap(ptr, FILESIZE);
    close(fd);
    unlink(FILENAME);
    std::cout << "Resources released, file deleted.\n";
}

int main() {
    int fd;
    void* ptr;
    
    while (true) {
        std::cout << "\nServer Menu:\n"
                     "1. Map file to memory\n"
                     "2. Write data\n"
                     "3. Exit\n"
                     "Enter choice: ";
        int choice;
        std::cin >> choice;

        switch (choice) {
            case 1:
                mapFile(fd, ptr);
                break;
            case 2:
                writeData(ptr);
                break;
            case 3:
                cleanup(fd, ptr);
                return 0;
            default:
                std::cout << "Invalid choice!\n";
        }
    }
}
