#!/bin/bash
# An emergency script to terminates all running processes
# Run chmod +x emergency.sh if permission denied

# Run as ./emergency.sh

pkill -9 -P $(pgrep -x randomTester)   # Kill child processes of randomTester
pkill -9 -P $(pgrep -x main)           # Kill child processes of main
pkill -9 randomTester                  # Kill randomTester itself
pkill -9 main                          # Kill main itself
pkill -9 mpirun                        # Kill MPI process launcher
pkill -9 serial                        # Kill serial itself (if it is running)