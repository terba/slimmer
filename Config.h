/*
	Config.h - Slimmer
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

#ifndef CONFIG_H
#define CONFIG_H

#include <string>

using namespace std;

class Config
{
public:
	int static processOptions(int argc, char* argv[]);

	const bool static verbose() { return mVerbose; }
	const string static lmsHost() { return mLmsHost; }
	const int static lmsPort() { return mLmsPort; }
	const string static lcdHost() { return mLcdHost; }
	const int static lcdPort() { return mLcdPort; }
	const string static playerId() { return mPlayerId; }
	const bool static fixedVolume() { return mFixedVolume; }
	const int static volume() { return mVolume; }
	const string static inputDeviceFile() { return mInputDeviceFile; }
	const string static encoding() { return mEncoding; }

protected:
	string static getMacAddress();

public:
	static const short int cRetryDelay; // In seconds
	static const double cPlayerStatusQueryInterval; // Double in seconds
	static const double cVolumeScreenHideDelay; // Double in seconds
	static const double cMenuScreenHideDelay; // Double in seconds
	static const double cPopupHideDelay; // Double in seconds
	static const double cButtonLongPressTime; // Double in seconds
	static const short int cVolumeStep; // In percent
	static const short int cNewMusicItems;
	static const short int cTrackRestartLimit; // In seconds

protected:
	bool static mVerbose;
	string static mLmsHost;
	int static mLmsPort;
	string static mLcdHost;
	int static mLcdPort;
	string static mPlayerId;
	bool static mFixedVolume;
	int static mVolume;
	string static mInputDeviceFile;
	string static mEncoding;
};

#endif // CONFIG_H
