# MPI Problems

## Overview
This repository contains two problems implemented in C using the MPI (Message Passing Interface) library. Both problems are based on parallel programming using the **Master-Slave** paradigm and **MPI_Send**/**MPI_Recv** for communication.

---

## 📂 Problem 1: Find Maximum in Array

### 💡 Description
You will develop a parallel program that outputs the maximum number in a given array.

- **Master Process**
  - Reads the size of the array and its elements.
  - Sends the assigned portion of the array to each slave process.
  - Receives the local maximum and its index from each slave.
  - Computes the global maximum and its index.
  - Prints the result.

- **Slave Processes**
  - Receive a portion of the array.
  - Calculate the local maximum and its index.
  - Send the result back to the master process.

> **Note:** The program handles the case where the array size is not divisible by the number of processes.

### Sample Output
```
Hello from master process.
Number of slave processes is 3
Enter the size of the array: 6
Enter the elements of the array:
1 4 2 8 5 3
Hello from slave#1 Max number in my partition is 4 and index is 1
Hello from slave#2 Max number in my partition is 8 and index is 0
Hello from slave#3 Max number in my partition is 5 and index is 0
Master process announce The maximum element is 8 at index 3
Thanks for using our program.
```

---

## 📂 Problem 2: Caesar Cipher Encryption

### 💡 Description
A parallel C program to implement **Caesar Cipher Encryption**. Each character is replaced by the character three positions ahead. You can **encode** or **decode** a message.

The program supports two modes:
1. Input from the console.
2. Input from a file.

- **Master Process**
  - Reads a string and the operation mode (encode/decode).
  - Scatters the string among the processes.
  - Gathers the encrypted/decrypted results from each process.
  - Prints the final result.

- **Slave Processes**
  - Receive a portion of the string.
  - Encode or decode based on the mode.
  - Send back the result.

> **Note:** The program handles the case where the string length is not divisible by the number of processes.

### Example
**Encryption**
```
Input: tutorial
Output: wxwruldo
```

**Decryption**
```
Input: wxwruldo
Output: tutorial
```

---

## 🚀 Compilation
Use the following command to compile:
```
mpicc problem1.c -o problem1
mpicc problem2.c -o problem2
```

## Running the Programs
### Problem 1
```
mpirun -np 6 ./problem1
```

### Problem 2 (Console input)
```
mpirun -np 4 ./problem2
```

### Problem 2 (File input)
You may modify the code to read from a file, or redirect input from a file:
```
mpirun -np 4 ./problem2 < input.txt
```

---

## 🛠️ Requirements

- MPI Compiler (e.g., `mpicc`)
- MPI Runtime Environment (e.g., `mpirun` / `mpiexec`)
