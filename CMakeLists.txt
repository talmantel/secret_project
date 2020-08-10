cmake_minimum_required(VERSION 3.16)
project(mmn14 C)

set(CMAKE_C_STANDARD 90)

add_executable(mmn14 main.c data.c actions.c first_pass.c output.c entries.c data.c parser.c second_pass.c instructions.c symbols.c errors.c linked_list.c extern.c labels.c labels.h)
