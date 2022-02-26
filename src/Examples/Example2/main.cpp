// Includes
// STL
#include <iostream>

// SFML
#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"

#include "Utils.h"
#include "Quadtree.h"

static const int WIDTH = 800;
static const int HEIGHT = 600;

using QueryRect = qtree::Rect;

static const qtree::Rect MAP_BOUNDS = { 0, 0, WIDTH, HEIGHT };

int main()
{
    // Seed random
    std::srand(std::time(nullptr));

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Quadtree");

    auto m_quadtree = std::make_shared<qtree::QuadTree<qtree::Point>>(MAP_BOUNDS, 4);

    std::vector<qtree::Point*> points;
    std::vector<qtree::Rect> bounds;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed)
            {
                // Place an object in the scene if Left Mouse Button has been clicked
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    std::cout << "the left button was pressed" << std::endl;
                    std::cout << "mouse x: " << event.mouseButton.x << std::endl;
                    std::cout << "mouse y: " << event.mouseButton.y << std::endl;

                    int x = event.mouseButton.x;
                    int y = event.mouseButton.y;
                    auto point = new qtree::Point(x, y);
                    auto bound = qtree::Rect(point->x, point->y, 10, 10);
                    bounds.push_back(bound);
                    m_quadtree->insert(*point, bound);
                }
            }
        }

        window.clear();

        //Tell the quadtree how to draw itself in the window
        m_quadtree->draw([&](const qtree::Rect& rect) 
        { 
            Utils::drawRectangle(window, rect, sf::Color::White);
        });

        //Draw triangles placed in te scene
        for (size_t i=0; i<bounds.size(); i++)
        {
            Utils::drawRectangle(window, bounds[i], sf::Color::White);
        }

        // draw query rectangleRange
        QueryRect queryRange(200, 200, 200, 200);
        Utils::drawRectangle(window, queryRange, sf::Color::Green);

        // Draw found by query rectangles
        auto found = m_quadtree->query(queryRange);
        for (const auto& n : found)
        {
            Utils::drawRectangle(window, n->bound, sf::Color::Green);
        }

        window.display();
    }

    //clean vector
    for (int i = 0; i < points.size(); i++)
    {
        delete points[i];
    }

	return 0;
}