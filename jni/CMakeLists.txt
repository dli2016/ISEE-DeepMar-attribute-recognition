add_library(jniisee_deepmar_attribute_recognition SHARED jni_isee_deepmar_attribute_recognition.cpp)

set (CMAKE_C_CFLAGS "${CMAKE_C_CFLAGS} -fPIC")
set (CMAKE_CXX_CFLAGS "${CMAKE_CXX_CFLAGS} -fPIC")

target_link_libraries(jniisee_deepmar_attribute_recognition isee_deepmar_attribute_recognition)

include_directories(jniisee_deepmar_attribute_recognition PUBLIC ${PROJECT_SOURCE_DIR}/include)
include_directories(jniisee_deepmar_attribute_recognition PUBLIC ${JAVA_HOME}/include)

if (MSVC)
    include_directories(jniisee_deepmar_attribute_recognition PUBLIC ${JAVA_HOME}/include/windows)
else ()
    include_directories(jniisee_deepmar_attribute_recognition PUBLIC ${JAVA_HOME}/include/linux)
endif ()

install(TARGETS jniisee_deepmar_attribute_recognition DESTINATION lib)
