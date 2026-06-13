# Pub-Sub Messaging System

**C/C++ | Linux | UDP | Multi-threading**

---

## Overview

A lightweight publish–subscribe system implemented in C/C++ on Linux.

Built around a central **coordinator** for message routing and subscription management. Designed with efficiency and simplicity in mind, similar to lightweight system components.

---

## Features

* UDP-based messaging (low overhead IPC communication)
* Multi-threaded coordinator
* Thread-safe message queue (mutex + condition variable)
* Thread synchronization for safe concurrency
* Topic-based pub/sub

---

## Components

* **Coordinator**
* **Publisher**
* **Subscriber**

---

## Technical Highlights

* POSIX UDP sockets
* pthread / std::thread
* Producer–consumer model

---

## Build & Run

```bash
make
./coordinator
./subscriber <topic>
./publisher <topic> "message"
```

---

## Notes

* UDP for simplicity (no delivery guarantee)
* Tested on: Ubuntu 22.04 (x86_64)

---

## Future Work

* epoll
* TCP support
* message buffering
