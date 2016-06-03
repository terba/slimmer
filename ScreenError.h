/*
	ScreenError.h - Slimmer
	Copyright (C) 2016  Terényi, Balázs (terenyi@freemail.hu)

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SCREENERROR_H
#define SCREENERROR_H

#include "Screen.h"
#include <string.h>

using namespace lcdapi;

class ScreenError : public Screen
{
public:
	ScreenError(LCDClient* parent);
	void setText(const string& text);

protected:
	static const constexpr char* cTitleText = "Error";
	static const constexpr int cTitleLength = strlen(cTitleText);
	LCDText mTitle;
	LCDScroller mScroller;
};

#endif // SCREENERROR_H
