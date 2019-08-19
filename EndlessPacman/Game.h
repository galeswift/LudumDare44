#pragma once

class Entity;
class System;

class Game
{
public:
	std::shared_ptr<Entity> CreateEntity();
	void RegisterSystem(std::shared_ptr<System> system);

	std::vector<std::shared_ptr<Entity>> m_entities;
	std::vector<std::shared_ptr<System>> m_systems;
};

extern Game g_game;

