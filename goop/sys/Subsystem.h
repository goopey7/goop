// Sam Collier 2023
#pragma once

namespace goop::sys
{

class Subsystem
{
  public:
	virtual int initialize() = 0;
	virtual int destroy() = 0;
};

} // namespace goop::sys
