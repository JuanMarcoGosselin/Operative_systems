Multi-Client Chat System
Homework 4 – Operating Systems / Inter-Process Communication
This repository contains the implementation of a multi-client chat system developed in C using POSIX threads and Inter-Process Communication (IPC). A central server manages client registration, message routing, disconnections, and server-side commands. Clients communicate with the server using one of two IPC backends, selectable at build time.
The project fulfills the assignment requirements, including multi-client functionality, message broadcasting, private messaging, user listing, structured message protocol, synchronization, documentation, and automated testing.
Features
Server
Accepts and manages multiple concurrent clients.
Maintains a registry of connected users.
Broadcasts messages to all clients.
Routes private messages to specific recipients.
Detects clean and unexpected disconnections.
Logs server events, including connections, disconnections, and message routing.
Provides server-side administrative commands:
/list
/stats
/kick <username>
/broadcast <message>
/shutdown
Client
Connects to the server using the selected IPC backend.
Performs username validation and login sequence.
Sends and receives broadcast and private messages.
Provides a local command interface:
/help
/users
/msg <user> <message>
/quit
Handles server shutdown and connection loss gracefully.
IPC Backends
Two IPC mechanisms are implemented. The desired backend is selected during compilation.
UNIX Domain Sockets
Implementation: src/ipc_sockets.c
POSIX Message Queues
Implementation: src/ipc_mq.c
Build selection:
make IPC=sockets
make IPC=mq
The default backend is UNIX Domain Sockets.
Repository Structure
homework4/
├── src/
│   ├── chat_server.c
│   ├── chat_client.c
│   ├── ipc_sockets.c
│   ├── ipc_mq.c
│   ├── message.c
│   └── utils.c
├── include/
│   ├── chat.h
│   ├── message.h
│   └── ipc.h
├── tests/
│   ├── test_server.sh
│   ├── test_client.sh
│   ├── test_stress.sh
│   └── test_disconnect.sh
├── docs/
│   ├── PROTOCOL.md
│   ├── ARCHITECTURE.md
│   └── TESTING.md
├── Makefile
├── README.md
└── demo.mp4
Build Instructions
Default build (UNIX Domain Sockets)
make
Build with POSIX Message Queues
make IPC=mq
Clean artifacts
make clean
Running the System
Start the server
./bin/chat_server
Start a client
./bin/chat_client
Message Protocol
The system uses a fixed-size message_t structure containing:
Message type (JOIN, LEAVE, BROADCAST, PRIVATE, LIST_USERS, and others)
Sender username
Receiver username (for private messages)
Text payload
Timestamp
Client identifier
A full specification is included in docs/PROTOCOL.md.
Synchronization
The server uses synchronization mechanisms including:
pthread_mutex_t for protecting the shared client list
Optional sem_t semaphores for backend-specific coordination
Thread-safe operations for adding, removing, and listing clients
Signal-safe and interruption-safe system calls where applicable
Further details are found in docs/ARCHITECTURE.md.
Testing
The tests/ directory contains automated scripts to validate system behavior.
Test Script	Description
test_server.sh	Starts and validates server processes
test_client.sh	Spawns multiple client instances
test_stress.sh	Stress tests message throughput
test_disconnect.sh	Tests handling of forced client disconnections
Testing methodology and expected behaviors are documented in docs/TESTING.md.
Demonstration
A demonstration video (demo.mp4) shows representative executions, including message routing, private messaging, connection handling, and server commands.
Notes
Additional optional features, such as message persistence, encryption, or file transfers, can be added without impacting the core architecture.

Juan Marco Gosselin Gamboa ®
