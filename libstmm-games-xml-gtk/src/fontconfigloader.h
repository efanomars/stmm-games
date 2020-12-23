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
 * File:   fontconfigloader.h
 */

#ifndef STMG_FONTCONFIG_LOADER_H
#define STMG_FONTCONFIG_LOADER_H

#include <string>
#include <memory>

namespace stmg
{

using std::unique_ptr;
using std::shared_ptr;

class FontConfigLoader
{
public:
	static unique_ptr<FontConfigLoader> create() noexcept;

	// returns empty string if error, FontDescription string otherwise
	// sFontFilePath must be absolute
	std::string addAppFontFile(const std::string& sFontFilePath) noexcept;

	~FontConfigLoader();
protected:
	FontConfigLoader() noexcept = default;
private:
	FontConfigLoader(const FontConfigLoader& oSource) = delete;
	FontConfigLoader& operator=(const FontConfigLoader& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_FONTCONFIG_LOADER_H */

