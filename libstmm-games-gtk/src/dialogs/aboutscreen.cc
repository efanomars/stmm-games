/*
 * File:   aboutscreen.cc
 *
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

#include "aboutscreen.h"
#include "../gamewindow.h"

#include "../gtkutil/gtkutilpriv.h"

#include <stmm-games/stdconfig.h>

#include <gtkmm.h>

//#include <iostream>
#include <cassert>
#include <string>
#include <utility>
#include <algorithm>

namespace stmg
{

static const Glib::ustring s_sStmmGamesWebSite = "https://www.efanomars.com/libraries/stmm-games";
static const Glib::ustring s_sStmmGamesCopyright = "stmm-games © 2019-2020 Stefano Marsili";

static constexpr const int32_t s_nButtonLeftRightMargin = 20;

AboutScreen::AboutScreen(GameWindow& oGameWindow, const MainWindowData& oMainWindowData, const Glib::RefPtr<Gdk::Pixbuf>& refLogoPixbuf) noexcept
: m_oGameWindow(oGameWindow)
, m_oData(oMainWindowData)
, m_refLogoPixbuf(refLogoPixbuf)
{
}
Gtk::Widget* AboutScreen::init() noexcept
{
	m_p0ScreenBoxAbout = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));

		Gtk::Box* m_p0BoxAbout = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
		m_p0ScreenBoxAbout->pack_start(*m_p0BoxAbout, true, true);
			addBigSeparator(m_p0BoxAbout);
			Gtk::Image* m_p0ImageAboutLogo = Gtk::manage(new Gtk::Image(m_refLogoPixbuf));
			m_p0BoxAbout->pack_start(*m_p0ImageAboutLogo, false, true);
				m_p0ImageAboutLogo->set_margin_top(5);
				m_p0ImageAboutLogo->set_margin_bottom(5);
			Gtk::Label* m_p0LabelAboutAppName = Gtk::manage(new Gtk::Label(m_oData.m_refStdConfig->getAppName()));
			m_p0BoxAbout->pack_start(*m_p0LabelAboutAppName, false, true);
				m_p0LabelAboutAppName->set_margin_top(5);
				m_p0LabelAboutAppName->set_margin_bottom(5);
				{
				Pango::AttrList oAttrList;
				Pango::AttrInt oAttrWeight = Pango::Attribute::create_attr_weight(Pango::WEIGHT_HEAVY);
				oAttrList.insert(oAttrWeight);
				Pango::AttrFloat oAttrScale = Pango::Attribute::create_attr_scale(1.8);
				oAttrList.insert(oAttrScale);
				m_p0LabelAboutAppName ->set_attributes(oAttrList);
				}
			Gtk::Label* m_p0LabelAboutVersion = Gtk::manage(new Gtk::Label(m_oData.m_refStdConfig->getAppVersion()));
			m_p0BoxAbout->pack_start(*m_p0LabelAboutVersion, false, true);
				m_p0LabelAboutVersion->set_margin_top(5);
				m_p0LabelAboutVersion->set_margin_bottom(5);
			Gtk::Label* m_p0LabelAboutCopyright = Gtk::manage(new Gtk::Label(m_oData.m_sCopyright.empty() ? s_sStmmGamesCopyright : Glib::ustring{m_oData.m_sCopyright}));
			m_p0BoxAbout->pack_start(*m_p0LabelAboutCopyright, false, true);
				m_p0LabelAboutCopyright->set_margin_top(3);
				m_p0LabelAboutCopyright->set_margin_bottom(3);
			if (! m_oData.m_sWebSite.empty()) {
				addBigSeparator(m_p0BoxAbout);
				Gtk::Label* m_p0LabelAboutWebsite = Gtk::manage(new Gtk::Label("<i>Website</i>"));
				m_p0BoxAbout->pack_start(*m_p0LabelAboutWebsite, false, true);
					m_p0LabelAboutWebsite->set_use_markup(true);
				Gtk::Label* m_p0LabelAboutWebsiteAdr = Gtk::manage(new Gtk::Label("<a href=\"" + m_oData.m_sWebSite + "\">" + m_oData.m_sWebSite + "</a>"));
				m_p0BoxAbout->pack_start(*m_p0LabelAboutWebsiteAdr, false, true);
					m_p0LabelAboutWebsiteAdr->set_use_markup(true);
					m_p0LabelAboutWebsiteAdr->set_ellipsize(Pango::ELLIPSIZE_END);
					m_p0LabelAboutWebsiteAdr->set_margin_bottom(5);
			}
			if (! m_oData.m_aAuthors.empty()) {
			addBigSeparator(m_p0BoxAbout);
			Gtk::ScrolledWindow* m_p0ScrolledAuthors = Gtk::manage(new Gtk::ScrolledWindow());
			m_p0BoxAbout->pack_start(*m_p0ScrolledAuthors, true, true);
				m_p0ScrolledAuthors->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
				Gtk::Box* m_p0VBoxScroller = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
				m_p0ScrolledAuthors->add(*m_p0VBoxScroller);
					Gtk::Label* m_p0LabelAboutAuthors = Gtk::manage(new Gtk::Label(Glib::ustring{"<big><b>Author"} + ((m_oData.m_aAuthors.size() == 1) ? "" : "s") + "</b></big>"));
					m_p0VBoxScroller->pack_start(*m_p0LabelAboutAuthors, false, true);
						m_p0LabelAboutAuthors->set_use_markup(true);
						m_p0LabelAboutAuthors->set_margin_top(3);
						m_p0LabelAboutAuthors->set_margin_bottom(3);
						//use text in scroller!
					for (const MainAuthorData& oAuthor : m_oData.m_aAuthors) {
						addSmallSeparator(m_p0VBoxScroller);
						assert(! oAuthor.m_sName.empty());
						Gtk::Label* m_p0LabelAboutAuthorName = Gtk::manage(new Gtk::Label("<b>" + oAuthor.m_sName + "</b>"));
						m_p0VBoxScroller->pack_start(*m_p0LabelAboutAuthorName, false, true);
							m_p0LabelAboutAuthorName->set_use_markup(true);
						if (! oAuthor.m_sEMail.empty()) {
						Gtk::Label* m_p0LabelAboutAuthorEMail = Gtk::manage(new Gtk::Label("<a href=\"mailto:" + oAuthor.m_sEMail + "\">" + oAuthor.m_sEMail + "</a>"));
						m_p0VBoxScroller->pack_start(*m_p0LabelAboutAuthorEMail, false, true);
							m_p0LabelAboutAuthorEMail->set_use_markup(true);
							m_p0LabelAboutAuthorEMail->set_ellipsize(Pango::ELLIPSIZE_END);
						}
						if (! oAuthor.m_sRole.empty()) {
						Gtk::Label* m_p0LabelAboutAuthorRole = Gtk::manage(new Gtk::Label("<i>" + oAuthor.m_sRole + "</i>"));
						m_p0VBoxScroller->pack_start(*m_p0LabelAboutAuthorRole, false, true);
							m_p0LabelAboutAuthorRole->set_use_markup(true);
							m_p0LabelAboutAuthorRole->set_ellipsize(Pango::ELLIPSIZE_END);
						}
					}
			}

			addBigSeparator(m_p0BoxAbout);
			Gtk::Label* m_p0LabelAboutStmmGamesWebsite = Gtk::manage(new Gtk::Label("<i>Based on the stmm-games library</i>"));
			m_p0BoxAbout->pack_start(*m_p0LabelAboutStmmGamesWebsite, false, true);
				m_p0LabelAboutStmmGamesWebsite->set_use_markup(true);
			Gtk::Label* m_p0LabelAboutStmmGamesWebsiteAdr = Gtk::manage(new Gtk::Label("<a href=\"" + s_sStmmGamesWebSite + "\">" + s_sStmmGamesWebSite + "</a>"));
			m_p0BoxAbout->pack_start(*m_p0LabelAboutStmmGamesWebsiteAdr, false, true);
				m_p0LabelAboutStmmGamesWebsiteAdr->set_use_markup(true);
				m_p0LabelAboutStmmGamesWebsiteAdr->set_ellipsize(Pango::ELLIPSIZE_END);
				m_p0LabelAboutStmmGamesWebsiteAdr->set_margin_bottom(5);

			addBigSeparator(m_p0BoxAbout);
			Gtk::Button* m_p0ButtonAboutOk = Gtk::manage(new Gtk::Button("Ok"));
			m_p0BoxAbout->pack_start(*m_p0ButtonAboutOk, false, false);
				m_p0ButtonAboutOk->set_margin_left(s_nButtonLeftRightMargin);
				m_p0ButtonAboutOk->set_margin_right(s_nButtonLeftRightMargin);
				m_p0ButtonAboutOk->signal_clicked().connect(
								sigc::mem_fun(*this, &AboutScreen::onButtonAboutOk) );
			addBigSeparator(m_p0BoxAbout);


	return m_p0ScreenBoxAbout;
}
bool AboutScreen::changeTo() noexcept
{
	return true;
}
void AboutScreen::onButtonAboutOk() noexcept
{
	m_oGameWindow.afterAbout();
}

} // namespace stmg
