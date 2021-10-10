/*
	ScreenSeek.cpp - Slimmer
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

#include "ScreenSeek.h"
#include <iostream>

ScreenSeek::ScreenSeek(LCDClient* parent) : Screen(parent, "Seek", "Seek")
{
	setHeartBeat(LCD_HEARTBEAT_OFF);
	hide();

	mTitle.set(cTitleText, width() / 2 - cTitleLength / 2 + 1, height()/2);
	mLeftBracket.set("[", 1, height()/2+1);
	mRightBracket.set("]", width(), height()/2+1);
	mBar.set((width() - 2) * parent->getCharWidth(), 2, height()/2+1);
	mBar.setPercentageMax((width() - 2) * parent->getCharWidth());

	add(&mTitle);
	add(&mLeftBracket);
	add(&mRightBracket);
	add(&mBar);
}

void ScreenSeek::update(const int position)
{
	mBar.setPercentage(position);
}
