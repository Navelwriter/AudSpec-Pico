# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "E:/VSARM/sdk/pico/pico-sdk/tools/pioasm"
  "E:/Pico/AudSpec-Pico/build/pioasm"
  "E:/Pico/AudSpec-Pico/build/pico-sdk/src/rp2_common/cyw43_driver/pioasm"
  "E:/Pico/AudSpec-Pico/build/pico-sdk/src/rp2_common/cyw43_driver/pioasm/tmp"
  "E:/Pico/AudSpec-Pico/build/pico-sdk/src/rp2_common/cyw43_driver/pioasm/src/PioasmBuild-stamp"
  "E:/Pico/AudSpec-Pico/build/pico-sdk/src/rp2_common/cyw43_driver/pioasm/src"
  "E:/Pico/AudSpec-Pico/build/pico-sdk/src/rp2_common/cyw43_driver/pioasm/src/PioasmBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "E:/Pico/AudSpec-Pico/build/pico-sdk/src/rp2_common/cyw43_driver/pioasm/src/PioasmBuild-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "E:/Pico/AudSpec-Pico/build/pico-sdk/src/rp2_common/cyw43_driver/pioasm/src/PioasmBuild-stamp${cfgdir}") # cfgdir has leading slash
endif()
