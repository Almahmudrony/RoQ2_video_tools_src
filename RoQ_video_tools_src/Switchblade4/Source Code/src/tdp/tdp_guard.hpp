#ifndef __TDP_GUARD_HPP__
#define __TDP_GUARD_HPP__

#include <stddef.h>
#include <new>

#include "tdp_exception.hpp"

namespace TDP
{
	class terResourceAllocationException : public terException
	{
	public:
		virtual const char *ErrorMsg() const;
	};

	// Scope guard, automatically frees a resource if the scope is exited before the
	// resource is released.
	template<class _T> class terSafePointer
	{
	protected:
		_T *_ptr;
		bool _hot;

	public:
		inline terSafePointer()
		{
			_hot = false;
		}

		inline ~terSafePointer()
		{
			if(_hot)
				delete _ptr;
		}

		inline _T *Unguard()
		{
			_hot = false;
			return _ptr;
		}

		inline void operator = (_T *rs)
		{
			_ptr = rs;
			if(_ptr)
				_hot = true;
		}

		inline operator _T * ()
		{
			return _ptr;
		}

		inline _T * operator -> () const
		{
			return _ptr;
		}

		inline _T * operator -> ()
		{
			return _ptr;
		}
	};
}

#endif
