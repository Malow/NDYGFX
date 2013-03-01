#ifndef MESHSTRIP
#define MESHSTRIP

#include "BoundingSphere.h"
#include <string>

//Avoid circular include
class Object3D;
class Material;
struct VertexNormalMap;


class MeshStrip
{
private:
	Object3D* RenderObject;
	VertexNormalMap* mesh;
	int nrOfVerts;
	int nrOfIndicies;
	int* indicies;
	std::string texture;
	Material* material;
	BoundingSphere bb;

public:
	MeshStrip();
	virtual ~MeshStrip();
	MeshStrip(const MeshStrip* origObj);

	VertexNormalMap* getVerts() const { return this->mesh; }
	void SetVerts(VertexNormalMap* verts) { this->mesh = verts; }
	int getNrOfVerts() const { return this->nrOfVerts; }
	void setNrOfVerts(int vertno) { this->nrOfVerts = vertno; }
	int* getIndicies() const { return this->indicies; }
	void SetIndicies(int* inds) { this->indicies = inds; }
	int getNrOfIndicies() const { return this->nrOfIndicies; }
	void setNrOfIndicies(int indcount) { this->nrOfIndicies = indcount; }
	const std::string& GetTexturePath() const { return this->texture; }
	void SetTexturePath(std::string path);

	Object3D* GetRenderObject() const { return this->RenderObject; }
	void SetRenderObject(Object3D* ro) { this->RenderObject = ro; }

	void SetMaterial(Material* mat);
	Material* GetMaterial() const { return this->material; }

	BoundingSphere& GetBoundingSphere() { return this->bb; }
	void SetBoundingSphere(BoundingSphere bb) { this->bb = bb; }
};

#endif