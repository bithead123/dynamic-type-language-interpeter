cmake_minimum_required(VERSION 3.2)
project(lang_cpp)

find_package(Boost COMPONENTS filesystem program_options REQUIRED)
#FIND_PACKAGE(Boost COMPONENTS  REQUIRED)


include_directories(${Boost_INCLUDE_DIR})

add_compile_options(-Wall)

add_executable(
    tt
    scan/token.h
    scan/scanner.h
    scan/scanner.cpp
    tools/log.h
    lang.cpp
    expr/expr.h
    expr/expr.cpp
    tools/ast_printer.h
    tools/ast_printer.cpp
    tools/token_print.h
)

target_include_directories(tt PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(tt ${Boost_LIBRARIES})