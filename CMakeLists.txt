cmake_minimum_required(VERSION 3.13) 

project(AkaBaseCode) 

add_subdirectory(lib/Aka) 

add_executable(AkaBaseCode 
	"src/main.cpp" 
	"src/Editor.cpp"
) 

target_link_libraries(AkaBaseCode Aka) 
