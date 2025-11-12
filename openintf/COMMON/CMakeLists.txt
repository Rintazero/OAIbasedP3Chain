# add_subdirectory(MESSAGES)
set(NR_INTF_VERSION 0 0 1)
make_version(NR_INTF_cc ${NR_INTF_VERSION})
string(REPLACE ";" "." NR_INTF_RELEASE "${NR_INTF_VERSION}")

if(NR_INTF_RELEASE VERSION_EQUAL "0.0.1")
  include(MESSAGES/nr-intf-0.0.1.cmake)
else()
  message(FATAL_ERROR "unknown NR_INTF_RELEASE ${NR_INTF_RELEASE}")
endif()

run_asn1c(${CMAKE_CURRENT_SOURCE_DIR}/${NR_INTF_GRAMMAR}
          "NR_"
          OUTPUT ${nr_intf_source} ${nr_intf_headers}
          OPTIONS -pdu=all -fcompound-names -gen-UPER -no-gen-BER -no-gen-JER -no-gen-OER -gen-APER -no-gen-example -findirect-choice
          COMMENT "NR INTF source files"
)   

add_custom_target(gen_nr_intf_hdrs DEPENDS ${nr_intf_headers})
add_library(asn1_nr_intf_hdrs INTERFACE)
add_dependencies(asn1_nr_intf_hdrs gen_nr_intf_hdrs)
target_include_directories(asn1_nr_intf_hdrs INTERFACE "${CMAKE_CURRENT_BINARY_DIR}")

add_library(asn1_nr_intf ${nr_intf_source})
target_link_libraries(asn1_nr_intf PUBLIC asn1_nr_intf_hdrs)
target_compile_options(asn1_nr_intf
                         PRIVATE -DASN_DISABLE_OER_SUPPORT -w
                         PUBLIC -DNR_INTF_VERSION=${NR_INTF_RELEASE})
if (ENABLE_TESTS)
  add_subdirectory(tests)
endif()