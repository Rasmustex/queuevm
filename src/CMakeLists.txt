add_executable(qvmi qvmi.c)
add_executable(quasm quasm.c)

target_link_libraries(qvmi PRIVATE qvm)
target_link_libraries(quasm PRIVATE qvm)
