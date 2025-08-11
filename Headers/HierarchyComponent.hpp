#pragma once
#include "precomp.h"

struct HierarchyComponent
{
	entt::entity parentID = entt::null;
	std::vector<entt::entity> childrenID;
};