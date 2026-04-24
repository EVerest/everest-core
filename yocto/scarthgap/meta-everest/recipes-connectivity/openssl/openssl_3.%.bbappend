FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

python __anonymous() {
    pv = d.getVar("PV")

    if bb.utils.vercmp_string_op(pv, "3.3.6", "<"):
        d.appendVar("SRC_URI", " file://openssl-3.2-feat-updates-to-support-status_request_v2.patch")
    else:
        d.appendVar("SRC_URI", " file://openssl-3.5-feat-updates-to-support-status_request_v2.patch")
}
