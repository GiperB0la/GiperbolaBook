#include "../include/Server.h"


int main()
{
	SetConsoleOutputCP(CP_UTF8);
	setlocale(LC_ALL, "Russian");

	Server server("127.0.0.1", 5555);
	server.start();

	return 0;
}