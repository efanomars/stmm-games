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
 * File:   allpreferencesloader.h
 */

#ifndef STMG_ALL_PREFERENCES_LOADER_H
#define STMG_ALL_PREFERENCES_LOADER_H

#include <memory>

namespace stmg
{

using std::shared_ptr;

class AllPreferences;

class AllPreferencesLoader
{
public:
	virtual ~AllPreferencesLoader() noexcept = default;

	/** Load or create a new (or recycled) AllPreferences instance.
	 * If there is a persisted instance it is loaded, otherwise it is created
	 * with default values.
	 * @return The instance. Is not null.
	 */
	virtual shared_ptr<AllPreferences> getPreferences() const noexcept = 0;
	/** Create a copy of an AllPreferences instance.
	 * This function should be preferred over just using the copy constructor of the
	 * instance because it allows the loader to possibly recycle another instance.
	 * @param refAllPreferences The instance to copy from. Cannot be null.
	 * @return The copy. Is not null.
	 */
	virtual shared_ptr<AllPreferences> getPreferencesCopy(const shared_ptr<AllPreferences>& refAllPreferences) const noexcept = 0;
	/** Persist an AllPreferences instance.
	 * @param refAllPreferences The instance to persist. Cannot be null.
	 * @return Whether the instance could be persisted.
	 */
	virtual bool updatePreferences(const shared_ptr<AllPreferences>& refAllPreferences) noexcept = 0;
};

} // namespace stmg

#endif	/* STMG_ALL_PREFERENCES_LOADER_H */

