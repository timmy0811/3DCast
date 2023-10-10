#pragma once

#include "windowsWrapper.hpp"
#include "Log.h"

#include <iostream>

#ifdef _DEBUG
#define GLCall(x) GLCLearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#else
#define GLCall(x) x
#endif

#define LOG(message) std::cout << message << std::endl
#define ASSERT(x) if(!(x)) __debugbreak();

#define GL_ASSERT(x, ...) {if(!(x)) { LOG_GL_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }

enum class LOG_COLOR { LOG = 15, WARNING = 14, OK = 10, FAULT = 12, SPECIAL_A = 11, SPECIAL_B = 13 };

static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
inline void LOGC(const std::string& msg, LOG_COLOR color = LOG_COLOR::LOG) {
	SetConsoleTextAttribute(hConsole, (int)color);
	std::cout << msg << '\n';
	SetConsoleTextAttribute(hConsole, 15);
}

void GLCLearError();
bool GLLogCall(const char* function, const char* file, int line);