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
 * File:   stdviewlayout.h
 */

#ifndef STMG_STD_VIEW_LAYOUT_H
#define STMG_STD_VIEW_LAYOUT_H

#include "themelayout.h"

#include <memory>
#include <string>
#include <vector>

#include <stdint.h>

namespace stmg { class AppPreferences; }
namespace stmg { class Layout; }
namespace stmg { class LevelShowThemeWidget; }
namespace stmg { class ThemeWidget; }
namespace stmg { class ThemeWidgetInteractive; }

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }

namespace stmg
{

using std::shared_ptr;

class StdViewLayout : public ThemeLayout
{
public:
	StdViewLayout(const shared_ptr<Layout>& refGameLayout, const shared_ptr<ThemeWidget>& refRootTW
				, const AppPreferences& oAppPreferences) noexcept;

	inline const shared_ptr<Layout>& getGameLayout() const noexcept { return m_refGameLayout; }

	void calcLayout(int32_t nDestW, int32_t nDestH, int32_t nTileMinW, double fWHTileRatio
					, int32_t& nTileW, int32_t& nTileH, int32_t& nLayoutConfig) noexcept;

	//
	void draw(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept;
	//
	void drawIfChanged(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept;
	//
	const std::vector< shared_ptr<LevelShowThemeWidget> >& getLevelShowThemeWidgets() const noexcept;
	const std::vector< shared_ptr<ThemeWidget> >& getCanChangeThemeWidgets() const noexcept;
	const std::vector< shared_ptr<ThemeWidgetInteractive> >& getInteractiveWidgets() const noexcept;

	/** Determines whether a position is within a LevelShowThemeWidget.
	 * @param nX The x position.
	 * @param nY The y position.
	 * @return The index into getLevelShowThemeWidgets() or -1 if outside.
	 */
	int32_t getLevelShowWidgetByPos(int32_t nX, int32_t nY) const noexcept;
	/** The interactive widget that encloses a position.
	 * If both a widget and an ancestor (certainly a container) encloses the
	 * position, the former is chosen.
	 * @param nX The x position.
	 * @param nY The y position.
	 * @return The index into StdViewLayout::getInteractiveWidgets() or -1 if outside.
	 */
	int32_t getInteractiveWidget(int32_t nX, int32_t nY) const noexcept;
	/** The interactive widget that encloses a position and is compatible with a owner context.
	 * If both a widget and an ancestor (certainly a container) encloses the
	 * position and is owner compatible, the former is chosen.
	 * @param nX The x position.
	 * @param nY The y position.
	 * @param nTeam The team of the widget or -1 if any.
	 * @param nMate The mate of the widget or -1 if any.
	 * @return The index into StdViewLayout::getInteractiveWidgets() or -1 if outside.
	 */
	int32_t getInteractiveWidget(int32_t nX, int32_t nY, int32_t nTeam, int32_t nMate) const noexcept;

	inline bool isValid() const noexcept { return m_bValid; }
	inline const std::string& getErrorString() const noexcept { return m_sErrorString; }
	void dump(int32_t nIndentSpaces) const noexcept override;
	using ThemeLayout::dump;
protected:
	void reInit(const shared_ptr<Layout>& refGameLayout, const shared_ptr<ThemeWidget>& refRootTW
				, const AppPreferences& oAppPreferences) noexcept;
private:
	void checkAndGet(const AppPreferences& oAppPreferences) noexcept;
	void traverse(const shared_ptr<ThemeWidget>& refTW, std::vector<int32_t>& aCanChangeDangling) noexcept;
	void setSizes(int32_t nLayoutConfig, int32_t nTileW) noexcept;
	void calcSize(const shared_ptr<ThemeWidget>& refTW, int32_t nLayoutConfig, int32_t nTileW) noexcept;
	void traverseDraw(const Cairo::RefPtr<Cairo::Context>& refCc, ThemeWidget& oTW, bool bSkipWidget) noexcept;
private:
	shared_ptr<Layout> m_refGameLayout;
	shared_ptr<LevelShowThemeWidget> m_refReferenceWidget;
	std::vector< shared_ptr<LevelShowThemeWidget> > m_aLevelShow;
	std::vector< shared_ptr<ThemeWidget> > m_aCanChange;
	std::vector< int32_t > m_aIfChanged; // Size: m_aCanChange.size(), Value: -1 or index into m_aCanChange
	std::vector< shared_ptr<ThemeWidgetInteractive> > m_aInteractive;

	//shortcut
	bool m_bIsSubshow;

	bool m_bValid;
	std::string m_sErrorString;
private:
	StdViewLayout(const StdViewLayout& oSource) = delete;
	StdViewLayout& operator=(const StdViewLayout& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_STD_VIEW_LAYOUT_H */

