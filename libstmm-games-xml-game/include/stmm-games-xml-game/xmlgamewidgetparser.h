/*
 * File:   xmlgamewidgetparser.h
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

#ifndef STMG_XML_GAME_WIDGET_PARSER_H
#define STMG_XML_GAME_WIDGET_PARSER_H

#include <stmm-games/relsizedgamewidget.h>
#include <stmm-games/containerwidget.h>
#include <stmm-games/gamewidget.h>
#include <stmm-games/ownertype.h>

#include <memory>
#include <vector>
#include <string>
#include <utility>

#include <stdint.h>

namespace stmg { class ConditionalCtx; }
namespace stmg { class LayoutCtx; }
namespace stmg { class XmlConditionalParser; }
namespace stmg { class XmlImageParser; }
namespace stmg { class XmlTraitsParser; }
namespace xmlpp { class Element; }

namespace stmg
{

using std::shared_ptr;
using std::unique_ptr;

class XmlLayoutParser;

////////////////////////////////////////////////////////////////////////////////
class XmlGameWidgetParser
{
public:
	virtual ~XmlGameWidgetParser() = default;
	/** Constructor.
	 * @param sGameWidgetName The game widget element name. Cannot be empty.
	 */
	XmlGameWidgetParser(const std::string& sGameWidgetName);

	/** The game widget name.
	 * @return The game widget name. Is not empty.
	 */
	const std::string& getGameWidgetName() const;

	/** Parses a game widget element.
	 * @param oCtx The context.
	 * @param p0Element The element. Cannot be null.
	 * @return The created widget. Is not null.
	 * @throws std::runtime_error.
	 */
	virtual shared_ptr<GameWidget> parseGameWidget(LayoutCtx& oCtx, const xmlpp::Element* p0Element) = 0;

protected:
	XmlConditionalParser& getXmlConditionalParser();
	XmlTraitsParser& getXmlTraitsParser();
	XmlImageParser& getXmlImageParser();

	/** Parse the base class attributes of GameWidget.
	 * Call this method if your widget derives directly from GameWidget.
	 * @param oInit The init structure to fill.
	 * @param oCtx The context.
	 * @param p0Element The element. Cannot be null.
	 */
	void parseGameWidget(GameWidget::Init& oInit, LayoutCtx& oCtx, const xmlpp::Element* p0Element);
	/** Parse the base class attributes of RelSizedGameWidget.
	 * Call this method if your widget derives directly from RelSizedGameWidget.
	 * @param oInit The init structure to fill.
	 * @param oCtx The context.
	 * @param p0Element The element. Cannot be null.
	 */
	void parseRelSizedGameWidget(RelSizedGameWidget::Init& oInit, LayoutCtx& oCtx, const xmlpp::Element* p0Element);
	/** Parse the base class attributes of ContainerWidget.
	 * Call this method if your widget is a container.
	 * @param oInit The init structure to fill.
	 * @param oCtx The context.
	 * @param p0WidgetElement The element. Cannot be null.
	 */
	void parseContainerWidget(ContainerWidget::Init& oInit, LayoutCtx& oCtx, const xmlpp::Element* p0WidgetElement);
	/** Parse the base class attributes of ContainerWidget.
	 * Call this method if your widget is a container and some child widget element names
	 * must be ignored.
	 * @param oInit The init structure to fill.
	 * @param oCtx The context.
	 * @param p0WidgetElement The element. Cannot be null.
	 * @param aSkipChildNames The child names ti skip.
	 */
	void parseContainerWidget(ContainerWidget::Init& oInit, LayoutCtx& oCtx, const xmlpp::Element* p0WidgetElement
							, const std::vector<std::string const*>& aSkipChildNames);
	/** Parse the child game widgets.
	 * This is used by container widgets.
	 * @param oCtx The context.
	 * @param p0Parent The parent element. Cannot be null.
	 * @return The widget instances.
	 */
	std::vector<shared_ptr<GameWidget>> parseChildWidgets(LayoutCtx& oCtx, const xmlpp::Element* p0Parent);
	/** Parse the child game widgets with exceptions.
	 * Even the child names that are skipped are added as valid child names with
	 * oCtx.addValidChildElementName(). There is therefore no need to check the
	 * valid names when oCtx.removeChecker() is called.
	 * @param oCtx The context.
	 * @param p0Parent The parent element. Cannot be null.
	 * @param aSkipChildNames The child element names that shouldn't be parsed.
	 * @return The widget instances.
	 */
	std::vector<shared_ptr<GameWidget>> parseChildWidgets(LayoutCtx& oCtx, const xmlpp::Element* p0Parent
														, const std::vector<std::string const*>& aSkipChildNames);

	/** Get event and message id for an event, given message name.
	 * The event must have been added to the level with its id.
	 * @param oCtx The context.
	 * @param p0Element The element containing the attributes. Cannot be null.
	 * @param bMandatory Whether event id and message must be defined.
	 * @param sEvIdAttrName The attribute name containing the event id. Cannot be empty.
	 * @param sMsgNameAttrName The attribute name containing the message. Cannot be empty.
	 * @param sMsgAttrName The attribute name containing the message name. Cannot be empty.
	 * @return The event id and the message or empty string and -1 if none of the attributes is defined.
	 * @throws std::runtime_error If the event with given id doesn't exist or the message name isn't an input to said event
	 *                            or the attributes are not defined and mandatory is true, etc.
	 */
	std::pair<std::string, int32_t> parseEvIdMessage(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, bool bMandatory
													, const std::string& sEvIdAttrName, const std::string& sMsgNameAttrName, const std::string& sMsgAttrName);

	std::pair<int32_t, OwnerType> parseVariable(LayoutCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sVarNameAttrName, bool bMandatory);
private:
	friend class XmlLayoutParser;
	const std::string m_sGameWidgetName;
	XmlLayoutParser* m_p1Owner; // set by XmlLayoutParser

private:
	XmlGameWidgetParser(const XmlGameWidgetParser& oSource) = delete;
	XmlGameWidgetParser& operator=(const XmlGameWidgetParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_GAME_WIDGET_PARSER_H */

