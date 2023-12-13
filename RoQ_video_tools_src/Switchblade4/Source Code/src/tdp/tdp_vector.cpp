#include "tdp_vector.hpp"

using namespace TDP;

const char *terVectorOversizeException::ErrorMsg() const
{
	return "Internal memory buffer size limit exceeded";
}


const char *terVectorAllocationException::ErrorMsg() const
{
	return "Internal memory buffer allocation failure";
}
