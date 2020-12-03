TARGET_DIR := bin
TARGET := $(TARGET_DIR)/app

KERN_SRC_DIR := $(CURDIR)/kernel
USER_SRC_DIR := $(CURDIR)/user/src

TEST_DIR := tests
TEST_SRC := test_app.py
TESTS := $(addprefix $(TEST_DIR)/, $(TEST_SRC))

USER_SRC := $(shell find $(USER_SRC_DIR) -type f -name '*.c')
KM_SRC := $(shell find $(KERN_SRC_DIR) -type f -name '*.c')
KM_OBJECTS := $(KM_SRC:%.c=%.o)

LDFLAGS = -L/lib/x86_64-linux-gnu/ -lnl-genl-3 -lnl-3
EXTRA_CFLAGS = -Wall -pedantic -g -O0 -I/usr/include/libnl3 -I$(CURDIR)/shared-headers -I$(CURDIR)/user/includes

obj-m := trng_module.o
trng_module-y := ${KM_OBJECTS}

.PHONY: all reload dirs app test
all: reload app

test: all
	@pytest $(TESTS)

module: dirs
	make -C /lib/modules/$(shell uname -r)/build M=$(KERN_SRC_DIR)

dirs:
	@mkdir -p $(TARGET_DIR)

app: dirs $(TARGET)

$(TARGET): $(USER_SRC)
	$(CC) $(EXTRA_CFLAGS) $^ $(LDFLAGS) -o $@

reload: module
	sudo rmmod trng_module || true
	sudo insmod $(KERN_SRC_DIR)/trng_module.ko

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(KERN_SRC_DIR) clean
	rm -rf $(TARGET_DIR) $(TEST_DIR)/__pycache__ $(TEST_DIR)/.pytest_cache
