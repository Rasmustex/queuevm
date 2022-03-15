add_library(
  qvm-lib
  src/qvm/qvm.h
  src/qvm/qvm.c
  )

target_include_directories(qvm-lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(qvm-lib PUBLIC queue-lib)
