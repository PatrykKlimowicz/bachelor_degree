SUMMARY = "Kernel module that recives gen-net msg and respond on them."
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://src/COPYING;md5=82798f771966b3e5c39c683e1db4bdba"

inherit module
TARGET_CFLAGS+="-I${STAGING_INCDIR}/libnl3"

DEPENDS += "libnl"

FILESEXTRAPATHS_prepend := "${THISDIR}/../../../kernel:"

SRC_URI = " \
	file://includes/trng_module_nl.h \
	file://includes/trng_module_sim.h \
	file://includes/trng_module_spi.h \
	file://../shared-headers/trng_module_shared_nl.h \ 
	file://src/trng_module_main.c \
	file://src/trng_module_nl.c \
	file://src/trng_module_sim.c \
	file://src/trng_module_spi.c \
	file://Makefile \
	file://src/COPYING \
"

S = "${WORKDIR}"
