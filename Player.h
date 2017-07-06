/*
	Player.h - Slimmer
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

#ifndef PLAYER_H
#define PLAYER_H

#include "Server.h"
#include <string>
#include "json/json.h"

using namespace std;

class Player
{
public:
	enum Mode {Play, Stop, Pause, Unknown};

	Player(string aId, Server& aServer);
	void update(const bool fullPlaylist = false);

	void setVolume(const int volume);
	int increaseVolume();
	int decreaseVolume();

	void next();
	void previous();
	void playPlaylistItem(const int index);
	void playFavorite(const string& id);
	void playlistControl(const Server::PlaylistControlCommand cmd, const Server::PlaylistControlType type, const string& id);
	void pause();

	void removePlaylistItem(const int index);
	void clearPlaylist();

	Json::Value radios(const string& plugin, const string& id = "");
	void playRadio(const Server::PlaylistControlCommand cmd, const string& plugin, const string& id);

	string id() const { return mId; }

	Mode mode() const { return mMode; }
	bool playing() const { return mMode == Mode::Play; }
	short playlistSize() const { return mPlaylistSize; }
	short playlistIndex() const { return mPlaylistIndex; }

	bool remote() const { return mRemote; }
	string remoteTitle() const { return mRemoteTitle; }
	string artist() const { return mArtist; }
	string album() const { return mAlbum; }
	string title() const { return mTitle; }
	float duration() const { return mDuration; }
	float time() const { return mTime; }

	int volume() const { return mVolume; }
	int repeat() const { return mRepeat; }
	int shuffle() const { return mShuffle; }

	bool powered() const { return mPowered; }

	Json::Value playlist() const { return mPlaylist; }

protected:
	string mId;
	Server& mServer;

	Mode mMode;
	short int mPlaylistSize;
	short int mPlaylistIndex;

	bool mRemote;
	string mRemoteTitle;
	string mArtist;
	string mAlbum;
	string mTitle;
	float mDuration;
	float mTime;

	int mVolume;
	int mRepeat;
	int mShuffle;

	bool mPowered;

	Json::Value mPlaylist;
};

#endif // PLAYER_H
