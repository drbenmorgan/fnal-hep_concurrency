if(ALT_CMAKE)
  include(altCMakeLists.cmake)
else()
# run_threadSafeOutputFileStream_test.sh
# test_threadSafeOutputFileStream.cpp
include(CetTest)

set(source_libraries hep_concurrency ${CPPUNIT} ${TBB} pthread)

cet_make_exec(ThreadSafeOutputFileStream_t LIBRARIES pthread hep_concurrency)
cet_test(runThreadSafeOutputFileStream_t.sh PREBUILT)
cet_test(serialtaskqueuechain_t LIBRARIES ${source_libraries})
cet_test(serialtaskqueue_t LIBRARIES ${source_libraries})
cet_test(waitingtasklist_t LIBRARIES ${source_libraries})
endif() # ALT_CMAKE
