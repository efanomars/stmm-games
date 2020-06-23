/*
 * File:   xmlutilfile.h
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

#ifndef STMG_XML_UTIL_FILE_H
#define STMG_XML_UTIL_FILE_H

#include <string>

namespace stmg { class File; }

namespace stmg
{

namespace XmlUtil
{

/** Create the path if it doesn't exist.
 * @param sPath The path.
 * @throws std::runtime_error if failed to create all directories
 */
void makePath(const std::string& sPath);
/** Create the path of directories of a file if it doesn't exist.
 * @param oFile The file. Must be defined and not buffered.
 */
void makePath(const File& oFile);

std::string getEnvString(const char* p0Name) noexcept;

#ifdef STMM_SNAP_PACKAGING
bool execCmd(const char* sCmd, std::string& sResult, std::string& sError) noexcept;
#endif //STMM_SNAP_PACKAGING

} // namespace XmlUtil

} // namespace stmg

#endif /* STMG_XML_UTIL_FILE_H */
