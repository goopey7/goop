// Sam Collier 2023
#pragma once

#include "goop/sys/Subsystem.h"
#include <string>

namespace goop::sys
{

class ResourceSubsystem : public Subsystem
{
  public:
	virtual bool load(const std::string& path) = 0;
  protected:
	bool bIsInitialized = false;
};

} // namespace goop::sys
