#include "main.h"

static int get_clients(
    struct ubus_context *ctx, struct ubus_object *obj,
    struct ubus_request_data *req, const char *method,
    struct blob_attr *msg
);

static int disconnect_client(
    struct ubus_context *ctx, struct ubus_object *obj,
    struct ubus_request_data *req, const char *method,
    struct blob_attr *msg
);

enum {
    DISCONNECT_NAME,
    __DISCONNECT_MAX
};

static const struct blobmsg_policy disconnect_policy[] = {
    [DISCONNECT_NAME] = { .name = "name", .type = BLOBMSG_TYPE_STRING },
};

static const struct ubus_method manager_methods[] = {
    UBUS_METHOD_NOARG("clients", get_clients),
    UBUS_METHOD("disconnect", disconnect_client, disconnect_policy)
};

static struct ubus_object_type manager_object_type =
    UBUS_OBJECT_TYPE(conf.server_name, manager_methods);

static struct ubus_object manager_object = {
    .name = conf.server_name,
    .type = &manager_object_type,
    .methods = manager_methods,
    .n_methods = ARRAY_SIZE(manager_methods),
};

static int get_clients(struct ubus_context *ctx, struct ubus_object *obj,
    struct ubus_request_data *req, const char *method,
    struct blob_attr *msg)
{
    void *tb;
    struct blob_buf b = {};
    blob_buf_init(&b, 0);
    tb = blobmsg_open_nested(&b, "clients", true);
    client *clients = get_clients_obj();

    if (conf.connected_clients_amount > 0) {
        for (int i = 0; i < conf.connected_clients_amount; i++) {
            struct json_object *jobj;
            jobj = json_object_new_object();
            json_object_object_add(jobj, "name", json_object_new_string(clients[i].name));
            json_object_object_add(jobj, "address", json_object_new_string(clients[i].addr));
            json_object_object_add(jobj, "bytes_recieved", json_object_new_string(clients[i].bytes_recv));
            json_object_object_add(jobj, "bytes_sent", json_object_new_string(clients[i].bytes_sent));
            json_object_object_add(jobj, "connected_since", json_object_new_string(clients[i].connected_since));
            blobmsg_add_json_element(&b, "hello", jobj);
            json_object_put(jobj);
        }
    }
    else {
        blobmsg_add_string(&b, "clients", "No clients found");
    }

    blobmsg_close_table(&b, tb);
    ubus_send_reply(ctx, req, b.head);
    blob_buf_free(&b);
    FREE(clients);
    free_clients(clients);

    return 0;
}

static int disconnect_client(struct ubus_context *ctx, struct ubus_object *obj,
    struct ubus_request_data *req, const char *method,
    struct blob_attr *msg)
{
    struct blob_attr *tb[__DISCONNECT_MAX];
    struct blob_buf b = {};

    blobmsg_parse(disconnect_policy, __DISCONNECT_MAX, tb, blob_data(msg), blob_len(msg));

    if (!tb[DISCONNECT_NAME])
        return UBUS_STATUS_INVALID_ARGUMENT;

    char name[MAX_NAME_LENGTH];
    strncpy(name, blobmsg_get_string(tb[DISCONNECT_NAME]), MAX_NAME_LENGTH);

    char *reply = kill_client(name);
    if (reply == NULL || strlen(reply) < 1) return -1;

    blob_buf_init(&b, 0);
    blobmsg_add_string(&b, "disconnect", reply);
    ubus_send_reply(ctx, req, b.head);
    blob_buf_free(&b);

    FREE(reply);
    return 0;
}

int init_ubus(struct ubus_context **ctx)
{
    uloop_init();

    *ctx = ubus_connect(NULL);
    if (!(*ctx)) {
        syslog(LOG_ERR, "Failed to init ubus!");
        return 1;
    }

    ubus_add_uloop(*ctx);
    ubus_add_object(*ctx, &manager_object);
    uloop_run();

    return UBUS_STATUS_OK;
}

void free_ubus(struct ubus_context *ctx)
{
    ubus_free(ctx);
    uloop_done();
}
