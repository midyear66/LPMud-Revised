/* LPMud web client — xterm.js terminal with WebSocket-to-telnet proxy */
(function () {
  'use strict';

  var term = new Terminal({
    cursorBlink: true,
    fontSize: 14,
    fontFamily: "'Courier New', 'Lucida Console', monospace",
    theme: {
      background: '#1a1a2e',
      foreground: '#e0e0e0',
      cursor: '#4ade80',
      selectionBackground: '#533483'
    },
    scrollback: 5000,
    convertEol: true
  });

  var fitAddon = new FitAddon.FitAddon();
  term.loadAddon(fitAddon);
  term.open(document.getElementById('terminal'));
  fitAddon.fit();

  window.addEventListener('resize', function () { fitAddon.fit(); });

  var ws = null;
  var pingInterval = null;
  var echoEnabled = true;
  var inputBuffer = '';
  var history = [];
  var historyIndex = -1;

  var reconnectEl = document.getElementById('reconnect');
  var reconnectText = document.getElementById('reconnect-text');
  var reconnectBtn = document.getElementById('reconnect-btn');

  function showReconnect(msg) {
    reconnectText.textContent = msg || 'Disconnected';
    reconnectEl.classList.add('visible');
  }

  function hideReconnect() {
    reconnectEl.classList.remove('visible');
  }

  function connect() {
    hideReconnect();
    if (pingInterval) { clearInterval(pingInterval); pingInterval = null; }

    var proto = location.protocol === 'https:' ? 'wss:' : 'ws:';
    ws = new WebSocket(proto + '//' + location.host + '/client/ws');

    ws.onopen = function () {
      ws.send(JSON.stringify({ type: 'connect' }));
      /* Heartbeat every 30s to keep the connection alive */
      pingInterval = setInterval(function () {
        if (ws && ws.readyState === WebSocket.OPEN) {
          ws.send(JSON.stringify({ type: 'ping' }));
        }
      }, 30000);
    };

    ws.onmessage = function (event) {
      var msg = JSON.parse(event.data);
      switch (msg.type) {
        case 'connected':
          term.focus();
          break;
        case 'output':
          term.write(msg.text);
          break;
        case 'echo':
          echoEnabled = msg.enabled;
          break;
        case 'disconnected':
          term.writeln('\r\n\x1b[31m--- Connection closed ---\x1b[0m');
          showReconnect('Disconnected');
          break;
        case 'error':
          term.writeln('\r\n\x1b[31m[Error] ' + msg.message + '\x1b[0m');
          showReconnect('Connection error');
          break;
      }
    };

    ws.onclose = function () {
      if (pingInterval) { clearInterval(pingInterval); pingInterval = null; }
      showReconnect('Connection lost');
    };

    ws.onerror = function () {
      if (pingInterval) { clearInterval(pingInterval); pingInterval = null; }
      showReconnect('Connection error');
    };
  }

  /* Input handling with local line buffer and command history */
  term.onData(function (data) {
    if (!ws || ws.readyState !== WebSocket.OPEN) return;

    if (data === '\r') {
      /* Enter -- send command */
      term.writeln('');
      ws.send(JSON.stringify({ type: 'input', text: inputBuffer }));
      if (inputBuffer.trim()) {
        history.push(inputBuffer);
        if (history.length > 200) history.shift();
      }
      inputBuffer = '';
      historyIndex = -1;

    } else if (data === '\x7f' || data === '\b') {
      /* Backspace */
      if (inputBuffer.length > 0) {
        inputBuffer = inputBuffer.slice(0, -1);
        if (echoEnabled) term.write('\b \b');
      }

    } else if (data === '\x1b[A') {
      /* Up arrow -- history back */
      if (!echoEnabled) return;
      if (history.length > 0) {
        var eraseLen = inputBuffer.length;
        if (eraseLen > 0) term.write('\b \b'.repeat(eraseLen));
        if (historyIndex === -1) historyIndex = history.length;
        if (historyIndex > 0) {
          historyIndex--;
          inputBuffer = history[historyIndex];
          term.write(inputBuffer);
        } else {
          inputBuffer = '';
        }
      }

    } else if (data === '\x1b[B') {
      /* Down arrow -- history forward */
      if (!echoEnabled) return;
      if (historyIndex !== -1) {
        var eraseLen2 = inputBuffer.length;
        if (eraseLen2 > 0) term.write('\b \b'.repeat(eraseLen2));
        historyIndex++;
        if (historyIndex >= history.length) {
          historyIndex = -1;
          inputBuffer = '';
        } else {
          inputBuffer = history[historyIndex];
          term.write(inputBuffer);
        }
      }

    } else if (data.charCodeAt(0) >= 32) {
      /* Printable character */
      inputBuffer += data;
      if (echoEnabled) term.write(data);
    }
  });

  reconnectBtn.addEventListener('click', function () {
    term.clear();
    connect();
  });

  /* Auto-connect on load */
  connect();
})();
