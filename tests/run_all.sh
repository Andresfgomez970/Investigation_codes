#!/bin/bash

ls *.out > names
for program in *.out;
do
    echo "---------------------------------------------------------------Running program" $program
    ./$program
done
