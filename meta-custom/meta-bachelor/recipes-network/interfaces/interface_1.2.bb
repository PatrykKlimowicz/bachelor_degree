DESCRIPTION = "Add interfaces file to /etc/network which allows connect to WiFi automatically"                                          
                                                                                 
LICENSE = "MIT"                                                                  
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI += " \
    file://interfaces \
"

inherit systemd allarch
                                                                         
do_install () {
	install -d ${D}${sysconfdir}/network
	install -m 0644 ${WORKDIR}/interfaces ${D}${sysconfdir}/network
} 

FILES_${PN} += "${sysconfdir}/network"
