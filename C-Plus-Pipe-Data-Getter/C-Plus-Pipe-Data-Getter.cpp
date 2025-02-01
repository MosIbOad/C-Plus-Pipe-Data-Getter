#include <windows.h>
#include <iostream>
#include <string>
#include "json.hpp"  // https://github.com/nlohmann/json

using json = nlohmann::json;

int main() {
    const char* pipeName = R"(\\.\pipe\MyPipe)";

    std::cout << "Baglanti kuruluyor..." << std::endl;
    HANDLE hPipe = CreateFileA(
        pipeName, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Baglanti basarisiz! Hata kodu: " << GetLastError() << std::endl;
        std::cout << "Enter tusuna basarak cikabilirsiniz...";
        std::cin.get();
        return 1;
    }

    std::cout << "Baglandi! Veri bekleniyor...\n" << std::endl;

    while (true) {
        char buffer[256] = { 0 };
        DWORD bytesRead;

        if (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &bytesRead, nullptr)) {
            if (bytesRead == 0) {
                std::cout << "Uyarı: Bos veri alindi, bekleniyor...\n" << std::endl;
                continue;
            }

            buffer[bytesRead] = '\0';

            std::cout << "Okunan Veri: [" << buffer << "]\n" << std::endl;

            try {
                json receivedData = json::parse(buffer);

                if (!receivedData.contains("key") || !receivedData.contains("value")) {
                    std::cerr << "Hata: JSON'da 'key' veya 'value' eksik!\n" << std::endl;
                    continue;
                }

                std::string key = receivedData["key"];
                std::string value = receivedData["value"];

                std::cout << "Key: " << key << " | Value: " << value << "\n" << std::endl;
            }
            catch (const json::exception& e) {
                std::cerr << "JSON exception error: " << e.what() << "\n" << std::endl;
            }
        }
        else {
            std::cerr << "Read error! Error code: " << GetLastError() << std::endl;
            break;
        }
    }

    std::cout << "Enter'a basarak cikabilirsiniz...";
    std::cin.get();
    CloseHandle(hPipe);
    return 0;
}
