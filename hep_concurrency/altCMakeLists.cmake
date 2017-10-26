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

#-----------------------------------------------------------------------
# Install
#
install(TARGETS hep_concurrency
  EXPORT ${PROJECT_NAME}Targets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )
# Install directory for headers - do this way as we don't have
# optional headers so no filtering/selection required
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/"
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
  FILES_MATCHING PATTERN "*.h"
  PATTERN "test" EXCLUDE
  )

#-----------------------------------------------------------------------
# Create support files
#
include(CMakePackageConfigHelpers)

# - Common to both trees
write_basic_package_version_file(
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  COMPATIBILITY SameMajorVersion
  )

# - Build tree (EXPORT only for now, config file needs some thought,
#   dependent on the use of multiconfig)
export(EXPORT ${PROJECT_NAME}Targets
  NAMESPACE ${PROJECT_NAME}::
  FILE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake"
  )

# - Install tree
configure_package_config_file("${PROJECT_SOURCE_DIR}/${PROJECT_NAME}Config.cmake.in"
  "${PROJECT_BINARY_DIR}/InstallCMakeFiles/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_CMAKEDIR}/${PROJECT_NAME}"
  PATH_VARS CMAKE_INSTALL_INCLUDEDIR
  )

install(FILES
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    "${PROJECT_BINARY_DIR}/InstallCMakeFiles/${PROJECT_NAME}Config.cmake"
  DESTINATION
    "${CMAKE_INSTALL_CMAKEDIR}/${PROJECT_NAME}"
    )

install(EXPORT ${PROJECT_NAME}Targets
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION "${CMAKE_INSTALL_CMAKEDIR}/${PROJECT_NAME}"
  )

#-----------------------------------------------------------------------
# Testing
if(BUILD_TESTING)
  add_subdirectory(test)
endif()
