#ifndef SYSMON_H
#define SYSMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include <pthread.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <time.h>

#define SHM_KEY 2206
#define SEM_KEY 2005

typedef struct {
	float cpu_usage;
	float ram_available;
	float ram_total;
	char ram_unit[8];
} SystemData;

extern SystemData *shared_data;
extern int shmid;
extern int semid;

int init_shared_memory();
int attach_shared_memory();
void detach_shared_memory();
void destroy_shared_memory();

int init_semaphore();
int get_semaphore();
void semaphore_wait();
void semaphore_go();
void destroy_semaphore();

timer_t create_timer(int signal);
void start_timer(timer_t timer, int wait_time, int wait_nsec);

#endif
