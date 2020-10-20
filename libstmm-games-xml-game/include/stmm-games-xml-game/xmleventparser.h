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
 * File:   xmleventparser.h
 */

#ifndef STMG_XML_EVENT_PARSER_H
#define STMG_XML_EVENT_PARSER_H

#include <stmm-games/event.h>

#include <memory>
#include <string>

#include <stdint.h>

namespace stmg { class Block; }
namespace stmg { class ConditionalCtx; }
namespace stmg { class GameCtx; }
namespace stmg { class XmlConditionalParser; }
namespace stmg { class XmlTraitsParser; }
namespace xmlpp { class Element; }

namespace stmg
{

using std::unique_ptr;

class XmlGameParser;

class XmlEventParser
{
public:
	virtual ~XmlEventParser() = default;
	/** Constructor.
	 * @param sEventName The event name. Cannot be empty.
	 */
	XmlEventParser(const std::string& sEventName);

	/** The event name.
	 * @return The event name.
	 */
	const std::string& getEventName() const;

	/** Parse and create an event instance.
	 * The implementation must call integrateAndAdd() to add the
	 * newly created event to the level which will own it.
	 * @param oCtx The game context.
	 * @param p0Element The element. Cannot be null.
	 * @return The created event. Non owning. Is not null.
	 * @throws std::runtime_error.
	 */
	virtual Event* parseEvent(GameCtx& oCtx, const xmlpp::Element* p0Element) = 0;

	/** Recycle the event if it is of a type created by this instance.
	 * The default implementation is empty.
	 * @param refEvent The event. Cannot be null. If recycled is null after function call.
	 */
	virtual void recycleEvents(std::unique_ptr<Event>& refEvent);

	/** Returns event type's msg value from msg name.
	 * Usually the msg name is the same as defined in the event class as an enum without
	 * the 'MESSAGE_' prefix.
	 *
	 * Subclasses of this function should call XmlEventParser::parseEventMsgName()
	 * after checking their own enums. The function will throw an error.
	 * @param oCtx The game context.
	 * @param p0Element The element containing the the attribute. Cannot be null.
	 * @param sAttr The attribute name containing the message name. Cannot be empty.
	 * @param sMsgName The message name. Cannot be empty.
	 * @return The message value.
	 * @throws If the msg name is unknown.
	 */
	virtual int32_t parseEventMsgName(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
									, const std::string& sMsgName);
	/** Returns event type listener group value from name.
	 * Usually listener group the name is the same as defined in the event class as
	 * an enum without the 'LISTENER_GROUP_' prefix.
	 *
	 * Since also the Event base class has at least the LISTENER_GROUP_FINISHED enum
	 * and therefore the name 'FINISHED' defined, subclasses of this function
	 * should call XmlEventParser::parseEventListenerGroupName() after checking
	 * their own enums.
	 * @param oCtx The game context.
	 * @param p0Element The element containing the the attribute. Cannot be null.
	 * @param sAttr The attribute name containing the listener group name. Cannot be empty.
	 * @param sListenerGroupName The listener group name. Cannot be empty.
	 * @return The listener group value.
	 * @throws If the listener group name is unknown.
	 */
	virtual int32_t parseEventListenerGroupName(GameCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
												, const std::string& sListenerGroupName);

protected:
	/** Return named block.
	 * @param oCtx The context.
	 * @param sName The block name. Cannot be empty.
	 * @param oBlock The block structure to be filled.
	 * @return Whether a block with the given name exists.
	 */
	bool getBlock(GameCtx& oCtx, const std::string& sName, Block& oBlock);

	/** Parses an event and adds it to the level.
	 * This method can be used by events that directly handle other events.
	 * @param oCtx The context.
	 * @param p0Element The event element. Cannot be null.
	 * @return The event. Not null.
	 */
	Event* parseChildEvent(GameCtx& oCtx, const xmlpp::Element* p0Element);

	/** Parse event base.
	 * This function should be called rather than filling the attributes by hand
	 * in the subclass parser.
	 * @param oCtx The context.
	 * @param p0Element The event element. Cannot be null.
	 * @param oInit The even initialization data to fill.
	 */
	void parseEventBase(GameCtx& oCtx, const xmlpp::Element* p0Element, Event::Init& oInit);

	/** Integrate the event add to level.
	 * Performs the xml specific integration that links the event to others and to widgets
	 * and adds the instance to the level, which will own it.
	 * @param oCtx The context.
	 * @param refEvent The event. Cannot be null.
	 * @param p0Element The event element. Cannot be null.
	 * @return The added event. Is always refEvent.get().
	 */
	Event* integrateAndAdd(GameCtx& oCtx, unique_ptr<Event> refEvent, const xmlpp::Element* p0Element);

	/** Helper function for the 'repeat' attribute.
	 * Expects the value to be &gt; 0 or -1 (infinite). Default is -1.
	 * @param oCtx The context.
	 * @param p0EventElement The event element. Cannot be null.
	 * @return The repeat value.
	 * @throws std::runtime_exception.
	 */
	int32_t parseEventAttrRepeat(GameCtx& oCtx, const xmlpp::Element* p0EventElement);
	/** Helper function for the 'step' attribute.
	 * Expects the value to be &gt; 0. Default is 1.
	 * @param oCtx The context.
	 * @param p0EventElement The event element. Cannot be null.
	 * @return The step value.
	 * @throws std::runtime_exception.
	 */
	int32_t parseEventAttrStep(GameCtx& oCtx, const xmlpp::Element* p0EventElement);
	/** Helper function for the 'prob' attribute.
	 * Expects the value to be &gt; 0. Default is 1.
	 * @param oCtx The context.
	 * @param p0EventElement The event element. Cannot be null.
	 * @return The prob value.
	 * @throws std::runtime_exception.
	 */
	int32_t parseEventAttrRandomProb(GameCtx& oCtx, const xmlpp::Element* p0EventElement);

	/** Tells whether a child element name of Event is reserved.
	 * Use this when using getXmlConditionalParser().visitElementChildren().
	 *
	 * Example: ListenerGroup or Filter.
	 * @param sElementName The element name in question. Cannot be empty.
	 * @return Whether the element name is reserved.
	 */
	bool isReservedChildElementOfEvent(const std::string& sElementName) const;
	//bool isReservedAttributeOfEvent(const char* p0AttrName) const;

	/** Get the conditional parser.
	 * Cannot be called from the subclass constructor.
	 * @return The parser.
	 */
	XmlConditionalParser& getXmlConditionalParser();
	/** Get the traits parser.
	 * Cannot be called from the subclass constructor.
	 * @return The parser.
	 */
	XmlTraitsParser& getXmlTraitsParser();

private:
	friend class XmlGameParser;
	const std::string m_sEventName;
	XmlGameParser* m_p1Owner; // set by XmlGameParser
	XmlConditionalParser* m_p0XmlConditionalParser; // set by XmlGameParser
	XmlTraitsParser* m_p0XmlTraitsParser; // set by XmlGameParser

private:
	XmlEventParser() = delete;
	XmlEventParser(const XmlEventParser& oSource) = delete;
	XmlEventParser& operator=(const XmlEventParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_EVENT_PARSER_H */

