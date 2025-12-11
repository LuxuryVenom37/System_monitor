CC = gcc
CFLAGS = -I./source -Wall
LDFLAGS = -lpthread

SRCDIR = source
BINDIR = bin
SRCS = 	$(SRCDIR)/main.c \
		$(SRCDIR)/server.c \
		$(SRCDIR)/first_client.c \
		$(SRCDIR)/second_client.c \
		$(SRCDIR)/data_collector.c \
		$(SRCDIR)/sysmon.c

TARGETS = main server first_client second_client data_collector
START_FILES = run_main run_client1 run_client2

all: $(BINDIR) $(TARGETS)
run: $(START_FILES)

$(BINDIR):
	mkdir -p $(BINDIR)

$(BINDIR)/main.o: $(SRCDIR)/main.c
	$(CC) $(CFLAGS) -c $< -o $@
$(BINDIR)/server.o: $(SRCDIR)/server.c
	$(CC) $(CFLAGS) -c $< -o $@
$(BINDIR)/first_client.o: $(SRCDIR)/first_client.c
	$(CC) $(CFLAGS) -c $< -o $@
$(BINDIR)/second_client.o: $(SRCDIR)/second_client.c
	$(CC) $(CFLAGS) -c $< -o $@
$(BINDIR)/data_collector.o: $(SRCDIR)/data_collector.c
	$(CC) $(CFLAGS) -c $< -o $@
$(BINDIR)/sysmon.o: $(SRCDIR)/sysmon.c
	$(CC) $(CFLAGS) -c $< -o $@
$(BINDIR)/termof.o: $(SRCDIR)/termof.c
	$(CC) $(CFLAGS) -c $< -o $@

main: $(BINDIR)/main.o $(BINDIR)/sysmon.o $(BINDIR)/termof.o
	$(CC) $(CFLAGS) -o $(BINDIR)/$@ $^ $(LDFLAGS)

server: $(BINDIR)/server.o $(BINDIR)/sysmon.o $(BINDIR)/termof.o
	$(CC) $(CFLAGS) -o $(BINDIR)/$@ $^ $(LDFLAGS)

first_client: $(BINDIR)/first_client.o $(BINDIR)/sysmon.o $(BINDIR)/termof.o
	$(CC) $(CFLAGS) -o $(BINDIR)/$@ $^ $(LDFLAGS)

second_client: $(BINDIR)/second_client.o $(BINDIR)/sysmon.o $(BINDIR)/termof.o
	$(CC) $(CFLAGS) -o $(BINDIR)/$@ $^ $(LDFLAGS)

data_collector: $(BINDIR)/data_collector.o $(BINDIR)/sysmon.o $(BINDIR)/termof.o
	$(CC) $(CFLAGS) -o $(BINDIR)/$@ $^ $(LDFLAGS)


clean:
	rm -rf $(BINDIR)

run_main:
	powershell.exe -Command "Start-Process wsl -ArgumentList 'bash -c \"./bin/main\"'"

run_client1:
	sleep 3
	powershell.exe -Command "Start-Process wsl -ArgumentList 'bash -c \"./bin/first_client\"'"

run_client2:
	sleep 3
	powershell.exe -Command "Start-Process wsl -ArgumentList 'bash -c \"./bin/second_client\"'"
