#pragma once

#define DELAY_START 5

//System includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>

//RUTOS Libraries
#include <libubox/blobmsg_json.h>
#include <libubus.h>
#include <json-c/json.h>

//Source includes
#include "utils.h"
#include "config.h"
#include "ubus.h"
#include "openvpn.h"

//Globals
config conf;
