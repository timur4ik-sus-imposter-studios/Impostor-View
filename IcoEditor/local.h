#pragma once
#include "framework.h"

extern std::vector<std::vector<std::wstring>> local_matrix;

typedef enum {
	EN = 0,
	RU = 1,
} Lang;

enum LocalElement{
	_FILE = 0,
	_ABOUT = 1,
	_SAVEAS = 2,
	_OPEN = 3,
	_SPECIAL = 4,
	_EXIT = 5,
	_PLUS = 6,
	_MINUS = 7,
	_FILE_NOT_SELECTED = 8,
	_SELECTLANG = 9,
	_INFO = 10,
	_ABOUT_BTN = 11
} ;

void SelectLang(void);
