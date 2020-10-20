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
 * File:   transparentthwidgetfactory.h
 */

#ifndef STMG_TRANSPARENT_TH_WIDGET_FACTORY_H
#define STMG_TRANSPARENT_TH_WIDGET_FACTORY_H

#include "stdthemewidgetfactory.h"
#include "widgetimpl/immutablethwidgetimpl.h"
#include "widgetimpl/relsizedthwidgetimpl.h"
#include "themewidget.h"

#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/recycler.h>

#include <memory>
#include <utility>

#include <stdint.h>

namespace stmg { class GameWidget; }
namespace stmg { class SegmentedFunction; }
namespace stmg { class StdTheme; }
namespace stmg { class ThemeContainerWidget; }
namespace stmg { class TransparentWidget; }

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }
namespace Glib { template <class T_CppObject> class RefPtr; }
namespace Pango { class Context; }

namespace stmg
{

class TransparentThWidgetFactory : public StdThemeWidgetFactory
{
public:
	explicit TransparentThWidgetFactory(StdTheme* p1Owner) noexcept;

	shared_ptr<ThemeWidget> create(const shared_ptr<GameWidget>& refGameWidget
									, double fTileWHRatio, const Glib::RefPtr<Pango::Context>& refFontContext) noexcept override;
private:
	class TransparentTWidget : public ThemeWidget
	{
	public:
		TransparentTWidget(TransparentThWidgetFactory* p1Owner
					, const shared_ptr<GameWidget>& refGameWidget, TransparentWidget* p0TransparentWidget) noexcept;
		void reInit(TransparentThWidgetFactory* p1Owner
					, const shared_ptr<GameWidget>& refGameWidget, TransparentWidget* p0TransparentWidget) noexcept;

		bool canChange() const noexcept override { return false; }

		NSize getSize() const noexcept override { return m_oImmuTW.getSize(); }
		NPoint getPos() const noexcept override { return m_oImmuTW.getPos(); }
		bool drawIfChanged(const Cairo::RefPtr<Cairo::Context>& /*refCc*/) noexcept override { return false; }
		void draw(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept override;
		const std::pair<SegmentedFunction, SegmentedFunction>& getSizeFunctions(int32_t nLayoutConfig) const noexcept override;
		void placeAndMaybeResizeIn(const NRect& oRect) noexcept override;
		inline ThemeContainerWidget* getParent() const noexcept { return ThemeWidget::getParent(); }
		void dump(int32_t nIndentSpaces, bool bHeader) const noexcept override;
	protected:
		void onAssignedToLayout() noexcept override { m_oImmuTW.onAssignedToLayout(); }
		void onRecalcSizeFunctions(ThemeWidget* p0ReferenceThemeWidget) noexcept override { m_oSizedTW.onRecalcSizeFunctions(p0ReferenceThemeWidget); }
		void sizeAndConfig(int32_t nTileW, int32_t nLayoutConfig) noexcept override;
	private:
		void reInitCommon() noexcept;
		bool isChanged() const noexcept { return false; }
		NSize getMinSize(int32_t nLayoutConfig) const noexcept;
		//double getTextMaxValueWHRatio(int32_t nDigits);
	private:
		friend class TransparentThWidgetFactory;
		friend class ImmutableThWidgetImpl<TransparentTWidget>;
		friend class RelSizedThemeWidgetImpl<TransparentTWidget>;
		TransparentThWidgetFactory* m_p1Owner;
		TransparentWidget* m_p0TransparentWidget;
		//
		ImmutableThWidgetImpl<TransparentTWidget> m_oImmuTW;
		RelSizedThemeWidgetImpl<TransparentTWidget> m_oSizedTW;
	};
private:
	Recycler<TransparentTWidget> m_oTransparentTWidgets;
};

} // namespace stmg

#endif	/* STMG_TRANSPARENT_TH_WIDGET_FACTORY_H */

