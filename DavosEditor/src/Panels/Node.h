#pragma once
#include <Davos.h>

namespace Davos {

	struct Node
	{
	public:
		Node(Entity id, const std::string& name, Node* parent) : id(id), name(name), parent(parent) {}

	public:
		Entity id = MAX_ENTITIES;
		std::string name = "Node";

		Node* parent = nullptr;
		std::vector<Node*> childs;
		uint32_t numChilds = 0;
		uint32_t totalChilds = 0;

		bool isRename = false;
	};

}
