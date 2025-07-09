PROTOC = protoc
PROTO_FILE = filechunk.proto
PROTO_C_OUT = filechunk.pb-c.c
PROTO_H_OUT = filechunk.pb-c.h

CFLAGS = -lprotobuf-c
CC = gcc

EXEC_DIR = exec

all: $(EXEC_DIR)/sender $(EXEC_DIR)/receiver

$(PROTO_C_OUT) $(PROTO_H_OUT): $(PROTO_FILE)
	$(PROTOC) --c_out=. $(PROTO_FILE)

$(EXEC_DIR):
	mkdir -p $(EXEC_DIR)

$(EXEC_DIR)/sender: sender.c $(PROTO_C_OUT) | $(EXEC_DIR)
	$(CC) sender.c $(PROTO_C_OUT) $(CFLAGS) -o $@

$(EXEC_DIR)/receiver: receiver.c $(PROTO_C_OUT) | $(EXEC_DIR)
	$(CC) receiver.c $(PROTO_C_OUT) $(CFLAGS) -o $@

clean:
	rm -f $(EXEC_DIR)/sender $(EXEC_DIR)/receiver $(PROTO_C_OUT) $(PROTO_H_OUT)
	rm -rf $(EXEC_DIR)

