# Chat System Message Protocol

This document defines the message protocol used by both the chat server and the chat clients.  
All messages exchanged between processes must follow this format to guarantee interoperability.

---

## 1. Message Format

All messages use a simple, fixed-header protocol:


