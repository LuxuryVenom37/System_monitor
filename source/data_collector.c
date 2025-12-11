#include "sysmon.h"
#include "termof.h"

#define INTERVAL_US 500000

int running = 1;

void handler_shutdown(int signal){
	sleep(1);

	move_cursor_to(5, 12);
	set_color('r', 0);
	printf("[DATA_COLLECTOR] Dostal som signál %d. Vypínam sa...", signal);
	running = 0;

	detach_shared_memory();
	destroy_semaphore();
//	pthread_exit(NULL);

	move_cursor_to(6, 12);
	printf("[DATA_COLLECTOR] VŠetky súČasti úspešne vypnuté.");
	exit(0);
}

void* cpu_usage(){
	unsigned long total_prev = 0, idle_prev = 0;

	move_cursor_to(6, 16);
	printf("[CPU_COLLECTOR] Získavam údaje o CPU...");
	while(running){
		unsigned long user, nice, system, idle, iowait, irq, softirq;
		unsigned long total;

		FILE *f = fopen("/proc/stat", "r");
		if(!f){
			perror("!!![COLL_CPU] Zlyhalo otvorenie /proc/stat");
			sleep(1);
			continue;
		}

		fscanf(f, "cpu %lu %lu %lu %lu %lu %lu %lu", &user, &nice, &system, &idle, &iowait, &irq, &softirq);
		fclose(f);
		total = user + nice + system + idle + iowait + irq + softirq;

		if(total_prev != 0){
			unsigned long diff_total = total - total_prev;
			unsigned long diff_idle = idle - idle_prev;
			float cpu_usage = (float) 100.0 * (diff_total - diff_idle) / diff_total;
//				printf("CPU: %.2f\n", cpu_usage);
			semaphore_wait();
			shared_data->cpu_usage = cpu_usage;
			semaphore_go();
		}

		total_prev = total;
		idle_prev = idle;

		usleep(INTERVAL_US);
	}

	return NULL;
}

void* ram_stats(){
	move_cursor_to(7, 16);
	printf("[RAM_COLLECTOR] Získavam údaje o RAM.");

	while(running){
		unsigned long total_mem = 0, available_mem = 0;
		char key[32];
		unsigned long value;
		char unit[8];

		FILE *f = fopen("/proc/meminfo", "r");
		if(!f){
			perror("!!![COLL_RAM] Zlyhalo otvorenie /proc/meminfo");
			sleep(1);
			continue;
		}

		while(fscanf(f, "%31s %lu %7s", key, &value, unit) == 3){
			if(strcmp(key, "MemTotal:") == 0)	total_mem = value;
			if(strcmp(key, "MemAvailable:") == 0)	available_mem = value;
		}
		fclose(f);

		semaphore_wait();
		snprintf(shared_data->ram_unit, sizeof(shared_data->ram_unit), "%s", unit);
		shared_data->ram_available = available_mem;
		shared_data->ram_total = total_mem;
		semaphore_go();
//			printf("RAM: %.2f %s\n", used_mem, unit);
		usleep(INTERVAL_US);
	}

	return NULL;
}

int main(){
	signal(SIGINT, handler_shutdown);
	setvbuf(stdout, NULL, _IONBF, 0);

	move_cursor_to(5, 12);
	printf("[DATA_COLLECTOR] Spúšťam čítanie údajov o využití systému...");

	if(attach_shared_memory() == -1){										//pripojenie zdielanej pamäte a zísaknie semafora
		perror("!!![DATA_COLLECTOR] Pripojenie shared memory zlyhala");		//
		exit(1);															//
	}																		//
	if(get_semaphore() == -1){												//
		perror("!!![DATA_COLLECTOR] Získanie semaforu zlyhala");			//
		exit(1);															//
	}																		//

	pthread_t cpu_tid, ram_tid;												//vytvorenie vlakien
	pthread_create(&cpu_tid, NULL, cpu_usage, NULL);						//
	pthread_create(&ram_tid, NULL, ram_stats, NULL);						//

	while(running){															//uspatie procesu
		pause();
	}

	pthread_join(cpu_tid, NULL);											//čakanie na vlákna
	pthread_join(ram_tid, NULL);											//
	printf("\n----------[DATA_COLLECTOR] Zastavujem... ----------\n");

	return 0;
}
