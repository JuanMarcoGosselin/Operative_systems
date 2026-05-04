for strategy in 1 2 3; do
    echo "Testing strategy $strategy"
    ./zombie_reaper $strategy
    # Verify no zombies remain
    ps aux | grep defunct
done

