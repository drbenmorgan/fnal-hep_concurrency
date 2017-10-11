# run_threadSafeOutputFileStream_test.sh
# test_threadSafeOutputFileStream.cpp

find_package(CppUnit REQUIRED)

include(CetTest)
set(source_libraries hep_concurrency CppUnit::CppUnit pthread)

add_executable(ThreadSafeOutputFileStream_t ThreadSafeOutputFileStream_t.cc)
target_link_libraries(ThreadSafeOutputFileStream_t hep_concurrency pthread)

cet_test(runThreadSafeOutputFileStream_t.sh HANDBUILT
  # Use TEST_EXEC to use exact script without needing PATH...
  TEST_EXEC ${CMAKE_CURRENT_SOURCE_DIR}/runThreadSafeOutputFileStream_t.sh
  # Script runs ThreadSafeOutputFileStream_t, so ensure its location is in test env PATH
  # Could also do this using cet_test_env, but better to be specific for
  # this single case.
  TEST_PROPERTIES ENVIRONMENT PATH=$<TARGET_FILE_DIR:ThreadSafeOutputFileStream_t>:$ENV{PATH}
  )
cet_test(serialtaskqueuechain_t LIBRARIES ${source_libraries})
cet_test(serialtaskqueue_t LIBRARIES ${source_libraries})
cet_test(waitingtasklist_t LIBRARIES ${source_libraries})
