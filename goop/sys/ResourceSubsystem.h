// Sam Collier 2023
#pragma once

#include "goop/sys/Subsystem.h"
#include <string>
#include <cstdint>

namespace goop::sys
{

class ResourceSubsystem : public Subsystem
{
  public:
	virtual uint32_t load(const std::string& path) = 0;
  protected:
	bool bIsInitialized = false;
};

} // namespace goop::sys
