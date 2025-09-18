#include "include/GiperbolaBookApp.hpp"


//int main(int argc, char* argv[])
//{
//    std::string ip_server = "127.0.0.1";
//    uint16_t port_server = 8888;
//    uint16_t local_port = 8484;
//
//    if (argc >= 2) {
//        ip_server = argv[1];
//    }
//
//    if (argc >= 3) {
//        try {
//            port_server = static_cast<uint16_t>(std::stoi(argv[2]));
//        }
//        catch (...) {
//            std::cerr << "Invalid server port, using default: " << port_server << std::endl;
//        }
//    }
//
//    if (argc >= 4) {
//        try {
//            local_port = static_cast<uint16_t>(std::stoi(argv[3]));
//        }
//        catch (...) {
//            std::cerr << "Invalid local port, using default: " << local_port << std::endl;
//        }
//    }
//
//    GiperbolaBookApp app(ip_server, port_server, local_port);
//    app.run();
//
//    return 0;
//}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    int argc = 0;
    LPWSTR* argvW = CommandLineToArgvW(GetCommandLineW(), &argc);

    std::vector<std::string> argv(argc);
    for (int i = 0; i < argc; i++) {
        int size = WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, nullptr, 0, nullptr, nullptr);
        std::string arg(size - 1, '\0');
        WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, arg.data(), size, nullptr, nullptr);
        argv[i] = std::move(arg);
    }
    LocalFree(argvW);

    std::string ip_server = "127.0.0.1";
    uint16_t port_server = 8888;
    uint16_t local_port = 8484;

    if (argc >= 2) {
        ip_server = argv[1];
    }

    if (argc >= 3) {
        try {
            port_server = static_cast<uint16_t>(std::stoi(argv[2]));
        }
        catch (...) {
            MessageBoxA(nullptr, "Invalid server port, using default", "Warning", MB_OK | MB_ICONWARNING);
        }
    }

    if (argc >= 4) {
        try {
            local_port = static_cast<uint16_t>(std::stoi(argv[3]));
        }
        catch (...) {
            MessageBoxA(nullptr, "Invalid local port, using default", "Warning", MB_OK | MB_ICONWARNING);
        }
    }

    GiperbolaBookApp app(ip_server, port_server, local_port);
    app.run();

    return 0;
}