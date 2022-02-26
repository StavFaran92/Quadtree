#include "Utils.h"

#include "Quadtree.h"

void Utils::drawCircle(sf::RenderTarget& window, const qtree::Circle& data, const sf::Color& outlineColor)
{
    sf::CircleShape circle;
    circle.setPosition(data.x - data.radius, data.y - data.radius);
    circle.setRadius(data.radius);
    circle.setFillColor(sf::Color::Transparent);
    circle.setOutlineColor(outlineColor);
    circle.setOutlineThickness(1);
    window.draw(circle);
}

void Utils::drawRectangle(sf::RenderTarget& window, const qtree::Rect& rect, const sf::Color& outlineColor)
{
    sf::RectangleShape rectangle;
    rectangle.setPosition(rect.x, rect.y);
    rectangle.setSize({ (float)rect.width, (float)rect.height });
    rectangle.setFillColor(sf::Color::Transparent);
    rectangle.setOutlineThickness(1);
    rectangle.setOutlineColor(outlineColor);
    window.draw(rectangle);
}

int Utils::getRandom(int max)
{
    return rand() % max + 1;
}