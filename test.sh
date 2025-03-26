#!/bin/bash
# A Test script
# Run chmod +x test.sh if permission denied

# Run as ./test.sh program <num_process> <bound>

# A reasonable extreme bound is 50000

if [ $# -lt 1 ]; then
    echo "Usage: ./test.sh program <num_process> <bound>"
    echo "Examples:"
    echo "  ./test.sh serial"
    echo "  ./test.sh main 4"
    echo "  ./test.sh main 4 100"
    exit 1  # Standard error exit code
fi

program=$1

if [ "$program" != "serial" ] && [ "$program" != "main" ]; then
    echo "Error: Program must be 'serial' or 'main'."
    exit 1
fi

# Compile the programs
make

echo "Start..."

# Handle datatrim execution with optional bound argument
if [ $# -eq 3 ]; then
    if [ "$program" != "main" ]; then
        echo "Usage: ./test.sh main <num_process> <bound>"
        exit 1
    fi
    ./datatrim -b "${3}"
else
    ./datatrim
fi

echo "Program: ${program}"

# Run Serial Version
if [ "$program" = "serial" ]; then
    ./serial
    ./serialtester
    exit 0
fi

# Run MPI Version
if [ "$program" = "main" ]; then
    if [ $# -lt 2 ]; then
        echo "Usage: ./test.sh main <num_process>"
        exit 1
    fi

    mpirun -np "$2" ./main
    ./serialtester
    exit 0
fi
