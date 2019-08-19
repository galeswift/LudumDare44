#pragma once

class Entity;

class System
{
public:
	virtual void Update(float dt) = 0;
	virtual void Draw(float dt, sf::RenderWindow& window);
};


class InputSystem : public System
{
public:
	virtual void Update(float dt) override;
};

class EntityCleanupSystem : public System
{

public:
	virtual void Update(float dt) override;
};

class EntityRenderSystem : public System
{
	virtual void Update(float dt) override
	{

	}
	virtual void Draw(float dt, sf::RenderWindow& window) override;
};

class CombatSystem : public System
{
public:
	void Update(float dt) override;
};
