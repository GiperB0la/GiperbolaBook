# 💬 GiperbolaBook

**GiperbolaBook** is a lightweight online messenger written in **C++**, where both the server and client are built on **non-blocking sockets** powered by **IOCP** (I/O Completion Ports).  
It combines a high-performance networking backend with a custom-designed graphical interface.

---

### ✨ Features
- 🌐 **Client-Server architecture** with multi-user support.  
- ⚡ **Non-blocking sockets + IOCP** for high performance and scalability.  
- 💬 **Real-time messaging** with minimal latency.  
- 🎨 **Custom SFML-based UI** with a unique design.  
- 👥 **User list** with support for private chats.  
- 📂 **Clean project structure** separating server and client logic.  

---

### 🖼️ Screenshot
<picture>
  <source media="(prefers-color-scheme: dark)" srcset="https://github.com/GiperB0la/GiperbolaBook/blob/main/Screen.jpg">
  <img alt="Screen" src="https://github.com/GiperB0la/GiperbolaBook/blob/main/Screen.jpg">
</picture>

---

```powershell
# Clone project
git clone https://github.com/GiperB0la/GiperbolaBook.git
cd GiperbolaBook
mkdir build && cd build

# Configure
cmake .. -DSFML_DIR="path/to/SFML/lib/cmake/SFML"

# Build (Release version)
cmake --build . --config Release
```
