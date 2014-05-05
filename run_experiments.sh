#!/bin/bash


# Recompile
make clean
make

rm -r ./results
# Results will go here
mkdir results

# For each input graph
for i in adjnoun dolphins polbooks zachary
do 

    # Initialize test variables to default values. Change 1 variable at a time.  
    input=../data/in.$i
    iterations=0
    particles=`cat ../data/particles.$i`
    lambda=0.6
    delta=0.1
    epsilon=0.0001
    labels=../data/labels.$i
    executions=20

    mkdir ./results/$i   

    #test lambda
    mkdir ./results/$i/lambda   
    for j in 0.2 0.4 0.6 0.8 1.0
    do 
        lambda=$j
        echo "Testing lambda=$j for graph $i"

        # run each test the pecified number of times        
        for (( k = 1 ; k <= $executions; k++ ))
        do 
            echo "  ./main_driver $input $particles $lambda $delta $epsilon -l $labels"
            ./main_driver $input $particles $lambda $delta $epsilon -l $labels > ./results/${i}/lambda/${i}_lambda_${lambda}_run_${k}
            sleep 1  # needed because otherwise the random number generator does not get properly reset
        done
    done
    lambda=0.6 # reset to default
    

    #test delta
    mkdir ./results/$i/delta   
    for j in 0.05 0.1 0.2 0.3 0.4 0.6 0.8
    do 
        delta=$j
        echo "Testing delta=$j for graph $i"

        # run each test the pecified number of times        
        for (( k = 1 ; k <= $executions; k++ ))
        do 
            echo "  ./main_driver $input $particles $lambda $delta $epsilon -l $labels"
            ./main_driver $input $particles $lambda $delta $epsilon -l $labels > ./results/${i}/delta/${i}_delta_${delta}_run_${k}
            sleep 1  # needed because otherwise the random number generator does not get properly reset
        done
    done
    delta=0.1 # reset to default


    #test epsilon
    mkdir ./results/$i/epsilon   
    for j in 0.01 0.001 0.0001 0.00001
    do 
        epsilon=$j
        echo "Testing epsilon=$j for graph $i"

        # run each test the pecified number of times        
        for (( k = 1 ; k <= $executions; k++ ))
        do 
            echo "  ./main_driver $input $particles $lambda $delta $epsilon -l $labels"
            ./main_driver $input $particles $lambda $delta $epsilon -l $labels > ./results/${i}/epsilon/${i}_epsilon_${epsilon}_run_${k}
            sleep 1  # needed because otherwise the random number generator does not get properly reset
        done
    done
    epsilon=0.0001 # reset to default
    
done

