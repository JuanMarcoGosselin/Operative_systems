# Test Plan for Homework4 Chat System

This document provides the **explicit test plan** for verifying correctness, stability, and robustness of the chat system.  
It is complementary to `TESTING.md`, which contains results.

---

# 1. Test Objectives

1. Verify that the chat server and clients operate as expected.
2. Ensure the message protocol is implemented correctly.
3. Confirm IPC backends (UNIX sockets and POSIX message queues) behave consistently.
4. Detect race conditions and crashes under load.
5. Validate error handling and malformed message rejection.

---

# 2. Test Environment

| Component            | Version / Notes                           |
|----------------------|--------------------------------------------|
| OS                   | Linux (Ubuntu recommended)                 |
| Compiler             | gcc / clang                                |
| IPC Methods          | UNIX domain sockets, POSIX message queues  |
| Tools                | valgrind, strace, bash, tmux               |

---

# 3. Test Categories

## 3.1 Functional Tests

### FT1 — Server startup
- **Action:** Run `./chat_server`
- **Expected:**  
  - IPC initializes  
  - Server prints “Listening”  
  - No crash on idle

---

### FT2 — Client connection
- **Action:** Run `./chat_client alice`
- **Expected:**  
  - CONNECT message sent  
  - Server acknowledges  
  - Broadcasts "alice joined"

---

### FT3 — Message exchange
- **Action:** Two or more clients exchange chat messages
- **Expected:**  
  - All clients receive all messages  
  - Order preserved  
  - No drops

---

### FT4 — Client disconnect
- **Action:** Ctrl+C or exit one client
- **Expected:**  
  - Server cleans up resources  
  - Broadcast: “user left”  
  - No zombie processes  

---

### FT5 — Server shutdown
- **Action:** Terminate server manually
- **Expected:**  
  - IPC layer closes cleanly  
  - Clients exit gracefully  

---

## 3.2 Protocol Validation Tests

### PV1 — Invalid header TYPE
- **Action:** Send message with TYPE=0x99
- **Expected:**  
  - Server sends ERROR  
  - Does not crash  

---

### PV2 — Incorrect LENGTH field
- **Action:** LENGTH too small / too large  
- **Expected:**  
  - Message rejected  
  - Client gets ERROR  

---

### PV3 — Oversized payload
- **Action:** >1024 bytes  
- **Expected:**  
  - ERROR payload too large  

---

## 3.3 IPC Backend Tests

### I1 — UNIX sockets connection
### I2 — UNIX sockets broadcast
### I3 — Message queue fan-out  
- **Expected for all:** identical behavior across backends  

---

# 3.4 Stress & Concurrency Tests

### S1 — 50 simultaneous clients
- **Expected:** stable, no leak

### S2 — Rapid-fire messages (1000/sec)
- **Expected:** no crash, occasional queueing allowed

### S3 — Random disconnections
- **Expected:** server remains stable

---

# 4. Negative Tests

### N1 — Corrupted packet mid-stream  
### N2 — Inject raw binary garbage  
### N3 — Kill client process suddenly  
### N4 — Server no longer reading  
- **Expected:** system handles errors safely  

---

# 5. Acceptance Criteria

The system passes if:

- All functional and protocol tests pass
- No memory leaks under valgrind (`0 errors`)
- No crashes under stress tests
- IPC backends behave identically at the API level

---

# 6. Additional Notes

- Each test should be repeated for **both** IPC backends.
- Timing-sensitive tests should be run inside `tmux` to avoid terminal interference.
- Randomized tests should be run multiple times for reliability.

---

This test plan ensures the system is thoroughly evaluated across correctness, stability, and protocol compliance.

