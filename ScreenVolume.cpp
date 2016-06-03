/*
	ScreenVolume.cpp - Slimmer
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

#include "ScreenVolume.h"

ScreenVolume::ScreenVolume(LCDClient* parent) : Screen(parent, "Volume", "Volume")
{
	setHeartBeat(LCD_HEARTBEAT_OFF);
	hide();

	mTitle.set(cTitleText, width() / 2 - cTitleLength / 2 + 1,2);
	mLeftBracket.set("[", 1, 3);
	mRightBracket.set("]", width(), 3);
	mBar.set((width() - 2) * parent->getCharWidth(), 2, 3);
	mBar.setPercentageMax((width() - 2) * parent->getCharWidth());

	add(&mTitle);
	add(&mLeftBracket);
	add(&mRightBracket);
	add(&mBar);
}

void ScreenVolume::update(const int volume)
{
	mBar.setPercentage(volume);
}
