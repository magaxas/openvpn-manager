#include "main.h"

static int socket_desc;

static int recv_all(char *buf, int timeout, bool flush)
{
    int size_recv, total_size= 0;
    struct timeval begin, now;
    char chunk[CHUNK_SIZE];
    double timediff;

    //Make socket non-blocking
    fcntl(socket_desc, F_SETFL, O_NONBLOCK);
    gettimeofday(&begin, NULL);

    while(true) {
        gettimeofday(&now, NULL);
        timediff = (now.tv_sec - begin.tv_sec) + 1e-6 * (now.tv_usec - begin.tv_usec);
        
        //If you got some data, then break after timeout
        if(total_size > 0 && timediff > timeout) {
            break;
        }
        //If you got no data at all, wait a little longer, twice the timeout
        else if(timediff > timeout*2) {
            break;
        }

        memset(chunk, 0, CHUNK_SIZE);
        if((size_recv = recv(socket_desc, chunk, CHUNK_SIZE, 0)) < 0) {
            //If nothing was received then we want to wait a little
            //before trying again, 0.1 seconds
            usleep(100000);
        }
        else if (flush) break;
        else {
            total_size += size_recv;
            syslog(LOG_DEBUG, "total: %d", total_size);
            buf = (char *) realloc(buf, total_size);
            strcat(buf, chunk);

            //Reset beginning time
            gettimeofday(&begin, NULL);
        }
    }

    return total_size;
}

int connect_openvpn()
{
    struct sockaddr_in server;

    //Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        syslog(LOG_ERR, "Could not create socket");
        return 1;
    }

    server.sin_addr.s_addr = inet_addr(conf.server_ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(conf.server_port);

    //Connect to remote server
    if (connect(socket_desc, (struct sockaddr*)&server, sizeof(server)) < 0) {
        syslog(LOG_ERR, "Socket connection error");
        return 2;
    } else {
        syslog(LOG_INFO, "Successfully connected to OpenVPN management server");
    }

    //Discard first line
    recv_all(NULL, 1, true);
    return 0;
}

static char *send_command(char *cmd)
{
    int rc = send(socket_desc, cmd, strlen(cmd)+1, 0);
    if (rc < 0) {
        syslog(LOG_WARNING, "Send failed, %s", strerror(errno));
        return NULL;
    }
    syslog(LOG_DEBUG, "Successfully sent cmd: \"%s\"", cmd);

    char *buf = (char*) calloc(CHUNK_SIZE, sizeof(char));
    if (buf == NULL) {
        syslog(LOG_WARNING, "Failed to allocate buf");
        return NULL;
    }

    int recv_bytes = recv_all(buf, 1, false);
    syslog(LOG_DEBUG, "Successfully recieved status reply: \"%s\"", buf);

    return buf;
}

static int count_clients(char *reply, bool worst_case)
{
    int clients = count_lines(reply);
    return worst_case ? (clients-7)/2 : clients;
}

char *get_clients_str()
{
    char *reply = send_command("status\n");
    int clients_count = count_clients(reply, true);

    if (reply == NULL || strlen(reply) < 1 || clients_count < 1) {
        syslog(LOG_INFO, "No reply");
        return NULL;
    }

    char *clients = (char*) calloc(
        MAX_LINE_LENGHT * clients_count,
        sizeof(char)
    );
    char *from = strstr(reply, "Connected Since\r\n") + 17;
    char *to = strstr(reply, "ROUTING TABLE\r\n");

    if (clients == NULL || from == NULL || to == NULL || to-from-1 < 1) {
        syslog(LOG_ERR, "Failed to alloc clients, from or to str");
        syslog(LOG_WARNING, "Probably invalid message recieved");
        return NULL;
    }

    memmove(clients, from, to-from-1);
    clients[to-from-1] = '\0';
    FREE(reply);

    return clients;
}

client *get_clients_obj()
{
    conf.connected_clients_amount = 0;
    char *cstr = get_clients_str();
    if (cstr == NULL) return NULL;

    conf.connected_clients_amount = count_clients(cstr, false) + 1;
    client *clients = (client*) malloc(sizeof(client) * conf.connected_clients_amount);
    if (clients == NULL) goto end;

    int i = 0;
    char *linetok = NULL;
    while((linetok = strtok_r(cstr, "\n", &cstr))) {
        int j = 0;
        char *ptr = strtok(linetok, ",");
        clients[i].name = (char*) calloc(strlen(ptr)+1, 1);
        strcpy(clients[i].name, ptr);

        while((ptr = strtok(NULL, ","))) {
            switch (j)
            {
            case 0:
                clients[i].addr = (char*) calloc(strlen(ptr)+1, 1);
                strcpy(clients[i].addr, ptr);
                break;
            case 1:
                clients[i].bytes_recv = (char*) calloc(strlen(ptr)+1, 1);
                strcpy(clients[i].bytes_recv, ptr);
                break;
            case 2:
                clients[i].bytes_sent = (char*) calloc(strlen(ptr)+1, 1);
                strcpy(clients[i].bytes_sent, ptr);
                break;
            case 3:
                clients[i].connected_since = (char*) calloc(strlen(ptr)+1, 1);
                strcpy(clients[i].connected_since, ptr);
                break;
            default:
                break;
            }
            ++j;
        }
        ++i;
    }

end:
    FREE(cstr);
    return clients;
}

void free_clients(client *clients)
{
    for (int i = 0; i < conf.connected_clients_amount; i++)
    {
        FREE(
            clients[i].addr,
            clients[i].bytes_recv,
            clients[i].bytes_sent,
            clients[i].connected_since,
            clients[i].name
        );
    }
}

char *kill_client(char *name)
{
    int cmd_length = strlen(name) + 8;
    char *cmd = (char*) calloc(cmd_length, sizeof(char));

    snprintf(cmd, cmd_length, "kill %s\n", name);
    char *reply = send_command(cmd);
    FREE(cmd);

    return reply;
}

int disconnect_openvpn()
{
    return close(socket_desc);
}
