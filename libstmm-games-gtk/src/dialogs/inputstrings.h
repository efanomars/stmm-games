/*
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
/*
 * File:   inputstrings.h
 */

#ifndef _ZIMTRIS_PLAYERS_INPUT_STRINGS_
#define _ZIMTRIS_PLAYERS_INPUT_STRINGS_

#include <stmm-input/hardwarekey.h>

#include <string>
#include <unordered_map>
#include <utility>
//#include <iostream>

namespace stmg
{

class InputStrings
{
public:
	InputStrings() noexcept
	{
		initKeyMap();
	}
	inline std::string getKeyString(stmi::HARDWARE_KEY eKey) const noexcept
	{
		auto itFind = m_oKeyStringMap.find(eKey);
		if (itFind == m_oKeyStringMap.end()) {
			return "";
		}
		return itFind->second;
	}
private:
	inline void addKey(stmi::HARDWARE_KEY eKey, const std::string& sStr) noexcept
	{
		m_oKeyStringMap.emplace(eKey, sStr);
	}

	void initKeyMap() noexcept;
private:
	std::unordered_map<stmi::HARDWARE_KEY, std::string> m_oKeyStringMap;
};

} // namespace stmg

#endif	/* _ZIMTRIS_PLAYERS_INPUT_STRINGS_ */
