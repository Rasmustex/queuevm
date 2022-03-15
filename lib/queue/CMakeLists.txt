add_library(
  queue-lib
  src/queue/queue.h
  src/queue/queue.c
  )

target_include_directories(queue-lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
