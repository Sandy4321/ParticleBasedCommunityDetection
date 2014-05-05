#!/bin/bash


# Initialize test variables to default values. .
test_set=zachary
input=./data/in.$test_set
iterations=0
particles=`cat ./data/particles.$test_set`
lambda=0.6
delta=0.1
epsilon=0.0001
labels=./data/labels.$test_set
executions=10

make clean ; make
for (( j = 1 ; j <= 1; j++ ))
do
    ./main_driver $input $particles $lambda $delta $epsilon -l $labels -w
    sleep 1  # needed because otherwise the random number generator does not get properly reset
done

