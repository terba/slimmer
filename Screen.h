/*
	Screen.h - Slimmer
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

#ifndef SCREEN_H
#define SCREEN_H

#include <lcdapi/include/LCDHeaders.h>

using namespace std;

class Screen : public lcdapi::LCDScreen
{
public:
	Screen(lcdapi::LCDClient* parent, const string& title, const string& id);
	bool visible();
	void show();
	void hide();
	void progressOn();
	void progressOff();

protected:
	static string toLCDString(const string& inString);
	int width() const;
	int height() const;

protected:
	bool mVisible;

	string trail(const string& s) const;

private:
	int mWidth;
	int mHeight;

	static const constexpr char* mTrailString = "      ";
};

#endif // SCREEN_H
