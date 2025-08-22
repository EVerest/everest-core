# SPDX-License-Identifier: Apache-2.0
# This code was generated and enhanced with the help of an AI assistant (ChatGPT by OpenAI).

import asyncio
import json
import os
import threading
import time
import tkinter as tk
from tkinter import ttk, scrolledtext, messagebox
import websockets

SETTINGS_FILE = "settings.json"
CALLS_FILE = "saved_calls.json"


class JsonRpcWebSocketClient:
    def __init__(self, root):
        self.root = root
        self.root.title("EVerest JSON-RPC WebSocket Client")
        self.ws = None
        self.connected = False
        self.connect_button_label = tk.StringVar(value="Connect")
        self.notification_filters = set()
        self.all_notifications = set()

        self.settings = self.load_settings()
        self.saved_calls = self.load_calls()

        self.setup_ui()

    def setup_ui(self):
        self.root.columnconfigure(0, weight=1)

        # Connection frame
        conn_frame = ttk.LabelFrame(self.root, text="Connection")
        conn_frame.grid(row=0, column=0, padx=10, pady=5, sticky="ew")

        ttk.Label(conn_frame, text="IP:").grid(row=0, column=0, sticky="w")
        self.ip_entry = ttk.Entry(conn_frame, width=15)
        self.ip_entry.grid(row=0, column=1, padx=5)
        self.ip_entry.insert(0, self.settings.get("ip", "127.0.0.1"))

        ttk.Label(conn_frame, text="Port:").grid(row=0, column=2, sticky="w")
        self.port_entry = ttk.Entry(conn_frame, width=7)
        self.port_entry.grid(row=0, column=3, padx=5)
        self.port_entry.insert(0, str(self.settings.get("port", 8080)))

        self.connect_button = ttk.Button(conn_frame, textvariable=self.connect_button_label,
                                         command=self.connect_or_disconnect)
        self.connect_button.grid(row=0, column=4, padx=5)

        # Request/Response console
        rr_frame = ttk.LabelFrame(self.root, text="Requests / Responses")
        rr_frame.grid(row=1, column=0, padx=10, pady=5, sticky="nsew")
        rr_frame.columnconfigure(0, weight=1)
        rr_frame.rowconfigure(0, weight=1)

        self.rr_text = scrolledtext.ScrolledText(rr_frame, height=10)
        self.rr_text.grid(row=0, column=0, sticky="nsew")

        self.rr_text_menu = tk.Menu(root, tearoff=0)
        self.rr_text_menu.add_command(label="Clear content", command=self.clear_rr_text)
        self.rr_text.bind("<Button-3>", self.show_rr_text_menu)

        # Notification frame with filters
        notif_frame = ttk.LabelFrame(self.root, text="Notifications")
        notif_frame.grid(row=2, column=0, padx=10, pady=5, sticky="nsew")
        notif_frame.columnconfigure(0, weight=1)
        notif_frame.rowconfigure(1, weight=1)

        self.notification_autoscroll = tk.BooleanVar(value=True)
        self.notification_autoscroll_checkbutton = ttk.Checkbutton(notif_frame, text='Autoscroll',
                                                                   variable=self.notification_autoscroll)
        self.notification_autoscroll_checkbutton.grid(row=0, column=0, sticky="w")

        self.filter_frame = ttk.Frame(notif_frame)
        self.filter_frame.grid(row=1, column=0, sticky="w")

        self.notification_text = scrolledtext.ScrolledText(notif_frame, height=10)
        self.notification_text.grid(row=2, column=0, sticky="nsew")

        self.notification_text_menu = tk.Menu(root, tearoff=0)
        self.notification_text_menu.add_command(label="Clear content", command=self.clear_notification_text)
        self.notification_text_menu.add_separator()
        self.notification_text_menu.add_checkbutton(label="Autoscroll", onvalue=True, offvalue=False,
                                                    variable=self.notification_autoscroll)
        self.notification_text.bind("<Button-3>", self.show_notification_text_menu)

        # Method call frame
        call_frame = ttk.LabelFrame(self.root, text="Method Call")
        call_frame.grid(row=3, column=0, padx=10, pady=5, sticky="ew")

        ttk.Label(call_frame, text="Method:").grid(row=0, column=0, sticky="w")
        self.method_entry = ttk.Entry(call_frame)
        self.method_entry.grid(row=0, column=1, sticky="ew", padx=5)

        ttk.Label(call_frame, text="Params (JSON):").grid(row=0, column=2, sticky="w")
        self.params_entry = ttk.Entry(call_frame)
        self.params_entry.grid(row=0, column=3, sticky="ew", padx=5)

        self.call_button = ttk.Button(call_frame, text="Call", command=self.send_custom_call)
        self.call_button.grid(row=0, column=4, padx=5)

        self.save_button = ttk.Button(call_frame, text="Save", command=self.save_current_call)
        self.save_button.grid(row=0, column=5, padx=5)

        ttk.Label(call_frame, text="Saved Calls:").grid(row=1, column=0, sticky="w")
        self.call_combobox = ttk.Combobox(call_frame, state="readonly")
        self.call_combobox.grid(row=1, column=1, columnspan=2, sticky="ew", padx=5)
        self.call_combobox.bind("<<ComboboxSelected>>", self.load_selected_call)

        self.delete_button = ttk.Button(call_frame, text="Delete", command=self.delete_selected_call)
        self.delete_button.grid(row=1, column=3, padx=5)

        call_frame.columnconfigure(1, weight=1)
        call_frame.columnconfigure(3, weight=1)

        self.refresh_saved_calls()

    def log(self, message):
        timestamp = int(time.time() * 1000)
        formatted = f"[{timestamp}] {message}\n"
        self.rr_text.insert(tk.END, formatted)
        self.rr_text.see(tk.END)

    def log_notification(self, message):
        timestamp = int(time.time() * 1000)
        msg_obj = json.loads(message)
        method = msg_obj.get("method")
        if method:
            self.all_notifications.add(method)
            self.update_filter_checkboxes()
            if method not in self.notification_filters:
                formatted = f"[{timestamp}] {message}\n"
                self.notification_text.insert(tk.END, formatted)
                if self.notification_autoscroll.get():
                    self.notification_text.see(tk.END)

    def clear_notification_text(self):
        self.notification_text.delete('1.0', tk.END)

    def clear_rr_text(self):
        self.rr_text.delete('1.0', tk.END)

    def show_notification_text_menu(self, event):
        self.notification_text_menu.tk_popup(event.x_root, event.y_root)

    def show_rr_text_menu(self, event):
        self.rr_text_menu.tk_popup(event.x_root, event.y_root)

    def update_filter_checkboxes(self):
        for widget in self.filter_frame.winfo_children():
            widget.destroy()
        for method in sorted(self.all_notifications):
            var = tk.BooleanVar(value=(method not in self.notification_filters))
            cb = ttk.Checkbutton(self.filter_frame, text=method, variable=var)
            cb.var = var
            cb.method = method
            cb.config(command=self.update_filters)
            cb.pack(side=tk.LEFT)

    def update_filters(self):
        self.notification_filters = set()
        for cb in self.filter_frame.winfo_children():
            if not cb.var.get():
                self.notification_filters.add(cb.method)

    def load_settings(self):
        if os.path.exists(SETTINGS_FILE):
            with open(SETTINGS_FILE, "r") as f:
                return json.load(f)
        return {}

    def save_settings(self):
        self.settings["ip"] = self.ip_entry.get()
        self.settings["port"] = int(self.port_entry.get())
        with open(SETTINGS_FILE, "w") as f:
            json.dump(self.settings, f)

    def load_calls(self):
        if os.path.exists(CALLS_FILE):
            with open(CALLS_FILE, "r") as f:
                return json.load(f)
        return {}

    def save_calls(self):
        with open(CALLS_FILE, "w") as f:
            json.dump(self.saved_calls, f)

    def refresh_saved_calls(self):
        self.call_combobox["values"] = list(self.saved_calls.keys())

    def load_selected_call(self, event):
        selected = self.call_combobox.get()
        if selected in self.saved_calls:
            call = self.saved_calls[selected]
            self.method_entry.delete(0, tk.END)
            self.method_entry.insert(0, call["method"])
            self.params_entry.delete(0, tk.END)
            self.params_entry.insert(0, json.dumps(call.get("params", "")))

    def delete_selected_call(self):
        selected = self.call_combobox.get()
        if selected in self.saved_calls:
            del self.saved_calls[selected]
            self.save_calls()
            self.refresh_saved_calls()

    def save_current_call(self):
        method = self.method_entry.get()
        params = self.params_entry.get()
        if not method:
            return
        try:
            parsed_params = json.loads(params) if params else None
        except json.JSONDecodeError:
            messagebox.showerror("Invalid JSON", "Params field is not valid JSON")
            return
        self.saved_calls[method] = {"method": method, "params": parsed_params}
        self.save_calls()
        self.refresh_saved_calls()

    def send_custom_call(self):
        if not self.connected or not self.ws:
            self.log("Not connected")
            return
        method = self.method_entry.get()
        params = self.params_entry.get()
        try:
            msg = {"jsonrpc": "2.0", "method": method, "id": int(time.time()*1000)}
            if params.strip():
                parsed = json.loads(params)
                msg["params"] = parsed
            asyncio.run_coroutine_threadsafe(self.ws.send(json.dumps(msg)), self.loop)
            self.log(f"Sent: {json.dumps(msg)}")
        except json.JSONDecodeError:
            self.log("Invalid JSON in params")

    def connect_or_disconnect(self):
        if self.ws and self.connected:
            self.connect_button.state(['disabled'])
            asyncio.run_coroutine_threadsafe(self.ws.close(), self.loop)
            self.log("Disconnected")
            self.connect_button.state(['!disabled'])
        else:
            self.connect_button.state(['disabled'])
            self.save_settings()
            ip = self.ip_entry.get()
            port = self.port_entry.get()
            uri = f"ws://{ip}:{port}"

            self.loop = asyncio.new_event_loop()
            threading.Thread(target=self.loop.run_until_complete, args=(self.connect_and_listen(uri),),
                             daemon=True).start()
            self.connect_button.state(['!disabled'])

    async def connect_and_listen(self, uri):
        try:
            async with websockets.connect(uri) as websocket:
                self.ws = websocket
                self.connected = True
                self.connect_button_label.set("Disconnect")
                hello = {"jsonrpc": "2.0", "method": "API.Hello", "id": 1}
                await websocket.send(json.dumps(hello))
                self.log(f"Sent: {json.dumps(hello)}")

                async for message in websocket:
                    try:
                        msg = json.loads(message)
                        if "method" in msg and "id" not in msg:
                            self.log_notification(message)
                        else:
                            self.log(message)
                    except json.JSONDecodeError:
                        self.log("Invalid JSON received")
        except Exception as e:
            self.log(f"Connection failed: {str(e)}")
        finally:
            self.connected = False
            self.ws = None
            self.connect_button_label.set("Connect")


if __name__ == "__main__":
    root = tk.Tk()
    app = JsonRpcWebSocketClient(root)
    root.mainloop()
