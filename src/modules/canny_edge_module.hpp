#pragma once
#include "module.hpp"

class CannnyEdgeModule : public Module
{

	// Inherit the constructors of Module (they are not inherited automatically)
	using Module::Module;

public:
	void update() override;
};