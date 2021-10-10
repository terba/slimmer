/*
	Controller.h - Slimmer
	Copyright (C) 2016-2017  Terényi, Balázs (terenyi@freemail.hu): Original Implmentation
	Copyright (C) 2021  Aaron White <w531t4@gmail.com>: Added Seek capability

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

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <vector>
#include <lcdapi/include/LCDHeaders.h>
#define EV_STANDALONE 1
#include "libev/ev++.h"
#include "EventHandler.h"
#include "Server.h"
#include "Player.h"
#include "Button.h"
#include "ScreenNowPlaying.h"
#include "ScreenError.h"
#include "ScreenMenu.h"
#include "ScreenVolume.h"
#include "ScreenSeek.h"

class Controller : public EventHandler
{
public:
	Controller();
	~Controller();
	void run();
	void handleEvent(const EventHandler::Event event);

protected:
	void updateStatus(ev::timer& w, int revents);
	void readInput(ev::io& w, int revents);
	void hideSeekScreen();
	void hideVolumeScreen();
	void hideMenuScreen();
	void showPopup(const std::string message);
	void hidePopup();
	void startStandby();
	void stopStandby();

protected:
	void actionShowQueue(MenuItem& selected);
	void actionPlayQueueItem(MenuItem& selected);
	void actionShowArtists(MenuItem& selected);
	void actionShowAlbums(MenuItem& selected);
	void actionShowNewMusic(MenuItem& selected);
	void actionShowFolder(MenuItem& selected);
	void actionShowFavorites(MenuItem& selected);
	void actionPlayFavorite(MenuItem& selected);
	void actionShowRadios(MenuItem& selected);
	void actionPlayRadio(MenuItem& selected, Server::PlaylistControlCommand command);
	void actionShowArtistAlbums(MenuItem& selected);
	void actionShowAlbumTracks(MenuItem& selected);
	void actionClearQueue(MenuItem& selected);
	void actionLoad(MenuItem& selected, Server::PlaylistControlCommand command);
	void actionRemoveQueueItem(MenuItem& selected);

	void setBacklight(const bool on);

protected:
	static const constexpr char* cEmptyQueueText = "Queue is empty";

	lcdapi::LCDClient mLcd;
	Server mServer;
	Player mPlayer;
	ScreenNowPlaying mNowPlayingScreen;
	ScreenError mErrorScreen;
	ScreenMenu mMenuScreen;
	ScreenVolume mVolumeScreen;
	ScreenSeek mSeekScreen;

	vector<ev::io*> mInputDevicesIo;

	ev::timer mStatusUpdateTimer;
	ev::timer mVolumeScreenHideTimer;
	ev::timer mSeekScreenHideTimer;
	ev::timer mMenuScreenHideTimer;
	ev::timer mPopupHideTimer;
	ev::timer mStandbyTimer;
	vector<int> mInputDeviceFileDescriptors;

	vector<Button*> mButtons;

	bool mInStandby;
};

#endif // CONTROLLER_H
