/*
	MenuItem.h - Slimmer
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

#ifndef MENUITEM_H
#define MENUITEM_H

#include <string>
#include <vector>

using namespace std;

class MenuItem
{
public:
	enum Type {
		MENU,
		QUEUE,
		QUEUEITEM,
		NEWMUSIC,
		FOLDERS,
		FOLDER,
		FAVORITES,
		FAVORITE,
		RADIOS,
		RADIOPLUGIN,
		RADIOMENU,
		RADIO,
		ARTISTS,
		ALBUMS,
		TRACKS,
		ARTIST,
		ALBUM,
		TRACK
	};

	MenuItem(const string& id, const Type type, const string& name, const bool numbering = false);

	void removeItem(const int index);
	void clearItems();
	void addItem(const MenuItem& item);
	bool hasItem() const;
	MenuItem& selectedItem();
	MenuItem& item(const int index);
	void setSelected(const int index);
	void setTop(const int index);

	string id() const { return mId; }
	string name() const { return mName; }
	Type type() const { return mType; }
	bool numbering() const { return mNumbering; }
	int itemCount() const { return mItems.size(); }
	int selected() const { return mSelected; }
	int top() const { return mTop; }

protected:
	string mId;
	Type mType;
	string mName;
	bool mNumbering;
	vector<MenuItem> mItems;

	int mSelected;
	int mTop;
};

#endif // MENUITEM_H
