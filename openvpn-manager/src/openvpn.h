#pragma once

#define CHUNK_SIZE 1024
#define MAX_LINE_LENGHT 200

typedef struct {
    char *name;
    char *addr;
    char *bytes_recv;
    char *bytes_sent;
    char *connected_since;
} client;

int connect_openvpn();

char *get_clients_str();
char *kill_client(char *name);

client *get_clients_obj();
void free_clients(client *clients);

int disconnect_openvpn();
