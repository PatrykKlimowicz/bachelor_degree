DESCRIPTION = "User app that send control words to kernel."
LICENSE = "MIT"

TARGET_CFLAGS += "-I${STAGING_INCDIR}/libnl3"

DEPENDS += " libnl"
S = "${WORKDIR}"

LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

FILESEXTRAPATHS_prepend := "${THISDIR}/../../../:"
FILESEXTRAPATHS_prepend := "${THISDIR}/../../../user:"

SRC_URI = " \
	file://user/includes/trng_cli.h \
	file://user/includes/trng_nl.h \
	file://shared-headers/trng_module_shared_nl.h \ 
	file://user/src/main.c \
	file://user/src/trng_cli.c \
	file://user/src/trng_nl.c \
	file://Makefile \
"


do_install () {
	oe_runmake install DESTDIR=${D} BINDIR=${bindir} SBINDIR=${sbindir} MANDIR=${mandir} INCLUDEDIR=${includedir}
}

PARALLEL_MAKE = ""
BBCLASSEXTEND = "native"

