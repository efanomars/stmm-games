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
 * File:   mainwindow.h
 */

#ifndef STMG_MAIN_WINDOW_H
#define STMG_MAIN_WINDOW_H

#include "themeloader.h"

#include <stmm-games-file/highscoresloader.h>
#include <stmm-games-file/allpreferencesloader.h>
#include <stmm-games-file/file.h>
#include <stmm-games-file/gameloader.h>

#include <stmm-games/util/basictypes.h>

#include <gtkmm.h>

#include <string>
#include <vector>
#include <memory>
#include <utility>

namespace stmg { class StdConfig; }

namespace stmg
{

using std::shared_ptr;
using std::unique_ptr;

struct MainAuthorData
{
	std::string m_sName; /**< The name of the author, */
	std::string m_sEMail; /**< The email of the author. Format example: "johndoe@foobar.cam". */
	std::string m_sRole; /**< The role or work done for the project. */
};
struct MainWindowData
{
	shared_ptr<StdConfig> m_refStdConfig; /**< The configuration data. Cannot be null. */
	unique_ptr<GameLoader> m_refGameLoader; /**< The game loader. Cannot be null. */
	unique_ptr<HighscoresLoader> m_refHighscoresLoader; /**< The highscores loader. Can be null. */
	unique_ptr<ThemeLoader> m_refThemeLoader; /**< The theme loader. Cannot be null. */
	unique_ptr<AllPreferencesLoader> m_refAllPreferencesLoader; /**< The preferences loader. Can be null. */
	File m_oIconFile; /**< The icon file. Can be undefined. */
	File m_oLogoFile; /**< The logo file. Can be undefined. */
	std::string m_sCopyright; /**< The Copyright string. If empty "stmm-games © 2019 Stefano Marsili" will be shown. */
	std::string m_sWebSite; /**< The web site. If empty "http://www.efanomars.com/libraries/stmm-games" will be shown. */
	std::vector<MainAuthorData> m_aAuthors; /**< The authors. */
	bool m_bPauseIfWindowDeactivated = true; /**< Whether to automatically pause the game when window deactivated. */
	bool m_bFullscreen = false; /**< Whether to run in fullscreen mode. */
	NSize m_oInitialSize = NSize{400, 600}; /**< The initial window size. Default: 400x600 pixel. */
};

/** Creates the game window.
 * @param oMainWindowData The initialization data.
 * @return The window or null and the error string if an error occurred.
 */
std::pair<Glib::RefPtr<Gtk::Window>, std::string> createMainWindow(MainWindowData&& oMainWindowData) noexcept;

} // namespace stmg

#endif	/* STMG_MAIN_WINDOW_H */

