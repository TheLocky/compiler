cmake_minimum_required(VERSION 3.3)
project(pascal_compiler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_TOKENIZER tokenizer/TokenTypes.h tokenizer/StatesData.cpp tokenizer/StatesData.h tokenizer/Tokenizer.cpp tokenizer/Tokenizer.h tokenizer/Token.cpp tokenizer/Token.h)
set(SOURCE_EXCEPTION exception/Exception.cpp exception/Exception.h)
set(SOURCE_PARSER parser/Parser.cpp parser/Parser.h parser/Node.cpp parser/Node.h)
set(SOURCE_FILES main.cpp ${SOURCE_TOKENIZER} ${SOURCE_EXCEPTION} ${SOURCE_PARSER})

include_directories(${pascal_compiler_SOURCE_DIR})

add_executable(pascal_compiler ${SOURCE_FILES})