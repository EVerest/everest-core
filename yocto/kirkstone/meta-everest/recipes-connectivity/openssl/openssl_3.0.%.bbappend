FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

python __anonymous() {
    pv = d.getVar("PV")

    if bb.utils.vercmp_string_op(pv, "3.0.19", "<"):
        d.appendVar("SRC_URI", " file://openssl-3.0.8-feat-updates-to-support-status_request_v2.patch")
    else:
        d.appendVar("SRC_URI", " file://openssl-3.0.19-feat-updates-to-support-status_request_v2.patch")
}
