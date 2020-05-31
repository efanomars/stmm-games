/*
 * File:   themewidget.h
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

#ifndef STMG_THEME_WIDGET_H
#define STMG_THEME_WIDGET_H

#include <stmm-games/util/basictypes.h>

#include <memory>
#include <utility>

#include <stdint.h>

namespace stmg { class GameWidget; }
namespace stmg { class SegmentedFunction; }

namespace Cairo { class Context; }
namespace Cairo { template <typename T_CastFrom> class RefPtr; }

namespace stmg
{

using std::shared_ptr;

class ThemeLayout;

class ThemeContainerWidget;

class ThemeWidget
{
public:
	virtual ~ThemeWidget() noexcept = default;

	/** The size of the widget.
	 * Should only be called after the widget has been positioned.
	 * @return The size.
	 */
	virtual NSize getSize() const noexcept = 0;
	/** The position of the widget.
	 * Should only be called after the widget has been positioned.
	 * @return The position.
	 */
	virtual NPoint getPos() const noexcept = 0;

	/** Whether the widget's appearance (or one of its parents) can change.
	 * This is a static property of the widget calculated when it is already
	 * inserted in its final layout and shouldn't change during its lifetime.
	 * If a call to drawIfChanged() might return `true` this function returns 
	 * `true`, `false` otherwise.
	 * @return Whether the widget can change.
	 */
	virtual bool canChange() const noexcept = 0;
	/** Deep draws a widget if it changed.
	 * If this is a ThemeContainerWidget it doesn't draw the children, just itself
	 * possibly on top of its parent (clipped to this widget).
	 * @see draw().
	 * @param refCc The context to conditionally draw to. Cannot be null.
	 * @return Whether the widget area was drawn.
	 */
	virtual bool drawIfChanged(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept = 0;
	/** Draws a widget.
	 * The draw is shallow, so it can assume that the parent was already drawn
	 * to the context.
	 *
	 * When this function is called this instance can assume that the parent
	 * was modified. For mutable widgets this could for example mean they
	 * should invalidate their cache if they have one.
	 *
	 * If a widget has no parents it must draw completely opaquely.
	 *
	 * @param refCc The context to draw to. Cannot be null.
	 */
	virtual void draw(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept = 0;
	/** Deep draws a widget.
	 * The default implementation calls drawDeep of the parent (if it has one)
	 * clipped to this widget boundaries and then draw() for this instance.
	 *
	 * This should be overridden for example if a widget knows it is completely
	 * opaque and drawing all the ancestors first is unnecessary.
	 * @param refCc The context to draw to. Cannot be null.
	 */
	virtual void drawDeep(const Cairo::RefPtr<Cairo::Context>& refCc) noexcept;

	/** Tells Whether the widget is a container.
	 * @return Whether the widget is subclass of ThemeContainerWidget.
	 */
	inline bool isContainer() const noexcept { return m_bIsContainer; }
	/** The game widget used to generate the theme widget.
	 * @return The model. Cannot be null;
	 */
	inline const shared_ptr<GameWidget>& getModel() const noexcept { return m_refModel; }
	/** The parent container of the widget.
	 * @return The parent or null if layout's root widget.
	 */
	inline ThemeContainerWidget* getParent() const noexcept { return m_p0Parent; }

	/** Gets the width and height functions that take the tile width as parameter.
	 * @param nLayoutConfig The layout config.
	 * @return The pair of functions.
	 */
	virtual const std::pair<SegmentedFunction, SegmentedFunction>& getSizeFunctions(int32_t nLayoutConfig) const noexcept = 0;
	/** Places and possibly resizes the widget within the given area.
	 * The placement and resizing depends on align value returned by the model's functions
	 * GameWidget::getWidgetAxisFitHoriz() and GameWidget::getWidgetAxisFitVert().
	 *
	 * The width and height should be bigger or equal the size of the widget as
	 * defined by its size functions with the parameters passed to sizeAndConfig().
	 *
	 * Container widgets must recursively call the placeAndMaybeResizeIn() of their
	 * children (they are responsible for their positioning).
	 * @param oRect The rectangle where the widget should be placed.
	 */
	virtual void placeAndMaybeResizeIn(const NRect& oRect) noexcept = 0;

	/** Writes object contents to std::cout.
	 * Should only do it in debug mode. The function is also exposed in release mode
	 * because it is virtual.
	 * @param nIndentSpaces Number of spaces put in front of each output line.
	 * @param bHeader Whether the header of the object should be displayed.
	 */
	virtual void dump(int32_t nIndentSpaces, bool bHeader) const noexcept;
	/** Writes object contents to std::cout (header included).
	 * @param nIndentSpaces Number of spaces put in front of each output line.
	 */
	void dump(int32_t nIndentSpaces) const noexcept
	{
		dump(nIndentSpaces, true);
	}
	/** Writes contents to std::cout without indentation.
	 */
	void dump() const noexcept
	{
		dump(0);
	}
protected:
	/** The theme layout.
	 * This function can only be called if the widget is already added to a layout.
	 *
	 * Subclasses that call this function must include themelayout.h header file.
	 * @return The theme layout.
	 */
	ThemeLayout& themeLayout() noexcept;
	/** The constant theme layout.
	 * This function can only be called if the widget is already added to a layout.
	 *
	 * Subclasses that call this function must include themelayout.h header file.
	 * @return The constant theme layout.
	 */
	const ThemeLayout& themeLayout() const noexcept;
	/** Signal that widget was added to a layout.
	 * When the theme widget tree is added to the theme layout this function
	 * is called for all its widgets. Container widgets don't need to call
	 * their children's onAssignedToLayout().
	 *
	 * The default implementation does nothing.
	 */
	virtual void onAssignedToLayout() noexcept {}
	/** Signal the widget should define its size functions.
	 * The reference widget can be used to determine this widget's size functions.
	 *
	 * Container widgets don't need to call their children's onRecalcSizeFunctions(),
	 * since it is done by the layout itself.
	 * @see getSizeFunctions().
	 * @param p0ReferenceThemeWidget The reference theme widget. Cannot be null.
	 */
	virtual void onRecalcSizeFunctions(ThemeWidget* p0ReferenceThemeWidget) noexcept;
	/** Sets the tile size of the widget and its configuration.
	 * The widget should evaluate and set the actual size of the widget through
	 * its size functions.
	 * 
	 * This function is called when the layout has determined the best tile size
	 * and layout config in order to maximize the tile size itself.
	 * @param nTileW The tile width. Must be &gt;0.
	 * @param nLayoutConfig The layout config.
	 */
	virtual void sizeAndConfig(int32_t nTileW, int32_t nLayoutConfig) noexcept = 0;

	/** Constructs the theme widget.
	 * @param refModel The model. Cannot be null.
	 */
	explicit ThemeWidget(const shared_ptr<GameWidget>& refModel) noexcept
	: m_p0Parent(nullptr)
	, m_bIsContainer(false)
	, m_p0ThemeLayout(nullptr)
	, m_refModel(refModel)
	{
	}
	/** Reinitializes the theme widget.
	 * @param refModel The model. Cannot be null.
	 */
	void reInit(const shared_ptr<GameWidget>& refModel) noexcept
	{
		m_p0Parent = nullptr;
		m_bIsContainer = false;
		m_p0ThemeLayout = nullptr;
		m_refModel = refModel;
	}
private:
	void setThemeLayout(ThemeLayout* p0ThemeLayout) noexcept
	{
		m_p0ThemeLayout = p0ThemeLayout;
	}
private:
	friend class ThemeContainerWidget;
	friend class ThemeLayout;
	ThemeContainerWidget* m_p0Parent;
	bool m_bIsContainer;
	ThemeLayout* m_p0ThemeLayout;
	shared_ptr<GameWidget> m_refModel;
};

} // namespace stmg

#endif	/* STMG_THEME_WIDGET_H */

