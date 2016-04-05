AUTOMAKE_OPTIONS = foreign
# AM_CPPFLAGS = -Wall -I${top_srcdir}/include 
AM_CXXFLAGS = -Wall ${THIRD_PARTY_DYN_LIBS} -I${top_srcdir}/include/ -I${top_srcdir}/include/third-party/libxml2 -I${top_srcdir}/include/third-party/libmodbus 
bin_PROGRAMS = ${top_srcdir}/bin/smb
__top_srcdir__bin_smb_SOURCES = main.cpp app_main.cpp 
# be very careful about lib linking order: lib A invoke lib B, then lib A should be former than lib B
__top_srcdir__bin_smb_LDADD = \
							  ${top_srcdir}/lib/libdb.a \
							  ${top_srcdir}/lib/libpc.a \
							  ${top_srcdir}/lib/libutils.a \
							  ${top_srcdir}/lib/libomc.a \
							  ${top_srcdir}/lib/libapp_gps.a \
							  ${top_srcdir}/lib/libapp_led.a \
							  ${top_srcdir}/lib/libapp_hmi.a \
							  ${top_srcdir}/lib/libapp_dvr.a \
							  ${top_srcdir}/lib/libadp_gps.a \
							  ${top_srcdir}/lib/libadp_led.a \
							  ${top_srcdir}/lib/libadp_hmi.a \
							  ${top_srcdir}/lib/libadp_dvr.a \
							  ${top_srcdir}/lib/libdrv_gps.a \
							  ${top_srcdir}/lib/libdrv_led.a \
							  ${top_srcdir}/lib/libdrv_serial.a \
							  ${top_srcdir}/lib/libdrv_spi.a \
							  ${top_srcdir}/lib/libos.a \
							  ${THIRD_PARTY_STATIC_LIBS_PATH}/libmodbus.lib \
							  ${top_srcdir}/lib/libtest.a 

