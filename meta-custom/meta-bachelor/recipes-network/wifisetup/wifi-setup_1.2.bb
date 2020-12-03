SUMMARY = "Configure wifi"
LICENSE = "CLOSED"

SRC_URI += " \
	file://wpa_supplicant-wlan0.conf \
"

do_install_append() {
	install -d ${D}${sysconfdir}/wpa_supplicant
	install -m 0644 ${WORKDIR}/wpa_supplicant-wlan0.conf ${D}/${sysconfdir}/wpa_supplicant/
	install -d ${D}${sysconfdir}/systemd/system/multi-user.target.wants
	ln -s ../../../../lib/systemd/system/wpa_supplicant@.service ${D}${sysconfdir}/systemd/system/multi-user.target.wants/wpa_supplicant@wlan0.service
}
