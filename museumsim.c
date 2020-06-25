//Jesse Thomas
//Project 2 - Dr. Stephen Lee

#include "sem.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <time.h>
#include "unistd.h"

void down(struct cs1550_sem *sem) {
  syscall(__NR_cs1550_down, sem);
}

void up(struct cs1550_sem *sem) {
  syscall(__NR_cs1550_up, sem);
}

struct sharedVar{
	//Semaphore for Visitor waiting for Guide
	struct cs1550_sem *vG;
	//Semaphore for Visitor waiting for Museum to Open
	struct cs1550_sem *vM;
	//Semaphore for Guide waiting for Visitor
	struct cs1550_sem *gV;
	//Semaphore for Guide waiting for Guide to Leave
	struct cs1550_sem *gG;
	//Semaphore for maxTourGuides in museum currently
	struct cs1550_sem *maxTourGuide;
	//Semaphore for max visitors allowed in museum, increase by 10 everytime a tour guide enters museum
	struct cs1550_sem *visitorMax;
	//Semaphore to protect shared data
	struct cs1550_sem *protect;
	//Semaphore for guide to sleep inside museum
	struct cs1550_sem *guideSleep;

	//Boolean for if museum is open
	bool museumOpen;

	//Integers for total visitors/guides, command line args, and leaving visitor/guide counts
	int vTotal, gTotal, m, k, pv, dv, sv, pg, dg, sg, vCount, gCount, vCount2;

	//For storing current time
	struct timeval tv;

};

struct sharedVar *sv;

void init(){
	//Initialize sharedVar struct
	sv = mmap(NULL, sizeof(struct sharedVar), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, 0, 0);

	//Initialize semaphores
	sv->vG = mmap(NULL,sizeof(struct cs1550_sem),
      PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, 0, 0);
	sv->vM = mmap(NULL,sizeof(struct cs1550_sem),
      PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, 0, 0);
	sv->gG = mmap(NULL,sizeof(struct cs1550_sem),
      PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, 0, 0);
	sv->gV = mmap(NULL,sizeof(struct cs1550_sem),
      PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, 0, 0);
	sv->maxTourGuide = mmap(NULL,sizeof(struct cs1550_sem),
      PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, 0, 0);
	sv->visitorMax = mmap(NULL,sizeof(struct cs1550_sem),
      PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, 0, 0);
	sv->protect = mmap(NULL,sizeof(struct cs1550_sem),
      PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, 0, 0);
	sv->guideSleep = mmap(NULL,sizeof(struct cs1550_sem),
      PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, 0, 0);

	//Initialize booleans and counts and current time
	sv->museumOpen = false;
	sv->vTotal = -1;
	sv->gTotal = -1;
	sv->vCount = 0;
	sv->vCount2 = -1;
	sv->gCount = -1;
	gettimeofday(&sv->tv,NULL);

	//Initialize the semaphores
	sv->vG->value = 0;
	sv->vM->value = 0;
	sv->gV->value = 0;
	sv->gG->value = 0;
	sv->maxTourGuide->value = 2;
	sv->visitorMax->value = 0;
	sv->protect->value = 1;
	sv->guideSleep->value = 0;
}

//return elapsed time whenever called
int getTime(){
	struct timeval tv1;
	gettimeofday(&tv1,NULL);
	return (int) ((((tv1.tv_sec - sv->tv.tv_sec) * 1000000) + (tv1.tv_usec - sv->tv.tv_usec)) / 1000000);
}

void tourGuideLeaves(){
	down(sv->protect);
	sv->gCount++;
	up(sv->protect);
	printf("Tour guide %d leaves the museum at time %d.", sv->gCount, getTime());
	printf("\n");
	fflush(stdout);
	up(sv->maxTourGuide);
	down(sv->protect);
	if(sv->gCount == sv->k - 1){
		fprintf(stderr,"The museum is now empty.\n");
		up(sv->protect);
		return;
	}
	//decrease visitor max by 10
	int i;
	for(i = 0; i < 10; i++){
		down(sv->visitorMax);
	}
	//if a waiting guide is sleeping, wake them up
	if(sv->gG->value < 0){
		up(sv->gG);
	}
	if(sv->guideSleep->value == -1 && sv->visitorMax->value == 10){
		up(sv->guideSleep);
	}
	else{ //if both guides are now gone, the museum is empty
		if(sv->maxTourGuide->value == 2){
			fprintf(stderr,"The museum is now empty.");
			printf("\n");
			sv->museumOpen = false;
		}
	}
	up(sv->protect);
	return;
}

void visitorLeaves(){
	printf("Visitor %d leaves the museum at time %d.",sv->vCount, getTime());
	printf("\n");
	fflush(stdout);
	down(sv->protect);
	up(sv->visitorMax);
	sv->vCount++;
	//If a guide is inside the museum but this visitor is the last to leave, wake guide
	if(sv->guideSleep->value == -2 && sv->visitorMax->value == 20){
		up(sv->guideSleep);
		up(sv->guideSleep);
	}
	else if(sv->guideSleep->value == -2 && sv->visitorMax->value == 10){
		up(sv->guideSleep);
	}
	else if(sv->guideSleep->value == -1 && sv->visitorMax->value == 10){
		up(sv->guideSleep);
	}
	up(sv->protect);
	return;
}

void tourMuseum(){
	down(sv->protect);
	sv->vCount2++;
	up(sv->protect);
	printf("Visitor %d tours museum at time %d.", sv->vCount2, getTime());
	printf("\n");
	fflush(stdout);
	sleep(2);
	return;
}

void openMuseum(){
	printf("Tour guide %d opens museum for tours at time %d.", sv->gTotal, getTime());
	printf("\n");
	fflush(stdout);
	down(sv->protect);
	sv->museumOpen = true;
	//if a visitor is waiting for museum to open, wake them
	if(sv->vM->value < 0){
		while(sv->vM->value < 0){
			up(sv->vM);
		}
	}
	up(sv->protect);
	down(sv->guideSleep);
	return;
}

void vistorArrives(){
	down(sv->protect);
	sv->vTotal++;
	up(sv->protect);
	printf("Visitor %d arrives at time %d.", sv->vTotal, getTime());
	printf("\n");
	fflush(stdout);
	down(sv->protect);
	int i = 0;
	//if museum is closed, wait for it to open
	if(sv->museumOpen == false){
		i++;
		up(sv->protect);
		down(sv->vM);
	}
	//if there are too many visitors, wait for another guide
	if(sv->visitorMax->value <= 0){
		i++;
		up(sv->protect);
		down(sv->vG);
	}
	//if a guide is waiting for you, wake them
	if(sv->gV->value < 0){
		up(sv->gV);
	}
	if(i == 0){
		up(sv->protect);
	}
	down(sv->visitorMax);
	return;
}

void tourguideArrives(){
	down(sv->protect);
	sv->gTotal++;
	up(sv->protect);
	printf("Tour guide %d arrives at time %d.", sv->gTotal, getTime());
	printf("\n");
	fflush(stdout);
	int k = 0;
	down(sv->protect);
	//if museum is closed, open it
	if(sv->museumOpen == false){
		up(sv->protect);
		down(sv->maxTourGuide);
		//add 10 to visitorMax semaphore to allow 10 visitors to enter
		int i;
		for(i = 0; i < 10; i++){
			up(sv->visitorMax);
			if(sv->vG->value < 0){
				up(sv->vG);
			}
		}
		return;
	}
	//if there are too many guides, wait for one to leave
	if(sv->maxTourGuide->value <= 0){
		k++;
		up(sv->protect);
		down(sv->gG);
	}
	//if a visitor is waiting for you, wake them
	else if(sv->vG->value < 0){
		int i;
		for(i = 0; i < 10; i++){
			if(sv->vG->value < 0){
				up(sv->vG);
			}
		}
	}
	//if a visitor isn't waiting for you, wait for one
	else if(sv->vG->value >= 0){
		k++;
		up(sv->protect);
		down(sv->gV);
	}
	if(k == 0){
		up(sv->protect);
	}
	int i;
	for(i = 0; i < 10; i++){
		up(sv->visitorMax);
	}
	down(sv->maxTourGuide);
	return;
} 

void createVisitorProcess(){
	//Seed for random
	srand(sv->sv);
	//For all given visitors m, create a visitor process
	int i;
	for(i = 0; i < sv->m; i++){
		int pid2 = fork();
		if(pid2 == 0){
			vistorArrives();
			tourMuseum();
			visitorLeaves();
			exit(0);
		}
		else{
			int value = rand() % 100 + 1; //random number between 1-100
			if(value > sv->pv){
				sleep(sv->dv);
			}
		}
	}
	for(i = 0; i < sv->m; i++){
		wait(NULL);
	}
	return;
}

void createGuideProcess(){
	//seed for random
	srand(sv->sg);
	//For all given guides k, create a guide process
	int i;
	for(i = 0; i < sv->k; i++){
		int pid2 = fork();
		if(pid2 == 0){
			tourguideArrives();
			openMuseum();
			tourGuideLeaves();
			exit(0);
		}
		int value = rand() % 100 + 1; //random number between 1-100
		if(value > sv->pg){
			sleep(sv->dg);
		}
	}
	for(i = 0; i < sv->m; i++){
		wait(NULL);
	}
	return;
}

int main(int argc, char* argv[]){

	printf("The museum is now empty.");
	printf("\n");
	fflush(stdout);

	//Initialize struct and shared variables
	init();

	//Read and store command line arguments in shared variables struct
	int i;
	for(i = 0; i < argc; i++){
		if(strcmp(argv[i],"-m") == 0){
			sv->m = atoi(argv[i+1]);
			i++;
		}
		else if(strcmp(argv[i],"-k") == 0){
			sv->k = atoi(argv[i+1]);
			i++;
		}
		else if(strcmp(argv[i],"-pv") == 0){
			sv->pv = atoi(argv[i+1]);
			i++;
		}
		else if(strcmp(argv[i],"-dv") == 0){
			sv->dv = atoi(argv[i+1]);
			i++;
		}
		else if(strcmp(argv[i],"-sv") == 0){
			sv->sv = atoi(argv[i+1]);
			i++;
		}
		else if(strcmp(argv[i],"-pg") == 0){
			sv->pg = atoi(argv[i+1]);
			i++;
		}
		else if(strcmp(argv[i],"-dg") == 0){
			sv->dg = atoi(argv[i+1]);
			i++;
		}
		else if(strcmp(argv[i],"-sg") == 0){
			sv->sg = atoi(argv[i+1]);
			i++;
		}
	}

	//create first visitor
	int pid = fork();

	if(pid == 0){
		createVisitorProcess();
		exit(0);
	} 
	else{
		createGuideProcess();
		exit(0);
	}
	wait(NULL);
	return 0;
}
