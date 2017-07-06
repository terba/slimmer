/*
	Player.cpp - Slimmer
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

#include "Player.h"
#include "Config.h"
#include <iostream>

Player::Player(string aId, Server& aServer) : mId(aId), mServer(aServer)
{
}

void Player::update(const bool fullPlaylist)
{
	Json::Value response = mServer.playerStatus(mId, fullPlaylist);

	if (Config::verbose())
		cout << "Status: " << response.toStyledString() << endl;

	try
	{
		string s = response["mode"].asString();
		if (s == "play")
			mMode = Play;
		else if (s == "stop")
			mMode = Stop;
		else if (s == "pause")
			mMode = Pause;
		else
			mMode = Unknown;

		mPowered = response["power"].asInt() == 1;

		mPlaylistSize = response["playlist_tracks"].asInt();
		s = response["playlist_cur_index"].asString();
		if (s.length() > 0)
			mPlaylistIndex = stoi(s);
		else
			mPlaylistIndex = 0;

		int loopIndex = fullPlaylist ? mPlaylistIndex : 0;

		mRemote = response["playlist_loop"][loopIndex]["remote"].asString() == "1";
		mRemoteTitle = response["playlist_loop"][loopIndex]["remote_title"].asString();
		mArtist = response["playlist_loop"][loopIndex]["artist"].asString();
		mAlbum = response["playlist_loop"][loopIndex]["album"].asString();
		mTitle = response["playlist_loop"][loopIndex]["title"].asString();
		mDuration = response["duration"].asFloat();
		mTime = response["time"].asFloat();

		mVolume = response["mixer volume"].asInt();
		mRepeat = response["playlist repeat"].asInt();
		mShuffle = response["playlist shuffle"].asInt();

		if (fullPlaylist)
			mPlaylist = mPlaylistSize == 0 ? Json::arrayValue : response["playlist_loop"];
	}
	catch (const Json::LogicError& e)
	{
		cerr << "[ERROR] Status: " << response.toStyledString() << endl;
		cerr << "[ERROR] Invalid JSON response from server: " << e.what() << endl;
	}
}

void Player::setVolume(const int volume)
{
	mServer.setPlayerVolume(mId, std::to_string(volume));
}

int Player::increaseVolume()
{
	mServer.setPlayerVolume(mId, "+" + std::to_string(Config::cVolumeStep));
	mVolume += Config::cVolumeStep;
	if (mVolume > 100) mVolume = 100;
	return mVolume;
}

int Player::decreaseVolume()
{
	mServer.setPlayerVolume(mId, "-" + std::to_string(Config::cVolumeStep));
	mVolume -= Config::cVolumeStep;
	if (mVolume < 0) mVolume = 0;
	return mVolume;
}

void Player::next()
{
	mServer.playPlaylistItem(mId, "+1");
}

void Player::previous()
{
	if (!mRemote && mTime > Config::cTrackRestartLimit)
	{
		// We restart the current track if it is played for more than cTrackRestartLimit
		// seconds instead of jumping to the previous track.
		this->playPlaylistItem(mPlaylistIndex);
	}
	else
		mServer.playPlaylistItem(mId, "-1");
}

void Player::playPlaylistItem(const int index)
{
	mServer.playPlaylistItem(mId, std::to_string(index));
}

void Player::playFavorite(const string& id)
{
	mServer.playFavorite(mId, id);
}

void Player::playlistControl(const Server::PlaylistControlCommand cmd, const Server::PlaylistControlType type, const string& id)
{
	mServer.playlistControl(mId, cmd, type, id);
}

void Player::pause()
{
	mServer.pause(mId);
}

void Player::removePlaylistItem(const int index)
{
	mServer.removePlaylistItem(mId, index);
}

void Player::clearPlaylist()
{
	mServer.clearPlaylist(mId);
}

Json::Value Player::radios(const string& plugin, const string& id)
{
	return mServer.radios(mId, plugin, id);
}

void Player::playRadio(const Server::PlaylistControlCommand cmd, const string& plugin, const string& id)
{
	mServer.playRadio(mId, cmd, plugin, id);
}
