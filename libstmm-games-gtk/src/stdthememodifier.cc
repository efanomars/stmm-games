/*
 * File:   stdthememodifier.cc
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

#include "stdthememodifier.h"

#include <cassert>

namespace stmg
{

StdThemeModifier::StdThemeModifier(StdTheme* p1Owner) noexcept
: m_p1Owner(p1Owner)
{
	assert(p1Owner != nullptr);
}
void StdThemeModifier::registerTileSize(int32_t /*nW*/, int32_t /*nH*/) noexcept
{
}

void StdThemeModifier::unregisterTileSize(int32_t /*nW*/, int32_t /*nH*/) noexcept
{
}

} // namespace stmg
