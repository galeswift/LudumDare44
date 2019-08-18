// EndlessPacman.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_set>

#define teREF(x) (void)(x)

#define IMPLEMENT_COMPONENT(x)	\
	static int GetComponentType() { return x; } 

enum ComponentTypes
{
	NONE,
	LOCAL_PLAYER,
	HEALTH,
	POSITION,
	GLOBAL_INPUT,
	GLOBAL_FONT,
};

class Component
{
public:

};

class LocalPlayerComponent : public Component
{
public:
	IMPLEMENT_COMPONENT(LOCAL_PLAYER)	
};

class HealthComponent : public Component
{
public:
	IMPLEMENT_COMPONENT(HEALTH);
	int m_health = 0;
	int m_maxHealth = 0;
};

class PositionComponent : public Component
{
public:
	IMPLEMENT_COMPONENT(POSITION);
	float x = 0, y = 0;
	int index = 0;
};

class GlobalInputComponent : public Component
{
public:
	IMPLEMENT_COMPONENT(GLOBAL_INPUT);

	bool IsKeyHeld(sf::Keyboard::Key key)
	{
		return m_heldKeys.find(key) != m_heldKeys.end();
	}

	bool WasKeyPressed(sf::Keyboard::Key key)
	{
		return m_pressedKeys.find(key) != m_pressedKeys.end();
	}

	bool WasKeyReleased(sf::Keyboard::Key key)
	{
		return m_releasedKeys.find(key) != m_releasedKeys.end();		
	}

	std::unordered_set<sf::Keyboard::Key> m_heldKeys;
	std::unordered_set<sf::Keyboard::Key> m_pressedKeys;
	std::unordered_set<sf::Keyboard::Key> m_releasedKeys;
};

class GlobalFontComponent : public Component
{
public:
	IMPLEMENT_COMPONENT(GLOBAL_FONT);
	GlobalFontComponent()
	{
		m_font.loadFromFile("consola.ttf");
	}

	sf::Font m_font;
};
class Entity
{
public:
	
	void Destroy()
	{
		m_componentMap.clear();
	}

	template <typename T>
	std::shared_ptr<T> AddComponent()
	{
		std::shared_ptr<Component> result = std::make_shared<T>();
		m_componentMap.insert(std::make_pair(T::GetComponentType(), result));
		return std::static_pointer_cast<T>(result);
	}

	template <typename T>
	bool HasComponent()
	{
		return m_componentMap.find(T::GetComponentType()) != m_componentMap.end();
	}

	template <typename T>
	std::shared_ptr<T> GetComponent()
	{
		if (m_componentMap.find(T::GetComponentType()) != m_componentMap.end())
		{
			return std::static_pointer_cast<T>(m_componentMap.at(T::GetComponentType()));
		}
		else
		{
			return nullptr;
		}
	}

	std::map<int, std::shared_ptr<Component>> m_componentMap;
};

std::shared_ptr<Entity> g_singletonEntity;

class System
{
public:
	virtual void Update(float dt) = 0;
	virtual void Draw(float dt, sf::RenderWindow& window) { teREF(dt); teREF(window); }
};


class Game
{
public:
	std::shared_ptr<Entity> CreateEntity()
	{
		std::shared_ptr<Entity> result = std::make_shared<Entity>();
		m_entities.push_back(result);
		return result;

	}
	void RegisterSystem(std::shared_ptr<System> system)
	{
		m_systems.push_back(system);
	}

	std::vector<std::shared_ptr<Entity>> m_entities;
	std::vector<std::shared_ptr<System>> m_systems;
};

Game g_game;

class InputSystem : public System
{
public:
	virtual void Update(float dt) override
	{				
		g_singletonEntity->GetComponent<GlobalInputComponent>()->m_pressedKeys.clear();
		g_singletonEntity->GetComponent<GlobalInputComponent>()->m_releasedKeys.clear();
	}
};

class EntityRenderSystem : public System
{
	virtual void Update(float dt) override
	{

	}
	virtual void Draw(float dt, sf::RenderWindow& window) override
	{		
		for (auto e : g_game.m_entities)
		{
			std::shared_ptr<PositionComponent> positionCmp = e->GetComponent<PositionComponent>();
			std::shared_ptr<HealthComponent> healthCmp = e->GetComponent<HealthComponent>();
			if (healthCmp != nullptr)
			{				
				char healthText[25];
				sprintf_s(healthText, 25, "%d/%d", healthCmp->m_health, healthCmp->m_maxHealth);
				sf::Text text;
				text.setString(healthText);				
				text.setFont(g_singletonEntity->GetComponent<GlobalFontComponent>()->m_font);
				text.setOutlineColor(sf::Color::White);
				text.setOutlineThickness(.1f);
				float healthRatio = (float)healthCmp->m_health / healthCmp->m_maxHealth;
				if (healthRatio < .33)
				{
					text.setFillColor(sf::Color::Red);
				}
				else if (healthRatio < .66)
				{
					text.setFillColor(sf::Color::Yellow);
				}
				else if (healthRatio < .9)
				{
					text.setFillColor(sf::Color(200,200,0,255));
				}
				else
				{
					text.setFillColor(sf::Color(0, 255, 0, 255));
				}

				if (positionCmp != nullptr) 
				{
					text.setPosition(positionCmp->x, positionCmp->y);
				}
				window.draw(text);
			}
		}
	}
};

class CombatSystem : public System
{
public:
	void Update(float dt) override
	{
		std::shared_ptr<Entity> localPlayer;
		for (auto e : g_game.m_entities)
		{
			if (e->HasComponent<LocalPlayerComponent>())
			{
				localPlayer = e;
				break;
			}
		}

		for (auto e : g_game.m_entities)
		{			
			if (!e->HasComponent<LocalPlayerComponent>())
			{
				std::shared_ptr<PositionComponent> positionCmp = e->GetComponent<PositionComponent>();
				std::shared_ptr<GlobalInputComponent> inputCmp = g_singletonEntity->GetComponent<GlobalInputComponent>();
				if (positionCmp != nullptr && 
					inputCmp->WasKeyPressed(sf::Keyboard::Key(positionCmp->index + sf::Keyboard::Key::Num1)))
				{
					std::shared_ptr<HealthComponent> healthComp = e->GetComponent<HealthComponent>();
					int baseHealAmount = 5 * (inputCmp->IsKeyHeld(sf::Keyboard::LShift) ? 3 : 1);
					int healAmount = std::min(baseHealAmount, healthComp->m_maxHealth - healthComp->m_health);
					if (localPlayer->GetComponent<HealthComponent>()->m_health >= healAmount)
					{
						healthComp->m_health += healAmount;
						localPlayer->GetComponent<HealthComponent>()->m_health -= healAmount;
					}
				}				
			}
		}		
	}
};

void RegisterSystems() 
{	
	g_game.RegisterSystem(std::make_shared<EntityRenderSystem>());
	g_game.RegisterSystem(std::make_shared<CombatSystem>());

	// At end to clear inputs
	g_game.RegisterSystem(std::make_shared<InputSystem>());
}

void CreatePlayer()
{
	std::shared_ptr<Entity> player = g_game.CreateEntity();
	std::shared_ptr<HealthComponent> healthCmp = player->AddComponent<HealthComponent>();
	std::shared_ptr<PositionComponent> positionCmp = player->AddComponent<PositionComponent>();
	std::shared_ptr<LocalPlayerComponent> localPlayerCmp = player->AddComponent<LocalPlayerComponent>();
	teREF(localPlayerCmp);

	healthCmp->m_health = 600;
	healthCmp->m_maxHealth = 600;
	positionCmp->x = 640;
	positionCmp->y = 360;
}

void CreateInjuredPlayers(std::vector<std::shared_ptr<Entity>>& injuredList)
{
	static const int numInjured = 8;
	for (int i = 0; i < numInjured; i++)
	{
		std::shared_ptr<Entity> injuredPlayer = g_game.CreateEntity();
		std::shared_ptr<HealthComponent> healthCmp = injuredPlayer->AddComponent<HealthComponent>();
		std::shared_ptr<PositionComponent> positionCmp = injuredPlayer->AddComponent<PositionComponent>();
		
		healthCmp->m_health = rand() % 40 + 10;
		healthCmp->m_maxHealth = 100;
		positionCmp->x = (float)(1280 / numInjured * i + (640 / numInjured) - 60);
		positionCmp->y = 160;
		positionCmp->index = i;

		injuredList.push_back(injuredPlayer);
	}

}
int main()
{
	RegisterSystems();

	g_singletonEntity = g_game.CreateEntity();
	g_singletonEntity->AddComponent<GlobalFontComponent>();
	g_singletonEntity->AddComponent<GlobalInputComponent>();

	std::vector<std::shared_ptr<Entity>> enemies;

	CreatePlayer();
	CreateInjuredPlayers(enemies);
	

	sf::RenderWindow window(sf::VideoMode(1280, 720), "Ludum Dare - 44");	
	sf::Text healthText;
	float dt = 0.0f;

	sf::Clock clock;
	while (window.isOpen())
	{
		dt = clock.restart().asSeconds();
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyReleased)
			{
				g_singletonEntity->GetComponent<GlobalInputComponent>()->m_releasedKeys.insert(event.key.code);
				g_singletonEntity->GetComponent<GlobalInputComponent>()->m_heldKeys.erase(event.key.code);
			}
			if (event.type == sf::Event::KeyPressed)
			{
				g_singletonEntity->GetComponent<GlobalInputComponent>()->m_heldKeys.insert(event.key.code);
				g_singletonEntity->GetComponent<GlobalInputComponent>()->m_pressedKeys.insert(event.key.code);
			}
		}
				

		for (std::vector<std::shared_ptr<System>>::iterator it = g_game.m_systems.begin(); it != g_game.m_systems.end(); it++)
		{
			(*it)->Update(dt);
		}		
		

		// Do Render
		window.clear();
		for (std::vector<std::shared_ptr<System>>::iterator it = g_game.m_systems.begin(); it != g_game.m_systems.end(); it++)
		{
			(*it)->Draw(dt, window);
		}

		window.display();		
	}

	g_singletonEntity->Destroy();

	return 0;
}