/*
	main.cpp - Slimmer
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
#include "Controller.h"

#include <iostream>
#include <chrono>
#include <thread>
#include "tclap/ArgException.h"

#define EV_STANDALONE 1
#include "libev/ev.c"

int main(int argc, char* argv[])
{
	try
	{
		Config::processOptions(argc, argv);
	}
	catch (const TCLAP::ArgException& e)
	{
		cerr << "[ERROR] " << e.error() << " " << e.argId() << endl;
		return EXIT_FAILURE;
	}

	if (Config::verbose())
	{
		cout << "Configuration:" << endl;
		cout << "  LMS: " << Config::lmsHost() << ":" << Config::lmsPort() << " named \"" << Config::lmsName() << "\"" << endl;
		cout << "  LCDd: " << Config::lcdHost() << ":" << Config::lcdPort() << endl;
		cout << "  PlayerId (MAC): " << Config::playerId() << endl;
		cout << "  Input device: " << endl;
		for (string const i_file : Config::inputDeviceFiles())
			cout << "    - " << i_file << endl;
	}

	while (1)
	{
		try
		{
			Controller c;
			c.run();
		}
		catch (const lcdapi::LCDException& e)
		{
			cerr << "[ERROR] LCDd communication problem: " << e.what() << endl;
		}

		// Sleeping for some time and retrying initialization
		this_thread::sleep_for(chrono::seconds(Config::cRetryDelay));
	}
}
