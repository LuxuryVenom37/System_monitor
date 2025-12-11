#include "sysmon.h"
#include "termof.h"

#define PORT 7531
#define SERVER_ADDR "127.0.0.1"
#define WAIT_TIME 1

int cl_sock = -1;

void handler_shutdown(int signal){
	show_cursor();
	set_color('r', 0);
	clear_screen();
//	printf("\n----------[CLIENT_1] Dostal som signál %d. Vypínam sa...----------\n", signal);
	if(cl_sock != -1)	close(cl_sock);
	exit(0);
}

void* server_comm(){
	int signal;
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGALRM);
	int draw = 1;

	while(1){																				//komunikacia so serverom
		cl_sock = socket(AF_INET, SOCK_STREAM, 0);												//vytvorenie socketu
		sigwait(&set, &signal);
		if(cl_sock == -1){																		//
			perror("!!![CLIENT_1] Soket zlyhal");												//
			exit(1);																			//
		}																						//

		struct sockaddr_in server_addr;															//nastavenie adresy servera a portu
		server_addr.sin_family = AF_INET;														//
		server_addr.sin_port = htons(PORT);														//
		inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);									//

		if(connect(cl_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){		//pripojenie na server
			perror("!!![CLIENT_1] Pripojenie na server zlyhalo");								//
			close(cl_sock);																		//
			sleep(2);																			//
			continue;																			//
		}																						//

		char buffer[128];																		//vytvorenie bufferu a priajtie správy
		recv(cl_sock, buffer, sizeof(buffer)-1, 0);												//
		float cpu, t_mem, a_mem;
		char unit[9];
		int items = sscanf(buffer, "%f %f %f %8s", &cpu, &t_mem, &a_mem, unit);
		if(items != 4)	printf("!!![CLIENT_1] Chyba čítania prijatých dát\n");

		int row, col;
		if(draw==1){
			clear_screen();
			hide_cursor();
			get_screen_size(&row, &col);
			draw_frame(row, col);
			draw=0;
		}

		move_cursor_to(4, 8);
			printf("%15s %7.2f %%", "CPU usage:", cpu);
		move_cursor_to(5, 8);
			printf("%13s %9.0f %.2s", "Total RAM:", t_mem, unit);
		move_cursor_to(6, 8);
			printf("%13s %9.0f %.2s", "Available RAM:", a_mem, unit);

		move_cursor_to(row-2, 30);

		close(cl_sock);																			//zatvorim socket
	}																						//koniec cyklu

}

int main(){
	signal(SIGINT, handler_shutdown);
	setvbuf(stdout, NULL, _IONBF, 0);

	sigset_t set_blocked;
	sigemptyset(&set_blocked);
	sigaddset(&set_blocked, SIGALRM);
	pthread_sigmask(SIG_BLOCK, &set_blocked, NULL);

	timer_t timer;

	pthread_t client_th;
	pthread_create(&client_th, NULL, server_comm, NULL);

	timer = create_timer(SIGALRM);
	start_timer(timer, WAIT_TIME, 0);

	while(1){
		pause();
	}
	pthread_join(client_th, NULL);

	return 0;
}
