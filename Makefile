# Student: Stefan Gvozdenovic
# Date: February 9, 2015
# Operating Systems CS3013 C'15 WPI
# Professor: Craig Shue
# Project3 - Semaphores and Mutexes
# Description: Phase 1
project_4: project_4.c
	gcc -pthread -o project_4 project_4.c
stefan: phase1.c
	gcc -pthread -o phase1 phase1.c
clean:
	rm -f *.o phase1
