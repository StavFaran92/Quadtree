#pragma once

// STL
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <functional>
#include <memory>

#ifdef _DEBUG
#define LOG_DEBUG(s) std::cout << "DEBUG | " << s << " | " __FUNCTION__ << std::endl;
#else
#define LOG_DEBUG(s)
#endif

namespace qtree
{
    // Forward decleration
    struct Point;
    struct Shape;
    struct Rect;
    struct Circle;
    template<typename T> class Node;
    template<typename T> class QuadTree;

    ///////////////////////////////////////////////////////
    //////////////////////  Shape  ////////////////////////
    ///////////////////////////////////////////////////////
    struct Shape {
        virtual bool intersects(const Rect& bound) const noexcept = 0;
        virtual bool contains(const Rect& bound) const noexcept = 0;
    };


    ///////////////////////////////////////////////////////
    //////////////////////  Point  ////////////////////////
    ///////////////////////////////////////////////////////
    struct Point {
        Point(double x, double y) : x(x), y(y) {};

        double x, y;
    };

    ///////////////////////////////////////////////////////
    //////////////////////  Circle  ///////////////////////
    ///////////////////////////////////////////////////////
    struct Circle : public Shape
    {
        Circle(const Circle& other) : x(other.x), y(other.y), radius(other.radius) {};
        Circle(double x, double y, double radius) : x(x), y(y), radius(radius){};

        bool intersects(const Rect& bound) const noexcept override;
        bool contains(const Rect& bound) const noexcept override;

        double x, y, radius;
    };

    ///////////////////////////////////////////////////////
    //////////////////////  Rectangle  ////////////////////
    ///////////////////////////////////////////////////////
    struct Rect : public Shape {
        Rect(const Rect& other) : Rect(other.x, other.y, other.width, other.height) 
        {}

        Rect(double x, double y, double width, double height) :
            x(x),
            y(y),
            width(width),
            height(height) 
        {}

        bool intersects(const Rect& other) const noexcept override;
        bool contains(const Rect& other) const noexcept override;

        double x, y, width, height;
    };

    ///////////////////////////////////////////////////////
    //////////////////////  Node  /////////////////////////
    ///////////////////////////////////////////////////////
    template<typename T>
    class Node {
        
    public:
        Node(T* data = {}, const Rect& bound = {}) :
            data(data),
            bound(bound) {};

    public:
        T* data;
        Rect  bound;

    private:
        friend class QuadTree<T>;
        std::vector<QuadTree<T>*> qt;
    };


    ///////////////////////////////////////////////////////
    /////////////////////  Quadtree  //////////////////////
    ///////////////////////////////////////////////////////
    template<typename T>
    class QuadTree {
    public:
        QuadTree(const Rect& bound, unsigned capacity);
        QuadTree(const QuadTree& other) : QuadTree(other.m_bounds, other.m_capacity) { }
        QuadTree();

        // Inserts an object into this quadtree
        inline bool insert(T& obj, const Point& point)
        {
            return insert(obj, Rect(point.x, point.y, 1, 1));
        }

        inline bool insert(T& obj, double x, double y)
        {
            return insert(obj, Point(x, y));
        }

        inline bool insert(T& obj, const Rect& bound)
        {
            auto node = std::make_shared<Node<T>>(&obj, bound);
            return insert(node);
        }
        
        bool remove(const Node<T>& node);
        inline std::unordered_set<const Node<T>*> query(const Shape& range);
        inline void draw(std::function<void(const Rect&)> func) const;
        inline void clear() noexcept;

        ~QuadTree();
    private:
        bool insert(std::shared_ptr<Node<T>> node);
        void subdivide();
        void discardEmptyBuckets();
        void query(const Shape& range, std::unordered_set<const Node<T>*>& foundObjects);
    private:
        bool         m_isLeaf = true;
        unsigned int m_level = 0;
        unsigned int m_capacity;
        Rect         m_bounds;
        QuadTree* m_parent = nullptr;
        QuadTree* m_children[4] = { nullptr, nullptr, nullptr, nullptr };
        std::vector<std::shared_ptr<Node<T>>> m_nodes;
    };

    ///////////////////////////////////////////////////////
    ///////////////  Quadtree implementation  /////////////
    ///////////////////////////////////////////////////////
    template<typename T>
    inline QuadTree<T>::QuadTree(const Rect& _bound, unsigned _capacity) :
        m_bounds(_bound),
        m_capacity(_capacity)
    {
        m_nodes.reserve(_capacity);
    }

    // Inserts an object into this quadtree
    template<typename T>
    inline bool QuadTree<T>::insert(std::shared_ptr<Node<T>> node)
    {
        if (!m_bounds.intersects(node->bound)) return false;

        // Subdivide if required
        if (m_isLeaf && m_nodes.size() >= m_capacity) {
            subdivide();
        }

        // insert object into it's leaves
        if (!m_isLeaf) {
            m_children[0]->insert(node);
            m_children[1]->insert(node);
            m_children[2]->insert(node);
            m_children[3]->insert(node);
        }
        else 
        {
            LOG_DEBUG("Insert node: " << &*node.get() << " Holding Point: " << node->data);
            m_nodes.push_back(node);
            node->qt.emplace_back(this);
        }

        return true;
    }

    // Removes an object from this quadtree
    template<typename T>
    inline bool QuadTree<T>::remove(const Node<T>& node)
    {
        for (auto& t : node.qt)
        {
            auto& nodes = t->m_nodes;
            nodes.erase(std::find_if(nodes.begin(), nodes.end(), [&](const auto& other) { return other.get() == &node; }));
            t->discardEmptyBuckets();
        }

        
        return true;
    }

    // Query the Quadtree for points inside the bound rect and return as vector
    template<typename T>
    inline std::unordered_set<const Node<T>*> QuadTree<T>::query(const Shape& range)
    {
        std::unordered_set<const Node<T>*> foundObjects;
        query(range, foundObjects);
        return foundObjects;
    }

    // Query the Quadtree for points inside the bound rect and return as vector
    template<typename T>
    inline void QuadTree<T>::query(const Shape& range, std::unordered_set<const Node<T>*>& foundObjects)
    {
        if (!range.intersects(m_bounds)) return;

        if (range.contains(m_bounds))
        {
            for (const auto& node : m_nodes)
            {
                foundObjects.insert(node.get());
            }
        }
        else
        {
            for (const auto& node : m_nodes)
            {
                if (range.intersects(node->bound))
                {
                    foundObjects.insert(node.get());
                }
            }
        }
        if (!m_isLeaf) 
        {
            // Get objects from leaves
            for (QuadTree* leaf : m_children) 
            {
                leaf->query(range, foundObjects);
            }
        }
    }

    // Removes all objects and children from this quadtree
    template<typename T>
    inline void QuadTree<T>::clear() noexcept {
        m_nodes.clear();

        if (!m_isLeaf) {
            for (QuadTree* child : m_children)
                child->clear();

            delete m_children[0];
            delete m_children[1];
            delete m_children[2];
            delete m_children[3];

            m_isLeaf = true;
        }
    }

    // Subdivides into four sub quadtrees
    template<typename T>
    inline void QuadTree<T>::subdivide() {
        double width = m_bounds.width * 0.5f;
        double height = m_bounds.height * 0.5f;
        double x = 0, y = 0;
        for (int i = 0; i < 4; ++i) {
            switch (i) {
            case 0: x = m_bounds.x + width; y = m_bounds.y; break; // Top right
            case 1: x = m_bounds.x;         y = m_bounds.y; break; // Top left
            case 2: x = m_bounds.x;         y = m_bounds.y + height; break; // Bottom left
            case 3: x = m_bounds.x + width; y = m_bounds.y + height; break; // Bottom right
            }
            m_children[i] = new QuadTree({ x, y, width, height }, m_capacity);
            m_children[i]->m_level = m_level + 1;
            m_children[i]->m_parent = this;
        }
        m_isLeaf = false;
    }

    // Discards buckets if all children are leaves and contain no Nodes
    template<typename T>
    inline void QuadTree<T>::discardEmptyBuckets() {
        if (!m_nodes.empty()) return;
        if (!m_isLeaf) {
            for (QuadTree* child : m_children)
                if (!child->m_isLeaf || !child->m_nodes.empty())
                    return;
        }

        clear();

        if (m_parent) m_parent->discardEmptyBuckets();
    }

    // draw the quadtree using a callback function
    template<typename T>
    inline void QuadTree<T>::draw(std::function<void(const Rect&)> func) const
    {
        func(m_bounds);

        if (!m_isLeaf) {
            for (QuadTree* child : m_children)
            {
                child->draw(func);
            }
        }
    }

    template<typename T>
    inline QuadTree<T>::~QuadTree() {
        clear();
    }

    ///////////////////////////////////////////////////////
    ///////////////  Circle implementation  ///////////////
    ///////////////////////////////////////////////////////
    inline bool Circle::intersects(const Rect& other) const noexcept 
    {
        double dx = abs(x - (other.x + other.width / 2));
        double dy = abs(y - (other.y + other.height / 2));

        if (dx > (other.width / 2 + radius)) { return false; }
        if (dy > (other.height / 2 + radius)) { return false; }

        if (dx <= (other.width / 2)) { return true; }
        if (dy <= (other.height / 2)) { return true; }

        return ((dx - other.width / 2) * (dx - other.width / 2) + 
            (dy - other.height / 2) * (dy - other.height / 2) <= (radius * radius));
    }

    inline bool Circle::contains(const Rect& other) const noexcept
    {
        double dx = std::max(abs(x - other.x - other.width / 2), abs(other.x + other.width / 2 - x));
        double dy = std::max(abs(y - other.y + other.height / 2), abs(other.y - other.height / 2 - y));
        return (radius * radius) >= (dx * dx) + (dy * dy);
    }

    ///////////////////////////////////////////////////////
    ///////////////  Rectangle implementation  ////////////
    ///////////////////////////////////////////////////////
    
    inline bool Rect::intersects(const Rect& other) const noexcept 
    {
        if (x > other.x + other.width)  return false;
        if (x + width < other.x)        return false;
        if (y > other.y + other.height) return false;
        if (y + height < other.y)       return false;
        return true;
    }

    inline bool Rect::contains(const Rect& other) const noexcept
    {
        if ((other.x + other.width) < (x + width)
            && (other.x) > (x)
            && (other.y) > (y)
            && (other.y + other.height) < (y + height))
            return true;
        return false;
    }
}