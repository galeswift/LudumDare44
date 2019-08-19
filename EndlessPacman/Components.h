#pragma once

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
	float m_health = 0;
	float m_maxHealth = 0;
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
		m_pendingDestroy = true;
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

	bool m_pendingDestroy = false;
	std::map<int, std::shared_ptr<Component>> m_componentMap;
};

extern std::shared_ptr<Entity> g_singletonEntity;