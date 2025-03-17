#include "include/ClientApp.h"
#include <windows.h>


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
    std::string ip = "127.0.0.1";
    int port = 5555;

    std::istringstream args(lpCmdLine);
    std::string arg1, arg2;

    if (args >> arg1) {
        ip = arg1;
    }
    if (args >> arg2) { 
        try {
            port = std::stoi(arg2);
        }
        catch (...) {
            MessageBoxA(NULL, "Некорректный порт. Используется значение по умолчанию: 5555", "Ошибка", MB_OK | MB_ICONWARNING);
        }
    }

    ClientApp clientApp(ip, port);
    clientApp.start();

    return 0;
}