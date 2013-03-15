#pragma once

#include "CircleAndRect.h"
#include "Observer.h"
#include <set>

class Entity;
class EntQuadTree;

// Test Function, Remove
void PrintTree(const EntQuadTree&);


class EntQuadTree : public Observer
{
	class Node
	{
		// --------
		// | 0	1 |
		// |	  |
		// | 2	3 |
		// --------
		Node* zChildNodes[4];
		Rect zRect;

		std::set<Entity*> zElements;
		size_t zNumElementsWithChildren;

	public:
		Node(const Rect& rect);
		virtual ~Node();

		// Insert element into node or children
		bool Insert(Entity* elem, const Vector2& pos);

		// Erase element from node and child nodes
		bool Erase(Entity* elem, const Vector2& pos);

		// Manually set a child node, deletes existing one by default
		void SetNode(unsigned int index, Node* ptrNode, bool deleteFlag=true);

		// Scan circle recursively and return amounts of additions
		size_t CircleScan(std::set<Entity*>& ents, const Circle& circle, unsigned int entType) const;

		// Scan rectangle recursively and return amounts of additions
		size_t RectangleScan(std::set<Entity*>& ents, const Rect& rect, unsigned int entType) const;

		// Fill ents set recursively and return amount of additions
		size_t FlatScan(std::set<Entity*>& ents, unsigned int entType) const;

		// Transfer entities
		size_t Transfer(Node* node);

		// Element count
		inline size_t GetNumElements() const { return zElements.size(); }

		// Child node count
		inline size_t GetNumNodes() const { return (zChildNodes[0]!=0) + (zChildNodes[1]!=0) + (zChildNodes[2]!=0) + (zChildNodes[3]!=0); }

		// Number of my elements and the child node elements
		inline size_t GetTotalElements() const { return zNumElementsWithChildren; }

		// Iterative entity count
		size_t CalcNumEntities(unsigned int entType=0) const;

		// Node rectangle
		inline const Rect& GetRect() const { return zRect; }

		// Child node access
		inline Node* GetNode(unsigned int index) const { return zChildNodes[index]; }
		size_t CountInRect( const Rect& rect ) const;
	};

	Node* zRoot;

public:
	EntQuadTree();
	virtual ~EntQuadTree();

	// Fill A Set Of All Elements
	size_t FlatScan(std::set<Entity*>& ents, unsigned int entType = 0) const;

	// Scan Circle Area
	size_t CircleScan(std::set<Entity*>& ents, const Circle& circle, unsigned int entType = 0) const;

	// Scan Rectangle Area
	size_t RectangleScan(std::set<Entity*>& ents, const Rect& rect, unsigned int entType = 0) const;

	// Count Elements in Rectangle Area
	size_t CountInRect( const Rect& rect ) const;

	// Management
	bool Insert(Entity* E);
	bool Erase(Entity* E);

	// Print Tree
	void PrintDebug(const std::string& msgLine);
	void BranchPrint(Node*, std::ofstream& file, unsigned int level);
	void PrintTree();

protected:
	bool Insert(Entity* E, const Vector2& pos);
	bool Erase(Entity* E, const Vector2& pos);
	virtual void OnEvent(Event* e);
};