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
 * File:   themectx.h
 */

#ifndef STMG_THEME_CTX_H
#define STMG_THEME_CTX_H

#include <stmm-games-xml-base/conditionalctx.h>

#include <stmm-games-file/file.h>

#include <string>
#include <memory>

#include <stdint.h>

namespace stmg { class AppConfig; }
namespace stmg { class StdTheme; }
namespace xmlpp { class Element; }

namespace stmg
{

using std::unique_ptr;

////////////////////////////////////////////////////////////////////////////////
class ThemeCtx : public ConditionalCtx
{
public:
	ThemeCtx(const shared_ptr<AppConfig>& refAppConfig, StdTheme& oTheme, const std::string& sThemeName, File oFile, const xmlpp::Element* p0RootElement);

	ThemeCtx(ThemeCtx&& oSource) = default;

	inline StdTheme& theme() { return m_oTheme; }
	inline bool isMain() const { return m_nThemeNr == 0; }
	inline int32_t getThemeNr() const { return m_nThemeNr; }
	inline const File& getThemeFile() const { return m_oCtxThemeFile; }
	inline const std::string& getThemeName() const { return m_sCtxThemeName; }
protected:
	std::string err(const std::string& sErr) override;
private:
	friend class XmlThemeParser;
	StdTheme& m_oTheme;
	const std::string m_sCtxThemeName;
	const File m_oCtxThemeFile;
	const xmlpp::Element* m_p0RootElement;
	int32_t m_nThemeNr; // set by XmlThemeParser
private:
	ThemeCtx() = delete;
};

} // namespace stmg

#endif	/* STMG_THEME_CTX_H */

