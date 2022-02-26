#pragma once
#include <iostream>
#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"

// Forward decleration
namespace qtree {
	struct Circle;
	struct Rect;
}

class Utils
{
public:
	static void drawCircle(sf::RenderTarget& window, const qtree::Circle& data, const sf::Color& outlineColor);
	static void drawRectangle(sf::RenderTarget& window, const qtree::Rect& rect, const sf::Color& outlineColor);
	static int getRandom(int max);
};

