# Redis Clone in C++

## Introduction

A Redis-compatible server implementation written in modern C++. This project aims to faithfully replicate the core features and behavior of Redis, including command parsing, network concurrency, data storage, and protocol handling. Designed with a focus on performance, code clarity, and extensibility, this Redis clone serves as both a learning resource and a foundation for experimenting with advanced database and networking concepts in C++.

## Features

- **Redis Protocol Compatibility:** Parses and responds to commands using the Redis Serialization Protocol (RESP).
- **Concurrent Client Handling:** Supports multiple simultaneous client connections using efficient event-driven I/O (epoll).
- **Core Command Support:** Implements essential Redis commands such as PING, ECHO, SET, and GET (with more to come).
- **In-Memory Data Store:** Stores key-value pairs in memory for fast access and manipulation.
- **Data Expiry:** Supports setting expiration times on keys (planned).
- **Persistence:** Plans for RDB file-based persistence to disk.
- **Replication:** Roadmap includes support for master-replica synchronization and replication commands.
- **Streams and Transactions:** Future support for Redis streams and transaction commands (MULTI/EXEC/DISCARD).
- **Configurable:** Will allow configuration of listening port and other server parameters.
- **Extensible Design:** Modular codebase for easy addition of new commands and features.

## Getting Started

1. Ensure you have `cmake` and a C++17-compatible compiler installed.
2. Build the project:
   ```sh
   mkdir build && cd build
   cmake ..
   make
   ```
3. Run the server:
   ```sh
   ./server
   ```
4. Connect using `nc` or `telnet`:
   ```sh
   nc 127.0.0.1 6379
   ```

## Feature Roadmap

### Core Features

- [x] Build your own Redis
- [x] Introduction
- [x] Repository Setup
- [x] Bind to a port
- [x] Respond to PING
- [x] Respond to multiple PINGs
- [x] Handle concurrent clients
- [ ] Implement the ECHO command
- [ ] Implement the SET & GET commands
- [ ] Expiry
- [ ] RDB Persistence
- [ ] RDB file config
- [ ] Read a key
- [ ] Read a string value
- [ ] Read multiple keys
- [ ] Read multiple string values
- [ ] Read value with expiry
- [ ] Replication
- [ ] Configure listening port
- [ ] The INFO command
- [ ] The INFO command on a replica
- [ ] Initial replication ID and offset
- [ ] Send handshake (1/3)
- [ ] Send handshake (2/3)
- [ ] Send handshake (3/3)
- [ ] Receive handshake (1/2)
- [ ] Receive handshake (2/2)
- [ ] Empty RDB transfer
- [ ] Single-replica propagation
- [ ] Multi-replica propagation
- [ ] Command processing
- [ ] ACKs with no commands
- [ ] ACKs with commands
- [ ] WAIT with no replicas
- [ ] WAIT with no commands
- [ ] WAIT with multiple commands

### Streams

- [ ] The TYPE command
- [ ] Create a stream
- [ ] Validating entry IDs
- [ ] Partially auto-generated IDs
- [ ] Fully auto-generated IDs
- [ ] Query entries from stream
- [ ] Query with -
- [ ] Query with +
- [ ] Query single stream using XREAD
- [ ] Query multiple streams using XREAD
- [ ] Blocking reads
- [ ] Blocking reads without timeout
- [ ] Blocking reads using $

### Transactions

- [ ] The INCR command (1/3)
- [ ] The INCR command (2/3)
- [ ] The INCR command (3/3)
- [ ] The MULTI command
- [ ] The EXEC command
- [ ] Empty transaction
- [ ] Queueing commands
- [ ] Executing a transaction
- [ ] The DISCARD command
- [ ] Failures within transactions
- [ ] Multiple transactions

## Contributing

Contributions are welcome! Please open issues or pull requests for bug fixes, improvements, or new features. Follow the best practices outlined above and ensure all code is well-tested and documented.

## License

This project is open source and available under the MIT License.
