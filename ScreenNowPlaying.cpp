/*
	ScreenNowPlaying.cpp - Slimmer
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

#include "ScreenNowPlaying.h"
#include <math.h>

ScreenNowPlaying::ScreenNowPlaying(LCDClient* parent) : Screen(parent, "NowPlaying", "NowPlaying")
{
	setHeartBeat(LCD_HEARTBEAT_OFF);

	mMode.move(1, 1);
	mTime.move(3, 1);
	mPlaylist.move(width()-5, 1);
	mLine2.set("", 1, 2, width(), 2, cScrollingSpeed, LCDScroller::Marquee);
	if (height() > 2)
		mLine3.set("", 1, 3, width(), 3, cScrollingSpeed, LCDScroller::Marquee);
	if (height() > 3)
		mLine4.set("", 1, 4, width(), 4, cScrollingSpeed, LCDScroller::Marquee);

	add(&mMode);
	add(&mTime);
	add(&mPlaylist);
	add(&mLine2);
	if (height() > 2) add(&mLine3);
	if (height() > 3) add(&mLine4);
}

void ScreenNowPlaying::update(const Player& player)
{
	// MODE
	string s = "?";
	if (player.mode() == Player::Play)
		s = ">";
	else if (player.mode() == Player::Stop)
		s = "-";
	else if (player.mode() == Player::Pause)
		s = "=";
	mMode.setText(s);

	// PLAYLIST INDEX AND SIZE
	string pls;
	if (player.playlistSize() == 0)
		pls = "0/0";
	else
		pls = std::to_string(player.playlistIndex() + 1) + "/" +  std::to_string(player.playlistSize());
	mPlaylist.move(width() - pls.length() + 1);
	mPlaylist.setText(pls);

	// PLAYING TIME
	s = std::to_string((int)(player.time() / 60.0f)) + ":";
	string sec = std::to_string((int)(fmod(player.time(), 60.0f)));
	sec.insert(0, 2 - sec.length(), '0');
	s += sec;
	if (player.duration() > 0)
	{
		string durs = "/" + std::to_string((int)(player.duration() / 60.0f)) + ":";
		sec = std::to_string((int)(fmod(player.duration(), 60.0f)));
		sec.insert(0, 2 - sec.length(), '0');
		durs += sec;
		if (3 + s.length() + durs.length() + pls.length() <= width()) s += durs;
	}
	mTime.setText(s);

	// EMPTY QUEUE
	if (player.playlistSize() == 0)
	{
		s = string(width() > cEmptyQueueTextLength ? width() / 2 - cEmptyQueueTextLength / 2 : 0, ' ') + cEmptyQueueText;
		if (height() > 3)
		{
			mLine2.setText("");
			mLine3.setText(s);
			mLine4.setText("");
		}
		else
			mLine2.setText(s);
		return;
	}

	// SONG INFO
	if (!player.remote())
	{
		if (height() > 3)
		{
			mLine2.setText(trail(toLCDString(player.artist())));
			mLine3.setText(trail(toLCDString(player.album())));
		}
		else
		{
			s = "";
			if (player.artist().length() > 0) s += player.artist() + ": ";
			if (player.album().length() > 0) s += player.album() + " - ";
			if (player.title().length() > 0) s += player.title();
			mLine2.setText(trail(toLCDString(s)));
		}
	}
	else
	{
		if (height() > 3)
		{
			mLine2.setText(trail(toLCDString(player.remoteTitle())));
			mLine3.setText(trail(toLCDString(player.artist())));
		}
		else
		{
			s = "";
			if (player.artist().length() > 0) s += player.artist() + ": ";
			if (player.title().length() > 0) s += player.title() + " ";
			s += "[" + player.remoteTitle() + "]";
			mLine2.setText(trail(toLCDString(s)));
		}
	}
	if (height() > 3) mLine4.setText(trail(toLCDString(player.title())));
}
