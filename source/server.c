#include "sysmon.h"
#include "termof.h"

#define PORT 7531
#define QUEUE 5

int server_socket = -1;

void handler_shutdown(int signal){
	sleep(2);

	move_cursor_to(7, 12);
	set_color('r', 0);
	printf("[SERVER] Dostal som signal %d. Vypínam sa...", signal);

	if(server_socket != -1)	close(server_socket);
	detach_shared_memory();
	destroy_semaphore();

	move_cursor_to(8, 12);
	set_color('r', 0);
	printf("[SERVER] SúČasti úspešne zatvorené, končim.");
	exit(0);
}

int main(){
	signal(SIGINT, handler_shutdown);
	setvbuf(stdout, NULL, _IONBF, 0);

	if(attach_shared_memory() == -1){										//pripojenie zdielanej pamäte a získanie semaforu
		perror("!!![SERVER] Pripojenie shared memory zlyhala");				//
		exit(1);															//
	}																		//
	if(get_semaphore() == -1){												//
		perror("!!![SERVER] Získanie semaforu zlyhala");					//
		exit(1);															//
	}																		//koniec bloku

	server_socket = socket(AF_INET, SOCK_STREAM, 0);						//vytvorenie socketu a zapnutie servera
	if(server_socket == -1){												//
		perror("!!![SERVER] Vytvorenie soketu zlyhalo");					//
		exit(1);															//
	}																		//koniec bloku

	struct sockaddr_in addr;												//nastavenie adresy, portu a priradenie k socketu servera
		addr.sin_family = AF_INET;											//
		addr.sin_port = htons(PORT);										//
		addr.sin_addr.s_addr = INADDR_ANY;									//

	int reuse = 1;															//nastavenie socket option
	setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));//

	if(bind(server_socket, (struct sockaddr*)&addr, sizeof(addr)) == -1){	//
		perror("!!![SERVER] Priradenie adresy zlyhalo");					//
		close(server_socket);												//
		exit(1);															//
	}																		//koniec bloku

	if(listen(server_socket, QUEUE) == -1){									//počúvanie klientov
		perror("!!![SERVER] Nedokážem počúvať");							//
		close(server_socket);												//
		exit(1);															//
	}																		//

	move_cursor_to(8, 12);
	printf("[SERVER] Počúvam na porte %d...", PORT);						//koniec bloku

	while(1){																//nekoneČný cyklus komunikácie s klientmi
		int client_socket = accept(server_socket, NULL, NULL);					//prijatie klienta
		if(client_socket == -1){												//
			perror("!!![SERVER] Prijatie klienta zlyhalo");						//
			continue;															//
		}																		//

		semaphore_wait();													//nastavenie semafora na wait
		float cpu = shared_data->cpu_usage;										//získam dáta zo shared memory
		float total_mem = shared_data->ram_total;								//
		float available_mem = shared_data->ram_available;						//
		char unit[8];															//
		snprintf(unit, sizeof(unit), "%s", shared_data->ram_unit);				//
		semaphore_go();														//nastavenie semafora na zelenú = prístupnú

		char buffer[128];																//vytvorím si buffer na odoslanie
		snprintf(buffer, sizeof(buffer), "%f %f %f %.8s", cpu, total_mem, available_mem, unit);//

		send(client_socket, buffer, strlen(buffer), 0);									//posielam správu klientovi

		close(client_socket);															//zatváram spojenie
	}

	return 0;
}
