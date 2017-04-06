# run_threadSafeOutputFileStream_test.sh
# test_threadSafeOutputFileStream.cpp
include(CetTest)

set(source_libraries pthread ${CPPUNIT} ${TBB} hep_concurrency)

cet_make_exec(ThreadSafeOutputFileStream_t LIBRARIES pthread hep_concurrency)
cet_test(runThreadSafeOutputFileStream_t.sh PREBUILT)
cet_test(serialtaskqueuechain_t LIBRARIES ${source_libraries})
cet_test(serialtaskqueue_t LIBRARIES ${source_libraries})
cet_test(waitingtasklist_t LIBRARIES ${source_libraries})
