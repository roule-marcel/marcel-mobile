#!/bin/bash

while true; do
	inotifywait .
	rsync -r . 192.168.1.1:~/marcelmobile
	echo "done"
done
