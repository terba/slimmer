/*
	Config.cpp - Slimmer
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

#include "Config.h"
#include "ApplicationInfo.h"
#include "BuildInfo.h"

#include <unistd.h>
#include <cstring>
#include <ifaddrs.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include "tclap/CmdLine.h"

const short int Config::cRetryDelay = 1;
const double Config::cPlayerStatusQueryInterval = 0.5;
const double Config::cVolumeScreenHideDelay = 1;
const double Config::cMenuScreenHideDelay = 8;
const double Config::cPopupHideDelay = 0.6;
const double Config::cButtonLongPressTime = 0.4;
const short int Config::cVolumeStep = 2;
const short int Config::cNewMusicItems = 100;
const short int Config::cTrackRestartLimit = 3;

bool Config::mVerbose;
string Config::mLmsHost;
int Config::mLmsPort;
string Config::mLcdHost;
int Config::mLcdPort;
string Config::mPlayerId;
bool Config::mFixedVolume;
int Config::mVolume;
string Config::mInputDeviceFile;
string Config::mEncoding;

int Config::processOptions(int argc, char* argv[])
{
	using namespace TCLAP;

	CmdLine cmd(string("Copyright (C) ") + APP_COPYRIGHT_DATE + "  " + APP_COPYRIGHT_HOLDER + "\nThis program comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it under certain conditions. See LINCENSE file in the source distribution.",
				' ',
				string(APP_VERSION_NUMBER) + " (Build " + BUILD_DATE + " " + BUILD_SYSTEM + " " + BUILD_SYSTEM_PROCESSOR + ")");

	SwitchArg verboseArg("v", "verbose", "be verbose", false);
	ValueArg<string> lmshostArg("s", "lmshost", "LMS host (default: localhost)", false, "localhost", "ip or hostname");
	ValueArg<int> lmsportArg("p", "lmsport", "LMS port (default: 9000)", false, 9000, "number");
	ValueArg<string> lcdhostArg("l", "lcdhost", "lcdproc host (default: localhost)", false, "localhost", "ip or hostname");
	ValueArg<int> lcdportArg("P", "lcdport", "lcdproc port (default: 13666)", false, 13666, "number");
	ValueArg<string> macArg("m", "mac", "the player's MAC address (default: automatic, first interface)", false, "", "AA:BB:CC:DD:EE:FF");
	SwitchArg fixedvolumeArg("f", "fixedvolume", "volume control disabled", false);
	ValueArg<int> volumeArg("o", "volume", "set volume on startup", false, -1, "0-100");
	ValueArg<string> inputArg("i", "input", "keyboard input device file (default: /dev/input/event0)", false, "/dev/input/event0", "input device file");
	ValueArg<string> encodingArg("e", "encoding", "the LCD's character encoding (default: ISO-8859-1)", false, "ISO-8859-1", "single-byte encoding");

	cmd.add(encodingArg);
	cmd.add(inputArg);
	cmd.add(volumeArg);
	cmd.add(fixedvolumeArg);
	cmd.add(macArg);
	cmd.add(lcdportArg);
	cmd.add(lcdhostArg);
	cmd.add(lmsportArg);
	cmd.add(lmshostArg);
	cmd.add(verboseArg);

	cmd.parse(argc, argv);

	mVerbose = verboseArg.getValue();
	mLmsHost = lmshostArg.getValue();
	mLmsPort = lmsportArg.getValue();
	mLcdHost = lcdhostArg.getValue();
	mLcdPort = lcdportArg.getValue();
	mPlayerId = macArg.getValue();
	mFixedVolume = fixedvolumeArg.getValue();
	mVolume = volumeArg.getValue();
	mInputDeviceFile = inputArg.getValue();
	mEncoding = encodingArg.getValue();

	if (mVolume < -1 || mVolume > 100)
		throw(ArgParseException("Invalid volume value", "--volume"));

	if (mPlayerId.empty())
		mPlayerId = getMacAddress();
}

std::string Config::getMacAddress()
{
	struct ifaddrs* interface_addrs  = NULL;
	if (getifaddrs(&interface_addrs) == -1 || !interface_addrs) return 0;

	int32_t sd = socket( PF_INET, SOCK_DGRAM, 0 );
	if (sd < 0)
	{
		freeifaddrs( interface_addrs );
		return 0;
	}

	std::ostringstream os;
	for (struct ifaddrs* ifa = interface_addrs; ifa != NULL; ifa = ifa->ifa_next)
	{
		if (ifa->ifa_data != 0 && !(ifa->ifa_flags & IFF_LOOPBACK) && (ifa->ifa_flags & IFF_RUNNING))
		{
			struct ifreq req;
			strcpy(req.ifr_name, ifa->ifa_name);
			if (ioctl(sd, SIOCGIFHWADDR, &req) != -1)
			{
				uint8_t* mac = (uint8_t*)req.ifr_ifru.ifru_hwaddr.sa_data;
				os << std::hex << std::setfill( '0' );
				for(int i = 0; i < 6; i++)
					os << setw(2) << (uint)mac[i] << (i < 5 ? ":" : "");
				break;
			}
		}
	}

	close( sd );
	freeifaddrs( interface_addrs );
	return os.str();
}
