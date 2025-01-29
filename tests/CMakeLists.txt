# Define test executables
add_executable(test_HttpServer unit/test_HttpServer.cpp)
add_executable(test_HttpClient unit/test_HttpClient.cpp unit/catch_amalgamated.cpp)
add_executable(HttpClientStressTest stress/httpClientStressTest.cpp)

# Include headers for tests
target_include_directories(test_HttpServer PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/include/Tests ${Boost_INCLUDE_DIRS})
target_include_directories(test_HttpClient PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/include/Tests ${Boost_INCLUDE_DIRS})
target_include_directories(HttpClientStressTest PRIVATE ${CMAKE_SOURCE_DIR}/include/Common ${CMAKE_SOURCE_DIR}/include/HttpClient ${Boost_INCLUDE_DIRS})

# Link the libraries to the test executables
target_link_libraries(test_HttpServer PRIVATE HttpServerLib)
target_link_libraries(test_HttpClient PRIVATE HttpClientLib)

# Enable testing
enable_testing()

# Register tests
# add_test(NAME TestHttpServer COMMAND test_HttpServer)
add_test(NAME TestHttpClient COMMAND test_HttpClient)
add_test(NAME TestHttpClientStress COMMAND HttpClientStressTest)
