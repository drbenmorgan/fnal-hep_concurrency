add_library(hep_concurrency SHARED
  GCCPrerequisite.h
  Likely.h
  SerialTaskQueue.cc
  SerialTaskQueue.h
  SerialTaskQueueChain.h
  SharedResourceNames.cc
  SharedResourceNames.h
  ThreadSafeAddOnlyContainer.h
  ThreadSafeOutputFileStream.cc
  ThreadSafeOutputFileStream.h
  WaitingTask.h
  WaitingTaskHolder.h
  WaitingTaskList.cc
  WaitingTaskList.h
  hardware_pause.h
  thread_safety_macros.h
  )
target_include_directories(hep_concurrency
  PUBLIC
   $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
   $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
   )
target_link_libraries(hep_concurrency PUBLIC TBB::tbb)

# ======================================================================
# Testing
if(BUILD_TESTING)
  add_subdirectory(test)
endif()
