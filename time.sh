#!/bin/bash
# Run this script to get the run times for 20 times of serial, main 1, main 2, main 3, main 4 (processes)
# Run chmod +x time.sh if permission denied

# Go to serial.c and main.c and set DEBUG to 1 if you want to save time

# Run as ./time.sh <bound>, where bound is used to generated the input data

# A reasonable extreme bound is 30000

# Compiles the  programs
make

if [ $# -eq 0 ]; then
    ./datatrim
elif [ $# -eq 1 ]; then
    ./datatrim -b ${1}
else
    echo "Usage: ./time.sh <bound>"
    exit 1
fi

clear

# paste into a command files
echo "serial       p1           p2           p3           p4" > ps_out.log

echo "Start..."
ATTEMPT=0
while [[ $ATTEMPT -ne 20 ]]; do
    let ATTEMPT+=1
    echo "Attempt ${ATTEMPT} started."

    mpirun -np 1 main
    if [ $? -ne 0 ]; then
        echo "Error: main 1 failed!"
        exit 1
    fi

    # echo -n "$(tail -n 1 s_out.log) " >> ps_out.log
    # echo -n "$(tail -n 1 p_out1.log) " >> ps_out.log
    # echo -n "$(tail -n 1 p_out2.log) " >> ps_out.log
    # echo -n "$(tail -n 1 p_out3.log) " >> ps_out.log
    # echo "$(tail -n 1 p_out4.log)" >> ps_out.log

    mpirun -np 2 main
    if [ $? -ne 0 ]; then
        echo "Error: main 2 failed!"
        exit 1
    fi

    # echo -n "$(tail -n 1 s_out.log) " >> ps_out.log
    # echo -n "$(tail -n 1 p_out1.log) " >> ps_out.log
    # echo -n "$(tail -n 1 p_out2.log) " >> ps_out.log
    # echo -n "$(tail -n 1 p_out3.log) " >> ps_out.log
    # echo "$(tail -n 1 p_out4.log)" >> ps_out.log

    mpirun -np 3 main
    if [ $? -ne 0 ]; then
        echo "Error: main 3 failed!"
        exit 1
    fi

    # echo -n "$(tail -n 1 s_out.log) " >> ps_out.log
    # echo -n "$(tail -n 1 p_out1.log) " >> ps_out.log
    # echo -n "$(tail -n 1 p_out2.log) " >> ps_out.log
    # echo -n "$(tail -n 1 p_out3.log) " >> ps_out.log
    # echo "$(tail -n 1 p_out4.log)" >> ps_out.log

    mpirun -np 4 main
    if [ $? -ne 0 ]; then
        echo "Error: main 4 failed!"
        exit 1
    fi

    # echo -n "$(tail -n 1 s_out.log) " >> ps_out.log
    # echo -n "$(tail -n 1 p_out1.log) " >> ps_out.log
    # echo -n "$(tail -n 1 p_out2.log) " >> ps_out.log
    # echo -n "$(tail -n 1 p_out3.log) " >> ps_out.log
    # echo "$(tail -n 1 p_out4.log)" >> ps_out.log

    ./serial
    if [ $? -ne 0 ]; then
        echo "Error: serial failed!"
        exit 1
    fi

    #paste s_out.log p_out1.log p_out2.log p_out3.log p_out4.log >> ps_out.log

    echo -n "$(tail -n 1 s_out.log) " >> ps_out.log
    echo -n "$(tail -n 1 p_out1.log) " >> ps_out.log
    echo -n "$(tail -n 1 p_out2.log) " >> ps_out.log
    echo -n "$(tail -n 1 p_out3.log) " >> ps_out.log
    echo "$(tail -n 1 p_out4.log)" >> ps_out.log

    echo "Attemp $ATTEMPT finished"

done


