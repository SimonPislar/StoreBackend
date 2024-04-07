cmake_minimum_required(VERSION 3.23)
project(inlupp2 C)

set(CMAKE_C_STANDARD 11)

add_executable(inlupp2
        common.h
        db.c
        db.h
        hash_table.c
        hash_table.h
        iterator.h
        linked_list.c
        linked_list.h
        list_iterator.c
        menu.c
        menu.h
        merch.h
        merch.c
        merch_tests.c
        shelves.c
        shelves.h
        utils.h
        utils.c
        shopping_cart.h
        shopping_cart.c)
