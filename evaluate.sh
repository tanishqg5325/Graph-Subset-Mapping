#!/bin/bash

./$1 $2
./minisat "$2.satinput" "$2.satoutput"
./main2 $2
python resources_given/check.py "$2.graphs" "$2.mapping"
