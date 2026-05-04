#!/bin/bash

echo "[TEST] Starting server..."
./chat_server &
SERVER_PID=$!
sleep 1

echo "[TEST] Running stress test with 20 clients..."

for i in {1..20}; do
    (
        echo "user$i"
        sleep 1
        for j in {1..5}; do
            echo "Message $j from user$i"
            sleep 0.2
        done
    ) | ./chat_client &
    CLIENT_PIDS[$i]=$!
done

sleep 5

echo "[TEST] Checking if server survived..."
if ps -p $SERVER_PID > /dev/null; then
    echo "[PASS] Server is stable."
else
    echo "[FAIL] Server crashed under stress."
fi

echo "[TEST] Cleaning up..."
kill -9 $SERVER_PID
for pid in "${CLIENT_PIDS[@]}"; do kill -9 $pid 2>/dev/null; done

echo "[PASS] Stress test complete."

