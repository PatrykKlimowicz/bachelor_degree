TARGET = app
SRC_DIR := user/src/
SOURCES = $(SRC_DIR)main.c $(SRC_DIR)trng_cli.c $(SRC_DIR)trng_nl.c
OBJECTS = $(SOURCES:.c=.o)
app_LDFLAGS = -lnl-genl-3 -lnl-3
app_CFLAGS = -Wall -g -O1 -I$(CURDIR)/user/includes -I$(CURDIR)/shared-headers

.PHONY : all clean install
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $^ $(LDFLAGS) $(app_LDFLAGS) -o $@

$(OBJECTS): %.o: %.c
	$(CC) $^ $(CFLAGS) $(app_CFLAGS) -c -o $@

clean:
	rm -fr $(TARGET) $(OBJECTS)

install: $(TARGET)
	mkdir -p $(DESTDIR)/$(BINDIR)
	install -m 0755 $< $(DESTDIR)/$(BINDIR)
