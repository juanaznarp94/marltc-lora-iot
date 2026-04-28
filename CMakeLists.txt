cmake_minimum_required(VERSION 3.10)
project(lora_radiolib_test)

# Create executable
add_executable(tx src/tx.cpp)
add_executable(rx src/rx.cpp)