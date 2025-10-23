#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
# SPDX-License-Identifier: Apache-2.0
# Copyright Pionix GmbH and Contributors to EVerest
#
"""
author: andreas.heinrich@pionix.de
This script starts a CMake target http server and triggers
regular rebuilds of a specified CMake target upon changes.
"""


import subprocess
import sys
import time
import argparse
import signal
from pathlib import Path
from rich.live import Live
from rich.console import Console
from rich.panel import Panel
from rich.layout import Layout
from threading import Thread


console = Console()


def run_target(build_dir: Path, target: str, live_panel, panel_size: int) -> None:    
    process = subprocess.Popen(
        [
            "cmake",
            "--build", build_dir.as_posix(),
            "--target", target
        ],
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True
    )
    output_lines = []
    output_lines.append(f"Running target {target} at {time.strftime('%X')}\n")
    for line in iter(process.stdout.readline, ""):
        line = line.rstrip()
        output_lines.append(line)
        output_lines = output_lines[-panel_size:]
        live_panel.update(Panel("\n".join(output_lines), title=f"{target} output"))
    process.wait()


def start_server(build_dir: Path, server_target: str, server_lines: list, live_panel, panel_size: int) -> subprocess.Popen:
    print(f"Starting server target {server_target}...")
    process = subprocess.Popen(
        [
            "cmake",
            "--build", str(build_dir),
            "--target", server_target
        ],
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
    )
    
    def read_server_output():
        for line in iter(process.stdout.readline, ""):
            line = line.rstrip()
            server_lines.append(line)
            server_lines[:] = server_lines[-panel_size:]
            live_panel.update(Panel("\n".join(server_lines), title=f"{server_target} output"))

    t = Thread(target=read_server_output, daemon=True)
    t.start()
    return process


def stop_server(proc: subprocess.Popen) -> None:
    if proc and proc.poll() is None:
        print("Stopping server...")
        proc.send_signal(signal.SIGINT)
        try:
            proc.wait(timeout=5)
        except subprocess.TimeoutExpired:
            proc.kill()


def main():
    parser = argparse.ArgumentParser(description="Watch CMake target and manage server target")
    parser.add_argument("watch_target", help="CMake target to monitor and rerun if changed")
    parser.add_argument("server_target", help="CMake target that runs the server")
    parser.add_argument("--build-dir", default="build", help="CMake build directory")
    parser.add_argument("--interval-ms", type=int, default=2000, help="Check interval in milliseconds")
    args = parser.parse_args()

    build_dir = Path(args.build_dir)
    watch_target = args.watch_target
    server_target = args.server_target

    panel_size = 10
    layout = Layout()
    layout.split_column(
        Layout(name="server", size=panel_size+2),
        Layout(name="watch", size=panel_size+2)
    )
    with Live(layout, console=console, refresh_per_second=1):
        server_lines = []
        server_lines.append("Starting server...")
        server_panel = Panel("\n".join(server_lines), title=f"{server_target} output")
        layout["server"].update(server_panel)
        watch_panel = Panel("\n\n\n", title=f"{watch_target} output")
        layout["watch"].update(watch_panel)

        server_proc = start_server(build_dir, server_target, server_lines, layout["server"], panel_size)
        try:
            while True:
                time.sleep(args.interval_ms / 1000)
                run_target(build_dir, watch_target, layout["watch"], panel_size)
        except KeyboardInterrupt:
            stop_server(server_proc)
            print("\n Exiting.")


if __name__ == "__main__":
    main()
