#pragma once
#include <iostream>
#include <utility>

// Use to print variables to stdout, assumes flush is handled externally

template<typename ...Args>
void print(Args&&... args) {
	(std::cout << ... << args);
}

template<typename ...Args>
void println(Args&&... args) {
	print(std::forward<Args>(args)..., '\n');
}