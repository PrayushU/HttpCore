# Define source files for HttpClient
set(CLIENT_SRC HttpClient.cpp)

# Create a library for HttpClient
add_library(HttpClientLib ${CLIENT_SRC})

# Include Boost and project headers
target_include_directories(HttpClientLib PRIVATE ${CMAKE_SOURCE_DIR}/include ${BOOST_INCLUDE_DIRS})

# Link Boost libraries
target_link_libraries(HttpClientLib PRIVATE ${BOOST_LIBRARIES})

# Create an executable for HttpClient
add_executable(HttpClient ${CLIENT_SRC})

# Link the library and Boost to the executable
target_link_libraries(HttpClient PRIVATE HttpClientLib ${BOOST_LIBRARIES})
