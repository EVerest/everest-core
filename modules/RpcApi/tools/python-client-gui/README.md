# JSON-RPC WebSocket Client GUI

A simple Python-based GUI application to connect to the EVerest JSON-RPC API.
Designed for debugging, development, and manual interaction with JSON-RPC services.

---

## 🚀 Features

- Connect via IP and port to a JSON-RPC WebSocket server.
- Automatically sends `API.Hello` after connecting.
- Displays JSON-RPC **requests**, **responses**, and **notifications** in separate consoles.
- Dynamically filters notifications by method name.
- Send custom JSON-RPC method calls with parameters.
- Save, load, and delete custom method calls (persisted to disk).
- All settings are saved and reloaded across sessions.
- UNIX timestamp (with millisecond precision) shown for all received messages.

---

## 🖥️ Requirements

Make sure the following packages are installed (on Debian/Ubuntu-based systems):

```bash
sudo apt update
sudo apt install -y python3 python3-pip python3-tk
```

### 📦 Install dependencies

Create and activate a virtual environment:

```bash
python3 -m venv venv
source venv/bin/activate
```

Install required packages:

```bash
pip install -r requirements.txt
```

---

## ▶️ How to Run

```bash
python3 everest-json-rpc-api.py
```

---

## 💾 Persistent Files

- `settings.json`: Stores the last used IP and port.
- `saved_calls.json`: Stores saved method calls with their parameters.

---

## 📘 Compatibility Matrix

| GUI Version | JSON-RPC Server API Version | Notes                    |
|-------------|-----------------------------|--------------------------|
| `0.0.1`     | `0.0.1`                     | Initial stable version   |

---

## 📝 License

Apache-2.0
