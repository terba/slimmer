/*
	ScreenError.cpp - Slimmer
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

#include "ScreenError.h"
#include "Config.h"

ScreenError::ScreenError(lcdapi::LCDClient* parent) : Screen(parent, "Error", "Error")
{
	setHeartBeat(LCD_HEARTBEAT_OFF);
	hide();

	mTitle.set(cTitleText, width() / 2 - cTitleLength / 2 + 1, height()/2);
	mScroller.set("", 1, height()/2+1, width(), height()/2+1, Config::scrollSpeed(), LCDScroller::Marquee);

	add(&mTitle);
	add(&mScroller);
}

void ScreenError::setText(const string& text)
{
	mScroller.setText(trail(toLCDString(text)));
}
