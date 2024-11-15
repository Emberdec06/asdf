CC = gcc
CFLAGS = -Wall -Wextra -O3
DEBUGFLAGS = -Wall -Wextra -g
LDFLAGS = -lwiringPi
SRC = webserver.c 
BIN = raspweb
INSTALL_DIR = /usr/local/bin
TARGET = $(INSTALL_DIR)/$(BIN)

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(BIN) $(LDFLAGS)

install: $(BIN)
	install -m 755 $(BIN) $(TARGET)

clean:
	@rm -f $(BIN)

debug: $(SRC)
	$(CC) $(DEBUGFLAGS) $(SRC) -o $(BIN) $(LDFLAGS)

clean-install: clean install

.PHONY: all install clean clean-install


