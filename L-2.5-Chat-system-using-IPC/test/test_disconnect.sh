#!/bin/bash

echo "[TEST] Starting server..."
./chat_server &
SERVER_PID=$!
sleep 1

echo "[TEST] Starting client..."
(echo "testuser"; sleep 2) | ./chat_client &
CLIENT_PID=$!

sleep 1

echo "[TEST] Killing client abruptly..."
kill -9 $CLIENT_PID

sleep 2

echo "[TEST] Checking if server is still alive..."
if ps -p $SERVER_PID > /dev/null; then
    echo "[PASS] Server handled disconnect."
else
    echo "[FAIL] Server crashed after client force-kill."
fi

echo "[TEST] Cleaning up..."
kill -9 $SERVER_PID 2>/dev/null

echo "[PASS] Disconnect test complete."

