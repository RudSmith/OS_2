#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>

#define FILENAME "shared_memory.bin"
#define FILESIZE 1024

int main() {
    int fd = -1;
    char* ptr = nullptr;
    bool isMapped = false;

    while (true) {
        std::cout << "\n--- Server Menu ---\n"
                  << "1. Выполнить проецирование\n"
                  << "2. Записать данные\n"
                  << "3. Завершить работу\n"
                  << "Выберите действие: ";
        int choice;
        std::cin >> choice;
        std::cin.ignore();  // Убираем остатки \n из ввода

        if (choice == 1) {  // Проецирование файла
            if (isMapped) {
                std::cout << "Проецирование уже выполнено!\n";
                continue;
            }
            fd = open(FILENAME, O_RDWR | O_CREAT | O_TRUNC, 0666);
            if (fd == -1) {
                perror("Failed to open file");
                return 1;
            }
            ftruncate(fd, FILESIZE);

            ptr = (char*) mmap(NULL, FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
            if (ptr == MAP_FAILED) {
                perror("Failed to mmap");
                close(fd);
                return 1;
            }
            isMapped = true;
            std::cout << "Сервер: Проецирование выполнено.\n";

        } else if (choice == 2) {  // Запись данных
            if (!isMapped) {
                std::cout << "Сначала выполните проецирование!\n";
                continue;
            }
            std::cout << "Введите данные для записи: ";
            std::string data;
            std::cin.ignore();
            std::getline(std::cin, data);

            strncpy(ptr, data.c_str(), FILESIZE - 1);
            ptr[data.size()] = '\0';
            msync(ptr, FILESIZE, MS_SYNC);
            std::cout << "Сервер: Данные записаны. Ожидаем клиента...\n";

            while (ptr[FILESIZE - 1] != '1') {
                usleep(100 * 1000);
            }

            std::cout << "Сервер: Клиент прочитал данные.\n";

        } else if (choice == 3) {  // Завершение работы
            if (isMapped) {
                munmap(ptr, FILESIZE);
                close(fd);
                unlink(FILENAME);
                std::cout << "Сервер: Файл удалён, ресурсы освобождены.\n";
            }
            std::cout << "Сервер завершает работу.\n";
            break;
        } else {
            std::cout << "Некорректный ввод!\n";
        }
    }
    return 0;
}