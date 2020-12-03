SUMMARY = "Configure network"
LICENSE = "CLOSED"

SRC_URI += " \
    file://eth0.network \
    file://wlan0.network \
"

do_install_append() {
    # Add custom systemd conf network files
    install -d ${D}${sysconfdir}/systemd/network
 
    # Add custom systemd conf network files
    install -m 0644 ${WORKDIR}/*.network ${D}${sysconfdir}/systemd/network/

}
