# Giperbola Messenger

A simple online messenger with a graphical interface built using **SFML**.  
The client-server architecture is implemented with **sockets**.

---

## ⚙️ Setup

To enable colored output in the Windows terminal, run:

```bat
reg add HKCU\Console /v VirtualTerminalLevel /t REG_DWORD /d 1 /f
```

## 🚀 Usage

You can run both client and server with command-line arguments:

```bat
reg Server.exe <ip> <port>
reg Client.exe <ip> <port>
```

If no arguments are provided, the default values will be used:

```bat
reg 127.0.0.1:5555
```

## 📚 Resources
<picture> <source media="(prefers-color-scheme: dark)" srcset="https://github.com/GiperB0la/GiperbolaBook/blob/main/Screen.png"> <source media="(prefers-color-scheme: light)" srcset="YOUR-LIGHTMODE-IMAGE"> <img alt="Screen" src="Screen.png"> </picture>
