#!/bin/bash

# Build C++ server
mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
cd ..

# Install Python deps
pip install -q -r bot/requirements.txt

# Start both processes
./build/pillio &
SERVER_PID=$!

python3 bot/bot.py &
BOT_PID=$!

echo "Server PID: $SERVER_PID"
echo "Bot PID: $BOT_PID"

# Wait for both
wait $SERVER_PID $BOT_PID
