# Minimum required CMake version
cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0167 NEW)
# cmake_minimum_required()

# Project name and version
project(HttpProject VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories for binaries and libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Include directories
include_directories(include)

# Find Boost (specify Asio and its dependencies)
find_package(Boost 1.70 REQUIRED COMPONENTS system url)
if (Boost_FOUND)
    message(STATUS "Boost found: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost libraries: ${Boost_LIBRARIES}")
else()
    message(FATAL_ERROR "Boost not found!")
endif()



# Pass Boost variables to subdirectories
set(BOOST_INCLUDE_DIRS ${Boost_INCLUDE_DIRS})
set(BOOST_LIBRARIES ${Boost_LIBRARIES})
set(BOOST_FOUND ${Boost_FOUND})

# Find OpenSSL
find_package(OpenSSL REQUIRED)
if (OPENSSL_FOUND)
    message(STATUS "OpenSSL found: ${OPENSSL_INCLUDE_DIR}")
    message(STATUS "OpenSSL libraries: ${OPENSSL_LIBRARIES}")
else()
    message(FATAL_ERROR "OpenSSL not found!")
endif()


# To include OpenSSL
find_package(OpenSSL REQUIRED)
if(OpenSSL_FOUND)
    message(STATUS "OpenSSL version: ${OPENSSL_VERSION}")
    message(STATUS "OpenSSL found: ${OPENSSL_INCLUDE_DIR}")
    message(STATUS "OpenSSL libraries: ${OPENSSL_LIBRARIES}")
else()
    message(FATAL_ERROR "OpenSSL not found!")
endif()

set(OPENSSL_INCLUDE_DIRS ${OPENSSL_INCLUDE_DIR})
set(OPENSSL_LIBRARIES ${OPENSSL_LIBRARIES})

# Generate compile_flags.txt for LSPs like clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Copy compile_commands.json to compile_flags.txt for LSP compatibility
add_custom_target(
    generate_compile_flags ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_BINARY_DIR}/compile_flags.txt
    DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json
)

# Add subdirectories
add_subdirectory(src/HttpServer)
add_subdirectory(src/HttpClient)
add_subdirectory(tests)
