# Operating Systems and System Programming – Practical Assignment 1 (Task 1 & 2)

This project contains my solutions for **Practical Assignment 1** of the course  
**“Operating Systems and System Programming”**:

- **Task 1:** File system and settings storage  
- **Task 2:** Virtual memory and shared libraries

Environment used:
- Ubuntu (Killercoda playground)
- GCC (GNU C compiler)
- POSIX system calls and libraries

---

## 1. Project Structure

```text
~/os-assignment1/
├── pa1/          # Practical Assignment 1 – File system, VM, shared libs
│   ├── sysinfo.c       # Task 1: system info, mount points, directory listing, settings
│   ├── points.h        # Task 2: shared library header
│   ├── points.c        # Task 2: shared library implementation (mmap)
│   ├── libpoints.so    # Task 2: built shared library
│   ├── use_points.c    # Task 2: runtime loader using dlopen/dlsym
│   └── points.txt      # Task 2: generated test file with 3D points
└── pa2/          # Practical Assignment 2 – Processes, IPC, Threads (not covered here)
    ├── ipc.c
    └── threads.c
    
# Instructions
- Open terminal
- Common terminal commands used:

mkdir – create directories
cd – change directory
nano – create/edit text files
gcc – compile C programs
./program – run an executable in the current directory

Steps:
    mkdir -p ~/os-assignment1
    cd ~/os-assignment1
    mkdir pa1 pa2
    cd pa1

   nano sysinfo.c
   paste code from sysinfo.c file
   Save: Ctrl+O, Enter
   Exit: Ctrl+X
   gcc sysinfo.c -o sysinfo



Usage – Non-interactive (assignment mode)
To see OS info, hostname, time, disk info, and a directory listing:
   ./sysinfo .
  ./sysinfo /etc

This will Print:

-OS name and version
-Hostname
-Local time and system time (UTC)
-All mount points with total/free space
-Show the last viewed directory loaded from ~/.sysinfo.conf (if it exists)
-List all files and sub-folders in the specified directory with:
-permissions
-size
-last modified time
-Save the directory as last_dir in ~/.sysinfo.conf

Usage – Interactive Menu (extra feature)
./sysinfo
You will see a text menu:
-Show system info & mount points
-List a directory
-Run a shell command (mini terminal)
-Quit


Task 2 – Virtual Memory & Shared Library (points.*, use_points.c)

Instructions:
Create points.h :
-cd ~/os-assignment1/pa1
-nano points.h

Create points.c
-nano points.c

Build the shared library
-gcc -fPIC -c points.c
-gcc -shared -o libpoints.so points.o
-ls   (libpoints.so in the directory. is displayed)


