----------------------------------------System monitor---------------------------------------------------------------
	Kompilácia súborov a spustenie:
		0. Pre odstránenie všetkých skompilovaných súborov a adresára bin spustite príkaz:
			make clean
		1. Na kompiláciu všetkých zdrojových súborov spustite príkaz:
			make
				-ak prebehne kompilácia správne vytvorí adresár s názvom "bin" so spustiteľnými súbormi
		2. Pre automatické spustenie zadajte príkaz:
			make run
		3. V prípade, že sa nepodarí automaticky spustiť všetky súbory, prejdite na manuálne spustenie.

	Manuálne spustenie:
		0. Pre odstránenie všetkých skompilovaných súborov a adresára bin spustite príkaz:
			make clean
		1. Na kompiláciu všetkých zdrojových súborov spustite príkaz:
			make
				-ak prebehne kompilácia správne vytvorí adresár s názvom "bin" so spustiteľnými súbormi
		2. Na spustenie hlavného programu treba z aktuálneho adresára zadať príkaz:
			./bin/main
		   		-main zabezpečí spustenie servera a zberača dát
		3. Ak sa server a zberač spustia správne (program vypíše stav), je možné sa pripojiť na server.

		UPOZORNENIE - pre správne vykreslenie treba klientov spúšťať v samostatnom terminálovom okne
		4. V novom terminálovom okne spustite príkaz:
			./bin/first_client
		5. Druhého klienta spustíte rovnakým spôsobom:
			./bin/second_client

	Použité technológie:
		Fork 	-tvorba procesov- použitý v súbore main.c na vytvorenie dvoch procesov - server a zberač dát
		Pthread -paralelné vlákna- použité v data_collector.c na vytvorenie dvoch vlákien:
					-jedno vlákno získava údaje o RAM
					-druhé vlákno získava údaje o CPU
				- vlákna sa používajú aj v klientoch - v jednom vlákne beží komunikácia so serverom a v druhom timer
		Sokety  - používané v serveri a klientoch na vzájomnú komunikáciu na lokálnej sieti
		IPC1	-Zdielaná pamäť SHM- je definovaná v súboroch sysmon.h a sysmon.c a používa sa na zdieľanie údajov
				medzi zberačom dát a serverom
		IPC2 	-Signály- signály sa využívajú v main.c, server.c, data_collector.c a v klientoch
	  	   		-Semafór- používa sa na riadenie prístupu k SHM a je definovaný v súboroch sysmon.h a sysmon.c
		Časovač - používa sa v klientoch na periodické získavanie dát zo servera a ich následný výpis

--------------------------------------by Viktor Bohunický-------------------------------------------------------------
![de:hub.de-Projekt](/obr/main_running.png)
