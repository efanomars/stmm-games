/*
 * Copyright © 2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   themeextradata.h
 */

#ifndef STMG_THEME_EXTRA_DATA_H
#define STMG_THEME_EXTRA_DATA_H

#include <string>
#include <unordered_map>
#include <tuple>

namespace xmlpp { class Element; }

namespace stmg
{

struct ThemeExtraData
{
	// The following are the fonts provided as files by the theme and are
	// loaded before parsing starts.
	// The most common fonts are expected to be already installed
	// by the system such as Arial, Times New Roman, etc.
	std::unordered_map<std::string, std::string> m_oFontFileDefs; // Key: font filename, Value: font description
	// StdTheme doesn't support aliases so we record their definition here.
	// Aliases create a sort of dependency tree that has to be resolved
	// after all font definitions are parsed.
	// Key: alias name, Value: (xml context number, the defining element, target font)
	std::unordered_map<std::string, std::tuple<int32_t, const xmlpp::Element*, std::string>> m_oFontAliases;
	// The first DefaultFont element encountered while parsing the xml themes hierarchy
	// defines the default font, but we also want to be able to set the default
	// to a font name that might not have been defined yet while parsing.
	// So if this is not empty, parsed DefaultFont elements won't call StdTheme::setDefaultFont,
	// The call will be done when all is parsed, by using this font name's definition.
	// If the name was not defined a parsing error occurs.
	std::string m_sDefaultFontName;
	int32_t m_nDefaultThemeNr; // The theme context number that set m_sDefaultFontName
	const xmlpp::Element* m_p0DefaultElement; // The element that set m_sDefaultFontName
};

} // namespace stmg

#endif	/* STMG_THEME_EXTRA_DATA_H */

