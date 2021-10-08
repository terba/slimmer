/*
	ScreenMenu.cpp - Slimmer
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

#include "ScreenMenu.h"
#include "Config.h"
#include <iostream>

ScreenMenu::ScreenMenu(LCDClient* parent, const string& serverVersion) : Screen(parent, "Menu", "Menu"), mMenu("", MenuItem::MENU, "Main")
{
	setHeartBeat(LCD_HEARTBEAT_OFF);
	hide();

	for (int i = 0; i < height(); i++)
	{
		mLines.push_back(new LCDText());
		mLines[i]->set("", 1, i + 1);
		add(mLines[i]);
	}

	// Creating the menu structure
	if (Config::backMenus())
		mMenu.addItem(MenuItem("", MenuItem::BACK, "Back"));
	mMenu.addItem(MenuItem("", MenuItem::QUEUE, "Playing queue", true));
	mMenu.addItem(MenuItem("", MenuItem::MENU, "Music library"));
	if (Config::backMenus())
		mMenu.item(mMenu.itemCount() -1).addItem(MenuItem("", MenuItem::BACK, "Back"));
	if (serverVersion >= "7.9.0")
		mMenu.item(mMenu.itemCount() - 1).addItem(MenuItem("", MenuItem::ALBUMARTISTS, "Album Artists"));
	mMenu.item(mMenu.itemCount() - 1).addItem(MenuItem("", MenuItem::ARTISTS, serverVersion >= "7.9.0" ? "All Artists" : "Artists"));
	mMenu.item(mMenu.itemCount() - 1).addItem(MenuItem("", MenuItem::ALBUMS, "Albums"));
	mMenu.item(mMenu.itemCount() - 1).addItem(MenuItem("", MenuItem::NEWMUSIC, "New Music"));
	mMenu.item(mMenu.itemCount() - 1).addItem(MenuItem("", MenuItem::FOLDERS, "Folders"));
	mMenu.addItem(MenuItem("", MenuItem::FAVORITES, "Favorites"));
	mMenu.addItem(MenuItem("", MenuItem::RADIOS, "Radios"));
	mPath.push_back(&mMenu);

	update();
}

ScreenMenu::~ScreenMenu()
{
	for (LCDText* line : mLines)
		delete line;
}

MenuItem& ScreenMenu::selectedItem() const
{
	return mPath.back()->selectedItem();
}

void ScreenMenu::removeItem(const int index)
{
	mPath.back()->removeItem(index);
	if (mPath.back()->selected() > mPath.back()->itemCount() - 1) mPath.back()->setSelected(mPath.back()->itemCount() - 1);
	update();
}

void ScreenMenu::removeSelected()
{
	mPath.back()->removeItem(mPath.back()->selected());
	if (mPath.back()->selected() > mPath.back()->itemCount() - 1) mPath.back()->setSelected(mPath.back()->itemCount() - 1);
	if (mPath.back()->itemCount() == 0) mPath.back()->setSelected(0);
	update();
}

void ScreenMenu::navigateUp()
{
	if (mPath.back()->selected() > 0)
		mPath.back()->setSelected(mPath.back()->selected() - 1);
	else
		mPath.back()->setSelected(mPath.back()->itemCount() - 1);
	update();
}

void ScreenMenu::navigateDown()
{
	if (mPath.back()->selected() < mPath.back()->itemCount() - 1)
		mPath.back()->setSelected(mPath.back()->selected() + 1);
	else
		mPath.back()->setSelected(0);
	update();
}

void ScreenMenu::select()
{	
	if (mPath.back()->selectedItem().hasItem())
	{
		mPath.push_back(&(mPath.back()->selectedItem()));
		update();
	}
}

void ScreenMenu::back()
{
	if (mPath.size() > 1)
	{
		mPath.pop_back();
		update();
	}
}

void ScreenMenu::backToMainMenu()
{
	while (mPath.size() > 1) mPath.pop_back();
	update();
}

void ScreenMenu::showPopup(const string& message)
{
	string s = mPrefixSelected;
	if (mPath.back()->numbering()) s+= std::to_string(mPath.back()->selected() + 1) + " ";
	s += "[" + message + "]";
	mLines[mPath.back()->selected() - mPath.back()->top()]->setText(s);
}

void ScreenMenu::hidePopup()
{
	update();
}

void ScreenMenu::update()
{
	// Scrolling: adjusting top. So mSelected will be always visible
	if (mPath.back()->selected() > mPath.back()->top() + height() - 1)
		mPath.back()->setTop(mPath.back()->selected() - height() + 1);
	if (mPath.back()->selected() < mPath.back()->top())
		mPath.back()->setTop(mPath.back()->selected());
	if (mPath.back()->itemCount() < mPath.back()->top() + height() && mPath.back()->top() > 0)
		mPath.back()->setTop(mPath.back()->itemCount() - height());

	int j = 0;
	for (int i = 0; i < height(); i++)
	{
		string s;
		if (mPath.back()->top() + i < mPath.back()->itemCount())
		{
			s = mPath.back()->top() + i == mPath.back()->selected() ? mPrefixSelected : mPrefixNotSelected;
			if (mPath.back()->numbering())
			{
				if (Config::backMenus())
				{
					if ((mPath.back()->item(mPath.back()->top() + i).name() == "Back") && (i == 0))
					{
						s+= "  ";
					}
					else
					{
						s+= std::to_string(mPath.back()->top() + j++ + 1) + " ";
					}
				} else
				{
					s+= std::to_string(mPath.back()->top() + i + 1) + " ";
				}

			}
			if (mPath.back()->top() + i < mPath.back()->itemCount())
				s += mPath.back()->item(mPath.back()->top() + i).name();
		}
		mLines[i]->setText(toLCDString(s));
	}
}
