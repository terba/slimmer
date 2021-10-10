/*
	ScreenSeek.h - Slimmer (ScreenSeek.h)
	Copyright (C) 2016-2017  Terényi, Balázs (terenyi@freemail.hu): Original Implmentation
	Copyright (C) 2021  Aaron White <w531t4@gmail.com>: Added Seek capability

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

#ifndef SCREENSEEK_H
#define SCREENSEEK_H

#include "Screen.h"
#include "Player.h"
#include <string.h>
#include <lcdapi/include/LCDHeaders.h>

using namespace lcdapi;

class ScreenSeek : public Screen
{
public:
	ScreenSeek(LCDClient* parent);
	void update(const int position);

protected:
	static const constexpr char* cTitleText = "Seek";
	static const constexpr int cTitleLength = strlen(cTitleText);
	LCDText mTitle;
	LCDText mLeftBracket;
	LCDText mRightBracket;
	LCDHorizontalBar mBar;
};

#endif // SCREENSEEK_H
