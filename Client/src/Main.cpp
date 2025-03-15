#include "../include/ClientApp.h"
#include <windows.h>


int main()
{
    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, "Russian");

    ClientApp clientApp("127.0.0.1", 5555);
    clientApp.start();

	return 0;
}