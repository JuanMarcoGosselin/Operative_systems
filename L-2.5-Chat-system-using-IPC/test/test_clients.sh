#!/bin/bash
# usage: ./test_client.sh username "message"
USERNAME=${1:-user}
MSG=${2:-"Hello everyone"}
printf "%s\n" "$USERNAME" | ./chat_client <<EOF
$MSG
/quit
EOF


