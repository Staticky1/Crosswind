#pragma once

#include <memory>
#include "Walnut/Image.h"
#include <vector>

class TextureGarbageCollector {

public:
	static void Submit(std::shared_ptr<Walnut::Image> image);

	static void Collect();

private:
	static inline std::vector<std::shared_ptr<Walnut::Image>> s_PendingDeletes;
};