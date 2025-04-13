# Distributed System: Leader Election with Heartbeat Mechanism

## Overview

This project simulates a **leader election process** in a distributed system using the **Bully Algorithm**. The algorithm elects a coordinator (leader) in a network of processes, where processes may fail (become "dead"). It also includes a **heartbeat mechanism** to periodically check for the coordinator's availability, and re-elect a new coordinator if necessary.

## Features

- **Leader Election**: Simulates the Bully Algorithm for leader election in a distributed system.
- **Fault Tolerance**: Handles the failure of processes (dead processes).
- **Heartbeat Mechanism**: Periodically checks the status of the coordinator to detect failures.
- **Concurrency**: Uses threads to simulate concurrent communication between processes.
- **Timestamped Logging**: All major actions are logged with timestamps for better clarity and debugging.

## How It Works

- The system consists of multiple processes, each with a unique ID.
- The **initiator process** starts the election by sending ELECTION messages to higher-numbered processes.
- If a process doesn't respond (because it is dead), it is ignored in the election.
- The **highest process ID** among the responding processes is elected as the new coordinator.
- A **heartbeat mechanism** is used to check if the coordinator is still alive periodically.
- If the coordinator fails, the system automatically triggers a new election.

## Prerequisites

- **C Compiler (gcc)**: This project uses the GCC compiler to build the application.
- **pthread Library**: The application uses pthreads for multithreading.

## Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/divyarupa0703/leader-with-election.git
    ```
2. Compile the code:

```bash
gcc -o election election.c -lpthread
```
3. Run the program:

```bash
./election
```
