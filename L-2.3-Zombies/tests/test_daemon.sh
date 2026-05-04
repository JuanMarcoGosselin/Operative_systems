./process_daemon
sleep 60
# Should never see zombies
ps aux | grep defunct
killall process_daemon
