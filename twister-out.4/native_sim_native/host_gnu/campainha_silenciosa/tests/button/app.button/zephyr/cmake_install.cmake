# Install script for directory: /home/momesso/zephyrproject/zephyr

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/zephyr/arch/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/zephyr/lib/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/zephyr/soc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/zephyr/boards/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/zephyr/subsys/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/zephyr/drivers/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/acpica/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/cmsis/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/cmsis-dsp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/cmsis-nn/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/cmsis_6/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/dhara/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/fatfs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/adi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/hal_afbr/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/hal_ambiq/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/atmel/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/hal_bouffalolab/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/hal_espressif/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/hal_ethos_u/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/hal_gigadevice/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/hal_infineon/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/hal_intel/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/microchip/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/hal_nordic/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/nuvoton/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/hal_nxp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/openisa/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/quicklogic/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/hal_realtek/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/hal_renesas/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/hal_rpi_pico/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/hal_sifli/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/hal_silabs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/hal_st/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/hal_stm32/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/hal_tdk/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/hal_telink/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/ti/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/hal_wch/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/hal_wurthelektronik/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/xtensa/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/hostap/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/liblc3/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/libmctp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/libmetal/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/libsbc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/littlefs/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/lora-basics-modem/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/loramac-node/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/lvgl/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/mbedtls/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/mcuboot/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/mipi-sys-t/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/nanopb/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/nrf_wifi/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/open-amp/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/openthread/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/percepio/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/picolibc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/segger/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/trusted-firmware-a/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/trusted-firmware-m/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/uoscore-uedhoc/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/zcbor/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/modules/nrf_hw_models/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/zephyr/kernel/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/zephyr/cmake/flash/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/zephyr/cmake/usage/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/momesso/zephyrproject/campainha_silenciosa/twister-out/native_sim_native/host_gnu/campainha_silenciosa/tests/button/app.button/zephyr/cmake/reports/cmake_install.cmake")
endif()

