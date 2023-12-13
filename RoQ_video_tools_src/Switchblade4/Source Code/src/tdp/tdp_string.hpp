#ifndef __TDP_STRING_HPP__
#define __TDP_STRING_HPP__

#include <stddef.h>
#include <string.h>
#include "tdp_exception.hpp"

namespace TDP
{
	enum
	{
		terStringStaticLength = 20,
		terStringMaxLength = 100000,
		terStringGranularity = 64,
	};

	class terStringOverflowException : public terException
	{
	public:
		virtual const char *ErrorMsg() const;
	};

	class terStringAllocationException : public terException
	{
	public:
		virtual const char *ErrorMsg() const;
	};

	class terBufferString
	{
	protected:
		size_t _len;
		const char *_str;
	public:
		terBufferString();
		terBufferString(const char *str, size_t len);

		size_t Length() const;
		const char *Str() const;

		bool operator == (const class terString &rs);
		bool operator != (const class terString &rs);
	};

	class terString
	{
	protected:
		char _staticBuf[terStringStaticLength];
		bool _isDynAlloc;
		char *_data;
		size_t _length;
		size_t _capacity;

		void SetToString(const char *str, size_t len);
		void Append(const char *str, size_t len);
		void Reallocate(size_t len);
		char *Ptr();

	public:
		terString();
		terString(const terBufferString &str);
		terString(const terString &str);
		terString(const char *cstr);
		terString(const char *cstr, size_t len);
		terString(bool b);
		terString(char c);
		terString(int i);
		terString(unsigned u);
		terString(float f);
		~terString();

		size_t Length() const;
		const char *CStr() const;

		char operator [](int idx) const;
		char &operator [](int idx);

		terString &operator = (const terString &str);
		terString &operator = (const terBufferString &bstr);
		terString &operator = (const char *cstr);

		terString operator + (const terString &rs) const;
		terString operator + (const char *rs) const;
		friend terString operator + (const char *cstr, const terString &rs);
		terString operator + (float rs) const;
		terString operator + (unsigned int rs) const;
		terString operator + (int rs) const;
		terString operator + (char rs) const;
		terString operator + (bool rs) const;

		terString &operator += (const terString &rs);
		terString &operator += (const char *rs);
		terString &operator += (float rs);
		terString &operator += (unsigned int rs);
		terString &operator += (int rs);
		terString &operator += (char rs);
		terString &operator += (bool rs);

		bool operator == (const terString &rs) const;
		bool operator == (const terBufferString &rs) const;
		bool operator == (const char *rs) const;
		friend bool operator == (const char *cstr, const terString &rs);

		bool operator != (const terString &rs) const;
		bool operator != (const terBufferString &rs) const;
		bool operator != (const char *rs) const;
		friend bool operator != (const char *cstr, const terString &rs);

		terString SubString(size_t start, size_t len) const;
		bool NCSEquals(const terString &rs) const;
	};

	// terBufferString method declarations
	inline terBufferString::terBufferString()
	{
		_str = NULL;
		_len = 0;
	}

	inline terBufferString::terBufferString(const char *str, size_t len)
	{
		_str = str;
		_len = len;
	}

	inline size_t terBufferString::Length() const
	{
		return _len;
	}

	inline const char *terBufferString::Str() const
	{
		return _str;
	}

	// terString method declarations
	inline const char *terString::CStr() const
	{
		if(_isDynAlloc)
			return _data;
		else
			return _staticBuf;
	}

	inline char *terString::Ptr()
	{
		if(_isDynAlloc)
			return _data;
		else
			return _staticBuf;
	}

	inline size_t terString::Length() const
	{
		return _length;
	}

	inline bool terString::operator != (const terString &rs) const
	{
		return !(*this == rs);
	}

	inline bool terString::operator != (const terBufferString &rs) const
	{
		return !(*this == rs);
	}

	inline bool terString::operator != (const char *rs) const
	{
		return !(*this == rs);
	}

	inline terString &terString::operator += (const char *rs)
	{
		Append(rs, strlen(rs));
		return *this;
	}

}

#endif
