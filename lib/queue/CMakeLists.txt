add_library(
  queue
  src/queue/queue.h
  src/queue/queue.c
  )

target_include_directories(queue PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
