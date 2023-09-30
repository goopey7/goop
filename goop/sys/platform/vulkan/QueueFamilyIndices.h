#include <cstdint>
#include <optional>

struct QueueFamilyIndices
{
	// it's possible that the device uses a separate queue for drawing and presentation
	// but this approach will at least handle both cases
	// TODO prefer that graphics and presentation are in same queue for improved performance
	// TODO investigate separating transfers to a separate queue
	std::optional<uint32_t> graphics; // graphics queues support transfer operations
	std::optional<uint32_t> present;
	// std::optional<uint32_t> transfer;
	bool isComplete() { return graphics.has_value() && present.has_value(); }
};
