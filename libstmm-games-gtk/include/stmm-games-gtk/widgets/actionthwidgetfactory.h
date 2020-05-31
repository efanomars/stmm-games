/*
 * File:   actionthwidgetfactory.h
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

#ifndef STMG_ACTION_TH_WIDGET_FACTORY_H
#define STMG_ACTION_TH_WIDGET_FACTORY_H

#include "stdthemewidgetfactory.h"
#include "themewidgetinteractive.h"

#include "themewidget.h"

#include "widgetimpl/immutablethwidgetimpl.h"
#include "widgetimpl/relsizedthwidgetimpl.h"

#include "gtkutil/frame.h"

#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/recycler.h>

#include <glibmm/refptr.h>

#include <pangomm/fontdescription.h>
#include <pangomm/layout.h>

#include <memory>
#include <string>
#include <utility>

#include <stdint.h>

namespace stmg { class ActionWidget; }
namespace stmg { class GameWidget; }
namespace stmg { class Image; }
namespace stmg { class SegmentedFunction; }
namespace stmg { class StdTheme; }
namespace stmg { class ThemeContainerWidget; }
namespace stmg { class TileAlpha; }
namespace stmg { class TileColor; }
namespace stmg { class TileFont; }
namespace stmi { class Event; }

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }
namespace Pango { class Context; }

namespace stmg
{

using std::shared_ptr;
using std::unique_ptr;

class ActionThWidgetFactory : public StdThemeWidgetFactory
{
public:
	/** Constructor.
	 * @param p1Owner The theme. Cannot be null.
	 * @param oTextColor The text color.
	 * @param oTextAlpha The text alpha.
	 * @param oTextFont The text font.
	 * @param oFrame The background frame.
	 */
	ActionThWidgetFactory(StdTheme* p1Owner
							, const TileColor& oTextColor, const TileAlpha& oTextAlpha, const TileFont& oTextFont
							, const Frame& oFrame) noexcept;
	shared_ptr<ThemeWidget> create(const shared_ptr<GameWidget>& refGameWidget
									, double fTileWHRatio
									, const Glib::RefPtr<Pango::Context>& refFontContext) noexcept override;
private:
	class KeyActionTWidget : public ThemeWidget, public ThemeWidgetInteractive
	{
	public:
		KeyActionTWidget(ActionThWidgetFactory* p1Owner
						, const shared_ptr<GameWidget>& refGameWidget, ActionWidget* p0ActionWidget
						, const Glib::RefPtr<Pango::Context>& refFontContext, double fTileWHRatio) noexcept;

		void reInit(ActionThWidgetFactory* p1Owner
					, const shared_ptr<GameWidget>& refGameWidget, ActionWidget* p0ActionWidget
					, const Glib::RefPtr<Pango::Context>& refFontContext, double fTileWHRatio) noexcept;

		//
		bool handleXYInput(const shared_ptr<stmi::Event>& refXYEvent, int32_t nTeam, int32_t nMate) noexcept override;
		//
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
		void drawBase(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept;
		void drawVariable(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept;
		bool isChanged() const noexcept { return false; } 
		NSize getMinSize(int32_t nLayoutConfig) const noexcept;
	private:
		void reInitCommon(const Glib::RefPtr<Pango::Context>& refFontContext, double fTileWHRatio) noexcept;
		inline const Glib::RefPtr<Pango::Layout>& getFontLayout() const noexcept { return m_refFontLayout; }
		void calcCanvas() noexcept;
		double getTextScale(int32_t nInCanvasW, int32_t nInCanvasH) noexcept;
	private:
		friend class ActionThWidgetFactory;
		friend class ImmutableThWidgetImpl<KeyActionTWidget>;
		friend class RelSizedThemeWidgetImpl<KeyActionTWidget>;
		ActionThWidgetFactory* m_p1Owner;
		ActionWidget* m_p0ActionWidget;
		//
		ImmutableThWidgetImpl<KeyActionTWidget> m_oImmuTW;
		RelSizedThemeWidgetImpl<KeyActionTWidget> m_oSizedTW;
		Glib::RefPtr<Pango::Layout> m_refFontLayout;
		int32_t m_nPixCanvasX;// relative to the widget rectangle
		int32_t m_nPixCanvasY;// relative to the widget rectangle
		int32_t m_nPixCanvasW;
		int32_t m_nPixCanvasH;
		shared_ptr<Image> m_refConstImg;
		std::string m_sConstText;
		int32_t m_nTextFontPixW;
		int32_t m_nTextFontPixH;
	};
private:
	Recycler<KeyActionTWidget> m_oKeyActionTWidgets;

	double m_fR1;
	double m_fG1;
	double m_fB1;
	double m_fA1;
	unique_ptr<Pango::FontDescription> m_refFont;
	const Frame m_oFrame;
};

} // namespace stmg

#endif	/* STMG_ACTION_TH_WIDGET_FACTORY_H */

