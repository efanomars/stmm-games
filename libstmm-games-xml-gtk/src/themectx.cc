/*
 * Copyright © 2019-2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   themectx.cc
 */

#include "themectx.h"

#include <stmm-games-gtk/stdtheme.h>
#include <stmm-games-file/file.h>

#include <utility>

namespace stmg { class AppConfig; }
namespace xmlpp { class Element; }

namespace stmg
{

ThemeCtx::ThemeCtx(const shared_ptr<AppConfig>& refAppConfig, StdTheme& oTheme, const std::string& sThemeName
					, File oFile, const xmlpp::Element* p0RootElement)
: ConditionalCtx(refAppConfig, oTheme.getNamed())
, m_oTheme(oTheme)
, m_sCtxThemeName(sThemeName)
, m_oCtxThemeFile(std::move(oFile))
, m_p0RootElement(p0RootElement)
, m_nThemeNr(0)
{
}

std::string ThemeCtx::err(const std::string& sErr)
{
	std::string sExtError;
	if (m_oCtxThemeFile.isBuffered()) {
		sExtError.append(m_sCtxThemeName);
	} else {
		sExtError.append(m_oCtxThemeFile.getFullPath());
	}
	sExtError.append(": ");
	sExtError.append(sErr);
	return ConditionalCtx::err(sExtError);
}

} // namespace stmg
