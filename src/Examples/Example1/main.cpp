#include <iostream>
#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"

#include "Utils.h"
#include "Quadtree.h"

static const int WIDTH = 800;
static const int HEIGHT = 600;
static const qtree::Rect MAP_BOUNDS = { 0, 0, WIDTH, HEIGHT };

using QueryRect = qtree::Rect;
using QueryCircle = qtree::Circle;

int main()
{
    // Seed random
    std::srand(std::time(nullptr));

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Quadtree");

    auto m_quadtree = std::make_shared<qtree::QuadTree<qtree::Point>>(MAP_BOUNDS, 4);

    QueryCircle query(200, 200, 100);
    //QueryRect query(200, 200, 200, 200);

    std::vector<qtree::Point*> points;

    //for (int i = 0; i < 100; i++)
    //{
    //    int x = getRandom(WIDTH);
    //    int y = getRandom(HEIGHT);
    //    auto point = new qtree::Point( x, y);
    //    points.push_back(point);
    //    auto node = new qtree::Node<qtree::Point>( qtree::Rect(x, y, 5, 5), point);
    //    m_quadtree->insert(node);
    //    //m_quadtree->insert(point, qtree::Rect(x, y, 5, 5));
    //}

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    int x = event.mouseButton.x;
                    int y = event.mouseButton.y;
                    auto point = new qtree::Point(x, y);
                    points.push_back(point);
                    m_quadtree->insert(*point, x, y);
                }
            }

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::A)
                {
                    auto found = m_quadtree->query(query);
                    for (const auto& n : found)
                    {
                        LOG_DEBUG("Found Node: " << n);
                        auto& point = n->data;
                        LOG_DEBUG("Found point: " << point);
                        m_quadtree->remove(*n);
                        points.erase(std::find(points.begin(), points.end(), point));

                    }
                }
            }
        }

        window.clear();

        //Tell the quadtree how to draw itself in the window
        m_quadtree->draw([&](const qtree::Rect& rect)
        {
            Utils::drawRectangle(window, rect, sf::Color::White);
        });

        // Draw points in quadtree
        for (const auto& p : points)
        {
            Utils::drawCircle(window, qtree::Circle(p->x, p->y, 3), sf::Color::White);
        }

        // draw query rectangle
        //Utils::drawRectangle(window, query, sf::Color::Green);

        // draw query circle
        Utils::drawCircle(window, query, sf::Color::Green);

        // Draw found by the query points 
        auto found = m_quadtree->query(query);
        for (const auto& n : found)
        {
            Utils::drawCircle(window, qtree::Circle(n->data->x, n->data->y, 3), sf::Color::Green);
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