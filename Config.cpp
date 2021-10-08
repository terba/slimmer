/*
	Config.cpp - Slimmer
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

#include "Config.h"
#include "ApplicationInfo.h"
#include "BuildInfo.h"

#include <stdexcept>
#include <unistd.h>
#include <cstring>
#include <ifaddrs.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "tclap/CmdLine.h"
#include <unicode/unistr.h>

const short int Config::cRetryDelay = 1;
const double Config::cPlayerStatusQueryInterval = 0.5;
const double Config::cPlayerStatusQueryIntervalInStandby = 3;
const double Config::cVolumeScreenHideDelay = 1;
const double Config::cMenuScreenHideDelay = 8;
const double Config::cPopupHideDelay = 0.6;
const double Config::cButtonLongPressTime = 0.4;
const short int Config::cVolumeStep = 2;
const short int Config::cNewMusicItems = 100;
const short int Config::cTrackRestartLimit = 3;
const double Config::cStandbyTimeout = 30;

bool Config::mVerbose;
string Config::mLmsName = "unknown";
string Config::mLmsHost;
int Config::mLmsPort;
string Config::mLcdHost;
int Config::mLcdPort;
string Config::mPlayerId;
bool Config::mFixedVolume;
bool Config::mBackMenus;
int Config::mVolume;
vector<string> Config::mInputDeviceFiles;
string Config::mEncoding;
int Config::mScrollSpeed;

int Config::processOptions(int argc, char* argv[])
{
	using namespace TCLAP;

	CmdLine cmd(string("Copyright (C) ") + APP_COPYRIGHT_DATE + "  " + APP_COPYRIGHT_HOLDER + "\nThis program comes with ABSOLUTELY NO WARRANTY. This is free software, and you are welcome to redistribute it under certain conditions. See LINCENSE file in the source distribution.",
				' ',
				string(APP_VERSION_NUMBER) + " (Build " + BUILD_DATE + " " + BUILD_SYSTEM + " " + BUILD_SYSTEM_PROCESSOR + ")");

	SwitchArg verboseArg("v", "verbose", "be verbose", false);
	ValueArg<string> lmshostArg("s", "lmshost", "LMS host (default: autodiscovery)", false, "", "ip or hostname");
	ValueArg<int> lmsportArg("p", "lmsport", "LMS HTTP port (default: autodiscovery)", false, 0, "number");
	ValueArg<string> lcdhostArg("l", "lcdhost", "lcdproc host (default: localhost)", false, "localhost", "ip or hostname");
	ValueArg<int> lcdportArg("P", "lcdport", "lcdproc port (default: 13666)", false, 13666, "number");
	ValueArg<string> macArg("m", "mac", "the player's MAC address (default: automatic, first interface)", false, "", "AA:BB:CC:DD:EE:FF");
	SwitchArg fixedvolumeArg("f", "fixedvolume", "volume control disabled", false);
	SwitchArg backmenusArg("b", "backmenus", "include selectable 'back' item in all menu navigation contexts", false);
	ValueArg<int> volumeArg("o", "volume", "set volume on startup", false, -1, "0-100");
	ValueArg<string> inputArg("i", "input", "keyboard input device file(s). Use comma to delimit multiple input files. (default: /dev/input/event0)", false, "/dev/input/event0", "input device file");
	ValueArg<string> encodingArg("e", "encoding", "the LCD's character encoding (default: ISO-8859-1)", false, "ISO-8859-1", "single-byte encoding");
	ValueArg<int> scrollspeedArg("c", "scrollspeed", "text scrolling speed (default: 3)", false, 3, "0-10");

	cmd.add(scrollspeedArg);
	cmd.add(encodingArg);
	cmd.add(inputArg);
	cmd.add(volumeArg);
	cmd.add(fixedvolumeArg);
	cmd.add(backmenusArg);
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
	mBackMenus = backmenusArg.getValue();
	mVolume = volumeArg.getValue();

	// Save string to stream. Pull off chunks, broken up by ','
	std::stringstream unparsed_data(inputArg.getValue());
	while(unparsed_data.good()) {
		std::string substr;
		std::getline(unparsed_data, substr, ',');
		mInputDeviceFiles.push_back(substr);
	}
	if (Config::verbose())
	{
		for (string const i_file : mInputDeviceFiles)
			cout << "found device input file: " << i_file << endl;
	}

	mEncoding = encodingArg.getValue();
	mScrollSpeed = scrollspeedArg.getValue();

	if (mVolume < -1 || mVolume > 100)
		throw(ArgParseException("Invalid volume value", "--volume"));
	if (mScrollSpeed < 0 || mScrollSpeed > 10)
		throw(ArgParseException("Invalid scrollspeed value", "--scrollspeed"));

	if (mPlayerId.empty())
		mPlayerId = getMacAddress();

	if (mLmsHost.empty())
		discoverLMS();

	if (mLmsHost.empty() || mLmsPort == 0)
	{
		if (Config::verbose()) cout << "Unsuccessful LMS discovery. Using defaults." << endl;
		mLmsHost = "localhost";
		mLmsPort = 9000;
	}

	// Testing the supplied encoding
	UnicodeString ucs = UnicodeString::fromUTF8(" ");
	int32_t targetsize = ucs.extract(0, ucs.length(), 0, 0, mEncoding.c_str());
	if (targetsize < 1)
	{
		throw(ArgParseException("Invalid character encoding specified", "--encoding"));
	}
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

void Config::discoverLMS()
{
	int fd;
	char buffer[200] = "eIPAD\0NAME\0JSON\0";

	if ((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
		throw runtime_error("Can not create discovery socket. Errno: " + errno);

	socklen_t broadcast = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) != 0)
		throw runtime_error("Can not set discovery socket options. Errno: " + errno);

	struct timeval timeout;
	timeout.tv_sec = 2;
	timeout.tv_usec = 0;
	if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) != 0)
		throw runtime_error("Can not set discovery socket timeout option. Errno: " + errno);

	struct sockaddr_in out;
	out.sin_family = AF_INET;
	out.sin_port = htons(3483);
	out.sin_addr.s_addr = htonl(INADDR_ANY);

	if (sendto(fd, buffer, 16, 0, (const struct sockaddr*)&out, sizeof(out)) != 16)
		throw runtime_error("Can not send discovery packet. Errno: " + errno);

	if (Config::verbose()) cout << "LMS discovery request sent" << endl;

	// Waiting for the first answer
	struct sockaddr_in in;
	socklen_t insize = sizeof(in);
	ssize_t recvSize = 1;
	while (recvSize > 0)
	{
		memset(buffer, 0, sizeof(buffer));
		recvSize = recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&in, &insize);
		if (recvSize > 0 && strncmp(buffer, "ENAME", 5) == 0)
		{
			// Answer should look like "ENAME[namesizebyte]nameJSON[portsizebyte]9001"
			mLmsHost = inet_ntoa(in.sin_addr);
			const char* pos = buffer + 6;
			mLmsName = string(pos, pos + pos[-1]);
			pos = pos + pos[-1];
			if (strncmp(pos, "JSON", 4) == 0)
			{
				pos = pos + 5;
				mLmsPort = stoi(string(pos, pos + pos[-1]));
				if (Config::verbose()) cout << "LMS discovery reply from " << inet_ntoa(in.sin_addr) << endl;
				recvSize = 0;
			}
		}
	}

	close(fd);
}
