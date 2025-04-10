cmake_minimum_required(VERSION 3.2)
project(lang_cpp)

find_package(Boost COMPONENTS program_options REQUIRED)

include_directories(${Boost_INCLUDE_DIR})

add_compile_options(-Wall)

add_executable(
    tt
    scan/scanner.h
    scan/scanner.cpp
    lang.h
    lang.cpp
    expr/expr.h
    expr/expr.cpp
    tools/ast_printer.h
    tools/ast_printer.cpp
)

target_include_directories(tt PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(tt ${Boost_LIBRARIES})