#!/bin/bash 

echo 'xterm -title "A" -hold -e "./my-router 10000 topology.txt" & sleep 1
xterm -title "B" -hold -e "./my-router 10001 topology.txt" & sleep 1
xterm -title "C" -hold -e "./my-router 10002 topology.txt" & sleep 1
xterm -title "D" -hold -e "./my-router 10003 topology.txt" & sleep 1
xterm -title "E" -hold -e "./my-router 10004 topology.txt" & sleep 1
xterm -title "F" -hold -e "./my-router 10005 topology.txt" & sleep 1' | shuf > exec
chmod 777 exec
./exec
rm exec
