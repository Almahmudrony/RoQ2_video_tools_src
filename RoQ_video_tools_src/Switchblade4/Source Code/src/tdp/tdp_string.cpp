#include <string.h>
#include <ctype.h>
#include <new>
#include "tdp_string.hpp"

using namespace TDP;

const char *terStringOverflowException::ErrorMsg() const
{
	return "Oversized string";
}

const char *terStringAllocationException::ErrorMsg() const
{
	return "String memory allocation failed";
}


terString::terString()
{
	_isDynAlloc = false;
	_length = 0;
	_capacity = terStringStaticLength;
	_staticBuf[0] = '\0';
}

terString::terString(const terString &str)
{
	SetToString(str.CStr(), str.Length());
}

terString::terString(const terBufferString &bstr)
{
	SetToString(bstr.Str(), bstr.Length());
}

terString::terString(const char *cstr)
{
	size_t len;
	len = strlen(cstr);
	SetToString(cstr, len);
}

terString::terString(const char *cstr, size_t len)
{
	SetToString(cstr, len);
}

terString::~terString()
{
	if(_isDynAlloc)
		delete[] _data;
}

void terString::Reallocate(size_t len)
{
	if(len < terStringStaticLength)
	{
		// Use static buffer
		_isDynAlloc = false;
	}
	else
	{
		_isDynAlloc = false;	// Mark as false in case allocation fails

		// Allocate dynamic space
		if(len > terStringMaxLength)
			throw terStringOverflowException();

		_capacity = len + terStringGranularity;
		_capacity -= _capacity % terStringGranularity;

		try
		{
			_data = new char[_capacity];
		}
		catch(std::bad_alloc &)
		{
			throw terStringAllocationException();
		}
		_isDynAlloc = true;
	}
}

void terString::SetToString(const char *str, size_t len)
{
	char *ptr;

	Reallocate(len);

	ptr = Ptr();

	_length = len;
	memcpy(ptr, str, len);
	ptr[len] = '\0';
}

void terString::Append(const char *str, size_t len)
{
	size_t capacity;
	char *ptr;

	if(_capacity > len && _capacity - len > _length)
	{
		memcpy(Ptr() + _length, str, len);
		_length += len;
		Ptr()[_length] = '\0';
	}
	else
	{
		*this = *this + str;
	}
}

terString &terString::operator = (const terString &str)
{
	SetToString(str.CStr(), str.Length());
	return *this;
}

terString &terString::operator = (const terBufferString &bstr)
{
	SetToString(bstr.Str(), bstr.Length());
	return *this;
}

terString &terString::operator = (const char *cstr)
{
	size_t len;
	len = strlen(cstr);
	SetToString(cstr, len);

	return *this;
}

// Equality tests
bool terString::operator == (const terString &rs) const
{
	if(this == &rs) return true;
	if(_length != rs._length) return false;
	if(_isDynAlloc)
		return memcmp(_data, rs._data, _length) == 0;
	else
		return memcmp(_staticBuf, rs._staticBuf, _length) == 0;
}

bool terString::operator == (const terBufferString &rs) const
{
	if(_length != rs.Length()) return false;
	return memcmp(CStr(), rs.Str(), _length) == 0;
}

bool terString::operator == (const char *rs) const
{
	size_t len;

	if(!rs)
		return _length == 0;

	len = strlen(rs);
	if(_length != len) return false;
	return memcmp(CStr(), rs, _length) == 0;
}

bool terString::NCSEquals(const terString &rs) const
{
	const char *ptr1, *ptr2;
	size_t len;

	if(this == &rs) return true;
	if(_length != rs._length) return false;

	len = _length;
	ptr1 = this->CStr();
	ptr2 = rs.CStr();

	while(len--)
	{
		if(toupper(*ptr1++) != toupper(*ptr2++))
			return false;
	}
	return true;
}


terString terString::operator + (const terString &rs) const
{
	terString str;
	char *ptr;

	str.Reallocate(_length + rs._length);
	ptr = str.Ptr();
	memcpy(ptr, this->CStr(), _length);
	memcpy(ptr + _length, rs.CStr(), rs._length);
	ptr[_length + rs._length] = '\0';

	return str;
}

terString terString::operator + (const char *rs) const
{
	terString str;
	size_t len;
	char *ptr;

	len = strlen(rs);

	str.Reallocate(_length + len);
	ptr = str.Ptr();
	memcpy(ptr, this->CStr(), _length);
	memcpy(ptr + _length, rs, len);
	ptr[_length + len] = '\0';
	str._length = _length + len;

	return str;
}

terString terString::SubString(size_t start, size_t len) const
{
	if(start >= _length)
		return terString();

	if(_length - start < len)
		len = _length - start;

	return terString(this->CStr() + start, len);
}

