# NixMon: Real-Time System Monitor — Architecture

## Tech Stack

C/C++ · TCP/IP Sockets · poll()/select() · ncurses/ANSI · JSON Protocol · /proc Filesystem · CMake + GCC · Git

---

## Layered Architecture

### 1. Presentation Layer — TUI Client Dashboard

Connects to the server, parses incoming JSON stream, and renders live-updating system metrics into a terminal dashboard using ncurses or ANSI escape codes.

Components: TCP Client Socket, JSON Parser, ncurses Renderer, Bar Gauges, ANSI Color Engine, Connection Handler

### 2. Communication Layer — TCP/IP + I/O Multiplexing

Reliable, ordered delivery of metric snapshots over persistent TCP connections. The server uses poll()/select() to multiplex across all connected clients without spawning per-client threads.

Components: BSD Sockets API, poll()/select(), JSON Framing, Heartbeat/Keepalive, Port Binding, Non-blocking I/O

### 3. Data Source Layer — Kernel Metric Collector

A lightweight daemon running on the Linux host. Reads /proc virtual files at a configurable interval, computes deltas (e.g., CPU % from tick counts), serializes snapshots, and pushes them to all connected clients.

Components: /proc/stat, /proc/meminfo, /proc/loadavg, /proc/uptime, Delta Calculator, JSON Serializer, Client Registry

---

## Data Flow Pipeline

1. **Read /proc** — Server opens /proc/stat, /proc/meminfo, /proc/loadavg, /proc/uptime as text files every 1–2s
2. **Compute** — Raw tick counts are diffed against previous snapshot to yield CPU %. Memory values parsed to usage %.
3. **Serialize** — Metrics packed into a newline-delimited JSON payload for framing.
4. **Transmit** — Server iterates its client fd set via poll(), writing the JSON frame to each connected socket.
5. **Render** — Client reads frame, parses JSON, and redraws the ncurses dashboard — bar gauges, numbers, load graph.

---

## Component Breakdown

### Server Daemon

The core data engine. Runs unprivileged on any Linux host, binds to a configurable TCP port, and streams system snapshots to all connected clients.

- Bind → Listen → Accept loop
- poll() event loop for I/O multiplexing
- Periodic /proc reads (timer-driven)
- Client fd table management
- Graceful disconnect handling
- Signal handling (SIGINT, SIGTERM)

### Metric Collector

Parses the Linux kernel's pseudo-filesystem to extract real-time system health metrics — no third-party libraries or root access required.

- /proc/stat → per-core CPU ticks
- /proc/meminfo → Total, Free, Available
- /proc/loadavg → 1m, 5m, 15m load
- /proc/uptime → system uptime seconds
- Two-snapshot delta for CPU %
- kB → % conversion for RAM

### TUI Client

A responsive terminal dashboard that connects to a NixMon server and visualizes the incoming data stream in real time.

- TCP connect + reconnect logic
- Newline-delimited JSON reader
- ncurses window layout engine
- Color-coded bar gauges (CPU/RAM)
- Load average trend display
- Keyboard input (q to quit)

### Wire Protocol

A simple, human-readable JSON protocol with newline framing. No binary encoding — easy to debug with netcat or telnet.

- Newline-delimited JSON frames
- One frame per collection interval
- Server-push model (no client polling)
- Debuggable via nc / telnet
- Extensible key-value schema
- UTF-8 text encoding

---

## Wire Protocol Sample

### Server → Client Payload

```json
{
  "cpu_pct":       23.7,
  "cpu_cores":     [31.2, 18.4, 22.1, 19.8],
  "mem_total_kb":  16384000,
  "mem_avail_kb":  9830400,
  "mem_pct":       40.0,
  "load_1m":       1.23,
  "load_5m":       0.98,
  "load_15m":      0.76,
  "uptime_sec":    482736,
  "timestamp":     1707091200
}
```

### CPU % Calculation from /proc/stat

```
Snapshot t₀:  cpu  74250 820 18563 96900 5342 0 502 0 0 0
Snapshot t₁:  cpu  74320 820 18590 96980 5345 0 504 0 0 0

CPU% = (Δbusy / Δtotal) × 100
     = ((74320+18590+504) - (74250+18563+502))
       / ((196559) - (196377)) × 100
     ≈ 54.4%
```

---

## Concurrency Model

The server uses a **single-threaded event loop** with `poll()` to multiplex I/O across all client connections. No threads are spawned per client — the server monitors all file descriptors in one system call and writes to whichever sockets are ready.

```
┌──────────────────┐       ┌─────────────────┐
│  poll() Event    │──fd0──│  Client A        │
│  Loop            │──fd1──│  Client B        │
│  (server thread) │──fd2──│  Client C        │
└──────────────────┘       └─────────────────┘
```

---

*NixMon · Abhishek Man Basnet · BSE 8th Sem · Nepal College of Information Technology*
