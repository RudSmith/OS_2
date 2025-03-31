#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <sys/select.h>

#define FILENAME "shared_memory.bin"
#define FILESIZE 1024

int main() {
    int fd = -1;
    char* ptr = nullptr;
    bool isMapped = false;

    while (true) {
        std::cout << "\n--- Client Menu ---\n"
                  << "1. Выполнить проецирование\n"
                  << "2. Прочитать данные\n"
                  << "3. Завершить работу\n"
                  << "Выберите действие: ";
        int choice;
        std::cin >> choice;
        std::cin.ignore();

        if (choice == 1) {  // Проецирование
            if (isMapped) {
                std::cout << "Проецирование уже выполнено!\n";
                continue;
            }

            fd = open(FILENAME, O_RDWR);
            if (fd == -1) {
                perror("Не удалось открыть файл");
                continue;
            }

            ptr = (char*) mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            if (ptr == MAP_FAILED) {
                perror("Ошибка mmap");
                close(fd);
                return 1;
            }
            isMapped = true;
            std::cout << "Клиент: Проецирование выполнено.\n";

        } else if (choice == 2) {  // Чтение данных с ожиданием доступности
            if (!isMapped) {
                std::cout << "Сначала выполните проецирование!\n";
                continue;
            }

            std::cout << "Клиент: Ожидание доступности данных...\n";

            fd_set read_fds;
            FD_ZERO(&read_fds);
            FD_SET(fd, &read_fds);

            struct timeval timeout = {5, 0}; // Ждём максимум 5 секунд

            int result = select(fd + 1, &read_fds, NULL, NULL, &timeout);
            if (result == -1) {
                perror("Ошибка select()");
                continue;
            } else if (result == 0) {
                std::cout << "Клиент: Таймаут ожидания данных.\n";
                continue;
            }

            std::cout << "Клиент получил: " << ptr << "\n";

            ptr[FILESIZE - 1] = '1';
            msync(ptr, FILESIZE, MS_SYNC);
            std::cout << "Клиент: Сообщил серверу, что данные прочитаны.\n";

        } else if (choice == 3) {  // Завершение работы
            if (isMapped) {
                munmap(ptr, FILESIZE);
                close(fd);
                std::cout << "Клиент: Файл отключён от памяти.\n";
            }
            std::cout << "Клиент завершает работу.\n";
            break;
        } else {
            std::cout << "Некорректный ввод!\n";
        }
    }
    return 0;
}