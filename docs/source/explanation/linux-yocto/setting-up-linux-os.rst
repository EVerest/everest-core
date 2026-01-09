.. _exp_linux_yocto_setup_linux_os:

#####################################
Setting up the Linux operating system
#####################################

In principle, you can use any Linux-based operating system as long as it
comes with the required dependencies to run EVerest.

We strongly recommend using Yocto as it has some advantages over other
distributions:

-  It can be set up to do reproducible builds with versioning.
-  Most CPUs and SoMs already come with Yocto board support packages
   (BSP).
-  EVerest comes with full support for selected Yocto long term support
   releases (LTS) (scarthgap as of the time of writing).
-  It can be nicely integrated with your CI/CD to build complete
   production images and update packages.
-  It provides a software bill of materials of all packages in the Linux
   system for managing licenses.
-  Broad community
-  Automatic generation of cross-compile toolchains, that can be used
   during the development phase.

You can find more information about the Yocto project here:
https://www.yoctoproject.org

.. warning::

   Setting up the Linux base system for your product is a quite complex task
   that should be performed by domain experts. In case you do not have experts
   in your team, consider getting help from a company specialized on this.

   The end product's reliability, security and user experience strongly depends
   on a sound architecture, implementation and maintenance strategy of the base
   Linux system. This should not be underestimated.

Covering all aspects of setting up a Linux base system is out of the
scope of this documentation, but we would like to give some examples and ideas
and point out some typical solutions to questions you will have on your
journey. Do not consider this complete!

Setup a Yocto build environment
-------------------------------

Yocto has comprehensive caching capabilities that mean build times are substantially
reduced for successive builds. However an initial build will take hours since initial
versions need to be fetched and built so that caches are populated. There is support
for sharing downloads and caches that can reduce build times and are worth
considering where you have a co-located team.

A good build machine will have lots of RAM, SSD storage and multi-core processor as
well as a fast Internet connection.

It is possible to use a high-performance laptop especially for incremental builds
once the initial build is complete.

.. warning::

   Running this inside of a virtual machine is not recommended.

A full Yocto build easily requires 50-100 GB of disk space, and it will
use multiple cores. So, make sure you have enough RAM per core (e.g. 2-3
GB per (hyperthread) core).

Install a Linux distribution supported by Yocto and install all
necessary dependencies. See here for more information about that:

https://docs.yoctoproject.org/ref-manual/system-requirements.html

Alternatively, consider building in a container. Once you move to
production, a build container will probably be needed anyway to build
images in your CI/CD.

It is also recommended to archive the containers to be able to do fully
reproducible builds of older versions in the future.

Yocto itself can produce builds that are completely tagged (i.e. each
source package is tagged with a fixed version or Git hash), so they are
in principle reproducible.

There are - however - a few build dependencies to the host system that
may prevent you from building your released 1.0 version in ten years
from now. As an example, the Python version in ten years from now may
not run the old bitbake correctly anymore. Also, the Yocto recipes
contain only download URLs and version tags, but not the source packages
itself.

Let's start with an example and set up the Yocto build environment that
we use for EVerest on the BelayBox hardware.

Building the BelayBox Yocto image
---------------------------------

An example can be found here for the BelayBox:

https://github.com/PionixPublic/dev-hardware-yocto

Check out the *README* in this repository on how to build and install
this Yocto on the BelayBox.

--------------------------------

**Authors**: Cornelius Claussen, Manuel Ziegler
