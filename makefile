CFLAGS=-O2 

main_driver:
	gcc $(CFLAGS) main_driver.c graph.c run_detection.c particle.c -o main_driver  -lm
clean:
	rm -f main_driver
