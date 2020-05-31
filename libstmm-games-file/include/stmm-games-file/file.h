/*
 * File:   file.h
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

#ifndef STMG_FILE_H
#define STMG_FILE_H

#include <string>

#include <stdint.h>

namespace stmg
{

class File
{
public:
	/** Undefined File constructor.
	 */
	File() noexcept;
	/** Filesystem path File constructor.
	 * @param sFullPath The full path to a file. Cannot be empty.
	 */
	explicit File(const std::string& sFullPath) noexcept;
	/** Memory buffer File constructor.
	 * The file doesn't manage the pointed to memory.
	 * @param p0Buffer The buffer start. Cannot be null.
	 * @param nBufferSize The buffer size. Cannot be negative.
	 */
	File(uint8_t* p0Buffer, int32_t nBufferSize) noexcept;
	/** Copy constructor.
	 * @param oSource The source.
	 */
	File(const File& oSource) noexcept;
	/** Assignment operator.
	 * @param oSource The source.
	 * @return This instance.
	 */
	File& operator=(const File& oSource) noexcept;
	/** Whether the file is defined.
	 * @return Whether path or buffer.
	 */
	bool isDefined() const noexcept;
	/** Whether the file is a memory buffer.
	 * @return Whether a buffer.
	 */
	bool isBuffered() const noexcept;
	/** The buffer.
	 * @return The buffer start or null if undefined or file path.
	 */
	uint8_t const* getBuffer() const noexcept;
	/** The size of the buffer.
	 * @return  The buffer size or 0 if undefined or file path.
	 */
	uint32_t getBufferSize() const noexcept;
	/** The file path.
	 * @return The path or empty string if undefined or buffer.
	 */
	const std::string& getFullPath() const noexcept;
private:
	std::string m_sFullPath;
	uint8_t* m_p0Buffer;
	int32_t m_nBufferSize;
};

} //namespace stmg

#endif	/* STMG_FILE_H */

