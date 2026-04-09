"""Web MUD client blueprint: terminal UI + WebSocket-to-telnet proxy."""

import json
import os
import socket
import threading

from flask import Blueprint, render_template, session
from flask_sock import Sock

from auth import login_required

webclient_bp = Blueprint("webclient", __name__, url_prefix="/client")
sock = Sock()

MUD_HOST = os.environ.get("MUD_HOST", "lpmud-server")
MUD_PORT = int(os.environ.get("MUD_PORT", "4000"))

# Telnet protocol bytes
IAC = 0xFF
WILL = 0xFB
WONT = 0xFC
DO = 0xFD
DONT = 0xFE
SB = 0xFA
SE = 0xF0
ECHO_OPT = 0x01


@webclient_bp.route("/")
@login_required
def client_view():
    return render_template("webclient.html")


def _parse_and_relay(telnet_sock, ws, shutdown_evt):
    """Read from telnet socket, strip IAC sequences, relay text to WebSocket."""
    STATE_DATA, STATE_IAC, STATE_NEG, STATE_SB, STATE_SB_IAC = range(5)
    state = STATE_DATA
    neg_cmd = 0

    try:
        while not shutdown_evt.is_set():
            try:
                data = telnet_sock.recv(4096)
            except socket.timeout:
                continue
            except OSError:
                break
            if not data:
                break

            text_parts = []
            for byte in data:
                if state == STATE_DATA:
                    if byte == IAC:
                        state = STATE_IAC
                    else:
                        text_parts.append(byte)

                elif state == STATE_IAC:
                    if byte == IAC:
                        text_parts.append(byte)
                        state = STATE_DATA
                    elif byte in (WILL, WONT, DO, DONT):
                        neg_cmd = byte
                        state = STATE_NEG
                    elif byte == SB:
                        state = STATE_SB
                    else:
                        state = STATE_DATA

                elif state == STATE_NEG:
                    option = byte
                    if option == ECHO_OPT:
                        if neg_cmd == WILL:
                            telnet_sock.sendall(bytes([IAC, DO, ECHO_OPT]))
                            _ws_send(ws, {"type": "echo", "enabled": False})
                        elif neg_cmd == WONT:
                            telnet_sock.sendall(bytes([IAC, DONT, ECHO_OPT]))
                            _ws_send(ws, {"type": "echo", "enabled": True})
                        elif neg_cmd == DO:
                            telnet_sock.sendall(bytes([IAC, WONT, option]))
                        elif neg_cmd == DONT:
                            pass
                    else:
                        if neg_cmd == WILL:
                            telnet_sock.sendall(bytes([IAC, DONT, option]))
                        elif neg_cmd == DO:
                            telnet_sock.sendall(bytes([IAC, WONT, option]))
                    state = STATE_DATA

                elif state == STATE_SB:
                    if byte == IAC:
                        state = STATE_SB_IAC

                elif state == STATE_SB_IAC:
                    if byte == SE:
                        state = STATE_DATA
                    else:
                        state = STATE_SB

            if text_parts:
                text = bytes(text_parts).decode("latin-1")
                _ws_send(ws, {"type": "output", "text": text})

    except Exception:
        pass
    finally:
        shutdown_evt.set()
        _ws_send(ws, {"type": "disconnected"})


def _ws_send(ws, msg):
    """Send JSON to WebSocket, ignoring errors on closed connections."""
    try:
        ws.send(json.dumps(msg))
    except Exception:
        pass


@sock.route("/ws", bp=webclient_bp)
def mud_proxy(ws):
    """WebSocket handler: bridge browser <-> MUD telnet.

    Mirrors the protocol used by the SSETCOweb custom-server.js:
    Browser sends:  {type: "connect"} | {type: "input", text: "..."} | {type: "disconnect"}
    Server sends:   {type: "connected", host, port} | {type: "output", text}
                  | {type: "echo", enabled} | {type: "disconnected"} | {type: "error", message}
    """
    if not session.get("logged_in"):
        _ws_send(ws, {"type": "error", "message": "Not authenticated"})
        return

    telnet_sock = None
    shutdown_evt = threading.Event()
    reader_thread = None

    try:
        while True:
            try:
                raw = ws.receive(timeout=5)
            except Exception:
                # WebSocket closed
                break

            # timeout — no message, just loop back
            if raw is None:
                continue

            try:
                msg = json.loads(raw)
            except (json.JSONDecodeError, TypeError):
                _ws_send(ws, {"type": "error", "message": "Invalid JSON"})
                continue

            msg_type = msg.get("type")

            if msg_type == "connect":
                # Close any prior connection
                if telnet_sock:
                    shutdown_evt.set()
                    try:
                        telnet_sock.close()
                    except OSError:
                        pass
                    if reader_thread:
                        reader_thread.join(timeout=2)
                    shutdown_evt.clear()
                    telnet_sock = None

                try:
                    telnet_sock = socket.create_connection(
                        (MUD_HOST, MUD_PORT), timeout=10
                    )
                    telnet_sock.settimeout(1.0)
                except (OSError, ConnectionRefusedError) as exc:
                    _ws_send(
                        ws,
                        {"type": "error", "message": f"Could not connect: {exc}"},
                    )
                    telnet_sock = None
                    continue

                _ws_send(
                    ws,
                    {"type": "connected", "host": MUD_HOST, "port": MUD_PORT},
                )

                reader_thread = threading.Thread(
                    target=_parse_and_relay,
                    args=(telnet_sock, ws, shutdown_evt),
                    daemon=True,
                )
                reader_thread.start()

            elif msg_type == "input":
                if telnet_sock and not shutdown_evt.is_set():
                    text = msg.get("text", "")
                    try:
                        telnet_sock.sendall(
                            (text + "\r\n").encode("latin-1", errors="replace")
                        )
                    except OSError:
                        shutdown_evt.set()

            elif msg_type == "ping":
                _ws_send(ws, {"type": "pong"})

            elif msg_type == "disconnect":
                if telnet_sock:
                    shutdown_evt.set()
                    try:
                        telnet_sock.close()
                    except OSError:
                        pass

    except Exception:
        pass
    finally:
        shutdown_evt.set()
        if telnet_sock:
            try:
                telnet_sock.close()
            except OSError:
                pass
        if reader_thread:
            reader_thread.join(timeout=2)
