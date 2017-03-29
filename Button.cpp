/*
	Button.cpp - Slimmer
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

#include "Button.h"
#include "Config.h"

Button::Button(EventHandler* const parent, const unsigned short key, const EventHandler::Event shortEvent, const EventHandler::Event longEvent, const EventHandler::Event repeatEvent) :
	mParent(parent), mKey(key), mShortEvent(shortEvent), mLongEvent(longEvent), mRepeatEvent(repeatEvent)
{
	if (mLongEvent)
		mPressTimer.set<Button, &Button::longPressTimeout>(this);
}

void Button::handleKey(const ButtonEvent event, const unsigned short key)
{
	if (key != mKey) return;
	switch (event)
	{
	case PRESS:
		if (mLongEvent)
			mPressTimer.start(Config::cButtonLongPressTime);
		else
			mParent->handleEvent(mShortEvent);
		break;
	case RELEASE:
		if (mLongEvent && mPressTimer.is_active())
		{
			mPressTimer.stop();
			mParent->handleEvent(mShortEvent);
		}
		break;
	case REPEAT:
		if (mRepeatEvent)
			mParent->handleEvent(mRepeatEvent);
		break;
	}
}

void Button::longPressTimeout()
{
	mParent->handleEvent(mLongEvent);
}
