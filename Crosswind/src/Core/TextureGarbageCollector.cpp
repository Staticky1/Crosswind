#include "TextureGarbageCollector.h"

void TextureGarbageCollector::Submit(std::shared_ptr<Walnut::Image> image)
{
	s_PendingDeletes.push_back(std::move(image));
}

void TextureGarbageCollector::Collect()
{
	s_PendingDeletes.clear();
}
