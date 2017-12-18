// bridge.c

#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "bridge.h"

//global variables so all threads can see them, critical section
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cvar[] = {PTHREAD_COND_INITIALIZER, PTHREAD_COND_INITIALIZER};
int num_cars_exit = 0;
int num_cars_bridge = 0;
int num_cars_waiting[] = {0, 0};
int traffic; //which lane of bridge is open

// helper functions for OneVehicle
void ArriveBridge(const int license,const int direction);
void OnBridge(const int license,const int direction);
void ExitBridge(const int license,const int direction);

// main
int main(int argc, char *argv[]){
  int num_cars;
  sscanf(argv[1],"%d",&num_cars);
  pthread_t cars[num_cars];
  car_t car[num_cars];
  srand(time(NULL)); //different randoms every run

  // loop to create each car, assigned random direction and speed
  for (int i=0;i<num_cars;i++){
    car[i].licenseplate = i;
    car[i].direction = rand()%2;  // assigns to possible directions between ints 0 and 1
    car[i].speed = rand()%10; // assign speeds between 9-0, 0 being fastest 9 being slowest
    //    printf("%d, %d, %d\n",car[i].licenseplate,car[i].direction,car[i].speed);
    int rc = pthread_create(&cars[i], NULL, OneVehicle,&car[i]);
    if (rc){
      printf("failed creating car");
      exit(-1);
    }
  }

  // run all cars simulatenously, wait for all threads to finish before exiting
  for (int i=0;i< num_cars;i++){
    pthread_join(cars[i], NULL);
  }

  // close bridge
  pthread_mutex_destroy(&mutex);
  printf("%d cars have exited successfully.",num_cars_exit);  
  return 0;
}

void *OneVehicle(void *vehicle){
  car_t *car = (car_t *) vehicle;
  int license = car->licenseplate;
  int direction = car->direction;
  int speed = car->speed;
  //  printf("%d, %d, %d\n",license, direction, speed);
  ArriveBridge(license, direction);
  OnBridge(license, direction);
  sleep(speed);
  ExitBridge(license,direction);
  pthread_exit(NULL);
}

// does not return until it is safe for car to get on bridge
void ArriveBridge(const int license,const int direction){
  printf("car %d waiting in direction %s\n",license, DIRECTION[direction]);
  pthread_mutex_lock(&mutex);  //acquire lock
  num_cars_waiting[direction]++;

  // first car gets to go, determines flow of traffic
  if (0== num_cars_bridge){
    traffic = direction;
  }

  // wait because car cannot go
  while ((traffic != direction) || (num_cars_bridge >= MAX_CARS)){
    pthread_cond_wait(&cvar[direction], &mutex); //wait and release lock
  }
  // has lock now
  // change condition and release lock
  //printf("car %d going in direction %s\n",license, DIRECTION[direction]);
  num_cars_waiting[direction]--;
  num_cars_bridge++;
  pthread_mutex_unlock(&mutex);
}

// print state of bridge and waiting cars
void OnBridge(const int license, const int direction){
  pthread_mutex_lock(&mutex);
  if (direction != traffic){
    printf("you have a collision!\n");
  }
  printf("car %d on bridge %s\n",license, DIRECTION[direction]);
  printf("Bridge State: %d cars on bridge %s\n",num_cars_bridge,DIRECTION[traffic]);
  printf("%d cars waiting to go to %s\n",num_cars_waiting[direction],DIRECTION[direction]);
  printf("%d cars waiting to go to %s\n",num_cars_waiting[1-direction],DIRECTION[1-direction]);
  pthread_mutex_unlock(&mutex);
}

// increments cars on bridge and cars that have exited, prevents starvation
void ExitBridge(const int license, const int direction){
  pthread_mutex_lock(&mutex);
  num_cars_bridge--;
  num_cars_exit++;
  printf("car %d went %s\n",license, DIRECTION[direction]);
  
  // prevent starvation, switch lanes after max_cars have crossed on one side
  if ((num_cars_exit%(MAX_CARS)==0)&&(num_cars_waiting[1-direction]>0)){ //max_cars and if cars waiting other direction, close the lane
    traffic = 1-direction; // block of rest of waiting cars
  }
  // if last car, open lane in opposite side
  if ((num_cars_bridge == 0)){
    if (traffic == direction){ //if num_cars < MAX_CARS, traffic will not flip so must flip at 0 no matter what 
      traffic = 1-direction;
    }
    pthread_cond_broadcast(&cvar[traffic]); // open the gate for opposing traffic after last car gets off bridge
  }

  // signal next car in lane
  pthread_cond_signal(&cvar[direction]);
  pthread_mutex_unlock(&mutex);
}


