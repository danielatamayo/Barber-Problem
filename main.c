#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h> 
//#define <limits.h>
//INT_MAX

//maximum number of threads.
#define MAX_CUSTOMERS 1000 

//functions
void randwait(int secs);
void *customer(void *num);
void *barber(void *);

//SEMAPHORES
//waitingRoom # of customers entering the waiting room 
sem_t waitingRoom;
//barberChair gives exclusive access to the barber chair
sem_t barberChair;
//barberDreams allows barber to sleep until customer arrives
sem_t barberDreams;
//seatBelt makes customer to wait until barber is done cutting his/her hair
sem_t seatBelt;
//flag to stop the barber thread when all cuts done
int allDone = 0;

int main(int argc, char *argv[])
{
    pthread_t btid;
    pthread_t tid[MAX_CUSTOMERS];
    int i, x, numCustomers, numChairs; int Number[MAX_CUSTOMERS];
    
    printf("Enter number of customers\n");
    scanf("%d",&x);
    numCustomers = x;
    printf("Enter number of chairs\n");
    scanf("%d",&x);
    numChairs = x;

    //1000 maximum number of customers allowed
    if (numCustomers > MAX_CUSTOMERS) {
       printf("The maximum number of Customers is %d\n", MAX_CUSTOMERS);
       system("PAUSE");   
       return 0;
    }

    printf("THE SLEEPY BARBER PROBLEM!\n");
    
    for (i = 0; i < MAX_CUSTOMERS; i++) {
        Number[i] = i;
    }

    //initialize semaphores
    sem_init(&waitingRoom, 0, numChairs);
    sem_init(&barberChair, 0, 1);
    sem_init(&barberDreams, 0, 0);
    sem_init(&seatBelt, 0, 0);
   
    //create thread barber
    pthread_create(&btid, NULL, barber, NULL);
   
    //create thread customers.
    for (i = 0; i < numCustomers; i++) {
        pthread_create(&tid[i], NULL, customer, (void *)&Number[i]);
    }

    //join each of the threads to wait for them to finish
    for (i = 0; i < numCustomers; i++) {
        pthread_join(tid[i],NULL);
    }

    //kill barber thread when all done
    allDone = 1;
    //wake barber
    sem_post(&barberDreams);
    pthread_join(btid,NULL);

    system("PAUSE");   
    return 0;

}//end main

//get wait  number
void randwait(int secs) {
     int len = 1; 
     sleep(len);
}

void *customer(void *number) {
     int num = *(int *)number; 
     //go to shop and take some time to arrive
     printf("Customer %d leaving for barber shop\n", num);
     randwait(3);
     printf("Customer %d arrived at barber shop\n", num);
     sem_wait(&waitingRoom);

     printf("Customer %d entering waiting room\n", num); 
     //wait for the barber chair to become free
     sem_wait(&barberChair);
     //goes to free chair so spot in the waiting room is free.
     sem_post(&waitingRoom);

     printf("Customer %d waking the barber\n", num);
     sem_post(&barberDreams); 
     //wait for the barber to finish cutting once he finishes give up chair
     sem_wait(&seatBelt); 
     sem_post(&barberChair);

     printf("Customer %d leaving barber shop.\n", num);
}

void *barber(void *junk){
	while (!allDone) { 
		//no one at the shop
   		printf("The barber is sleeping\n"); 
   		sem_wait(&barberDreams);
    
   		if (!allDone){ 
    			printf("					The barber is cutting hair\n");
    			//takes random amount of time to cut customer's hair
			randwait(3);
    			printf("					The barber has finished cutting hair\n"); 
			//give up barber chair when done
    			sem_post(&seatBelt);
   		}
   		else {
   	     		printf("FINISHED! Barber is going home for the day\n");
  	 	}
	}
}

