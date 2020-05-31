/*
 * File:   varthwidgetfactory.h
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

#ifndef STMG_VAR_TH_WIDGET_FACTORY_H
#define STMG_VAR_TH_WIDGET_FACTORY_H

#include "stdthemewidgetfactory.h"
#include "widgetimpl/mutablethwidgetimpl.h"
#include "widgetimpl/relsizedthwidgetimpl.h"
#include "gtkutil/frame.h"
#include "themewidget.h"

#include <stmm-games/widgets/varwidget.h>
#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/recycler.h>

#include <glibmm/refptr.h>
#include <pangomm/fontdescription.h>
#include <pangomm/layout.h>

#include <memory>
#include <string>
#include <utility>

#include <stdint.h>

namespace stmg { class GameWidget; }
namespace stmg { class Image; }
namespace stmg { class SegmentedFunction; }
namespace stmg { class StdTheme; }
namespace stmg { class ThemeContainerWidget; }
namespace stmg { class TileAlpha; }
namespace stmg { class TileColor; }
namespace stmg { class TileFont; }

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }
namespace Pango { class Context; }

namespace stmg
{

using std::shared_ptr;
using std::unique_ptr;

class VarThWidgetFactory : public StdThemeWidgetFactory
{
public:
	VarThWidgetFactory(StdTheme* p1Owner, bool bTitlePreValue
						, const TileColor& oTitleColor, const TileAlpha& oTitleAlpha, const TileFont& oTitleFont
						, const shared_ptr<Image>& refTitleBgImg
						, const TileColor& oValueColor, const TileAlpha& oValueAlpha, const TileFont& oValueFont
						, const shared_ptr<Image>& refValueBgImg
						, const Frame& oFrame) noexcept;

	shared_ptr<ThemeWidget> create(const shared_ptr<GameWidget>& refGameWidget
									, double fTileWHRatio, const Glib::RefPtr<Pango::Context>& refFontContext) noexcept override;
private:
	class VarTWidget : public ThemeWidget
	{
	public:
		VarTWidget(VarThWidgetFactory* p1Owner
					, const shared_ptr<GameWidget>& refGameWidget, VarWidget* p0VarWidget
					, const Glib::RefPtr<Pango::Context>& refFontContext, double fTileWHRatio) noexcept;
		void reInit(VarThWidgetFactory* p1Owner
					, const shared_ptr<GameWidget>& refGameWidget, VarWidget* p0VarWidget
					, const Glib::RefPtr<Pango::Context>& refFontContext, double fTileWHRatio) noexcept;

		bool canChange() const noexcept override { return true; }

		NSize getSize() const noexcept override { return m_oMutaTW.getSize(); }
		NPoint getPos() const noexcept override { return m_oMutaTW.getPos(); }
		bool drawIfChanged(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept override { return m_oMutaTW.drawIfChanged(refCc); }
		void draw(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept override { m_oMutaTW.draw(refCc); }
		const std::pair<SegmentedFunction, SegmentedFunction>& getSizeFunctions(int32_t nLayoutConfig) const noexcept override;
		void placeAndMaybeResizeIn(const NRect& oRect) noexcept override;
		inline ThemeContainerWidget* getParent() const noexcept { return ThemeWidget::getParent(); }
		void dump(int32_t nIndentSpaces, bool bHeader) const noexcept override;
	protected:
		void onAssignedToLayout() noexcept override { m_oMutaTW.onAssignedToLayout(); }
		void onRecalcSizeFunctions(ThemeWidget* p0ReferenceThemeWidget) noexcept override { m_oSizedTW.onRecalcSizeFunctions(p0ReferenceThemeWidget); }
		void sizeAndConfig(int32_t nTileW, int32_t nLayoutConfig) noexcept override;
	private:
		void drawBase(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept;
		void drawVariable(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept;
		bool isChanged() const noexcept { return m_p0VarWidget->isChanged(); } 
		NSize getMinSize(int32_t nLayoutConfig) const noexcept;
		double getTextMaxValueWHRatio(int32_t nDigits) noexcept;
	private:
		void reInitCommon(const Glib::RefPtr<Pango::Context>& refFontContext, double fTileWHRatio) noexcept;
		void calcCanvas() noexcept;
	private:
		friend class VarThWidgetFactory;
		friend class MutableThWidgetImpl<VarTWidget>;
		friend class RelSizedThemeWidgetImpl<VarTWidget>;
		VarThWidgetFactory* m_p1Owner;
		VarWidget* m_p0VarWidget;
		//
		MutableThWidgetImpl<VarTWidget> m_oMutaTW;
		RelSizedThemeWidgetImpl<VarTWidget> m_oSizedTW;
		//Glib::RefPtr<Pango::Context> m_refFontContext;
		Glib::RefPtr<Pango::Layout> m_refTitleFontLayout;
		int32_t m_nTitleFontPixW;
		int32_t m_nTitleFontPixH;
		Glib::RefPtr<Pango::Layout> m_refValueFontLayout;
		int32_t m_nPixCanvasX;
		int32_t m_nPixCanvasY;
		int32_t m_nPixCanvasW;
		int32_t m_nPixCanvasH;
		int32_t m_nPixMinCanvasWidth;
		int32_t m_nPixMinCanvasHeight;
		bool m_bConstTitleBesideValue;
		int32_t m_nConstTitleFactorPerc;
		std::string m_sConstTitle;
		float m_fTitleAlign;
		int32_t m_nPixTitleX;
		int32_t m_nPixTitleY;
		int32_t m_nPixTitleW;
		int32_t m_nPixTitleH;
		double m_fMaxValueWHRatio;
		float m_fValueAlign;
		int32_t m_nPixValueX;
		int32_t m_nPixValueY;
		int32_t m_nPixValueW;
		int32_t m_nPixValueH;
		int32_t m_nPixValueTextX;
		int32_t m_nPixValueTextY;
		int32_t m_nPixValueTextW;
		int32_t m_nPixValueTextH;
	};
private:
	Recycler<VarTWidget> m_oVarTWidgets;

	const bool m_bTitlePreValue;
	double m_fTitleR1;
	double m_fTitleG1;
	double m_fTitleB1;
	double m_fTitleA1;
	unique_ptr<Pango::FontDescription> m_refTitleFont; // TODO is this needed?
	const shared_ptr<Image> m_refTitleBgImg;
	double m_fValueR1;
	double m_fValueG1;
	double m_fValueB1;
	double m_fValueA1;
	unique_ptr<Pango::FontDescription> m_refValueFont; // TODO is this needed?
	const shared_ptr<Image> m_refValueBgImg;
	const Frame m_oFrame;
};

} // namespace stmg

#endif	/* STMG_VAR_TH_WIDGET_FACTORY_H */

