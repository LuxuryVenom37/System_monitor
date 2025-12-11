#include "sysmon.h"
#include "termof.h"

pid_t pid_server, pid_collector;

void handler_shutdown(int signal){
	int row, col;

	clear_screen();
	get_screen_size(&row, &col);
	draw_frame(row, col);

	set_color('r', 0);
	move_cursor_to(4, 8);
	printf("[MAIN] Zachytil som signál %d, vypínam System monitor...", signal);

	sleep(3);

	if(pid_collector > 0) 	kill(pid_collector, SIGINT);
	if(pid_server > 0)		kill(pid_server, SIGINT);

	while(wait(NULL) > 0);

	detach_shared_memory();
	destroy_shared_memory();
	destroy_semaphore();

	move_cursor_to(9, 8);
	set_color('r', 0);
	printf("[MAIN] Všetky procesy úspešne ukončené. Vypínam sa.");

	sleep(5);
	clear_screen();
	show_cursor();

	exit(0);
}

int main(){
	signal(SIGINT, handler_shutdown);
	setvbuf(stdout, NULL, _IONBF, 0);
	int row, col;

	clear_screen();
	hide_cursor();
	get_screen_size(&row, &col);
	draw_frame(row, col);
	set_color('r', 0);
	move_cursor_to(4, 8);
	printf("[MAIN] Zapínam System monitor...");

	if(init_shared_memory() == -1){											//inicializácia shmem a semafora
		perror("!!![MAIN] Inicializácia shared memory zlyhala");			//
		exit(1);															//
	}																		//
	if(init_semaphore() == -1){												//
		perror("!!![MAIN] Inicializácia semaforu zlyhala");					//
		exit(1);															//
	}																		//koniec bloku

	sleep(1);

	pid_collector = fork();													//vytvorenie fork() procesu pre zberač dát
	if(pid_collector < 0){													//
	  perror("!!![MAIN] Fork zlyhal");										//
	  exit(1);																//
	}																		//
	if(pid_collector == 0){													//
		execl("./bin/data_collector", "collector", NULL);//
		perror("!!![MAIN] Nepodarilo sa spustit zberač dát");				//
		exit(1);															//
	}																		//koniec bloku

	sleep(1);

	pid_server = fork();													//vytvorenie fork() procesu pre server
	if(pid_server < 0){														//
	  perror("!!![MAIN] Fork zlyhal");										//
	  exit(1);																//
	}																		//
	if(pid_server == 0){													//
		execl("./bin/server", "server", NULL);//
		perror("!!![MAIN] Nepodarilo sa spustit server");					//
		exit(1);															//
	}																		//koniec bloku

	move_cursor_to(9, 8);
	printf("[MAIN] Všetky procesy boli úspešne spustené.");
	while(1){																//cakanie na ostatne procesy
		pause();															//
	}																		//

	return 0;
}
