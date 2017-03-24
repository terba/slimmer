/*
	Server.cpp - Slimmer
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

#include "Server.h"
#include "Config.h"
#include <algorithm>
#include <iostream>

Server::Server(const string& url) : mHttpClient(url), mClient(mHttpClient)
{
	Json::Value params;
	params.append("");
	Json::Value query;
	query.append("version");
	query.append("?");
	params.append(query);
	mVersion = mClient.CallMethod("slim.request", params)["_version"].asString();
}

Json::Value Server::artists(const bool albumArtists)
{
	Json::Value params;
	params.append("");
	Json::Value query;
	query.append("artists");
	query.append("0"); // <start>
	query.append(cMaxResponseItems); // <itemsPerResponse>
	if (albumArtists)
	{
		query.append("role_id:ALBUMARTIST");
	}
	params.append(query);

	return mClient.CallMethod("slim.request", params)["artists_loop"];
}

Json::Value Server::albums(const string& artistId)
{
	Json::Value params;
	params.append("");
	Json::Value query;
	query.append("albums");
	query.append("0"); // <start>
	query.append(cMaxResponseItems);
	if (!artistId.empty())
		query.append("artist_id:" + artistId);
	params.append(query);

	return mClient.CallMethod("slim.request", params)["albums_loop"];
}

Json::Value Server::newAlbums()
{
	Json::Value params;
	params.append("");
	Json::Value query;
	query.append("albums");
	query.append("0"); // <start>
	query.append(Config::cNewMusicItems);
	query.append("sort:new");
	params.append(query);

	return mClient.CallMethod("slim.request", params)["albums_loop"];
}

Json::Value Server::tracks(const string& albumId)
{
	Json::Value params;
	params.append("");
	Json::Value query;
	query.append("tracks");
	query.append("0"); // <start>
	query.append(cMaxResponseItems);
	query.append("album_id:" + albumId);
	params.append(query);

	return mClient.CallMethod("slim.request", params)["titles_loop"];
}

Json::Value Server::favorites()
{
	Json::Value params;
	params.append("");
	Json::Value query;
	query.append("favorites");
	query.append("items");
	query.append("0"); // <start>
	query.append(cMaxResponseItems); // <itemsPerResponse>
	params.append(query);

	return mClient.CallMethod("slim.request", params)["loop_loop"];
}

Json::Value Server::radioPlugins()
{
	Json::Value params;
	params.append("");
	Json::Value query;
	query.append("radios");
	query.append("0"); // <start>
	query.append(cMaxResponseItems); // <itemsPerResponse>
	params.append(query);

	return mClient.CallMethod("slim.request", params)["radioss_loop"];
}

Json::Value Server::folders(const string& folderId)
{
	Json::Value params;
	params.append("");
	Json::Value query;
	query.append("musicfolder");
	query.append("0"); // <start>
	query.append(cMaxResponseItems); // <itemsPerResponse>
	if (!folderId.empty())
		query.append("folder_id:" + folderId);
	params.append(query);

	return mClient.CallMethod("slim.request", params)["folder_loop"];
}

Json::Value Server::playerStatus(const string& playerId, const bool fullPlaylist)
{
	Json::Value params;
	params.append(playerId);
	Json::Value query;
	query.append("status");
	if (fullPlaylist)
	{
		query.append("0");
		query.append(cMaxResponseItems);
	}
	else
	{
		query.append("-");
		query.append("1");
	}
	query.append("tags:alNx");
	params.append(query);

	return mClient.CallMethod("slim.request", params);
}

void Server::setPlayerVolume(const string& playerId, const string& volume)
{
	Json::Value params;
	params.append(playerId);
	Json::Value query;
	query.append("mixer");
	query.append("volume");
	query.append(volume);
	params.append(query);

	mClient.CallMethod("slim.request", params);
}

void Server::playPlaylistItem(const string& playerId, const string& index)
{
	Json::Value params;
	params.append(playerId);
	Json::Value query;
	query.append("playlist");
	query.append("index");
	query.append(index);
	params.append(query);

	mClient.CallMethod("slim.request", params);
}

void Server::removePlaylistItem(const string& playerId, const int index)
{
	Json::Value params;
	params.append(playerId);
	Json::Value query;
	query.append("playlist");
	query.append("delete");
	query.append(index);
	params.append(query);

	mClient.CallMethod("slim.request", params);
}

void Server::clearPlaylist(const string& playerId)
{
	Json::Value params;
	params.append(playerId);
	Json::Value query;
	query.append("playlist");
	query.append("clear");
	params.append(query);

	mClient.CallMethod("slim.request", params);
}

void Server::playFavorite(const string& playerId, const string& favId)
{
	Json::Value params;
	params.append(playerId);
	Json::Value query;
	query.append("favorites");
	query.append("playlist");
	query.append("play");
	query.append("item_id:" + favId);
	params.append(query);

	mClient.CallMethod("slim.request", params);
}

void Server::pause(const string& playerId)
{
	Json::Value params;
	params.append(playerId);
	Json::Value query;
	query.append("pause");
	params.append(query);

	mClient.CallMethod("slim.request", params);
}

void Server::playlistControl(const string& playerId, const PlaylistControlCommand cmd, const PlaylistControlType type, const string& id)
{
	Json::Value params;
	params.append(playerId);
	Json::Value query;
	query.append("playlistcontrol");
	string s = "cmd:";
	switch (cmd)
	{
	case ADD:
		s += "add";
		break;
	case LOAD:
		s += "load";
		break;
	}
	query.append(s);
	switch (type)
	{
	case GENRE:
		s = "genre_id:";
		break;
	case ARTIST:
		s = "artist_id:";
		break;
	case ALBUM:
		s = "album_id:";
		break;
	case TRACK:
		s = "track_id:";
		break;
	case YEAR:
		s = "year:";
		break;
	case PLAYLIST:
		s = "playlist_id:";
		break;
	case FOLDER:
		s = "folder_id:";
		break;
	case PLAYLISTNAME:
		s = "playlist_name:";
		break;
	}
	query.append(s+ id);
	query.append("play_index:0");
	params.append(query);

	mClient.CallMethod("slim.request", params);
}

Json::Value Server::radios(const string& playerId, const string& plugin, const string& id)
{
	Json::Value params;
	params.append(playerId);
	Json::Value query;
	query.append(plugin);
	query.append("items");
	query.append("0"); // <start>
	query.append(cMaxResponseItems); // <itemsPerResponse>
	if (!id.empty())
		query.append("item_id:" + id);
	params.append(query);

	return mClient.CallMethod("slim.request", params)["loop_loop"];
}

void Server::playRadio(const string& playerId, const PlaylistControlCommand cmd, const string& plugin, const string& id)
{
	Json::Value params;
	params.append(playerId);
	Json::Value query;
	query.append(plugin);
	query.append("playlist");
	switch (cmd)
	{
	case ADD:
		query.append("add");
		break;
	case LOAD:
		query.append("load");
		break;
	}
	query.append("item_id:" + id);
	params.append(query);

	mClient.CallMethod("slim.request", params);
}
