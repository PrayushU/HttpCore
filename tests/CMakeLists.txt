# Define test executables
add_executable(test_HttpServer test_HttpServer.cpp)
add_executable(test_HttpClient test_HttpClient.cpp)

# Include headers for tests
target_include_directories(test_HttpServer PRIVATE ${CMAKE_SOURCE_DIR}/include ${Boost_INCLUDE_DIRS})
target_include_directories(test_HttpClient PRIVATE ${CMAKE_SOURCE_DIR}/include ${Boost_INCLUDE_DIRS})

# Link the libraries to the test executables
target_link_libraries(test_HttpServer PRIVATE HttpServerLib)
target_link_libraries(test_HttpClient PRIVATE HttpClientLib)

# Enable testing
enable_testing()

# Register tests
add_test(NAME TestHttpServer COMMAND test_HttpServer)
add_test(NAME TestHttpClient COMMAND test_HttpClient)
