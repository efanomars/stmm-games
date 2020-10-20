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
 * File:   previewthwidgetfactory.h
 */

#ifndef STMG_PREVIEW_TH_WIDGET_FACTORY_H
#define STMG_PREVIEW_TH_WIDGET_FACTORY_H

#include "stdthemewidgetfactory.h"
#include "themewidget.h"
#include "theme.h"
#include "widgetimpl/mutablethwidgetimpl.h"
#include "widgetimpl/relsizedthwidgetimpl.h"
#include "gtkutil/frame.h"
#include "gtkutil/tilesizing.h"

#include <stmm-games/widgets/previewwidget.h>
#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/recycler.h>

#include <glibmm/refptr.h>
#include <pangomm/context.h>
#include <pangomm/fontdescription.h>
#include <pangomm/layout.h>

#include <memory>
#include <string>
#include <utility>

#include <stdint.h>

namespace stmg { class Block; }
namespace stmg { class GameWidget; }
namespace stmg { class SegmentedFunction; }
namespace stmg { class StdTheme; }
namespace stmg { class ThemeContainerWidget; }
namespace stmg { class TileAlpha; }
namespace stmg { class TileColor; }
namespace stmg { class TileFont; }

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }

namespace stmg
{

using std::shared_ptr;
using std::unique_ptr;

class ThemeContext;

class PreviewThWidgetFactory : public StdThemeWidgetFactory
{
public:
	PreviewThWidgetFactory(StdTheme* p1Owner
						, const TileColor& oTextColor, const TileAlpha& oTextAlpha, const TileFont& oTextFont
						, const Frame& oFrame, const TileSizing& oTileSizing) noexcept;

	shared_ptr<ThemeWidget> create(const shared_ptr<GameWidget>& refGameWidget
									, double fTileWHRatio, const Glib::RefPtr<Pango::Context>& refFontContext) noexcept override;
private:
	class PreviewTWidget : public ThemeWidget, public Theme::RuntimeVariablesEnv
	{
	public:
		PreviewTWidget(PreviewThWidgetFactory* p1Owner
						, const shared_ptr<GameWidget>& refGameWidget, PreviewWidget* p0PreviewWidget
						, const Glib::RefPtr<Pango::Context>& refFontContext, double fTileWHRatio) noexcept;
		void reInit(PreviewThWidgetFactory* p1Owner
					, const shared_ptr<GameWidget>& refGameWidget, PreviewWidget* p0PreviewWidget
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

		int32_t getVariableIdFromName(const std::string& sVarName) noexcept override;
		int32_t getVariableValue(int32_t nVarId) noexcept override;

		protected:
		void onAssignedToLayout() noexcept override { m_oMutaTW.onAssignedToLayout(); }
		void onRecalcSizeFunctions(ThemeWidget* p0ReferenceThemeWidget) noexcept override { m_oSizedTW.onRecalcSizeFunctions(p0ReferenceThemeWidget); }
		void sizeAndConfig(int32_t nTileW, int32_t nLayoutConfig) noexcept override;
	private:
		void drawBase(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept;
		void drawVariable(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept;
		bool isChanged() const noexcept { return m_p0PreviewWidget->isChanged(); }
		NSize getMinSize(int32_t nLayoutConfig) const noexcept;
	private:
		void reInitCommon(const Glib::RefPtr<Pango::Context>& refFontContext, double fTileWHRatio) noexcept;
		void calcCanvas() noexcept;
		void drawText(const Cairo::RefPtr<Cairo::Context>& refCc
					, int32_t nX, int32_t nY, int32_t nW, int32_t nH, const std::string& sText) noexcept;
		void drawPreviewBlock(const Cairo::RefPtr<Cairo::Context>& refCc, StdTheme* p0StdTheme
							, const Block& oBlock, int32_t nShape
							, int32_t nTileW, int32_t nTileH, int32_t nPosX, int32_t nPosY) noexcept;
	private:
		friend class PreviewThWidgetFactory;
		friend class MutableThWidgetImpl<PreviewTWidget>;
		friend class RelSizedThemeWidgetImpl<PreviewTWidget>;
		PreviewThWidgetFactory* m_p1Owner;
		PreviewWidget* m_p0PreviewWidget;
		//
		MutableThWidgetImpl<PreviewTWidget> m_oMutaTW;
		RelSizedThemeWidgetImpl<PreviewTWidget> m_oSizedTW;
		Glib::RefPtr<Pango::Context> m_refFontContext;
		Glib::RefPtr<Pango::Layout> m_refFontLayout;
		int32_t m_nPixCanvasX;
		int32_t m_nPixCanvasY;
		int32_t m_nPixCanvasW;
		int32_t m_nPixCanvasH;
		// The number of shown tiles should be adjusted if the blocks get bigger
		int32_t m_nTotTilesW;
		int32_t m_nTotTilesH;
		shared_ptr<ThemeContext> m_refPreviewTc;
		//
		static const int32_t s_nPixCanvasMinSizeW;
		static const int32_t s_nPixCanvasMinSizeH;
		static const int32_t s_nPixMargin;
	};
private:
	Recycler<PreviewTWidget> m_oPreviewTWidgets;

	//friend class PreviewTWidget;
	unique_ptr<Pango::FontDescription> m_refFont; // TODO is this needed?
	double m_fR1;
	double m_fG1;
	double m_fB1;
	double m_fA1;
	const Frame m_oFrame;
	const TileSizing m_oTileSizing;
};

} // namespace stmg

#endif	/* STMG_PREVIEW_TH_WIDGET_FACTORY_H */

