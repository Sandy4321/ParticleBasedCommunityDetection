#!/bin/bash

#for i in 100 1000 10000 100000 1000000
#do 
	make clean ; make
    for (( j = 1 ; j <= 50; j++ ))
    do 
		./main_driver in.ucidata-zachary 1000 2 0.6 | grep convergence  
       # ./main_driver in.ucidata-zachary 1000 2 0.6  
       # ./main_driver in.dolphins 1000 2 0.6  labels.dolphins
        sleep 1  # needed because otherwise the random number generator does not get properly reset
    done
#done
