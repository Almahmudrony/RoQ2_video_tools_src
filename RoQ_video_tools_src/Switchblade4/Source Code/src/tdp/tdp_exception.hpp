#ifndef __TDP_EXCEPTION_HPP__
#define __TDP_EXCEPTION_HPP__

namespace TDP
{

	class terException
	{
	public:
		virtual const char *ErrorMsg() const;
	};

}

#endif
