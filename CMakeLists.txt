# mongooseDemo1

cmake_minimum_required(VERSION 3.5)
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED True)
if (WIN32)
	set (CMAKE_TOOLCHAIN_FILE c:/projects/github/microsoft/vcpkg/scripts/buildsystems/vcpkg.cmake)
else()
	add_compile_options (-rdynamic)
	add_link_options    (-rdynamic)
endif()

project (mongooseDemo1)

find_package (OpenSSL REQUIRED)

add_executable (mongooseDemo1
	main.cpp
	mongoose.h mongoose.c)

target_compile_definitions (mongooseDemo1
	PRIVATE MG_TLS=MG_TLS_OPENSSL)

target_include_directories (mongooseDemo1
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
	PRIVATE ${OPENSSL_INCLUDE_DIR})

target_link_libraries (mongooseDemo1
	PRIVATE OpenSSL::SSL OpenSSL::Crypto)
