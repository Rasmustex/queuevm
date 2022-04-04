add_executable(qvmi qvmi.c)
add_executable(quasm quasm.c)
add_executable(unquasm unquasm.c)

target_link_libraries(qvmi PRIVATE qvm_exec)
target_link_libraries(quasm PRIVATE qvm)
target_link_libraries(unquasm PRIVATE qvm)
