/*
 * File:  layout.h
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

#ifndef STMG_LAYOUT_H
#define STMG_LAYOUT_H

#include "gameproxy.h"
#include "util/namedobjindex.h"

#include <string>
#include <memory>
#include <vector>

#include <stdint.h>

namespace stmg { class AppPreferences; }

namespace stmg
{

using std::shared_ptr;

class Game;
class GameWidget;
class LevelShowWidget;

class Layout
{
public:
	/** Layout constructor.
	 * If parameter `refReferenceLevelShow` is null, the first LevelShowWidget 
	 * found in the refRootGW tree is used. If it is not null it must be in the
	 * refRootGW tree.
	 * 
	 * If `bSubshows` is `false` then the stage(s) is shown.
	 * @param refRootGW The root widget. Cannot be null.
	 * @param refReferenceLevelShow The reference level show. Can be null.
	 * @param bAllTeamsInOneLevel Whether all teams play on same stage.
	 * @param bSubshows Whether player's subshow is shown.
	 * @param refPrefs The app preferences. Cannot be null.
	 */
	Layout(const shared_ptr<GameWidget>& refRootGW
			, const shared_ptr<LevelShowWidget>& refReferenceLevelShow
			, bool bAllTeamsInOneLevel, bool bSubshows
			, const shared_ptr<AppPreferences>& refPrefs) noexcept;
	// Note: despite there's no virtual destructor this class can be
	//       subclassed to allow reInit() to be called and objects be recycled.

	/** Get the root widget.
	 * @return The root widget. Cannot be null.
	 */
	inline const shared_ptr<GameWidget>& getRoot() const noexcept { return m_refRootGW; }
	/** The reference (sub)show used for layout calculations.
	 * All sizes of widgets in the layout are factors of the size of this widget.
	 * @return Cannot be null.
	 */
	const shared_ptr<LevelShowWidget>& getReferenceShow() const noexcept { return m_refReferenceLevelShow; }
	inline bool isAllTeamsInOneLevel() const noexcept { return m_bAllTeamsInOneLevel; }
	/** Tells whether subshows or shows of the stage are shown.
	 * @return Whether LevelShowWidgets in layout are all subshows.
	 */
	inline bool isSubshows() const noexcept { return m_bSubshows; }
	/** Tells whether the layout is valid.
	 */
	inline bool isValid() const noexcept { return m_bValid; }
	/** The error string for an invalid layout.
	 * @return The error string. Empty if valid layout.
	 */
	inline const std::string& getErrorString() const noexcept { return m_sErrorString; }

	/** Find a named widget.
	 * @param sName The name of the widget.
	 * @param nTeam The team of the widget. Is -1 if game widget.
	 * @param nMate The mate of the widget. Is -1 if game or team widget.
	 * @return The widget. Null if none defined with given name.
	 */
	const shared_ptr<GameWidget>& getWidgetNamed(const std::string& sName, int32_t nTeam, int32_t nMate) const noexcept;
	/** The preferences.
	 * @return The preferences. Cannot be null;
	 */
	const shared_ptr<AppPreferences>& getPrefs() noexcept { return m_refPrefs; } 
	#ifndef NDEBUG
	void dump(int32_t nIndentSpaces) const noexcept;
	void dump() const noexcept
	{
		dump(0);
	}
	#endif //NDEBUG
protected:
	/** Initialization.
	 * Subclass this class and make this public for recycling of objects.
	 * @param refRootGW The root widget. Cannot be null.
	 * @param refReferenceLevelShow The reference level show. Can be null.
	 * @param bAllTeamsInOneLevel Whether all teams play on same stage.
	 * @param bSubshows Whether player's subshow is shown.
	 * @param refPrefs The app preferences. Cannot be null.
	 */
	void reInit(const shared_ptr<GameWidget>& refRootGW
				, const shared_ptr<LevelShowWidget>& refReferenceLevelShow
				, bool bAllTeamsInOneLevel, bool bSubshows
				, const shared_ptr<AppPreferences>& refPrefs) noexcept;

private:
	std::string getMangledName(const std::string& sName, int32_t nTeam, int32_t nMate) const noexcept;
	void checkAndGet(const AppPreferences& oAppPreferences) noexcept;
	void traverse(const shared_ptr<GameWidget>& refGW, std::vector< shared_ptr<LevelShowWidget> >& aLSW
					, std::string& sDuplicateName) noexcept;
	void traverseSetLayout(const shared_ptr<GameWidget>& refGW) noexcept;
	void traverseOnAddedToLayout(const shared_ptr<GameWidget>& refGW) noexcept;
	void traverseSetGameProxy(const shared_ptr<GameWidget>& refGW) noexcept;
	void traverseOnAddedToGame(const shared_ptr<GameWidget>& refGW) noexcept;
	friend class Game;
	void setGame(Game* p0Game) noexcept; // Called by game instance
private:
	shared_ptr<GameWidget> m_refRootGW;
	shared_ptr<LevelShowWidget> m_refReferenceLevelShow;
	shared_ptr<AppPreferences> m_refPrefs;
	bool m_bAllTeamsInOneLevel;
	bool m_bSubshows;
	bool m_bValid;
	std::string m_sErrorString;
	AssignableNamedObjIndex< shared_ptr<GameWidget> > m_oNamedWidgets;
	GameProxy m_oGameProxy;
private: // no implementation
	Layout(const Layout& oSource) = delete;
	Layout& operator=(const Layout& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_LAYOUT_H */

