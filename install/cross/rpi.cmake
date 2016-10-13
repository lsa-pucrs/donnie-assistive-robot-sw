SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_SYSTEM_PROCESSOR arm)
SET(CMAKE_SYSTEM_VERSION 1)

SET(DEVROOT /opt/raspberrypi)
SET(PIROOT ${DEVROOT}/root)
SET(PITOOLS ${DEVROOT}/tools)

SET(TOOLROOT ${PITOOLS}/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64)

# specify the cross compiler
SET(CMAKE_C_COMPILER   ${TOOLROOT}/bin/arm-linux-gnueabihf-gcc)
SET(CMAKE_CXX_COMPILER ${TOOLROOT}/bin/arm-linux-gnueabihf-g++)

SET(CMAKE_SYSROOT ${PIROOT})
SET(CMAKE_FIND_ROOT_PATH ${PIROOT})

# set pkg environment variables so pkg-config can work on the RPi folder
# https://autotools.io/pkgconfig/cross-compiling.html
# TODO amory.  acho q posso remover isso
set(ENV{SYSROOT} ${PIROOT})
set(ENV{PKG_CONFIG_SYSROOT_DIR} ${PIROOT})
set(ENV{PKG_CONFIG_LIBDIR} "${PIROOT}/usr/lib/pkgconfig:${PIROOT}/usr/local/lib/pkgconfig:${PIROOT}/usr/lib/arm-linux-gnueabihf/pkgconfig:${PIROOT}/usr/share/pkgconfig")
MESSAGE( STATUS "PKG_CONFIG_LIBDIR = $ENV{PKG_CONFIG_LIBDIR}")

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# add new lib dirs
# TODO amory.  acho q posso remover isso
link_directories(${PIROOT}/usr/lib/arm-linux-gnueabihf/)

