set(LIB_NAME ${PROJECT_NAME})

cet_make_library(LIBRARY_NAME ${LIB_NAME}
  SOURCE
  SerialTaskQueue.cc
  SharedResourceNames.cc
  ThreadSafeOutputFileStream.cc
  WaitingTaskList.cc
  LIBRARIES
  ${TBB}
  )

install_headers()
install_source()

add_subdirectory(test)
