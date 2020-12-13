SUMMARY = "Recipe that install files for website on apache server"
SECTION = "website"
LICENSE = "CLOSED"

SRC_URI = " \
    file://website.html \
    file://style.css \
    file://config.js \
    file://helper.js \
    file://controller.js \
    file://LEDView.js \
    file://model.js \
    file://RandomView.js \
    file://LEDView.js \
"

inherit allarch

do_install() {
    install -d ${D}${exec_prefix}/share/apache2/default-site/htdocs/
    install -m 0644 ${WORKDIR}/website.html ${D}${exec_prefix}/share/apache2/default-site/htdocs/
    install -m 0644 ${WORKDIR}/*.js ${D}${exec_prefix}/share/apache2/default-site/htdocs/
    install -m 0644 ${WORKDIR}/style.css ${D}${exec_prefix}/share/apache2/default-site/htdocs/
}

FILES_${PN} += "${exec_prefix}/share/apache2/default-site/htdocs/website.html"
FILES_${PN} += "${exec_prefix}/share/apache2/default-site/htdocs/style.css"
FILES_${PN} += "${exec_prefix}/share/apache2/default-site/htdocs/config.js"
FILES_${PN} += "${exec_prefix}/share/apache2/default-site/htdocs/helper.js"
FILES_${PN} += "${exec_prefix}/share/apache2/default-site/htdocs/controller.js"
FILES_${PN} += "${exec_prefix}/share/apache2/default-site/htdocs/model.js"
FILES_${PN} += "${exec_prefix}/share/apache2/default-site/htdocs/LEDView.js"
FILES_${PN} += "${exec_prefix}/share/apache2/default-site/htdocs/RandomView.js"
