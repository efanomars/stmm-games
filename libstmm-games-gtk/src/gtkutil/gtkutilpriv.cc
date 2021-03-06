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
 * File:   gtkutilpriv.cc
 */

#include "../gtkutil/gtkutilpriv.h"

//#include <cassert>
//#include <iostream>


namespace stmg
{

void addSmallSeparator(Gtk::Box* p0Box) noexcept
{
	addSeparator(p0Box, 10, false);
}
void addSmallSeparator(Gtk::Box* p0Box, bool bExpand) noexcept
{
	addSeparator(p0Box, 10, bExpand);
}
void addBigSeparator(Gtk::Box* p0Box) noexcept
{
	addSeparator(p0Box, 20, false);
}
void addBigSeparator(Gtk::Box* p0Box, bool bExpand) noexcept
{
	addSeparator(p0Box, 20, bExpand);
}
void addSeparator(Gtk::Box* p0Box, int32_t nHeightPix) noexcept
{
	addSeparator(p0Box, nHeightPix, false);
}
void addSeparator(Gtk::Box* p0Box, int32_t nHeightPix, bool bExpand) noexcept
{
	Gtk::Image* m_p0ImageSeparator = Gtk::manage(new Gtk::Image());
	p0Box->pack_start(*m_p0ImageSeparator, bExpand, false);
		m_p0ImageSeparator->set_size_request(-1, nHeightPix);
}

} // namespace stmg
