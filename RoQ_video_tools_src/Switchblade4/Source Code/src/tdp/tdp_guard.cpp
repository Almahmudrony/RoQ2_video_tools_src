#include "tdp_guard.hpp"

using namespace TDP;

const char *terResourceAllocationException::ErrorMsg() const
{
	return "Resource allocation failed";
}
