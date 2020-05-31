/*
 * File:   progressthwidgetfactory.h
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

#ifndef STMG_PROGRESS_TH_WIDGET_FACTORY_H
#define STMG_PROGRESS_TH_WIDGET_FACTORY_H

#include "stdthemewidgetfactory.h"
#include "widgetimpl/mutablethwidgetimpl.h"
#include "widgetimpl/relsizedthwidgetimpl.h"
#include "gtkutil/frame.h"
#include "themewidget.h"

#include <stmm-games/tile.h>
#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/recycler.h>
#include <stmm-games/widgets/progresswidget.h>

#include <memory>
#include <utility>

#include <stdint.h>

namespace stmg { class GameWidget; }
namespace stmg { class SegmentedFunction; }
namespace stmg { class StdTheme; }
namespace stmg { class ThemeContainerWidget; }

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }
namespace Glib { template <class T_CppObject> class RefPtr; }
namespace Pango { class Context; }

namespace stmg
{

class ProgressThWidgetFactory : public StdThemeWidgetFactory
{
public:
	ProgressThWidgetFactory(StdTheme* p1Owner
							, const TileColor& oNormalColor, const TileColor& oDangerColor
							, const Frame& oFrame) noexcept;

	shared_ptr<ThemeWidget> create(const shared_ptr<GameWidget>& refGameWidget
									, double fTileWHRatio, const Glib::RefPtr<Pango::Context>& refFontContext) noexcept override;
private:
	class ProgressTWidget : public ThemeWidget
	{
	public:
		ProgressTWidget(ProgressThWidgetFactory* p1Owner
					, const shared_ptr<GameWidget>& refGameWidget, ProgressWidget* p0ProgressWidget
					, double fTileWHRatio) noexcept;
		void reInit(ProgressThWidgetFactory* p1Owner
					, const shared_ptr<GameWidget>& refGameWidget, ProgressWidget* p0ProgressWidget
					, double fTileWHRatio) noexcept;

		bool canChange() const noexcept override { return true; }

		NSize getSize() const noexcept override { return m_oMutaTW.getSize(); }
		NPoint getPos() const noexcept override { return m_oMutaTW.getPos(); }
		bool drawIfChanged(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept override { return m_oMutaTW.drawIfChanged(refCc); }
		void draw(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept override { m_oMutaTW.draw(refCc); }
		const std::pair<SegmentedFunction, SegmentedFunction>& getSizeFunctions(int32_t nLayoutConfig) const noexcept override;
		void placeAndMaybeResizeIn(const NRect& oRect) noexcept override;
		inline ThemeContainerWidget* getParent() const { return ThemeWidget::getParent(); }
		void dump(int32_t nIndentSpaces, bool bHeader) const noexcept override;
	protected:
		void onAssignedToLayout() noexcept override { m_oMutaTW.onAssignedToLayout(); }
		void onRecalcSizeFunctions(ThemeWidget* p0ReferenceThemeWidget) noexcept override { m_oSizedTW.onRecalcSizeFunctions(p0ReferenceThemeWidget); }
		void sizeAndConfig(int32_t nTileW, int32_t nLayoutConfig) noexcept override;
	private:
		void reInitCommon(double fTileWHRatio) noexcept;
		void drawBase(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept;
		void drawVariable(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept;
		bool isChanged() const noexcept { return m_p0ProgressWidget->isChanged(); } 
		NSize getMinSize(int32_t nLayoutConfig) const noexcept;
	private:
		void calcCanvas() noexcept;
	private:
		friend class ProgressThWidgetFactory;
		friend class MutableThWidgetImpl<ProgressTWidget>;
		friend class RelSizedThemeWidgetImpl<ProgressTWidget>;
		ProgressThWidgetFactory* m_p1Owner;
		ProgressWidget* m_p0ProgressWidget;
		//
		MutableThWidgetImpl<ProgressTWidget> m_oMutaTW;
		RelSizedThemeWidgetImpl<ProgressTWidget> m_oSizedTW;
		int32_t m_nPixCanvasX;
		int32_t m_nPixCanvasY;
		int32_t m_nPixCanvasW;
		int32_t m_nPixCanvasH;
		int32_t m_nPixMinCanvasWidth;
		int32_t m_nPixMinCanvasHeight;
		int32_t m_nMinValue;
		int32_t m_nMaxValue;
		int32_t m_nThresholdValue;
		bool m_bThresholdValid;
		bool m_bDangerBelow;
	};
private:
	Recycler<ProgressTWidget> m_oProgressTWidgets;

	const TileColor m_oNormalColor;
	const TileColor m_oDangerColor;
	const Frame m_oFrame;
};

} // namespace stmg

#endif	/* STMG_PROGRESS_TH_WIDGET_FACTORY_H */

