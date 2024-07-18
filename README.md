# Mining Synchronization Solution
This project is a solution to an Operating System assignment focusing on thread synchronization using POSIX semaphores and pthreads. The problem simulates a mining operation involving explorers, miners, and transporters. The following guidelines and specifications are implemented in the solution:

## Problem Description
- **Explorers**: Simulated by threads that search for materials within a field, taking between 2-4 seconds to find the material. Once found, they signal the miners and continue exploring.
- **Miners**: Activated by explorers' signals, these threads extract the material, taking 2-5 seconds. After extraction, miners wait for the next signal at the extraction location.
- **Transporters**: Transport the extracted material to storage, requiring 4 seconds for a round trip. Once completed, they wait at the storage for the next signal.
  
## Features
- Each explorer, miner, and transporter is simulated by separate threads running continuously.
- Thread operations are synchronized using semaphores to ensure mutual exclusion and proper sequence of actions.
- Threads print messages to indicate their current operations (e.g., exploring, mining, transporting).
- The program gracefully handles interruption (CTRL+C) to cancel all threads and terminate the application.
  
## Implementation Details
- The solution is implemented in POSIX C, utilizing pthread for multithreading and semaphores for synchronization.
- Proper error handling for thread creation and semaphore initialization is included.
- The source code is well-commented and uses meaningful variable and function names.
  
## Requirements
- POSIX-compliant operating system (e.g., Linux).
- GCC compiler or any other POSIX-compliant C compiler.
  
## How to Run
1) Compile the program using gcc -pthread -o mining_solution mining_solution.c.
2) Run the compiled program with ./mining_solution.
3) Press CTRL+C to terminate the program.
