/** @file */
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

    /** \brief 
     * Shape struct which represents a geometrical 2D shape
     * 
     */
    struct Shape {
        /**
         * intersects
         * 
         * Return if the given bound intersects with this Shape
         * 
         * \param bound A rect object to check againts
         * \return True if intersects, False otherwise
         */
        virtual bool intersects(const Rect& bound) const noexcept = 0;

        /**
         * contains
         *
         * Return if the given bound is contained within this Shape
         *
         * \param bound A rect object to check againts
         * \return True if contained, False otherwise
         */
        virtual bool contains(const Rect& bound) const noexcept = 0;
    };


    /** \brief
     * Point struct which represents a Point in a 2D space
     *
     */
    struct Point {
        /** Constructor */
        Point(double x, double y) : x(x), y(y) {};

        double x, y;
    };

    /** \brief
     * Circle struct which extends Shape, this struct represents a 2D Circle
     *
     */
    struct Circle : public Shape
    {
        /** Constructor */
        Circle(const Circle& other) : x(other.x), y(other.y), radius(other.radius) {};

        /** Constructor */
        Circle(double x, double y, double radius) : x(x), y(y), radius(radius){};

        /** See delecration of Shape */
        bool intersects(const Rect& bound) const noexcept override;

        /** See delecration of Shape */
        bool contains(const Rect& bound) const noexcept override;

        /** The Circle X coord, Center of the circle */
        double x; 

        /**< The Circle Y coord, Center of the circle */
        double y;
        double radius;
    };

    /** \brief
     * Rect struct which extends Shape, this struct represents a 2D Rectangle
     *
     */
    struct Rect : public Shape 
    {
        /** Constructor */
        Rect(const Rect& other) : Rect(other.x, other.y, other.width, other.height) 
        {}

        /** Constructor */
        Rect(double x, double y, double width, double height) :
            x(x),
            y(y),
            width(width),
            height(height) 
        {}

        /** See delecration of Shape */
        bool intersects(const Rect& other) const noexcept override;

        /** See delecration of Shape */
        bool contains(const Rect& other) const noexcept override;

        double x, y, width, height;
    };

    /** \Brief
     * The main object used by the quad tree to handle data
     * 
     * The main object used by the quad tree to handle data, it contains a pointer to the data and a bound 
     * object which is the object bound representation in the 2D space
     * 
     */
    template<typename T>
    class Node {
        
    public:
        /** Constructor */
        Node(T* data = {}, const Rect& bound = {}) :
            data(data),
            bound(bound) {};

    public:
        T* data = nullptr;
        Rect bound;

    private:
        friend class QuadTree<T>;
        std::vector<QuadTree<T>*> qt;
    };


    /** \brief
     * Quadtree data structure
     *
     * A quadtree is a tree data structure in which each internal node has exactly four children. 
     * Quadtrees are the two-dimensional analog of octrees and are most often used to partition 
     * a two-dimensional space by recursively subdividing it into four quadrants or regions. 
     * The data associated with a leaf cell varies by application, 
     * but the leaf cell represents a "unit of interesting spatial information".
     * 
     * From Wikipedia, the free encyclopedia
     *
     */
    template<typename T>
    class QuadTree {
    public:
        /** Constructor */
        QuadTree(const Rect& bound, unsigned capacity);

        /** Copy Constructor */
        QuadTree(const QuadTree& other) : QuadTree(other.m_bounds, other.m_capacity) { }

        /** insert
         *
         *  Insert an object into the quadtree 
         * 
         *  \param obj      object to insert into the quadtree
         *  \param point    the object position in the scene
         * 
         *  \return     True or false wether the insertion was successful
         */
        inline bool insert(T& obj, const Point& point){ return insert(obj, Rect(point.x, point.y, 1, 1)); }

        /** insert
         * 
         * Insert an object into the quadtree 
         * 
         * \param obj   object to insert into the quadtree
         * \param x     object X coordinate
         * \param y     object Y coordinate
         * \return      True or false wether the insertion was successful
         */
        inline bool insert(T& obj, double x, double y){ return insert(obj, Point(x, y)); }

        /** insert
         * 
         * Insert an object into the quadtree 
         * 
         * \param obj       object to insert into the quadtree
         * \param bound     object's bound in space
         * \return          True or false wether the insertion was successful
         */
        inline bool insert(T& obj, const Rect& bound){ return insert(std::make_shared<Node<T>>(&obj, bound)); }
        
        /** remove
         * 
         * Remove an element from the quadtree
         * 
         * \param node  The node to be removed from the quadtree
         * \return True or false wether the removal was successful
         */
        bool remove(const Node<T>& node);

        /** query
         * 
         * Query the Quadtree with a given range, this will return all the objects in the quadtree with a bound that intersects the given range
         * 
         * \param range     A shape that will be used to query the Quadtree
         * \return          A set of unique elements which their bound intersects the given range
         */
        inline std::unordered_set<const Node<T>*> query(const Shape& range);

        /** draw
         * 
         * Draw the quadtree using a callback function that accepts Rect and returns void
         * 
         * Example usage:
         * draw([](Rect& rect){ awesome_draw_rectangle_function(rect.x, rect.y, rect.width, rect.height); })
         * 
         * \param func      A callback function to draw the quadtree with
         */
        inline void draw(std::function<void(const Rect&)> func) const;

        /** clear
         * 
         * Clear the quadtree and it's children recursively
         * 
         * \return 
         */
        inline void clear() noexcept;

        ~QuadTree();
    private:
        QuadTree() = delete;
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

    /** Quadtree implementation  */
    template<typename T>
    inline QuadTree<T>::QuadTree(const Rect& _bound, unsigned _capacity) :
        m_bounds(_bound),
        m_capacity(_capacity)
    {
        m_nodes.reserve(_capacity);
    }

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

    template<typename T>
    inline bool QuadTree<T>::remove(const Node<T>& node)
    {
        for (auto t : node.qt)
        {
            auto& nodes = t->m_nodes;
            nodes.erase(std::find_if(nodes.begin(), nodes.end(), [&](const auto& other) { return other.get() == &node; }));
            t->discardEmptyBuckets();
        }

        
        return true;
    }

    template<typename T>
    inline std::unordered_set<const Node<T>*> QuadTree<T>::query(const Shape& range)
    {
        std::unordered_set<const Node<T>*> foundObjects;
        query(range, foundObjects);
        return foundObjects;
    }

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

    /** Circle implementation */
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

    /** Rectangle implementation */
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