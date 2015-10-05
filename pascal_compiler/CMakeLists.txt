cmake_minimum_required(VERSION 3.3)
project(pascal_compiler)

set(GTEST_DIR "C:/Git/gtest")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp tokenizer/lexems.h tokenizer/StateController.cpp tokenizer/StateController.h tokenizer/Tokenizer.cpp tokenizer/Tokenizer.h tokenizer/Token.cpp tokenizer/Token.h exception/TokenException.cpp exception/TokenException.h)

add_executable(pascal_compiler ${SOURCE_FILES})