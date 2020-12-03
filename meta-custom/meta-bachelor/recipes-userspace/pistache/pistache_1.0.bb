SUMMARY = "A high-performance REST Toolkit written in C++ "
HOMEPAGE = "https://github.com/oktal/pistache"
LICENSE="MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"
DEPENDS = "gtest"

SRCREV = "4355267e20a912992006b2ca914147a8cb49c3fa" 
SRC_URI = "git://github.com/oktal/pistache.git"
 
S = "${WORKDIR}/git"
 
inherit cmake
 
BBCLASSEXTEND = "native nativesdk"
 
FILES_${PN} += "${libdir}/* ${includedir}"
 


