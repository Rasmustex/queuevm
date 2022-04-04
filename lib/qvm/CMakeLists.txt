add_library(
  qvm
  src/qvm/qvm.h
  src/qvm/qvm.c
  )

add_library(
  qvm_exec
  src/qvm_exec/qvm_exec.h
  src/qvm_exec/qvm_exec.c
  )

target_include_directories(qvm PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(qvm PUBLIC queue)

target_link_libraries(qvm_exec PUBLIC qvm)
