CC=gcc
CFLAGS=-O3 -Wall
LDFLAGS=-lpthread
SRC_DIR=./
BIN_DIR=bin/
HEADER_DIR=includes/

all: $(BIN_DIR)run

clean:
	@[ -d $(BIN_DIR) ] && echo "Cleaning." && rm -rf $(BIN_DIR) || echo "Nothing to clean."

define make_obj
	$(CC) $(CFLAGS) -o $(BIN_DIR)$(1).o -c $(SRC_DIR)$(1).c
endef

$(BIN_DIR)run: $(BIN_DIR)ServerMain.o $(BIN_DIR)Errors.o $(BIN_DIR)Common.o $(BIN_DIR)Networking.o $(BIN_DIR)Core.o $(BIN_DIR)Commands.o
	$(CC) -o $(BIN_DIR)run $(^) $(LDFLAGS)

$(BIN_DIR)ServerMain.o: $(SRC_DIR)ServerMain.c $(HEADER_DIR)Conf.h $(HEADER_DIR)Errors.h $(HEADER_DIR)Common.h $(HEADER_DIR)Structures.h $(HEADER_DIR)Commands.h
	[ ! -d bin ] && mkdir bin; true
	$(call make_obj,ServerMain)

$(BIN_DIR)Errors.o: $(SRC_DIR)Errors.c
	$(call make_obj,Errors)

$(BIN_DIR)Common.o: $(SRC_DIR)Common.c $(HEADER_DIR)Structures.h $(HEADER_DIR)Errors.h $(HEADER_DIR)Conf.h
	$(call make_obj,Common)

$(BIN_DIR)Networking.o: $(SRC_DIR)Networking.c $(HEADER_DIR)Errors.h $(HEADER_DIR)Conf.h $(HEADER_DIR)Structures.h $(HEADER_DIR)Core.h
	$(call make_obj,Networking)

$(BIN_DIR)Commands.o: $(SRC_DIR)Commands.c $(HEADER_DIR)Structures.h $(HEADER_DIR)Conf.h $(HEADER_DIR)Core.h $(HEADER_DIR)Common.h
	$(call make_obj,Commands)

$(BIN_DIR)Core.o: $(SRC_DIR)Core.c $(HEADER_DIR)Errors.h $(HEADER_DIR)Conf.h $(HEADER_DIR)Commands.h
	$(call make_obj,Core)
