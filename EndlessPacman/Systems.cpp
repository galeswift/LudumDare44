#include "stdafx.h"

#include "Components.h"
#include "GameConst.h"
#include "Game.h"
#include "Systems.h"

void CombatSystem::Update(float dt)
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
			std::shared_ptr<HealthComponent> healthComp = e->GetComponent<HealthComponent>();
			std::shared_ptr<GlobalInputComponent> inputCmp = g_singletonEntity->GetComponent<GlobalInputComponent>();

			if (healthComp != nullptr)
			{
				healthComp->m_health -= dt * 2;
			}
			if (positionCmp != nullptr &&
				healthComp != nullptr && 
				inputCmp->WasKeyPressed(sf::Keyboard::Key(positionCmp->index + sf::Keyboard::Key::Num1)))
			{
				float baseHealAmount = 5.0f * (inputCmp->IsKeyHeld(sf::Keyboard::LShift) ? 3 : 1);
				float healAmount = std::min(baseHealAmount, healthComp->m_maxHealth - healthComp->m_health);
				if (localPlayer->GetComponent<HealthComponent>()->m_health >= healAmount)
				{
					healthComp->m_health += healAmount;
					localPlayer->GetComponent<HealthComponent>()->m_health -= healAmount;
				}
			}

			if (healthComp && 
				(healthComp->m_health >= healthComp->m_maxHealth ||
				healthComp->m_health <= 0))
			{
				e->Destroy();
			}
		}
	}
}

void EntityCleanupSystem::Update(float dt)
{
	for (int i = (int)g_game.m_entities.size() - 1; i >= 0; i--)
	{
		if (g_game.m_entities[i]->m_pendingDestroy)
		{
			g_game.m_entities.erase(g_game.m_entities.begin() + i);
		}
	}
}

void InputSystem::Update(float dt)
{
	g_singletonEntity->GetComponent<GlobalInputComponent>()->m_pressedKeys.clear();
	g_singletonEntity->GetComponent<GlobalInputComponent>()->m_releasedKeys.clear();
}

void System::Draw(float dt, sf::RenderWindow & window) { teREF(dt); teREF(window); }

void EntityRenderSystem::Draw(float dt, sf::RenderWindow & window)
{
	for (auto e : g_game.m_entities)
	{
		std::shared_ptr<PositionComponent> positionCmp = e->GetComponent<PositionComponent>();
		std::shared_ptr<HealthComponent> healthCmp = e->GetComponent<HealthComponent>();
		if (healthCmp != nullptr)
		{
			char healthText[25];
			sprintf_s(healthText, 25, "%.0f/%.0f", healthCmp->m_health, healthCmp->m_maxHealth);
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
				text.setFillColor(sf::Color(200, 200, 0, 255));
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
