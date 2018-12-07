/*
 * server daemon of Linux Easy Profiling
 * Copyright (c) 2016, Bob Liu <bo-liu@hotmail.com> 
 *
 * Licensed under GPLv2 or later.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include "jsonrpc-c.h"

static int debug; /* enable this to printf */
#define DEBUG_PRINT(fmt, args...) \
	do { if(debug) \
	printf(fmt, ## args); \
	} while(0)

#define PORT 12307  // the port users will be connecting to
#define PROC_BUFF 8192
unsigned char proc_buff[PROC_BUFF];

#define CMD_BUFF 16384
unsigned char cmd_buff[CMD_BUFF];

struct jrpc_server my_server;

unsigned char *endstring = "lepdendstring";

cJSON * say_hello(jrpc_context * ctx, cJSON * params, cJSON *id)
{
	return cJSON_CreateString("Hello!lepdendstring");
}

cJSON * read_proc(jrpc_context * ctx, cJSON * params, cJSON *id)
{
	int fd;
	int size;
	cJSON *result;
	unsigned char proc_path[50];

	if (!ctx->data)
		return NULL;

	snprintf(proc_path, 50, "/proc/%s", (char *)ctx->data);
	DEBUG_PRINT("read_proc: path: %s\n", proc_path);

	fd = open(proc_path, O_RDONLY);
	if (fd < 0) {
		printf("Open file:%s error.\n", proc_path);
		return NULL;
	}

	memset(proc_buff, 0, PROC_BUFF);
	size = read(fd, proc_buff, PROC_BUFF);
	close(fd);
	DEBUG_PRINT("read %d bytes from %s\n", size, proc_path);
	strcat(proc_buff, endstring);
	return cJSON_CreateString(proc_buff);
}


cJSON * list_all(jrpc_context * ctx, cJSON * params, cJSON *id)
{
	int i;
	memset(proc_buff, 0, PROC_BUFF);
	for (i = 0; i < my_server.procedure_count; i++) {
		strcat(proc_buff, my_server.procedures[i].name);
		strcat(proc_buff, " ");
	}
	strcat(proc_buff, endstring);
	return cJSON_CreateString(proc_buff);
}

int main(int argc, char **argv)
{
	int fd;

	debug = (argc == 2) && (!strcmp(argv[1], "--debug"));
	/*
	 * we need to dup2 stdout to pipes for sub-commands
	 * so, don't close them; but we want to mute errors
	 * just like a typical daemon
	 */
	daemon(0, 1);
	fd = open ("/dev/null", O_RDWR, 0);
	if (fd != -1)
		dup2 (fd, STDERR_FILENO);

	jrpc_server_init(&my_server, PORT);
	jrpc_register_procedure(&my_server, say_hello, "SayHello", NULL);
	jrpc_register_procedure(&my_server, list_all, "ListAllMethod", NULL);
	jrpc_register_procedure(&my_server, read_proc, "GetProcMeminfo", "meminfo");
	jrpc_register_procedure(&my_server, read_proc, "GetProcLoadavg", "loadavg");
	jrpc_register_procedure(&my_server, read_proc, "GetProcVmstat", "vmstat");
	jrpc_register_procedure(&my_server, read_proc, "GetProcZoneinfo", "zoneinfo");
	jrpc_register_procedure(&my_server, read_proc, "GetProcBuddyinfo", "buddyinfo");
	jrpc_register_procedure(&my_server, read_proc, "GetProcCpuinfo", "cpuinfo");
	jrpc_register_procedure(&my_server, read_proc, "GetProcSlabinfo", "slabinfo");
	jrpc_register_procedure(&my_server, read_proc, "GetProcSwaps", "swaps");
	jrpc_register_procedure(&my_server, read_proc, "GetProcInterrupts", "interrupts");
	jrpc_register_procedure(&my_server, read_proc, "GetProcSoftirqs", "softirqs");
	jrpc_register_procedure(&my_server, read_proc, "GetProcDiskstats", "diskstats");
	jrpc_register_procedure(&my_server, read_proc, "GetProcVersion", "version");
	jrpc_register_procedure(&my_server, read_proc, "GetProcStat", "stat");
	jrpc_register_procedure(&my_server, read_proc, "GetProcModules", "modules");

	jrpc_server_run(&my_server);
	jrpc_server_destroy(&my_server);
	
	return 0;
}
