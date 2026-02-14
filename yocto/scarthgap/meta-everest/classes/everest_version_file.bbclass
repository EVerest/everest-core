# add everest release version information to the root file system

inherit everest

python do_everest_generate_version() {
    import oe.packagedata
    import os
    from datetime import datetime

    # obtain package name and version from important packages
    # see license_image.bbclass license_create_manifest() for the approach

    pkg_dic = {}
    for pkg in everest_important_packages(d):
        pkg_info = os.path.join(d.getVar('PKGDATA_DIR'),
                                'runtime-reverse', pkg)
        pkg_name = os.path.basename(os.readlink(pkg_info))
        info = oe.packagedata.read_pkgdatafile(pkg_info)

        # items of interest: PN PV
        pkg_dic[pkg_name] = {'name': info['PN'], 'version': info['PV']}

    # JSON information to add to the version file
    release = {}
    release['channel'] = os.environ.get('EVEREST_UPDATE_CHANNEL', "unknown")
    release['datetime'] = datetime.now().isoformat("T") + "Z"

    version = None
    try:
        version_file = d.getVar('IMAGE_ROOTFS') + d.getVar('everest_VERSION_FILE')
        with open(version_file, "r") as fp:
            version = fp.read().strip()
    except:
        version = None

    if version is None or version == "":
        if "everest-core" in pkg_dic:
            version = pkg_dic['everest-core']['version']
        else:
            bb.warn("Unable to determine everest release")
            version = '<unknown>'

    release['version'] = version
    release['components'] = list(pkg_dic.values())

    # write version file
    everest_write_version(d, release)
}

def everest_important_packages(d):
    from oe.rootfs import image_list_installed_packages
    pkgs = image_list_installed_packages(d)

    # recipes from recipes-core
    important = [
        "everest-admin-panel",
        "everest-cmake",
        "everest-core",
        "everest-framework",
        "everest-libmodbus",
        "everest-node-red-flows",
        "json-schema-validator",
        "libcbv2g",
        "libevse-security",
        "libfsm",
        "libiso15118",
        "liblog",
        "libnfc-nci",
        "libocpp",
        "libslac",
        "libtimer",
    ]

    additional = d.getVar('EVEREST_RELEASE_PACKAGES')
    for pkg in additional.split(' '):
        pkg = pkg.strip()
        if pkg != "":
            important.append(pkg)

    return sorted([x for x in pkgs if x in important])

def everest_show_all_packages(d):
    # can be added to ROOTFS_POSTPROCESS_COMMAND to show available packages for
    # updating the important list in everest_important_packages()

    from oe.rootfs import image_list_installed_packages
    pkgs = image_list_installed_packages(d)
    for p in sorted(pkgs):
        bb.warn("Installed: %s" % str(p))

def everest_write_version(d, release_info):
    import json

    output = d.getVar('IMAGE_ROOTFS') + d.getVar('EVEREST_RELEASE_FILE')

    with open(output, "w", encoding="utf-8") as json_file:
        json.dump(release_info, json_file, indent=2)

# add processing to the end of the list
ROOTFS_POSTPROCESS_COMMAND:append = " do_everest_generate_version;"
