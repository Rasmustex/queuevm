add_library(
  qvm
  src/qvm/qvm.h
  src/qvm/qvm.c
  )

target_include_directories(qvm PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(qvm PUBLIC queue)
