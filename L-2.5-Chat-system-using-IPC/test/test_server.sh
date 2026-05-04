#!/bin/bash
./chat_server &
echo "Server started with PID $!"
sleep 1
ps aux | grep chat_server

