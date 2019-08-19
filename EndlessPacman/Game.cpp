#include "stdafx.h"
#include "Components.h"
#include "Game.h"

std::shared_ptr<Entity> Game::CreateEntity()
{
	std::shared_ptr<Entity> result = std::make_shared<Entity>();
	m_entities.push_back(result);
	return result;
}

void Game::RegisterSystem(std::shared_ptr<System> system)
{
	m_systems.push_back(system);
}
