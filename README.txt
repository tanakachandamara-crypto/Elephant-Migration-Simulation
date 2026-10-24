#TANAKA CHANDAMARA C24158553K
# ZIMPARKS Wildlife Migration Simulation (Windows Guide)

## Overview
This project is an Open MPI parallel simulation modeling the migration of elephant herds across Zimbabwe's savannah, from Hwange National Park to Victoria Falls. The simulation evaluates dynamic terrain penalties across different regions and utilizes collective communication broadcasts to track positions and determine the winning herd.

## Prerequisites for Windows
To compile and run this project natively on Windows, you must have the following installed:
1. **GCC Compiler**: e.g., MinGW-w64 (via MSYS2).
2. **Microsoft MPI (MS-MPI)**: Both the runtime (`msmpisetup.exe`) and the SDK (`msmpisdk.msi`).

## Compilation Instructions (Windows/MS-MPI)
Since Windows does not natively support the `mpicc` wrapper, you must compile the C file using `gcc` and explicitly link the MS-MPI libraries. 

Navigate to where the wildlife_migration.c file is located , once inside the folder press the ALT + D and type cmd where it is highlighted. This opens command prompt in the directory where the file is located and paste the code below
 
```cmd
gcc wildlife_migration.c -o wildlife_migration.exe -I"C:\Program Files (x86)\Microsoft SDKs\MPI\Include" -L"C:\Program Files (x86)\Microsoft SDKs\MPI\Lib\x64" -lmsmpi -lm

## Running The Program
To run the Built program you first force the open cmd window to adopt UTF-8 encoding by pasting the code below

```cmd
chcp 65001

This then activates the code page 

Then you paste the code below to run the exe file

```cmd
mpiexec -n 4 wildlife_migration.exe

You can replace 4 with any number of heards you want