/*
 * Copyright (c) 2005, The Black Sheep, Department of Computer Science, The University of Auckland
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 * Neither the name of The Black Sheep, The Department of Computer Science or The University of Auckland nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 Contributors and list of changes:

 Cameron Skinner
   Converted original Robocup Rescue software into librescue
 */

#include "output.h"
#include <cstring>

namespace Librescue
{

	OutputBuffer::OutputBuffer()
	{
		m_bytes.clear();
		m_index = 0;
	}

	OutputBuffer::~OutputBuffer()
	{
	}

	void OutputBuffer::clear()
	{
		m_bytes.clear();
		m_index = 0;
	}

	Cursor OutputBuffer::cursor() const
	{
		return m_index;
	}

	void OutputBuffer::setCursor(Cursor cursor)
	{
		m_index = cursor;
	}

	Cursor OutputBuffer::writeInt32(INT_32 value)
	{
		m_bytes.push_back((value >> 24)&0xFF);
		m_bytes.push_back((value >> 16)&0xFF);
		m_bytes.push_back((value >> 8)&0xFF);
		m_bytes.push_back(value & 0xFF);
		m_index += 4;
		return m_index;
	}

	Cursor OutputBuffer::writeInt32(INT_32 value, Cursor position)
	{
		// First make sure that m_bytes is big enough
		m_bytes.reserve(position + 4);
		m_bytes[position] = ((value >> 24)&0xFF);
		m_bytes[position + 1] = ((value >> 16)&0xFF);
		m_bytes[position + 2] = ((value >> 8)&0xFF);
		m_bytes[position + 3] = (value & 0xFF);
		return position + 4;
	}

	Cursor OutputBuffer::writeString(const char* string)
	{
		int length = strlen(string);
		return writeString(string, length + 1); // Use length+1 to make sure that the null byte at the end gets written
	}

	Cursor OutputBuffer::writeString(std::string string)
	{
		return writeString(string.c_str(), string.length() + 1); // Use length+1 to make sure that the null byte at the end gets written
	}

	Cursor OutputBuffer::writeString(const char* string, INT_32 size)
	{
		writeInt32(size);
		for(int i = 0; i < size; ++i)
		{
			m_bytes.push_back(string[i]);
			++m_index;
		}
		return m_index;
	}

	Cursor OutputBuffer::write(int size, const Bytes& buffer)
	{
		for(int i = 0; i < size; ++i)
		{
			m_bytes.push_back(buffer[i]);
		}
		m_index += size;
		return m_index;
	}

	Cursor OutputBuffer::write(int size, Byte* buffer)
	{
		for(int i = 0; i < size; ++i)
		{
			m_bytes.push_back(buffer[i]);
		}
		m_index += size;
		return m_index;
	}

	Cursor OutputBuffer::write(const Bytes& buffer)
	{
		m_bytes.insert(m_bytes.end(), buffer.begin(), buffer.end());
		m_index += buffer.size();
		return m_index;
	}

	Cursor OutputBuffer::writeSize(Cursor base)
	{
		Cursor current = cursor();
		writeInt32(current - base, base - INT_32_SIZE);
		setCursor(current);
		return current;
	}

	const Bytes& OutputBuffer::buffer() const
	{
		return m_bytes;
	}

	int OutputBuffer::size() const
	{
		return m_index;
	}

	void OutputBuffer::log(FILE* file) const
	{
		long s = m_bytes.size();
		//	LOG_DEBUG("Writing output to file. Size=%d",(int)s);
		Byte b[4];
		b[0] = (Byte)((s >> 24) & 0xFF);
		b[1] = (Byte)((s >> 16) & 0xFF);
		b[2] = (Byte)((s >> 8) & 0xFF);
		b[3] = (Byte)(s & 0xFF);
		fwrite(b, sizeof (Byte), 4, file);
		long written = 0;
		while(written < s)
		{
			written += fwrite(&m_bytes[written], sizeof (Byte), s - written, file);
		}
		fflush(file);
	}
}
