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
 * File:   aboutscreen.h
 */

#ifndef STMG_ABOUT_SCREEN_H
#define STMG_ABOUT_SCREEN_H

#include <gtkmm.h>

#include <memory>
#include <vector>

#include <stdint.h>

namespace stmg { class GameWindow; }
namespace stmg { class MainWindowData; }

namespace stmg
{

using std::shared_ptr;


class AboutScreen
{
public:
	AboutScreen(GameWindow& oGameWindow, const MainWindowData& oMainWindowData, const Glib::RefPtr<Gdk::Pixbuf>& refLogoPixbuf) noexcept;

	// returns widget to add to container
	Gtk::Widget* init() noexcept;

	bool changeTo() noexcept;

private:
	//Signal handlers:
	void onButtonAboutOk() noexcept;

private:
	GameWindow& m_oGameWindow;
	const MainWindowData& m_oData;
	const Glib::RefPtr<Gdk::Pixbuf>& m_refLogoPixbuf;
	Gtk::Box* m_p0ScreenBoxAbout = nullptr;
		//Gtk::Box* m_p0BoxAbout = nullptr;
			//Gtk::Image* m_p0ImageAboutLogo = nullptr;
			//Gtk::Label* m_p0LabelAboutAppName = nullptr;
			//Gtk::Label* m_p0LabelAboutVersion = nullptr;
			//Gtk::Label* m_p0LabelAboutCopyright = nullptr;
			//Gtk::Button* m_p0ButtonAboutOk = nullptr;
};

} // namespace stmg

#endif	/* STMG_ABOUT_SCREEN_H */

