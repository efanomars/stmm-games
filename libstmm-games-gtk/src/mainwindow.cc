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
 * File:   mainwindow.cc
 */

#include "mainwindow.h"

#include "gamewindow.h"

#include <cassert>

namespace stmg
{

std::pair<Glib::RefPtr<Gtk::Window>, std::string> createMainWindow(MainWindowData&& oMainWindowData) noexcept
{
	assert(oMainWindowData.m_refStdConfig);
	assert(oMainWindowData.m_refGameLoader);
	assert(oMainWindowData.m_refThemeLoader);
	return GameWindow::create(std::move(oMainWindowData));
}

} // namespace stmg
