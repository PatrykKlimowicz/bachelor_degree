SUMMARY = "Recipe that install files for website on apache server"
SECTION = "website"
LICENSE = "CLOSED"

SRC_URI = " \
    file://index.html \
    file://script.js \
"

inherit allarch

do_install() {
    install -d ${D}${exec_prefix}/share/apache2/default-site/htdocs/
    install -m 0644 ${WORKDIR}/index.html ${D}${exec_prefix}/share/apache2/default-site/htdocs/
    install -m 0644 ${WORKDIR}/script.js ${D}${exec_prefix}/share/apache2/default-site/htdocs/
    install -m 0644 ${WORKDIR}/style.css ${D}${exec_prefix}/share/apache2/default-site/htdocs/
}

FILES_${PN} += "${exec_prefix}/share/apache2/default-site/htdocs/index.html"
FILES_${PN} += "${exec_prefix}/share/apache2/default-site/htdocs/script.js"
FILES_${PN} += "${exec_prefix}/share/apache2/default-site/htdocs/style.css"
