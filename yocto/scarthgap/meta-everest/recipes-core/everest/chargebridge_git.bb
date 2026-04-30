LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/Apache-2.0;md5=89aea4e17d99a7cacdbeed46a0096b10"

SRC_URI = "file://chargebridge.service \
           file://10-chargebridge.conf"

inherit systemd

SYSTEMD_SERVICE:${PN} = "chargebridge.service"

FILES:${PN} += "${systemd_system_unitdir}/everest.service.d/"

do_install() {
    if ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'true', 'false', d)}; then
        install -d ${D}${systemd_system_unitdir}
        install -m 0644 ${WORKDIR}/chargebridge.service ${D}${systemd_system_unitdir}/
        install -d ${D}${systemd_system_unitdir}/everest.service.d
        install -m 0644 ${WORKDIR}/10-chargebridge.conf ${D}${systemd_system_unitdir}/everest.service.d/
    fi
}
