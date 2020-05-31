/*
 * Copyright © 2020  Stefano Marsili, <stemars@gmx.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   main.cc
 */

#include "setup.h"

#include "nlohmann/json.hpp"

#include <stmm-games-gtk/mainwindow.h>

#include <gtkmm.h>

#include <iostream>
#include <cassert>
#include <string>
#include <fstream>

#include <stdlib.h>


namespace stmg
{

using nlohmann::json;

void printVersion(const std::string& sAppVarsion) noexcept
{
	std::cout << sAppVarsion << '\n';
}
void printUsage(const std::string& sAppName) noexcept
{
	std::cout << "Usage: " << sAppName << " [OPTION] JSONCONF" << '\n';
	std::cout << "Omni game. JSONCONF must be a json file." << '\n';
	std::cout << "If JSONCONF isn't a path the file name is searched in '../data/conf/'" << '\n';
	std::cout << "Option:" << '\n';
	std::cout << "  -h --help        Prints this message." << '\n';
	std::cout << "  -f --fullscreen  Start in fullscreen." << '\n';
	std::cout << "  -t --testing     Enable testing of custom games." << '\n';
}

void evalNoArg(int& nArgC, char**& aArgV, const std::string& sOption1, const std::string& sOption2, bool& bVar) noexcept
{
	if (aArgV[1] == nullptr) {
		return;
	}
	const bool bIsOption1 = (sOption1 == std::string(aArgV[1]));
	if (bIsOption1 || ((!sOption2.empty()) && (sOption2 == std::string(aArgV[1])))) {
		bVar = true;
		--nArgC;
		++aArgV;
	}
}
int omniMain(int nArgC, char** aArgV) noexcept
{
	const std::string sOmni = "omni";

	MainWindowData oMainWindowData;

	bool bHelp = false;
	bool bTestMode = false;
	bool bFullscreen = false;
	std::string sPathName;

	char* p0ArgVZeroSave = ((nArgC >= 1) ? aArgV[0] : nullptr);
	while (nArgC >= 2) {
		auto nOldArgC = nArgC;
		evalNoArg(nArgC, aArgV, "--help", "-h", bHelp);
		if (bHelp) {
			printUsage(sOmni);
			return EXIT_SUCCESS; //---------------------------------------------
		}
		evalNoArg(nArgC, aArgV, "--testing", "-t", bTestMode);
		evalNoArg(nArgC, aArgV, "--fullscreen", "-f", bFullscreen);
		//
		if (nOldArgC == nArgC) {
			const std::string sTemp = aArgV[1];
			if ((aArgV[1] != nullptr) && ! sTemp.empty()) {
				if (sPathName.empty() && (sTemp.substr(0,1) != "-")) {
					sPathName = sTemp;
					--nArgC;
					++aArgV;
				} else {
					if (sPathName.empty()) {
						std::cerr << "Unknown argument: " << sTemp << '\n';
					} else {
						std::cerr << "path already defined!" << '\n';
					}
					return EXIT_FAILURE; //-----------------------------------------
				}
			} else {
				std::cerr << "Unknown argument: (null)" << '\n';
				return EXIT_FAILURE; //---------------------------------------------
			}
		}
		aArgV[0] = p0ArgVZeroSave;
	}

	if (sPathName.empty()) {
		std::cerr << "Joson conf file not defined" << '\n';
		return EXIT_FAILURE; //-----------------------------------------------------
	}
	if (sPathName.find("/") == std::string::npos) {
		sPathName = "../data/conf/" + sPathName;
	}

	std::ifstream oStream(sPathName, std::ifstream::binary);
	json oJsonPrj = json::object();
	try {
		oStream >> oJsonPrj;
	} catch (const json::parse_error& oErr) {
		std::cerr << "Couldn't load file '" << sPathName << "'" << '\n';
		std::cerr << "    Error: " << oErr.what() << '\n';
		return false; //------------------------------------------------------------
	}

	Glib::RefPtr<Gtk::Application> refApp =
			Gtk::Application::create("net.exampleappsnirvana." + sOmni);

	std::string sErr = omniSetup(oMainWindowData, oJsonPrj, sOmni, "99.99", bTestMode, bFullscreen);
	if (! sErr.empty()) {
		std::cerr << sErr << '\n';
		return EXIT_FAILURE; //-------------------------------------------------
	}

	auto oPairMainWin = createMainWindow(std::move(oMainWindowData));
	Glib::RefPtr<Gtk::Window>& refMainWin = oPairMainWin.first;
	if (!refMainWin) {
		std::cout << "Error: Couldn't create window: " << oPairMainWin.second << '\n';
		return EXIT_FAILURE; //-------------------------------------------------
	}
	return refApp->run(*(refMainWin.operator->()));
}

} //namespace stmg

int main(int argc, char** argv)
{
	return stmg::omniMain(argc, argv);
}

