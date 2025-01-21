# Define source files for HttpServer
set(SERVER_SRC HttpServer.cpp)

# Create a library for HttpServer
add_library(HttpServerLib ${SERVER_SRC})

# Include project and Boost headers
target_include_directories(HttpServerLib PRIVATE ${CMAKE_SOURCE_DIR}/include ${BOOST_INCLUDE_DIRS})

# Link Boost.System to the library
target_link_libraries(HttpServerLib PRIVATE ${BOOST_LIBRARIES})

# Create an executable for HttpServer
add_executable(HttpServer ${SERVER_SRC})

# Link the library and Boost to the executable
target_link_libraries(HttpServer PRIVATE HttpServerLib ${BOOST_LIBRARIES})
