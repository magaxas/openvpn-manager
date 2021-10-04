#include "main.h"

int init_config(config *conf, int argc, char **argv)
{
    if (argc < 3) {
        syslog(LOG_ERR, "Not enough arguments!");
        return -1;
    }

    snprintf(conf->server_name, MAX_NAME_LENGTH, "openvpn.%s", argv[1]);
    snprintf(conf->server_ip, MAX_IP_LENGTH, "%s", argv[2]);
    conf->server_port = atoi(argv[3]);
    conf->connected_clients_amount = 0;

    if (conf->server_ip == NULL || conf->server_name == NULL) {
        syslog(LOG_ERR, "Failed to set server ip or name!");
        return -1;
    }

    return 0;
}
