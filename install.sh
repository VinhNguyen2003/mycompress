#!/bin/bash
sudo apt-get update

sudo apt-get install -y libz-dev libzip-dev libarchive-dev

make

sudo cp mycompress /usr/local/bin

sudo chmod +x /usr/local/bin/mycompress

echo "Installation completed!"
