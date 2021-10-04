#pragma once

#define MAX_NAME_LENGTH 100
#define MAX_IP_LENGTH 45

typedef struct
{
    char server_name[MAX_NAME_LENGTH];
    char server_ip[MAX_IP_LENGTH];
    int server_port;
    int connected_clients_amount;
} config;

int init_config(config *conf, int argc, char **argv);
