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
 * File:   levelshowthwidgetfactory.h
 */

#ifndef STMG_LEVEL_SHOW_TH_WIDGET_FACTORY_H
#define STMG_LEVEL_SHOW_TH_WIDGET_FACTORY_H

#include "stdthemewidgetfactory.h"
#include "levelshowthemewidget.h"

#include "widgetimpl/mutablethwidgetimpl.h"

#include "gtkutil/frame.h"
#include "gtkutil/segmentedfunction.h"

#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/recycler.h>

#include <memory>
#include <utility>

#include <stdint.h>

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }
namespace Glib { template <class T_CppObject> class RefPtr; }
namespace Pango { class Context; }
namespace stmg { class GameWidget; }
namespace stmg { class LevelShowWidget; }
namespace stmg { class StdTheme; }
namespace stmg { class ThemeContainerWidget; }
namespace stmg { class ThemeWidget; }
namespace stmi { class Event; }

namespace stmg
{

class LevelShowThWidgetFactory : public StdThemeWidgetFactory
{
public:
	/** Constructor.
	 * For each frame's side the maximum of the minimal size in pixels and the size in tiles
	 * is chosen. Ex. `nTopPixel = max(nTileW * fMinTop, oFrame.m_nPixMinTop)`.
	 * @param p1Owner The theme to which this factory will be added.
	 * @param oFrame The frame.
	 * @param fMinTop The minimum size of the frame top border in tiles.
	 * @param fMinBottom The minimum size of the frame bottom border in tiles.
	 * @param fMinLeft The minimum size of the frame left border in tiles.
	 * @param fMinRight The minimum size of the frame right border in tiles.
	 */
	LevelShowThWidgetFactory(StdTheme* p1Owner, const Frame& oFrame, double fMinTop, double fMinBottom, double fMinLeft, double fMinRight) noexcept;
	//
	shared_ptr<ThemeWidget> create(const shared_ptr<GameWidget>& refGameWidget
									, double fTileWHRatio, const Glib::RefPtr<Pango::Context>& refFontContext) noexcept override;
private:
	class LevelShowTWidget : public LevelShowThemeWidget
	{
	public:
		LevelShowTWidget(LevelShowThWidgetFactory* p1Owner, const shared_ptr<GameWidget>& refGameWidget, LevelShowWidget* p0LevelShowWidget
						, double fTileWHRatio) noexcept;
		void reInit(LevelShowThWidgetFactory* p1Owner, const shared_ptr<GameWidget>& refGameWidget, LevelShowWidget* p0LevelShowWidget
					, double fTileWHRatio) noexcept;
		// The contents of the widget always change but the widget itself doesn't
		// this is already taken into account by the layout, which calls
		// drawDeep (which uses cached background) clipped to the canvas before
		// drawing the board on top of it
		bool canChange() const noexcept override { return false; }

		NSize getSize() const noexcept override { return m_oLeafTW.getSize(); }
		NPoint getPos() const noexcept override { return m_oLeafTW.getPos(); }
		bool drawIfChanged(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept override { return m_oLeafTW.drawIfChanged(refCc); }
		void drawDeep(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept override { m_oLeafTW.drawDeep(refCc); }
		void draw(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept override { m_oLeafTW.draw(refCc); }
		void placeAndMaybeResizeIn(const NRect& oRect) noexcept override;
		inline ThemeContainerWidget* getParent() const noexcept { return LevelShowThemeWidget::getParent(); }

		const std::pair<SegmentedFunction, SegmentedFunction>& getSizeFunctions(int32_t nLayoutConfig) const noexcept override;

		NPoint getCanvasPixPos() noexcept override;
		bool handleXYInput(const shared_ptr<stmi::Event>& refXYEvent, int32_t nTeam, int32_t nMate) noexcept override;
		void dump(int32_t nIndentSpaces, bool bHeader) const noexcept override;
	protected:
		void onAssignedToLayout() noexcept override { m_oLeafTW.onAssignedToLayout(); }
		void onRecalcSizeFunctions(ThemeWidget* p0ReferenceThemeWidget) noexcept override;
		void sizeAndConfig(int32_t nTileW, int32_t nLayoutConfig) noexcept override;
	private:
		void reInitCommon(LevelShowWidget* p0LevelShowWidget, double fTileWHRatio) noexcept;
		void drawBase(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept;
		void drawVariable(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept;
		bool isChanged() const noexcept { return false; }
	private:
		void calcCanvas() noexcept;
	private:
		friend class LevelShowThWidgetFactory;
		friend class MutableThWidgetImpl<LevelShowTWidget>;
		LevelShowThWidgetFactory* m_p1Owner;
		MutableThWidgetImpl<LevelShowTWidget> m_oLeafTW; // holds the size and pos of the widget
		double m_fTop; // in tiles
		double m_fBottom; // in tiles
		double m_fLeft; // in tiles
		double m_fRight; // in tiles
		int32_t m_nTileW;
		int32_t m_nTileH;
		int32_t m_nCanvasPixX; // relative to the widget's origin
		int32_t m_nCanvasPixY; // relative to the widget's origin
		int32_t m_nCanvasPixW; // width of board area (no margins)
		int32_t m_nCanvasPixH; // height of board area (no margins)
		std::pair<SegmentedFunction, SegmentedFunction> m_oSizeFunctions;
	};
private:
	Recycler<LevelShowTWidget> m_oLevelShowTWidgets;
	// The following is not switched!
	Frame m_oFrame;
	double m_fMinTop; // in tiles
	double m_fMinBottom; // in tiles
	double m_fMinLeft; // in tiles
	double m_fMinRight; // in tiles
};

} // namespace stmg

#endif	/* STMG_LEVEL_SHOW_TH_WIDGET_FACTORY_H */

