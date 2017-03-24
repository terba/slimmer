/*
	ScreenMenu.h - Slimmer
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

#ifndef SCREENMENU_H
#define SCREENMENU_H

#include <vector>
#include <lcdapi/include/LCDHeaders.h>

#include "Screen.h"
#include "Player.h"
#include "MenuItem.h"

using namespace lcdapi;

class ScreenMenu : public Screen
{
public:
	ScreenMenu(LCDClient* parent, const string& serverVersion);
	~ScreenMenu();

	MenuItem& selectedItem() const;
	MenuItem& pathItem(const int index) const { return *mPath[index]; }
	int level() const { return mPath.size(); }

	void removeItem(const int index);
	void removeSelected();

	void navigateUp();
	void navigateDown();
	void select();
	void back();
	void backToMainMenu();

	void showPopup(const string& message);
	void hidePopup();

protected:
	void update();

protected:
	static const constexpr char* mPrefixNotSelected = " ";
	static const constexpr char* mPrefixSelected = ">";

protected:
	vector<LCDText*> mLines;
	MenuItem mMenu;
	vector<MenuItem*> mPath;
};

#endif // SCREENMENU_H
