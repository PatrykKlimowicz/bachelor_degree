obj-m := trng_module.o
EXTRA_CFLAGS = -I$(M)/includes/ -I$(M)/../shared-headers
trng_module-y := src/trng_module_main.o src/trng_module_nl.o src/trng_module_sim.o src/trng_module_spi.o
SRC=$(CURDIR)


.PHONY : all modules_install clean
all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) 

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c src/*.o src/*.cmd
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers
