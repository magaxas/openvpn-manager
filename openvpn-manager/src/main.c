#include "main.h"

int main(int argc, char **argv)
{
    openlog(NULL, LOG_CONS | LOG_PERROR, LOG_DAEMON);

    //TODO: find better way to do this
    syslog(LOG_INFO, "Waiting %ds before starting...", DELAY_START);
    sleep(DELAY_START);

    syslog(LOG_INFO, "Started openvpn-manager.");

    struct ubus_context *ctx = NULL;
    if (init_config(&conf, argc, argv) != 0 || 
        connect_openvpn() != 0 ||
        init_ubus(&ctx) != UBUS_STATUS_OK
    ) goto end;

end:
    free_ubus(ctx);
    disconnect_openvpn();
    syslog(LOG_INFO, "Exiting watsoniot-daemon...");
    closelog();

    return 0;
}
