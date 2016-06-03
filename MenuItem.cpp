/*
	MenuItem.cpp - Slimmer
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

#include "MenuItem.h"

MenuItem::MenuItem(const string& id, const Type type, const string& name, const bool numbering)
	: mId(id), mType(type), mName(name), mNumbering(numbering)
{
	mSelected = 0;
	mTop = 0;
}

void MenuItem::removeItem(const int index)
{
	mItems.erase(mItems.begin() + index);
}

void MenuItem::clearItems()
{
	mItems.clear();
	mSelected = 0;
	mTop = 0;
}

void MenuItem::addItem(const MenuItem& item)
{
	mItems.push_back(item);
}

bool MenuItem::hasItem() const
{
	return mItems.size() > 0;
}

MenuItem& MenuItem::selectedItem()
{
	return mItems[mSelected];
}

MenuItem& MenuItem::item(const int index)
{
	return mItems[index];
}

void MenuItem::setSelected(const int index)
{
	mSelected = index;
}

void MenuItem::setTop(const int index)
{
	mTop = index;
}
