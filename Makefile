# Student: Stefan Gvozdenovic
# Date: February 9, 2015
# Operating Systems CS3013 C'15 WPI
# Professor: Craig Shue
# Project3 - Semaphores and Mutexes
# Description: Phase 1

project4: project4.c
	gcc -pthread -o project4 project4.c
clean:
	rm -f *.o phase1
