# NixMon

Real-time Linux system monitor over TCP sockets. A lightweight server reads kernel metrics from `/proc` and streams them to connected clients, which render a live terminal dashboard.

Built as a minor project for BSE 8th Semester, Pokhara University.

## What It Does

The server runs on any Linux machine (no root needed) and collects CPU usage, memory stats, load averages, and uptime by reading `/proc/stat`, `/proc/meminfo`, `/proc/loadavg`, and `/proc/uptime`. It pushes JSON snapshots every 1–2 seconds over TCP to all connected clients. The client parses the stream and draws a live-updating TUI dashboard with bar gauges and color-coded metrics.

## Architecture

```
┌────────────┐    TCP/JSON     ┌────────────┐
│  /proc     │───────────────▶│  TUI       │
│  Server    │    (push)       │  Client    │
│  (Linux)   │◀───────────────│  (any OS)  │
└────────────┘    connect      └────────────┘
```

- **Server**: Single-threaded event loop using `poll()` for I/O multiplexing across multiple clients
- **Client**: ncurses/ANSI-based terminal dashboard with live-updating gauges
- **Protocol**: Newline-delimited JSON — debuggable with `nc` or `telnet`

## Building

### Prerequisites

- GCC or G++ (C11/C++17)
- CMake 3.16+ (or GNU Make)
- ncurses development headers (`ncurses-devel` on Fedora, `libncurses-dev` on Debian/Ubuntu)

### Compile

```bash
mkdir build && cd build
cmake ..
make
```

This produces two binaries: `nixmon-server` and `nixmon-client`.

## Usage

### Start the server

```bash
./nixmon-server -p 9090
```

The server binds to port 9090 and begins reading `/proc` metrics.

### Connect a client

```bash
./nixmon-client -h <server-ip> -p 9090
```

Press `q` to quit the client.

### Debug with netcat

```bash
nc <server-ip> 9090
```

You'll see raw JSON frames printed line by line — useful for verifying the server is working.

## Protocol

Each frame is a single JSON object terminated by a newline:

```json
{"cpu_pct":23.7,"cpu_cores":[31.2,18.4,22.1,19.8],"mem_total_kb":16384000,"mem_avail_kb":9830400,"mem_pct":40.0,"load_1m":1.23,"load_5m":0.98,"load_15m":0.76,"uptime_sec":482736,"timestamp":1707091200}
```

## Project Structure

```
nixmon/
├── src/
│   ├── server/          
│   └── client/          
├── include/             
├── docs/                
├── tests/               
├── CMakeLists.txt
├── .gitignore
├── LICENSE
└── README.md
```

## Metrics Collected

| Metric | Source | Method |
|--------|--------|--------|
| CPU % (overall + per-core) | `/proc/stat` | Two-snapshot delta of tick counts |
| RAM usage | `/proc/meminfo` | MemTotal, MemFree, MemAvailable |
| Load averages (1/5/15 min) | `/proc/loadavg` | Direct read |
| System uptime | `/proc/uptime` | Direct read |

## Limitations

- Server only runs on Linux (depends on `/proc` filesystem)
- Monitoring only — no process management or service control
- No authentication or encryption (intended for trusted networks)

## Author

**Abhishek Man Basnet** (211703)
BSE 8th Semester, Nepal College of Information Technology
Supervisor: Er. Madan Kadariya

## License

MIT
