/*
	Screen.cpp - Slimmer
	Copyright (C) 2016-2017  Terényi, Balázs (terenyi@freemail.hu)

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

#include "Screen.h"
#include "Config.h"
#include <unicode/unistr.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

Screen::Screen(lcdapi::LCDClient* parent, const string& title, const string& id) : lcdapi::LCDScreen(parent, title, id)
{
	mWidth = parent->getWidth();
	mHeight = parent->getHeight();
	mVisible = true;
}

bool Screen::visible()
{
	return mVisible;
}

void Screen::show()
{
	mVisible = true;
	setPriority(lcdapi::LCD_PRIORITY_FOREGROUND);
}

void Screen::hide()
{
	mVisible = false;
	setPriority(lcdapi::LCD_PRIORITY_HIDDEN);
}

void Screen::progressOn()
{
	setHeartBeat(lcdapi::LCD_HEARTBEAT_ON);
}

void Screen::progressOff()
{
	setHeartBeat(lcdapi::LCD_HEARTBEAT_OFF);
}

int Screen::width() const
{
	return mWidth;
}

int Screen::height() const
{
	return mHeight;
}

string Screen::trail(const string& s) const
{
	return s.length() > mWidth ? s + mTrailString : s;
}

/* This version of toLCDString uses ICU to convert to the desired
 * encoding of your LCD. But ICU adds about 20MB to slimmers memory
 * consumption because of linking to libicudata.so, so it is advisable
 * to compile a minimal ICU containing only the conversion functions (and data).
*/
string Screen::toLCDString(const string& s)
{
	UnicodeString ucs = UnicodeString::fromUTF8(StringPiece(s.c_str()));
	char target[200];
	int32_t targetsize = ucs.extract(0, ucs.length(), target, sizeof(target), Config::encoding().c_str());
	target[targetsize] = 0;

	// Escaping double quotes
	string ret = target;
	size_t pos = 0;
	while((pos = ret.find('\"', pos)) != string::npos)
	{
		 ret.replace(pos, 1, "\\\"");
		 pos += 2;
	}

	return ret;
}
