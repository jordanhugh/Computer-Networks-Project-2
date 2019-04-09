#!/bin/bash

echo 'xterm -title "H" -hold -e "./my-router 10006" & sleep 1' | shuf > exec
chmod 777 exec
./exec
rm exec