cmake_minimum_required(VERSION 3.27)
project(ex1 C)

set(CMAKE_C_STANDARD 11)

add_executable(ex1
        test.txt
        markov_chain.c
        tweets_generator.c
        linked_list.c
        linked_list.h
        markov_chain.h)
