/*
 * File:   stdthemedrawingcontext.cc
 *
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

#include "stdthemedrawingcontext.h"
#include "stdthemecontext.h"

#include <stmm-games/util/basictypes.h>

namespace stmg
{

StdThemeDrawingContext::StdThemeDrawingContext() noexcept
: m_p1Owner(nullptr)
, m_p0SelectedImage(nullptr)
{
}
void StdThemeDrawingContext::reInit() noexcept
{
	m_p1Owner = nullptr;
	m_p0SelectedImage = nullptr;
}
NSize StdThemeDrawingContext::getTileSize() const noexcept
{
	return m_p1Owner->getTileSize();
}
const Glib::RefPtr<Pango::Context>& StdThemeDrawingContext::getFontContext() noexcept
{
	return m_p1Owner->getFontContext();
}

Image* StdThemeDrawingContext::getSelectedImage() noexcept
{
	return m_p0SelectedImage;
}
void StdThemeDrawingContext::setSelectedImage(Image* p0Image) noexcept
{
	m_p0SelectedImage = p0Image;
}

} // namespace stmg
