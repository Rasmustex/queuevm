add_executable(qvm qvm.c)
add_executable(quasm quasm.c)

target_link_libraries(qvm PRIVATE qvm-lib)
target_link_libraries(quasm PRIVATE qvm-lib)
