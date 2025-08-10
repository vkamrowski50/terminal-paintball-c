# Terminal Paintball (C, Sockets)

Developed by Victor Kamrowski

A 4-player, turn-based **terminal paintball** game implemented in **C** with TCP sockets.  
Multiple clients connect to a central server; players take turns choosing to **hide** (set a position 0–2) or **shoot** a specific player and position. Last player standing wins.

---

## Features
- **Client–server architecture** over TCP
- **For 4 concurrent players** with turn-based coordination
- **Commands:** `POSITION <0|1|2>`, `SHOOT <player_id> <0|1|2>`, `quit`
- Makefile and modular C code (`client.c`, `server.c`, `shared.h`)

---

## Build
```bash
make
```

This produces two binaries: `serverfile` and `clientfile`.

---

## Run (Local Test)
Use **four terminals** on the same machine.

**Terminal A — start server:**
```bash
./serverfile
```

**Terminals B–E — start clients:**
```bash
./clientfile
```

The server will accept clients until 4 are connected, then begin the turn loop.

---

## Gameplay
When prompted on a client:
- **Hide:** `POSITION 1`
- **Shoot:** `SHOOT 3 0`  (shoot player 3 at position 0)
- **Quit:** `quit`

Each player starts with 3 lives. A successful hit removes 1 life from the target. The **last player alive** wins.

---

## End & Clean
- **Quit clients:** type `quit` in each client terminal.
- **Stop server:** press `Ctrl+C` in the server terminal.
- **Clean build artifacts:**
```bash
make clean
```

---

## Project Structure
- `server.c` — Accepts clients, coordinates turns, processes commands, broadcasts updates
- `client.c` — Connects to the server, reads user input, displays server responses
- `shared.h` — Shared declarations and common constants
- `makefile` — Build targets: `make`, `make clean`

---

## Notes
- Designed for **local testing**. Default port is set in the source; ensure the port is free.
- Requires a POSIX environment with a C compiler (e.g., macOS or Linux).
- **Repository import notice:** This repository was imported in a single commit from a private development repo where it was originally created and maintained. The commit history was reset to remove unrelated school references.

---

## Skills Demonstrated
- Network programming (sockets)
- Concurrent client handling in C
- Protocol/command parsing
- Modular C design and build automation

___

## Author
Victor Kamrowski  
GitHub: [vkamrowski50](https://github.com/vkamrowski50)
