# Operating Systems and System Programming – Practical Assignments 1 

This project contains my solutions for the **Operating Systems and System Programming** practical assignment 1.

Environment used:  
- Ubuntu (Killercoda playground)  
- GCC (GNU C compiler)  
- POSIX system calls and libraries

---

## 1. Project Structure

```text
~/os-assignmetnts1/
├── pa1/          # Practical Assignment 1 – File system, VM, shared libs
│   ├── sysinfo.c
│   ├── points.h
│   ├── points.c
│   ├── libpoints.so
│   ├── use_points.c
│   └── points.txt   # generated test file
└── pa2/          # Practical Assignment 2 – Processes, IPC, Threads
    ├── ipc.c
    └── threads.c
# Instructions
- Open terminal
- Comands:(mkdir- create directorys, cd- to move in folders, nano- to create file)

   mkdir -p ~/os-assignment1
   cd ~/os-assignment1
   mkdir pa1 pa2
   cd pa1
   nano sysinfo.c
   paste code from sysinfo.c file
   Save: Ctrl+O, Enter
   Exit: Ctrl+X
   gcc sysinfo.c -o sysinfo

To see OS info, hostname, time, disk info, and a directory listing:
   ./sysinfo . 
   ./sysinfo /etc



