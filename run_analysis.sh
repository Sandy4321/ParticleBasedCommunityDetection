#!/bin/bash

# For each input graph
for i in adjnoun dolphins polbooks zachary
do 

    # Initialize test variables to default values. Change 1 variable at a time.  
    input=./results/$i
    iterations=0
    particles=`cat ./data/particles.$i`
    lambda=0.6
    delta=0.1
    epsilon=0.001
    labels=./data/labels.$i
    executions=20
	sum_acc=0;
	tmp_acc=0;
    sum_iter=0;
    tmp_iter=0;
    sum_rean=0;
    tmp_rean=0;
	sum_part=0;
    tmp_part=0;	

   # mkdir ./results/$i   

    #analyze lambda
	rm -r ./results/$i/analysis
    mkdir ./results/$i/analysis   
    for j in 0.2 0.4 0.6 0.8 1.0
    do 
        lambda=$j
        sum_acc=0;
		tmp_acc=0;
		sum_iter=0;
		tmp_iter=0;
		sum_rean=0;
		tmp_rean=0;
		sum_part=0;
		tmp_part=0;
		
		echo "Analyzing lambda=$j for graph $i"

        # read the results file for each execution        
        for (( k = 1 ; k <= $executions; k++ ))
        do 
            echo " Reading ${i}/lambda/${i}_lambda_${lambda}_run_${k}"
	        tmp_acc=`cat ./results/${i}/lambda/${i}_lambda_${lambda}_run_${k} | grep accuracy | awk -F " " '{print $2}'`
			tmp_iter=`cat ./results/${i}/lambda/${i}_lambda_${lambda}_run_${k} | grep final_no_iterations | awk -F " " '{print $2}'`
			tmp_rean=`cat ./results/${i}/lambda/${i}_lambda_${lambda}_run_${k} | grep no_reanimated_particles | awk -F " " '{print $2}'`
			tmp_part=`cat ./results/${i}/lambda/${i}_lambda_${lambda}_run_${k} | grep avg_no_particles_per_node | awk -F " " '{print $2}'`
			echo $tmp_acc
			echo $tmp_iter
			echo $tmp_rean
			echo $tmp_part

			sum_acc=$(echo "$sum_acc+$tmp_acc" | bc -l)
			sum_iter=$(echo "$sum_iter+$tmp_iter" | bc -l)
			sum_rean=$(echo "$sum_rean+$tmp_rean" | bc -l)
			sum_part=$(echo "$sum_part+$tmp_part" | bc -l)
			echo "sum_acc= $sum_acc" 
			echo "sum_iter= $sum_iter"
			echo "sum_rean= $sum_rean"
			echo "sum_part= $sum_part"
            sleep 1  # needed because otherwise the random number generator does not get properly reset
        done
		echo "scale=2;($sum_acc/$executions)" | bc -l >> ./results/${i}/analysis/${i}_lambda_accuracy
		echo "scale=2;($sum_iter/$executions)" | bc -l >> ./results/${i}/analysis/${i}_lambda_iterations
		echo "scale=2;($sum_rean/$executions)" | bc -l >> ./results/${i}/analysis/${i}_lambda_reanimated_particles
		echo "scale=2;($sum_part/$executions)" | bc -l >> ./results/${i}/analysis/${i}_lambda_particled_per_node
	done
    lambda=0.6 # reset to default
    
#<<COMMENT
    #analyze delta
      
    for j in 0.05 0.1 0.2 0.3 0.4 0.6 0.8
    do 
        delta=$j
		sum_acc=0;
        tmp_acc=0;
		sum_iter=0;
        tmp_iter=0;
        sum_rean=0;
        tmp_rean=0;
        sum_part=0;
        tmp_part=0;
        echo "Testing delta=$j for graph $i"

        # analyze each result for the specified number of runs        
        for (( k = 1 ; k <= $executions; k++ ))
        do 
            echo " Reading ${i}/delta/${i}_delta_${delta}_run_${k}"
            tmp_acc=`cat ./results/${i}/delta/${i}_delta_${delta}_run_${k} | grep accuracy | awk -F " " '{print $2}'`
			tmp_iter=`cat ./results/${i}/delta/${i}_delta_${delta}_run_${k} | grep final_no_iterations | awk -F " " '{print $2}'`
            tmp_rean=`cat ./results/${i}/delta/${i}_delta_${delta}_run_${k} | grep no_reanimated_particles | awk -F " " '{print $2}'`
            tmp_part=`cat ./results/${i}/delta/${i}_delta_${delta}_run_${k} | grep avg_no_particles_per_node | awk -F " " '{print $2}'`
            echo $tmp_acc
            echo $tmp_iter
            echo $tmp_rean
            echo $tmp_part

            sum_acc=$(echo "$sum_acc+$tmp_acc" | bc -l)
            sum_iter=$(echo "$sum_iter+$tmp_iter" | bc -l)
            sum_rean=$(echo "$sum_rean+$tmp_rean" | bc -l)
            sum_part=$(echo "$sum_part+$tmp_part" | bc -l)
            echo "sum_acc= $sum_acc"
            echo "sum_iter= $sum_iter"
            echo "sum_rean= $sum_rean"
            echo "sum_part= $sum_part"
            sleep 1  # needed because otherwise the random number generator does not get properly reset
        done
        echo "scale=2;($sum_acc/$executions)" | bc -l >> ./results/${i}/analysis/${i}_delta_accuracy
        echo "scale=2;($sum_iter/$executions)" | bc -l >> ./results/${i}/analysis/${i}_delta_iterations
        echo "scale=2;($sum_rean/$executions)" | bc -l >> ./results/${i}/analysis/${i}_delta_reanimated_particles
        echo "scale=2;($sum_part/$executions)" | bc -l >> ./results/${i}/analysis/${i}_delta_particled_per_node

    done
    delta=0.1 # reset to default


    #test epsilon

    for j in 0.01 0.001 0.0001 
    do 
        epsilon=$j
		sum_acc=0;
        tmp_acc=0;
		sum_iter=0;
        tmp_iter=0;
        sum_rean=0;
        tmp_rean=0;
        sum_part=0;
        tmp_part=0;
        echo "Testing epsilon=$j for graph $i"

        # analyze each result for the specified number of runs        
        for (( k = 1 ; k <= $executions; k++ ))
        do 
			echo " Reading ${i}/epsilon/${i}_epsilon_${epsilon}_run_${k}"
            tmp_acc=`cat ./results/${i}/epsilon/${i}_epsilon_${epsilon}_run_${k} | grep accuracy | awk -F " " '{print $2}'`
			tmp_iter=`cat ./results/${i}/epsilon/${i}_epsilon_${epsilon}_run_${k} | grep final_no_iterations | awk -F " " '{print $2}'`
            tmp_rean=`cat ./results/${i}/epsilon/${i}_epsilon_${epsilon}_run_${k} | grep no_reanimated_particles | awk -F " " '{print $2}'`
            tmp_part=`cat ./results/${i}/epsilon/${i}_epsilon_${epsilon}_run_${k} | grep avg_no_particles_per_node | awk -F " " '{print $2}'`
            echo $tmp_acc
            echo $tmp_iter
            echo $tmp_rean
            echo $tmp_part

            sum_acc=$(echo "$sum_acc+$tmp_acc" | bc -l)
            sum_iter=$(echo "$sum_iter+$tmp_iter" | bc -l)
            sum_rean=$(echo "$sum_rean+$tmp_rean" | bc -l)
            sum_part=$(echo "$sum_part+$tmp_part" | bc -l)
            echo "sum_acc= $sum_acc"
            echo "sum_iter= $sum_iter"
            echo "sum_rean= $sum_rean"
            echo "sum_part= $sum_part"
            sleep 1  # needed because otherwise the random number generator does not get properly reset
        done
        echo "scale=2;($sum_acc/$executions)" | bc -l >> ./results/${i}/analysis/${i}_epsilon_accuracy
        echo "scale=2;($sum_iter/$executions)" | bc -l >> ./results/${i}/analysis/${i}_epsilon_iterations
        echo "scale=2;($sum_rean/$executions)" | bc -l >> ./results/${i}/analysis/${i}_epsilon_reanimated_particles
        echo "scale=2;($sum_part/$executions)" | bc -l >> ./results/${i}/analysis/${i}_epsilon_particled_per_node


    done
#COMMENT
    epsilon=0.0001 # reset to default
done
