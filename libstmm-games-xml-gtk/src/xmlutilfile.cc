/*
 * File:   xmlutilfile.cc
 *
 * Copyright © 2019  Stefano Marsili, <stemars@gmx.ch>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>
 */

#include "xmlutilfile.h"

#include <stmm-games-file/file.h>

#include <string>
#include <iostream>
#include <cassert>
#include <stdexcept>

#ifdef STMM_SNAP_PACKAGING
#include <array>
#include <string.h>
#include <stdlib.h>
#endif //STMM_SNAP_PACKAGING
#include <errno.h>
#include <sys/stat.h>

namespace stmg
{

namespace XmlUtil
{

// Modified from https://stackoverflow.com/questions/675039/how-can-i-create-directory-tree-in-c-linux
typedef struct stat Stat;

namespace Private
{
void makeDir(const char* p0Dir)
{
	Stat oStat;

	if (::stat(p0Dir, &oStat) != 0) {
		// Directory does not exist. EEXIST for race condition.
		const auto nRet = ::mkdir(p0Dir, 0755); // rwx r-x r-x
		if (nRet != 0) {
			if (errno != EEXIST) {
				throw std::runtime_error("Error: Could not create directory " + std::string(p0Dir));
			}
		}
	} else if (!S_ISDIR(oStat.st_mode)) {
		throw std::runtime_error("Error: " + std::string(p0Dir) + " not a directory");
	}
}
}
void makePath(const std::string& sPath)
{
	std::string sWorkPath = sPath;
	std::string::size_type nBasePos = 0;
	do {
		const auto nNewPos = sWorkPath.find('/', nBasePos);
		if (nNewPos == std::string::npos) {
			Private::makeDir(sWorkPath.c_str());
			break; // do ------
		} else if (nBasePos != nNewPos) {
			// not root or double slash
			sWorkPath[nNewPos] = '\0';
			Private::makeDir(sWorkPath.c_str());
			sWorkPath[nNewPos] = '/';
		}
		nBasePos = nNewPos + 1;
	} while (true);
}

void makePath(const File& oFile)
{
	assert(oFile.isDefined() && ! oFile.isBuffered());
	const std::string& sFile = oFile.getFullPath();
	const auto nPos = sFile.rfind('/');
	if (nPos == std::string::npos) {
		// no directories in the path
		return; //--------------------------------------------------------------
	}
	makePath(sFile.substr(0, nPos));
}

std::string getEnvString(const char* p0Name) noexcept
{
	const char* p0Value = ::secure_getenv(p0Name);
	std::string sValue{(p0Value == nullptr) ? "" : p0Value};
	return sValue;
}
#ifdef STMM_SNAP_PACKAGING
bool execCmd(const char* sCmd, std::string& sResult, std::string& sError) noexcept
{
	::fflush(nullptr);
	sError.clear();
	sResult.clear();
	std::array<char, 128> aBuffer;
	FILE* pFile = ::popen(sCmd, "r");
	if (pFile == nullptr) {
		sError = std::string("Error: popen() failed: ") + ::strerror(errno) + "(" + std::to_string(errno) + ")";
		return false; //--------------------------------------------------------
	}
	while (!::feof(pFile)) {
		if (::fgets(aBuffer.data(), sizeof(aBuffer), pFile) != nullptr) {
			sResult += aBuffer.data();
		}
	}
	const auto nRet = ::pclose(pFile);
	return (nRet == 0);
}
#endif //STMM_SNAP_PACKAGING

} // namespace XmlUtil

} // namespace stmg
