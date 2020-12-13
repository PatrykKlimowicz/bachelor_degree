DESCRIPTION = "Server app with pistache server and communication with kernel."
LICENSE = "MIT"

TARGET_CXXFLAGS+="-I${STAGING_INCDIR}/libnl3"
TARGET_CXXFLAGS+="-I${STAGING_INCDIR}/pistache"

inherit systemd

DEPENDS += "libnl pistache"
S = "${WORKDIR}"

LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = " \
	file://serverapp.cpp \
	file://Server.hpp \   
	file://Makefile \
	file://serverapp.service \
"

SYSTEMD_SERVICE_${PN} = "serverapp.service"
SYSTEMD_AUTO_ENABLE_${PN} = "enable"

do_install () {
	oe_runmake install DESTDIR=${D} BINDIR=${bindir} SBINDIR=${sbindir} MANDIR=${mandir} INCLUDEDIR=${includedir}

	install -d ${D}${systemd_unitdir}/system
	install -m 0644 ${WORKDIR}/serverapp.service ${D}${systemd_unitdir}/system
}

PARALLEL_MAKE = ""
BBCLASSEXTEND = "native"
