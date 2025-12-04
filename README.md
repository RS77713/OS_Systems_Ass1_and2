# Operating Systems and System Programming – Practical Assignment 1 (Task 1 & 2)

This project contains my solutions for **Practical Assignment 1** of the course  
“Operating Systems and System Programming”:

- **Task 1:** File system and settings storage  
- **Task 2:** Virtual memory and shared libraries (Linux `.so` loaded with `dlopen`)

Environment used:

- Ubuntu (Killercoda playground)
- GCC (GNU C compiler)
- POSIX system calls and libraries

----------------------------------------------------------------------------------
Task 1
----------------------------------------------------------------------------------
Develop an application that displays for users the system information like the name and version of the operation system, computer name, the current system and local time, available logical drives, and their characteristics, etc.

The user can select the drive or folder and see the list of the folders and files inside with their characteristics. The application should store some settings. 
## 1. Project Structure

```text
~/os-assignment1/
├── pa1/          # Practical Assignment 1 – File system, VM, shared libs
│   ├── sysinfo.c       # Task 1: system info, mount points, directory listing, settings
│   ├── sysinfo         # compiled Task 1 binary (optional, can be rebuilt)
│   ├── points.h        # Task 2: shared library header
│   ├── points.c        # Task 2: shared library implementation (mmap)
│   ├── points.o        # object file for libpoints.so (can be rebuilt)
│   ├── libpoints.so    # Task 2: built shared library
│   ├── use_points.c    # Task 2: runtime loader using dlopen/dlsym
│   ├── use_points      # compiled Task 2 binary (optional, can be rebuilt)
│   └── points.txt      # Task 2: generated test file with 3D points
└── pa2/          # Practical Assignment 2 – Processes, IPC, Threads (not used in this assignment)
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

----------------------------------------------------------------------------------------------------------------------------
Task 2 – Virtual Memory & Shared Library
----------------------------------------------------------------------------------------------------------------------------
Develop DLL to read and process a large file (>2GB). The file should be created by the student (the example is in the table below). The file must be loaded in parts into virtual memory and processed in parts. Register the DLL in the system in the way you choose and justify your choice.

Examples of a data file

Nr.
File name
File content
Operation

1.Coordinates.dat
Text file. Each line is three integer numbers separated by spaces– the coordinates of the point x y z.
Find the coordinates of the point that is farthest from the center of the coordinate system. Return the number of the line in the file.
2. Develop an application that calls the developed DLL and demonstrates its work printing detailed information on the screen. Register the DLL in the system in the way you choose and justify your choice.


Instructions:
Create points.h 
-nano points.h  (copy code from points.h)

Create points.c
-nano points.c  (copy code from points.c)

Create points.c
-nano use_points.c  (copy code from use_points.c)

Build the shared library
-gcc -fPIC -c points.c
-gcc -shared -o libpoints.so points.o
-ls   (libpoints.so  points.c  points.h  points.o  points.txt  use_points  use_points.c in the pa1 directory is displayed)
-ls -lh points.txt
-ls -lh
Data should be displayed:
(ubuntu:~/os-assignment1/pa1$ ls -lh
total 16M
-rwxr-xr-x 1 root root  16K Dec  4 19:18 libpoints.so
-rw-r--r-- 1 root root 3.6K Dec  4 19:17 points.c
-rw-r--r-- 1 root root  450 Dec  4 19:16 points.h
-rw-r--r-- 1 root root 4.0K Dec  4 19:18 points.o
-rw-r--r-- 1 root root  16M Dec  4 19:21 points.txt
-rwxr-xr-x 1 root root  16K Dec  4 19:22 use_points
-rw-r--r-- 1 root root 1.5K Dec  4 19:17 use_points.c)

- check that the format is correct:    head points.txt 
Data should be displayed:
(ubuntu:~/os-assignment1/pa1$ head  points.txt
-7803 7560 3755
-6040 5031 -8445
8117 -5000 -9724
7037 7596 -2123
-231 3976 -2463
-7705 3616 1218
11 -3859 6355
-1270 6445 9416
-6562 7743 -1747
3784 2449 -9372)

- LD_LIBRARY_PATH=. ./use_points points.txt
Data should be displayed:
(Processing file: points.txt
Farthest point found:
  Line number: 418389
  Coordinates: (-9962, -9995, 9970)
  Distance^2 : 298542369)



Assignment 2 Task 1
