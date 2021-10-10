/*
	Config.h - Slimmer
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

#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>

using namespace std;

class Config
{
public:
	int static processOptions(int argc, char* argv[]);

	const bool static verbose() { return mVerbose; }
	const string static lmsName() { return mLmsName; }
	const string static lmsHost() { return mLmsHost; }
	const int static lmsPort() { return mLmsPort; }
	const string static lcdHost() { return mLcdHost; }
	const int static lcdPort() { return mLcdPort; }
	const string static playerId() { return mPlayerId; }
	const bool static fixedVolume() { return mFixedVolume; }
	const bool static backMenus() { return mBackMenus; }
	const int static volume() { return mVolume; }
	const vector<string> static inputDeviceFiles() { return mInputDeviceFiles; }
	const string static encoding() { return mEncoding; }
	const int static scrollSpeed() { return mScrollSpeed; }

protected:
	string static getMacAddress();
	void static discoverLMS();

public:
	static const short int cRetryDelay; // In seconds
	static const double cPlayerStatusQueryInterval; // Double in seconds
	static const double cPlayerStatusQueryIntervalInStandby; // Double in seconds
	static const double cVolumeScreenHideDelay; // Double in seconds
	static const double cSeekScreenHideDelay; // Double in seconds
	static const double cMenuScreenHideDelay; // Double in seconds
	static const double cPopupHideDelay; // Double in seconds
	static const double cButtonLongPressTime; // Double in seconds
	static const short int cVolumeStep; // In percent
	static const short int cSeekStep; // In percent
	static const short int cNewMusicItems;
	static const short int cTrackRestartLimit; // In seconds
	static const double cStandbyTimeout; // Double in seconds

protected:
	bool static mVerbose;
	string static mLmsName;
	string static mLmsHost;
	int static mLmsPort;
	string static mLcdHost;
	int static mLcdPort;
	string static mPlayerId;
	bool static mFixedVolume;
	bool static mBackMenus;
	int static mVolume;
	vector<string> static mInputDeviceFiles;
	string static mEncoding;
	int static mScrollSpeed;
};

#endif // CONFIG_H
