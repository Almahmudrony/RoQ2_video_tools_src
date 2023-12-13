#ifndef __TDP_VECTOR_HPP__
#define __TDP_VECTOR_HPP__

#include <stddef.h>
#include <new>
#include <stdio.h>
#include "tdp_exception.hpp"

namespace TDP
{
	enum
	{
		terVectorMaxBytes_k		= 0x3ffffff,	// 64MB
	};

	class terVectorOversizeException : public terException
	{
	public:
		virtual const char *ErrorMsg() const;
	};
	class terVectorAllocationException : public terException
	{
	public:
		virtual const char *ErrorMsg() const;
	};

	// Simple vector with automatic deallocation
	template<class _T> class terVector
	{
	protected:
		_T *_data;
		unsigned int _count;

	public:
		inline terVector()
		{
			_data = NULL;
			_count = 0;
		}

		inline ~terVector()
		{
			if(_data)
			{
				delete[] _data;
			}
		}

		/*
		inline const _T &operator [] (int idx) const
		{
			return _data[idx];
		}

		inline _T &operator [] (int idx)
		{
			return _data[idx];
		}
		*/

		void Alloc(unsigned int count)
		{
			if(_data)
			{
				delete[] _data;
				_data = NULL;
			}

			if(!count)
				return;
			if(terVectorMaxBytes_k / sizeof(_T) < count)
				throw terVectorOversizeException();
			try
			{
				_data = new _T[count];
			}
			catch(std::bad_alloc &)
			{
				throw terVectorAllocationException();
			}
			_count = count;
		}

		inline void Alloc(unsigned int count, const _T &baseValue)
		{
			unsigned int i;
			Alloc(count);

			for(i=0;i<count;i++)
				_data[i] = baseValue;
		}

		inline void Free()
		{
			if(_data)
			{
				delete[] _data;
				_data = NULL;
			}
		}

		inline unsigned int Count() const
		{
			return _count;
		}

		inline bool Valid() const
		{
			return _data != NULL;
		}

		inline operator const _T *() const
		{
			return _data;
		}

		inline operator _T *()
		{
			return _data;
		}

		inline void Fill(const _T &val)
		{
			_T *ptr = _data;
			unsigned int c = _count;

			while(c--)
				*ptr++ = val;
		}
	};
}

#endif
