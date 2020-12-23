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
 * File:   parserctx.h
 */

#ifndef STMG_PARSER_CTX_H
#define STMG_PARSER_CTX_H

#include <stmm-games/apppreferences.h>

#include <vector>
#include <stdexcept>
#include <cassert>
#include <algorithm>
#include <memory>
#include <string>

#include <stdint.h>

namespace stmg { class AppConfig; }
namespace stmg { class Named; }
namespace xmlpp { class Element; }

namespace stmg
{

using std::shared_ptr;

////////////////////////////////////////////////////////////////////////////////
class ParserCtx
{
public:
	/** Constructor.
	 * Used when parsing theme.
	 * @param refAppConfig The app config. Cannot be null.
	 * @param oNamed The named.
	 */
	ParserCtx(const shared_ptr<AppConfig>& refAppConfig, Named& oNamed)
	: m_refAppConfig(refAppConfig)
	, m_refAppPreferences()
	, m_oNamed(oNamed)
	{
		assert(refAppConfig);
	}
	/** Constructor.
	 * Used when parsing game.
	 * @param refAppPreferences The app preferences. Cannot be null.
	 * @param oNamed The named.
	 */
	ParserCtx(const shared_ptr<AppPreferences>& refAppPreferences, Named& oNamed)
	: m_refAppConfig((assert(refAppPreferences), refAppPreferences->getAppConfig()))
	, m_refAppPreferences(refAppPreferences)
	, m_oNamed(oNamed)
	{
	}
	virtual ~ParserCtx();
	/** The app config.
	 * @return The config. Is not null.
	 */
	inline const shared_ptr<AppConfig>& appConfig() { return m_refAppConfig; }
	/** The app preferences.
	 * Not defined in a theme context.
	 * @return The preferences or null if not relevant.
	 */
	inline const shared_ptr<AppPreferences>& appPreferences() { return m_refAppPreferences; }
	/** The named assets.
	 * @return The named of the game or theme being parsed.
	 */
	inline Named& named() { return m_oNamed; }

	/** Push a context.
	 * This string context can be used to give better error messages.
	 * It is used by ParserCtx::error().
	 * @param sStr The string. Cannot be empty.
	 */
	inline void pushCtx(const std::string& sStr) { m_oStack.push_back(sStr); }
	/** Pop a context.
	 * Each pushCtx() should have a corresponding popCtx().
	 */
	inline void popCtx() { m_oStack.pop_back(); }

public:
	/** Adds a checker for the element.
	 * A checker is used to make sure all attributes and child elements of an element
	 * are expected.
	 *
	 * If an element already has a checker a reference count is increased.
	 *
	 * The check happens (if requested) in the corresponding removeChecker() function.
	 * @param p0Element The element. Cannot be null.
	 */
	virtual void addChecker(const xmlpp::Element* p0Element);
	/** Declares a child element name as valid.
	 * @param p0Element The element. Cannot be null.
	 * @param sChildElementName The child element name. Cannot be empty.
	 */
	void addValidChildElementName(const xmlpp::Element* p0Element, const std::string& sChildElementName);
	/** Declares a child element name as valid.
	 * @param p0Element The element. Cannot be null.
	 * @param sChildElementName The child element name. Cannot be empty.
	 */
	void addValidChildElementNames(const xmlpp::Element* p0Element, const std::string& sChildElementName)
	{
		addValidChildElementName(p0Element, sChildElementName);
	}
	/** Declares multiple child element names as valid.
	 * @param p0Element The element. Cannot be null.
	 * @param sChildElementName A child element name. Cannot be empty.
	 * @param oNames Further child element names.
	 */
	template<typename ...Names>
	void addValidChildElementNames(const xmlpp::Element* p0Element, const std::string& sChildElementName, const Names&... oNames)
	{
		addValidChildElementName(p0Element, sChildElementName);
		addValidChildElementNames(p0Element, oNames...);
	}
	/** Declares an attribute name as valid.
	 * @param p0Element The element. Cannot be null.
	 * @param sAttrName The attribute name. Cannot be empty.
	 */
	void addValidAttrName(const xmlpp::Element* p0Element, const std::string& sAttrName);
	/** Declares an attribute name as valid.
	 * @param p0Element The element. Cannot be null.
	 * @param sAttrName The attribute name. Cannot be empty.
	 */
	void addValidAttrNames(const xmlpp::Element* p0Element, const std::string& sAttrName)
	{
		addValidAttrName(p0Element, sAttrName);
	}
	/** Declares attribute names as valid.
	 * @param p0Element The element. Cannot be null.
	 * @param sAttrName The attribute name. Cannot be empty.
	 * @param oNames Further attribute names.
	 */
	template<typename ...Names>
	void addValidAttrNames(const xmlpp::Element* p0Element, const std::string& sAttrName, const Names&... oNames)
	{
		addValidAttrName(p0Element, sAttrName);
		addValidAttrNames(p0Element, oNames...);
	}
	/** Removes a checker for the element.
	 *
	 * @param p0Element The element. Cannot be null.
	 * @param bCheck Whether to check both attributes child element names.
	 */
	void removeChecker(const xmlpp::Element* p0Element, bool bCheck)
	{
		removeChecker(p0Element, bCheck, bCheck);
	}
	/** Removes a checker for the element.
	 * This should always be paired with a addChecker() call.
	 *
	 * If an element has multiple checkers, the check is only performed when the
	 * last checker is removed.
	 *
	 * The bCheckChildElements parameter should be set to false if you are iterating
	 * over all child elements and checking their validity explicitely. Same for
	 * bCheckAttrs.
	 * @param p0Element The element. Cannot be null.
	 * @param bCheckChildElements Whether to check child element names.
	 * @param bCheckAttrs Whether to check attributes.
	 */
	virtual void removeChecker(const xmlpp::Element* p0Element, bool bCheckChildElements, bool bCheckAttrs);
public:
	/** Creates an exception for the current context.
	 * @param sErr The error string. Can be empty.
	 * @return The exception.
	 */
	std::runtime_error error(const std::string& sErr);
	/** The line number within a document of an element or attribute.
	 * @param p0Element The element. Cannot be null.
	 * @param sAttr The attribute name. If empty or not found the element's line number is returned.
	 * @return The line number or -1 if unknown.
	 */
	static int32_t getLine(const xmlpp::Element* p0Element, const std::string& sAttr);

	#ifndef NDEBUG
	void dump() const;
	#endif //NDEBUG
protected:
	/** Builds complete error string from context.
	 * The base implementation concatenates strings passed to pushCtx().
	 * @param s The error without context.
	 * @return The error string with context.
	 */
	virtual std::string err(const std::string& s);

protected:
	struct Checker
	{
		const xmlpp::Element* m_p0Element = nullptr;
		int32_t m_nCounter = 0;
		bool m_bCheckChildrenElements = false;
		bool m_bCheckAttrs = false;
		std::vector<std::string> m_aValidChildrenNames;
		std::vector<std::string> m_aValidAttrNames;
	};
protected:
	std::vector<Checker>::iterator getChecker(const xmlpp::Element* p0Element);
protected:
	const shared_ptr<AppConfig> m_refAppConfig;
	shared_ptr<AppPreferences> m_refAppPreferences;
	Named& m_oNamed;
	std::vector<std::string> m_oStack;
	std::vector<Checker> m_aCheckers;
private:
	ParserCtx() = delete;
	ParserCtx(const ParserCtx& oSource) = delete;
	ParserCtx& operator=(const ParserCtx& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_PARSER_CTX_H */

