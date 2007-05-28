#!/bin/sh

./gen-diphones.py | sort | uniq -c | sort -n | tail -500 | cut -b 8- | sort> frequent-clusters
./strip.py > selected-clusters
