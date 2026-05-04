# System Architecture

This document explains the overall structure of the chat system, including modules, workflow, and IPC abstractions.

---

# 1. Overview

The system is composed of:

- **chat_server**: central message router
- **chat_client**: connects to the server and exchanges messages
- **IPC layer**: pluggable backend (UNIX sockets or POSIX message queues)
- **Message layer**: parses and serializes protocol messages
- **Utilities**: logging and error handling helpers

---

# 2. Component Diagram



