# Define source files for HttpClient
set(CLIENT_SRC SampleHttpClient.cpp HttpClient.cpp)

# Create a library for HttpClient
add_library(HttpClientLib ${CLIENT_SRC})

# Include Boost and project headers
target_include_directories(HttpClientLib 
  PUBLIC ${CMAKE_SOURCE_DIR}/include/HttpClient ${CMAKE_SOURCE_DIR}/include/Common
  PRIVATE ${CMAKE_SOURCE_DIR}/include ${BOOST_INCLUDE_DIRS}
)

# Link Boost libraries
target_link_libraries(HttpClientLib PRIVATE ${BOOST_LIBRARIES})

# Create an executable for HttpClient
add_executable(SampleHttpClient ${CLIENT_SRC})

# Link the library and Boost to the executable
target_link_libraries(SampleHttpClient PRIVATE HttpClientLib ${BOOST_LIBRARIES})
