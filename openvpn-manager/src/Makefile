LDFLAGS:=-lubox -lubus -lblobmsg_json -ljson-c
CFLAGS:=-Wall
BIN:=openvpn-manager

SRC:=$(wildcard *.c)
OBJS:=$(SRC:.c=.o)

.PHONY=all clean

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) $(BIN) $(OBJS)
