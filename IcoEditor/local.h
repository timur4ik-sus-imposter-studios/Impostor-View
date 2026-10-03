#pragma once
#include "framework.h"

extern const std::vector<std::vector<std::wstring>> local_matrix;

typedef enum {
	EN = 1,
	RU = 2,
  DE = 3
} Lang;

typedef enum {
	_FILE = 0,
	_INFO = 1,
	_SAVEAS = 2,
	_OPEN = 3,
	_SPECIAL = 4,
	_EXIT = 5,
	_PLUS = 6,
	_MINUS = 7,
	_FILE_NOT_SELECTED = 8
} LocalElement;
