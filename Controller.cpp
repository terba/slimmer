/*
	Controller.cpp - Slimmer
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

#include "Controller.h"
#include "Config.h"
#include "ErrorInput.h"
#include <iostream>
#include <thread>
#include <jsonrpccpp/common/exception.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unicode/ucnv.h>

Controller::Controller()
	: mLcd(Config::lcdHost(), Config::lcdPort()),
	  mServer("http://" + Config::lmsHost() + ":" + to_string(Config::lmsPort()) + "/jsonrpc.js"),
	  mNowPlayingScreen(&mLcd),
	  mErrorScreen(&mLcd),
	  mMenuScreen(&mLcd, mServer.version()),
	  mPlayer(Config::playerId(), mServer),
	  mVolumeScreen(&mLcd),
	  mSeekScreen(&mLcd)
{
	// We disable syncing in LCDAPI, so it will not wait for command completion and response from LCDd.
	// Refreshing the display is many times faster with this, so fast browsing in the menus is now OK.
	mLcd.disableSync();

	mButtons.push_back(new Button(this, KEY_ENTER, SELECT, SELECTLONG));
	mButtons.push_back(new Button(this, KEY_LEFT, LEFT, NONE, LEFT));
	mButtons.push_back(new Button(this, KEY_RIGHT, RIGHT, NONE, RIGHT));
	mButtons.push_back(new Button(this, KEY_BACKSPACE, BACK, BACKLONG));
	mButtons.push_back(new Button(this, KEY_SPACE, FORWARD));

	for (string const i_file : Config::inputDeviceFiles())
	{
		int temp;
		if ((temp = open(i_file.c_str(), O_RDONLY | O_NONBLOCK)) == -1)
			throw ErrorInput("Can not open device file " + i_file);
		mInputDeviceFileDescriptors.push_back(temp);
	}

	for (int const i_fd : mInputDeviceFileDescriptors)
	{
		ev::io* mInputDeviceIoP = new ev::io();
		mInputDeviceIoP->set<Controller, &Controller::readInput>(this);
		mInputDeviceIoP->start(i_fd, ev::READ);
		mInputDevicesIo.push_back(mInputDeviceIoP);
	}

	mStatusUpdateTimer.set<Controller, &Controller::updateStatus>(this);
	mStatusUpdateTimer.start(Config::cPlayerStatusQueryInterval, Config::cPlayerStatusQueryInterval);

	mVolumeScreenHideTimer.set<Controller, &Controller::hideVolumeScreen>(this);
	mSeekScreenHideTimer.set<Controller, &Controller::hideSeekScreen>(this);
	mMenuScreenHideTimer.set<Controller, &Controller::hideMenuScreen>(this);
	mPopupHideTimer.set<Controller, &Controller::hidePopup>(this);

	mStandbyTimer.set<Controller, &Controller::startStandby>(this);
	mStandbyTimer.start(Config::cStandbyTimeout);

	if (Config::verbose())
	{
		cout << "Connected to LCDd ("
			 << "Version: " << mLcd.getServerVersion()
			 << ", Protocol: " << mLcd.getProtocolVersion()
			 << ", Width: " << mLcd.getWidth()
			 << ", Height: " << mLcd.getHeight()
			 << ", Char width: " << mLcd.getCharWidth()
			 << ", Char height: " << mLcd.getCharHeight() << ")" << endl;
		cout << "LMS version: " + mServer.version() << endl;
	}

	updateStatus(mStatusUpdateTimer, 0);
	setBacklight(true);
	mInStandby = false;
}

Controller::~Controller()
{
	for (ev::io* mInputDevice : mInputDevicesIo)
	{
		mInputDevice->stop();
		delete mInputDevice;
	}
	mStatusUpdateTimer.stop();
	mVolumeScreenHideTimer.stop();
	mSeekScreenHideTimer.stop();
	mMenuScreenHideTimer.stop();
	mStandbyTimer.stop();
	for (int i_fd : mInputDeviceFileDescriptors)
		close(i_fd);
	for (Button* button : mButtons) delete button;
}

void Controller::updateStatus(ev::timer& w, int revents)
{
	try
	{
		mPlayer.update(false);
		if (mErrorScreen.visible())
			mErrorScreen.hide();
		mNowPlayingScreen.update(mPlayer);
		if (mPlayer.playing())
			stopStandby();
	}
	catch (const jsonrpc::JsonRpcException& e)
	{
		string message;
		if (e.GetMessage().find("libcurl error: 52") != string::npos)
			message = "Player " + mPlayer.id() + " is unknown at the server";
		else if (e.GetMessage().find("libcurl error: 7") != string::npos)
			message = "No connection to LMS server at " + Config::lmsHost() + ":" + to_string(Config::lmsPort());
		else if (e.GetMessage().find("libcurl error: 28") != string::npos)
			message = "Slow connection to LMS server at " + Config::lmsHost() + ":" + to_string(Config::lmsPort());
		else
			message = e.what();
		hideMenuScreen();
		mErrorScreen.setText(message);
		mErrorScreen.show();
		if (Config::verbose()) cerr << message << endl;
	}
}

void Controller::run()
{
	try
	{
		if (Config::volume() != -1) mPlayer.setVolume(Config::volume());
	}
	catch (const jsonrpc::JsonRpcException& e)
	{
		cerr << "[ERROR] Startup volume can not be set: " << e.what() << endl;
	}
	ev::default_loop loop;
	loop.run(0);
}

void Controller::handleEvent(const Event event)
{
	if (Config::verbose())
		cout << "Event: " << event << endl;

	stopStandby();
	bool menuWasVisible = mMenuScreen.visible();

	try
	{
		// Seek management
		if ((event == LEFT || event == RIGHT) && mSeekScreen.visible())
		{
			int position = event == LEFT ? mPlayer.seekBehind() : mPlayer.seekAhead();
			mSeekScreen.update(position);
			mSeekScreenHideTimer.start(Config::cSeekScreenHideDelay);
		}

		// Volume management
		if (!Config::fixedVolume() && (event == LEFT || event == RIGHT) && !mMenuScreen.visible() && !mSeekScreen.visible())
		{
			int volume = event == LEFT ? mPlayer.decreaseVolume() : mPlayer.increaseVolume();
			mVolumeScreen.update(volume);
			if (!mVolumeScreen.visible())
				mVolumeScreen.show();
			mVolumeScreenHideTimer.start(Config::cVolumeScreenHideDelay);
		}

		// Show menu
		if (!mMenuScreen.visible() && event == SELECT)
		{
			if (mVolumeScreen.visible()) mVolumeScreen.hide();
			if (mSeekScreen.visible())
			{
				mSeekScreenHideTimer.stop();
				mPlayer.seek(mPlayer.timetarget());
				mSeekScreen.hide();
			} else {
				mMenuScreen.show();
				mMenuScreenHideTimer.start(Config::cMenuScreenHideDelay);
			}
		}

		// Prev/Next song
		if (!mMenuScreen.visible() && (event == BACK || event == FORWARD))
		{
			if (event == BACK)
				mPlayer.previous();
			else
				mPlayer.next();
		}

		// Pause/Resume
		if (!mMenuScreen.visible() && event == SELECTLONG)
		{
			mPlayer.pause();
		}

		// Menu navigation
		if (menuWasVisible)
		{
			mMenuScreenHideTimer.start(Config::cMenuScreenHideDelay);
			switch (event)
			{
			case LEFT:
				mMenuScreen.navigateUp();
				break;
			case RIGHT:
				mMenuScreen.navigateDown();
				break;
			case SELECT:
			{
				MenuItem& selected = mMenuScreen.selectedItem();
				switch (selected.type())
				{
				case MenuItem::QUEUE:
					actionShowQueue(selected);
					break;
				case MenuItem::QUEUEITEM:
					actionPlayQueueItem(selected);
					break;
				case MenuItem::ALBUMARTISTS:
				case MenuItem::ARTISTS:
					actionShowArtists(selected);
					break;
				case MenuItem::ALBUMS:
					actionShowAlbums(selected);
					break;
				case MenuItem::NEWMUSIC:
					actionShowNewMusic(selected);
					break;
				case MenuItem::FOLDERS:
				case MenuItem::FOLDER:
					actionShowFolder(selected);
					break;
				case MenuItem::FAVORITES:
					actionShowFavorites(selected);
					break;
				case MenuItem::FAVORITE:
					actionPlayFavorite(selected);
					break;
				case MenuItem::RADIOS:
				case MenuItem::RADIOPLUGIN:
				case MenuItem::RADIOMENU:
					actionShowRadios(selected);
					break;
				case MenuItem::RADIO:
					actionPlayRadio(selected, Server::LOAD);
					break;
				case MenuItem::ARTIST:
					actionShowArtistAlbums(selected);
					break;
				case MenuItem::ALBUM:
					actionShowAlbumTracks(selected);
					break;
				case MenuItem::TRACK:
					actionLoad(selected, Server::LOAD);
					break;
				case MenuItem::BACK:
					if (mMenuScreen.level() == 1)
					{
						mMenuScreenHideTimer.stop();
						mMenuScreen.hide();
					}
					else
						mMenuScreen.back();
					break;
				case MenuItem::SEEK:
					mSeekScreen.update((mPlayer.seekReset() * 100) / mPlayer.duration());
					if (!mSeekScreen.visible())
					{
						mMenuScreen.hide();
						mSeekScreen.show();
					}
					mSeekScreenHideTimer.start(Config::cSeekScreenHideDelay);
					break;
				default:
					mMenuScreen.select();
				}
				break;
			}
			case SELECTLONG:
			{
				MenuItem& selected = mMenuScreen.selectedItem();
				switch (selected.type())
				{
				case MenuItem::QUEUE:
					actionClearQueue(selected);
					break;
				case MenuItem::QUEUEITEM:
					actionPlayQueueItem(selected);
					break;
				case MenuItem::ARTISTS:
					actionShowArtists(selected);
					break;
				case MenuItem::ALBUMS:
					actionShowAlbums(selected);
					break;
				case MenuItem::NEWMUSIC:
					actionShowNewMusic(selected);
					break;
				case MenuItem::FOLDERS:
					actionShowFolder(selected);
					break;
				case MenuItem::FAVORITES:
					actionShowFavorites(selected);
					break;
				case MenuItem::FAVORITE:
					actionPlayFavorite(selected);
					break;
				case MenuItem::RADIOS:
				case MenuItem::RADIOPLUGIN:
					actionShowRadios(selected);
					break;
				case MenuItem::RADIOMENU:
				case MenuItem::RADIO:
					actionPlayRadio(selected, Server::LOAD);
					break;
				case MenuItem::ARTIST:
				case MenuItem::ALBUM:
				case MenuItem::TRACK:
				case MenuItem::FOLDER:
					actionLoad(selected, Server::LOAD);
					break;
				case MenuItem::BACK:
					mMenuScreenHideTimer.stop();
					mMenuScreen.hide();
					break;
				default:
					mMenuScreen.select();
				}
				break;
			}
			case BACK:
				if (mMenuScreen.level() == 1)
				{
					mMenuScreenHideTimer.stop();
					mMenuScreen.hide();
				}
				else
					mMenuScreen.back();
				break;
			case BACKLONG:
				mMenuScreen.backToMainMenu();
				break;
			case FORWARD:
				MenuItem& selected = mMenuScreen.selectedItem();
				switch (selected.type())
				{
				case MenuItem::QUEUEITEM:
					actionRemoveQueueItem(selected);
					break;
				case MenuItem::FAVORITE: // TODO: Add favorite to the queue
					break;
				case MenuItem::ARTIST:
				case MenuItem::ALBUM:
				case MenuItem::TRACK:
				case MenuItem::FOLDER:
					actionLoad(selected, Server::ADD);
					break;
				case MenuItem::RADIOMENU:
				case MenuItem::RADIO:
					actionPlayRadio(selected, Server::ADD);
					break;
				}
				break;
			}
		}
	}
	catch (const jsonrpc::JsonRpcException& e)
	{
		mMenuScreen.progressOff();
		cerr << "[ERROR] " << e.what() << endl;
	}
}

void Controller::hideVolumeScreen()
{
	mVolumeScreen.hide();
}

void Controller::hideSeekScreen()
{
	mSeekScreen.hide();
}

void Controller::hideMenuScreen()
{
	mMenuScreen.hide();
}

void Controller::showPopup(const string message)
{
	mMenuScreen.showPopup(message);
	mPopupHideTimer.start(Config::cPopupHideDelay);
}

void Controller::hidePopup()
{
	mMenuScreen.hidePopup();
}

void Controller::startStandby()
{
	mInStandby = true;
	setBacklight(false);
	mStatusUpdateTimer.set(Config::cPlayerStatusQueryIntervalInStandby, Config::cPlayerStatusQueryIntervalInStandby);
}

void Controller::stopStandby()
{
	if (mInStandby)
	{
		mInStandby = false;
		setBacklight(true);
		mStatusUpdateTimer.set(Config::cPlayerStatusQueryInterval, Config::cPlayerStatusQueryInterval);
	}
	mStandbyTimer.start(Config::cStandbyTimeout);
}

void Controller::readInput(ev::io& w, int revents)
{
	struct input_event inputEvent;
	int readSize;

	for (int const i_fd : mInputDeviceFileDescriptors)
	{
		while ((readSize = read(i_fd, &inputEvent, sizeof(inputEvent))) > 0)
		{
			if (readSize == sizeof(inputEvent) && inputEvent.type == EV_KEY && (inputEvent.value == 1 || inputEvent.value == 0 || inputEvent.value == 2))
			{
				Button::ButtonEvent buttonEvent = inputEvent.value == 1 ? Button::PRESS : inputEvent.value == 0 ? Button::RELEASE : Button::REPEAT;
				for (Button* const button : mButtons)
					button->handleKey(buttonEvent, inputEvent.code);
			} else if (readSize == sizeof(inputEvent) && inputEvent.type == EV_REL && (inputEvent.value == 1 || inputEvent.value == -1))
			{
				if (Config::verbose())
					cout << "Matched EV_SYN event. inputEvent.value=" << inputEvent.value << " inputEvent.code=" << inputEvent.code << endl;
				if (inputEvent.value == 1)
				{
					if (Config::verbose())
						cout << "Matched inputEvent.value == 1" << endl;
					for (Button* const button : mButtons)
					{
						button->handleKey((Button::ButtonEvent) Button::PRESS, KEY_LEFT);
						button->handleKey((Button::ButtonEvent) Button::RELEASE, KEY_LEFT);
					}
				} else if (inputEvent.value == -1)
				{
					if (Config::verbose())
						cout << "Matched inputEvent.value == -1" << endl;
					for (Button* const button : mButtons)
					{
						button->handleKey((Button::ButtonEvent) Button::PRESS, KEY_RIGHT);
						button->handleKey((Button::ButtonEvent) Button::RELEASE, KEY_RIGHT);
					}
				}
			}
		}
		if (readSize == -1 && errno != EAGAIN && errno != EINTR)
		{
			cerr << "[ERROR] Input device read error" << endl;
		}
	}

}

void Controller::actionShowQueue(MenuItem& selected)
{
	mMenuScreen.progressOn();
	mPlayer.update(true);
	if (mPlayer.playlist().size())
	{
		selected.clearItems();
		if (Config::backMenus())
			selected.addItem(MenuItem("", MenuItem::BACK, "Back"));
		for (int i = 0; i < mPlayer.playlist().size(); i++)
			selected.addItem(MenuItem(std::to_string(i), MenuItem::QUEUEITEM, mPlayer.playlist()[i]["title"].asString()));
		mMenuScreen.select();
	}
	else
		showPopup(cEmptyQueueText);
	mMenuScreen.progressOff();
}

void Controller::actionPlayQueueItem(MenuItem& selected)
{
	showPopup("Playing");
	mPlayer.playPlaylistItem(std::stoi(selected.id()));
}

void Controller::actionShowArtists(MenuItem& selected)
{
	mMenuScreen.progressOn();
	Json::Value artists = mServer.artists(selected.type() == MenuItem::ALBUMARTISTS);
	selected.clearItems();
	if (Config::backMenus())
		selected.addItem(MenuItem("", MenuItem::BACK, "Back"));
	for (int i = 0; i < artists.size(); i++)
		selected.addItem(MenuItem(artists[i]["id"].asString(), MenuItem::ARTIST, artists[i]["artist"].asString()));
	mMenuScreen.select();
	mMenuScreen.progressOff();
}

void Controller::actionShowAlbums(MenuItem& selected)
{
	mMenuScreen.progressOn();
	Json::Value albums = mServer.albums();
	selected.clearItems();
	if (Config::backMenus())
		selected.addItem(MenuItem("", MenuItem::BACK, "Back"));
	for (int i = 0; i < albums.size(); i++)
		selected.addItem(MenuItem(albums[i]["id"].asString(), MenuItem::ALBUM, albums[i]["album"].asString(), true));
	mMenuScreen.select();
	mMenuScreen.progressOff();
}

void Controller::actionShowNewMusic(MenuItem& selected)
{
	mMenuScreen.progressOn();
	Json::Value albums = mServer.newAlbums();
	selected.clearItems();
	if (Config::backMenus())
		selected.addItem(MenuItem("", MenuItem::BACK, "Back"));
	for (int i = 0; i < albums.size(); i++)
		selected.addItem(MenuItem(albums[i]["id"].asString(), MenuItem::ALBUM, albums[i]["album"].asString(), true));
	mMenuScreen.select();
	mMenuScreen.progressOff();
}

void Controller::actionShowFolder(MenuItem& selected)
{
	mMenuScreen.progressOn();
	Json::Value folders = mServer.folders(selected.id());
	selected.clearItems();
	if (Config::backMenus())
		selected.addItem(MenuItem("", MenuItem::BACK, "Back"));
	for (int i = 0; i < folders.size(); i++)
		selected.addItem(MenuItem(folders[i]["id"].asString(), folders[i]["type"] == "folder" ? MenuItem::FOLDER : MenuItem::TRACK, folders[i]["filename"].asString()));
	mMenuScreen.select();
	mMenuScreen.progressOff();
}

void Controller::actionShowFavorites(MenuItem& selected)
{
	mMenuScreen.progressOn();
	Json::Value favorites = mServer.favorites();
	selected.clearItems();
	if (Config::backMenus())
		selected.addItem(MenuItem("", MenuItem::BACK, "Back"));
	for (int i = 0; i < favorites.size(); i++)
		selected.addItem(MenuItem(favorites[i]["id"].asString(), MenuItem::FAVORITE, favorites[i]["name"].asString()));
	mMenuScreen.select();
	mMenuScreen.progressOff();
}

void Controller::actionPlayFavorite(MenuItem& selected)
{
	showPopup("Playing");
	mPlayer.playFavorite(selected.id());
}

void Controller::actionShowRadios(MenuItem& selected)
{
	mMenuScreen.progressOn();
	switch (selected.type())
	{
		case MenuItem::RADIOS:
		{
			Json::Value radios = mServer.radioPlugins();
			selected.clearItems();
			if (Config::backMenus())
				selected.addItem(MenuItem("", MenuItem::BACK, "Back"));
			for (int i = 0; i < radios.size(); i++)
				if (radios[i]["type"] == "xmlbrowser")
					selected.addItem(MenuItem(radios[i]["cmd"].asString(), MenuItem::RADIOPLUGIN, radios[i]["name"].asString()));
		} break;
		case MenuItem::RADIOPLUGIN:
		{
			Json::Value radios = mPlayer.radios(selected.id());
			selected.clearItems();
			if (Config::backMenus())
				selected.addItem(MenuItem("", MenuItem::BACK, "Back"));
			for (int i = 0; i < radios.size(); i++)
				selected.addItem(MenuItem(radios[i]["id"].asString(), MenuItem::RADIOMENU, radios[i]["name"].asString()));
		} break;
		case MenuItem::RADIOMENU:
		{
			Json::Value radios = mPlayer.radios(mMenuScreen.pathItem(2).id(), selected.id());
			selected.clearItems();
			if (Config::backMenus())
				selected.addItem(MenuItem("", MenuItem::BACK, "Back"));
			for (int i = 0; i < radios.size(); i++)
				selected.addItem(MenuItem(radios[i]["id"].asString(),
								radios[i]["hasitems"].asInt() == 1 ? MenuItem::RADIOMENU : MenuItem::RADIO,
								radios[i]["name"].asString().empty() ? radios[i]["title"].asString() : radios[i]["name"].asString()));
		} break;
	}
	mMenuScreen.select();
	mMenuScreen.progressOff();
}

void Controller::actionPlayRadio(MenuItem& selected, Server::PlaylistControlCommand command)
{
	showPopup(command == Server::ADD ? "Added" : "Loaded");
	mPlayer.playRadio(command, "music", selected.id());
}

void Controller::actionShowArtistAlbums(MenuItem& selected)
{
	mMenuScreen.progressOn();
	Json::Value albums = mServer.albums(selected.id());
	selected.clearItems();
	if (Config::backMenus())
		selected.addItem(MenuItem("", MenuItem::BACK, "Back"));
	for (int i = 0; i < albums.size(); i++)
		selected.addItem(MenuItem(albums[i]["id"].asString(), MenuItem::ALBUM, albums[i]["album"].asString(), true));
	mMenuScreen.select();
	mMenuScreen.progressOff();
}

void Controller::actionShowAlbumTracks(MenuItem& selected)
{
	mMenuScreen.progressOn();
	Json::Value tracks = mServer.tracks(selected.id());
	selected.clearItems();
	if (Config::backMenus())
		selected.addItem(MenuItem("", MenuItem::BACK, "Back"));
	for (int i = 0; i < tracks.size(); i++)
		selected.addItem(MenuItem(tracks[i]["id"].asString(), MenuItem::TRACK, tracks[i]["title"].asString()));
	mMenuScreen.select();
	mMenuScreen.progressOff();
}

void Controller::actionClearQueue(MenuItem& selected)
{
	showPopup("Queue cleared");
	mPlayer.clearPlaylist();
}

void Controller::actionLoad(MenuItem& selected, Server::PlaylistControlCommand command)
{
	showPopup(command == Server::ADD ? "Added" : "Loaded");

	Server::PlaylistControlType controlType;
	switch (selected.type())
	{
	case MenuItem::ARTIST:
		controlType = Server::ARTIST;
		break;
	case MenuItem::ALBUM:
		controlType = Server::ALBUM;
		break;
	case MenuItem::TRACK:
		controlType = Server::TRACK;
		break;
	case MenuItem::FOLDER:
		controlType = Server::FOLDER;
		break;
	}

	mPlayer.playlistControl(command, controlType, selected.id());
}

void Controller::actionRemoveQueueItem(MenuItem& selected)
{
	mPlayer.removePlaylistItem(std::stoi(selected.id()));
	mMenuScreen.removeSelected();
	if (mMenuScreen.itemCount() == 0)
	{
		mMenuScreen.back();
		showPopup(cEmptyQueueText);
	}
}

void Controller::setBacklight(const bool on)
{
	mLcd.setBackLight(on ? lcdapi::LCD_BACKLIGHT_ON : lcdapi::LCD_BACKLIGHT_OFF);
}
