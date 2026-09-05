#!/bin/bash

bash "/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/task1/scripts/GeneralRun.sh" naive
bash "/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/task1/scripts/GeneralRun.sh" reorder
bash "/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/task1/scripts/GeneralRun.sh" unroll
bash "/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/task1/scripts/SIMDRun.sh"
bash "/home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/task1/scripts/TileRun.sh"
python3 /home/shivam/Documents/IIT Bombay/CS 683/cs683-assignment1/task1/scripts/Process.py
