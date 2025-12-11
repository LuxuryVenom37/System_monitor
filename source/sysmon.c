#include "sysmon.h"

SystemData *shared_data = NULL;
int shmid = -1;
int semid = -1;


int init_shared_memory(){
    shmid = shmget(SHM_KEY, sizeof(SystemData), IPC_CREAT | 0666);
    if(shmid == -1){
        perror("!!![SHMEM] Vytvorenie zlyhalo");
        return -1;
    }

    shared_data = (SystemData*) shmat(shmid, NULL, 0);
    if(shared_data == (void*) -1){
        perror("!!![SHMEM] Pripojenie zlyhalo");
        shared_data = NULL;
        return -1;
    }

    shared_data->cpu_usage = 0.0f;
    shared_data->ram_available = 0.0f;
    shared_data->ram_total = 0.0f;
	memset(shared_data->ram_unit, 0, sizeof(shared_data->ram_unit));

    return 0;
}

int attach_shared_memory(){
    shmid = shmget(SHM_KEY, sizeof(SystemData), 0666);
    if(shmid == -1){
        perror("!!![SHMEM] Získanie shmid zlyhalo");
        return -1;
    }

    shared_data = (SystemData*) shmat(shmid, NULL, 0);
    if(shared_data == (void*) -1){
        perror("!!![SHMEM] Pripojenie zlyhalo");
        shared_data = NULL;
        return -1;
    }

    return 0;
}

void detach_shared_memory(){
    if(shared_data != NULL){
        shmdt(shared_data);
        shared_data = NULL;
    }
}

void destroy_shared_memory(){
    if(shmid != -1){
        shmctl(shmid, IPC_RMID, NULL);
        shmid = -1;
    }
}



int init_semaphore(){
    semid = semget(SEM_KEY, 1, IPC_CREAT | 0666);
    if(semid == -1){
        perror("!!![SEM] Vytvorenie zlyhalo");
        return -1;
    }

    if(semctl(semid, 0, SETVAL, 1) == -1){
        perror("!!![SEM] Inicializácia zlyhala");
        return -1;
    }

    return 0;
}

int get_semaphore(){
    semid = semget(SEM_KEY, 1, 0666);
    if(semid == -1){
        perror("!!![SEM] Získanie semid zlyhalo");
        return -1;
    }

    return 0;
}

void semaphore_wait(){
    struct sembuf op;
    op.sem_num = 0;
    op.sem_op = -1;
    op.sem_flg = 0;

    if(semop(semid, &op, 1) == -1){
        perror("!!![SEM] Čakanie zlyhalo");
    }
}

void semaphore_go(){
    struct sembuf op;
    op.sem_num = 0;
    op.sem_op = 1;
    op.sem_flg = 0;

    if(semop(semid, &op, 1) == -1){
        perror("!!![SEM] Uvoľnenie zlyhalo");
    }
}

void destroy_semaphore(){
    if(semid != -1){
        semctl(semid, 0, IPC_RMID, 0);
        semid = -1;
    }
}



timer_t create_timer(int signal){
	struct sigevent on_signal;

	on_signal.sigev_notify = SIGEV_SIGNAL;
	on_signal.sigev_signo = signal;

	timer_t timer;

	timer_create(CLOCK_REALTIME, &on_signal, &timer);

	return timer;
}

void start_timer(timer_t timer, int wait_sec, int wait_nsec){
	 struct itimerspec timer_val;

	 timer_val.it_value.tv_sec = wait_sec;
	 timer_val.it_value.tv_nsec = wait_nsec;

	 timer_val.it_interval.tv_sec = wait_sec;
	 timer_val.it_interval.tv_nsec = wait_nsec;

	 timer_settime(timer, CLOCK_REALTIME, &timer_val, NULL);
}
