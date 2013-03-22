#pragma once

//--------------------------------------------------------------------------------------------------
//
//	Written by Markus Tillman for project "Not dead yet" at Blekinge Tekniska Högskola.
// 
//	Helper class for hardware instancing of billboards and meshes.
//	Requirements:	ID3D11Device*, ID3D11DeviceContext*.
//	
//--------------------------------------------------------------------------------------------------

#include "Vertex.h"
#include "DirectX.h"
#include <vector>
#include "Billboard.h"
#include "BillboardCollection.h"
#include "Mesh.h"


class StaticMesh;
class AnimatedMesh;

/*struct BillboardData
{
	VertexBillboard1			s_Vertex;
	ID3D11ShaderResourceView*	s_SRV; 
	
	BillboardData() : s_Vertex(), s_SRV(NULL) {}
	BillboardData(VertexBillboard1 vertex, ID3D11ShaderResourceView* srv) : s_Vertex(vertex), s_SRV(srv) {}
};*/

struct BillboardGroup
{
	unsigned int				s_Size; 
	unsigned int				s_StartLocation;
	ID3D11ShaderResourceView*	s_SRV;
	float						s_CullNearDistance;
	float						s_CullFarDistance;
	
	BillboardGroup() : s_Size(0), s_StartLocation(0), s_SRV(NULL), s_CullNearDistance(0.0f), s_CullFarDistance(std::numeric_limits<float>::infinity()) {}
	BillboardGroup(	unsigned int size, unsigned int startLocation, ID3D11ShaderResourceView* srv,
					float cullNearDistance, float cullFarDistance)
	:	s_Size(size), s_StartLocation(startLocation), s_SRV(srv), 
		s_CullNearDistance(cullNearDistance), s_CullFarDistance(cullFarDistance) {}
};

struct StripData
{
	struct InstancedDataStruct
	{
		D3DXMATRIX	s_WorldMatrix; //translation, rotation, scale
		//D3DXMATRIX	s_WorldInverseTransposeMatrix; 

		InstancedDataStruct() 
		{
			D3DXMatrixIdentity(&s_WorldMatrix);
			//s_WorldInverseTransposeMatrix = s_WorldMatrix;
		}
	} InstancedData;

	MeshStrip*	 s_MeshStrip;

	StripData() : s_MeshStrip(NULL) {}
	StripData(MeshStrip* meshStrip) : s_MeshStrip(meshStrip) {}
};
struct StripGroup
{
	unsigned int		s_StartLocation;
	unsigned int		s_Size; 
	MeshStrip*			s_MeshStrip;

	StripGroup() : s_StartLocation(0), s_Size(0), s_MeshStrip(NULL) {}
	StripGroup(unsigned int startLocation, unsigned int size, MeshStrip* meshStrip)
		: s_StartLocation(startLocation), s_Size(size), s_MeshStrip(meshStrip)
	{}
};

struct AnimatedStripData
{
	struct AnimatedInstancedDataStruct
	{
		D3DXMATRIX	s_WorldMatrix; //translation, rotation, scale

		AnimatedInstancedDataStruct() 
		{
			D3DXMatrixIdentity(&s_WorldMatrix);
		}
	} InstancedData;

	MeshStrip*	 s_MeshStripOne;
	MeshStrip*	 s_MeshStripTwo;

	AnimatedStripData() : s_MeshStripOne(NULL), s_MeshStripTwo(NULL) {}
	AnimatedStripData(MeshStrip* meshStripOne, MeshStrip* meshStripTwo) : s_MeshStripOne(meshStripOne), s_MeshStripTwo(meshStripTwo) {}
};
struct AnimatedStripGroup
{
	unsigned int		s_StartLocation;
	unsigned int		s_Size; 
	MeshStrip*			s_MeshStripOne;
	MeshStrip*			s_MeshStripTwo;

	AnimatedStripGroup() : s_StartLocation(0), s_Size(0), s_MeshStripOne(NULL), s_MeshStripTwo(NULL) {}
	AnimatedStripGroup(unsigned int startLocation, unsigned int size, MeshStrip* meshStripOne, MeshStrip* meshStripTwo)
		: s_StartLocation(startLocation), s_Size(size), s_MeshStripOne(meshStripOne), s_MeshStripTwo(meshStripTwo) {}
};

bool SortBillboardData(BillboardData &billboardLeft, BillboardData &billboardRight);
bool SortStripData(StripData &stripLeft, StripData &stripRight);
bool SortAnimatedStripData(AnimatedStripData &stripLeft, AnimatedStripData &stripRight);

class InstancingHelper
{
	private:
		ID3D11Device*			g_Device;
		ID3D11DeviceContext*	g_DeviceContext;

	private:
		//BILLBOARD
		bool zRenderGrassFlag;
		string zGrassFilePath;
		const MaloW::Array<Billboard*>* zBillboardsReference;
		const MaloW::Array<BillboardCollection*>* zBillboardCollectionsReference;
		//Counter
		unsigned int zBillboardInstanceBufferSize;
		//Billboard data
		std::vector<BillboardData> zBillboardData;
		//Instance group 
		std::vector<BillboardGroup> zBillboardGroups;
		//Instance Buffer
		ID3D11Buffer* zBillboardInstanceBuffer; //Shall contain vertex data of billboardData.

		

		//MESHES(MESHSTRIPS)
		//Counters
		unsigned int zStripInstanceBufferSize;
		//Vector containing strip data of all meshes that are added each frame.
		std::vector<StripData> zStripData;
		//Instance group (groups of strips that share the same texture and vertex data)
		std::vector<StripGroup> zStripGroups;
		//Instance Buffer (containing all instanced data)
		ID3D11Buffer* zStripInstanceBuffer; 



		//ANIMATED MESHES(MESHSTRIPS)
		//Counters
		unsigned int zAnimatedStripInstanceBufferSize;
		//Vector containing strip data (in groups of 2) of all meshes that are added each frame.
		std::vector<AnimatedStripData> zAnimatedStripData;
		//Instance group (groups of strips that share the same texture and vertex data)
		std::vector<AnimatedStripGroup> zAnimatedStripGroups;
		//Instance Buffer (containing all instanced data)
		ID3D11Buffer* zAnimatedStripInstanceBuffer; 
		
    private:
		void ExpandBillboardInstanceBuffer();
		void ExpandStripInstanceBuffer();
		void ExpandAnimatedStripInstanceBuffer();

	public:
		InstancingHelper();
		virtual~InstancingHelper();

		HRESULT Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

		//BILLBOARD
		bool GetRenderGrassFlag() { return this->zRenderGrassFlag; }
		void SetRenderGrassFlag(bool flag) { this->zRenderGrassFlag = flag; }
		string GetGrassFilePath() { return this->zGrassFilePath; }
		void SetGrassFilePath(string filePath) { this->zGrassFilePath = filePath; }
		void AddBillboardsReference(const MaloW::Array<Billboard*>& billboards);
		void AddBillboardCollectionsReference(const MaloW::Array<BillboardCollection*>& billboardCollections);

		unsigned int GetNrOfBillboards() { return this->zBillboardData.size() + this->zBillboardsReference->size() + this->zBillboardCollectionsReference->size(); }
		unsigned int GetNrOfBillboardGroups() { return this->zBillboardGroups.size(); }
		unsigned int GetBillboardDataCapacity() { return this->zBillboardData.capacity(); }
		unsigned int GetBillboardGroupCapacity() { return this->zBillboardGroups.capacity(); }
		BillboardData& GetBillboardData(unsigned int index) { return this->zBillboardData[index]; }
		BillboardGroup& GetBillboardGroup(unsigned int index) { return this->zBillboardGroups[index]; }
		ID3D11Buffer* GetBillboardInstanceBuffer() { return this->zBillboardInstanceBuffer; }  

		void AddBillboard(Billboard* billboard);
		void AddBillboard(Mesh* meshWithBillboard);
		/*	Sorts, creates instance groups and updates the instance buffer.	*/
		void PreRenderBillboards(bool shadowmap = false);
		void PostRenderBillboards() { this->zBillboardData.clear(); this->zBillboardGroups.clear(); }

		//MESH
		void AddStaticMesh(StaticMesh* staticMesh);

		unsigned int GetNrOfStrips() { return this->zStripData.size(); }
		unsigned int GetNrOfStripGroups() { return this->zStripGroups.size(); }
		unsigned int GetStripDataCapacity() { return this->zStripData.capacity(); }
		unsigned int GetStripGroupCapacity() { return this->zStripGroups.capacity(); }
		StripData& GetStripData(unsigned int index) { return this->zStripData[index]; }
		StripGroup& GetStripGroup(unsigned int index) { return this->zStripGroups[index]; }
		ID3D11Buffer* GetStripInstanceBuffer() { return this->zStripInstanceBuffer; }  

		/*	Sorts, creates instance groups and updates the instance buffer.	*/
		void PreRenderStrips();
		void PostRenderStrips() { this->zStripData.clear(); this->zStripGroups.clear(); }



		//ANIMATED MESH
		void AddAnimatedMesh(AnimatedMesh* animatedMesh, float timer);
		unsigned int GetNrOfAnimatedStrips() { return this->zAnimatedStripData.size(); }
		unsigned int GetNrOfAnimatedStripGroups() { return this->zAnimatedStripGroups.size(); }
		unsigned int GetAnimatedStripDataCapacity() { return this->zAnimatedStripData.capacity(); }
		unsigned int GetAnimatedStripGroupCapacity() { return this->zAnimatedStripGroups.capacity(); }
		AnimatedStripData& GetAnimatedStripData(unsigned int index) { return this->zAnimatedStripData[index]; }
		AnimatedStripGroup& GetAnimatedStripGroup(unsigned int index) { return this->zAnimatedStripGroups[index]; }
		ID3D11Buffer* GetAnimatedStripInstanceBuffer() { return this->zAnimatedStripInstanceBuffer; }  

		/*	Sorts, creates instance groups and updates the instance buffer.	*/
		void PreRenderAnimatedStrips();
		void PostRenderAnimatedStrips() { this->zAnimatedStripData.clear(); this->zAnimatedStripGroups.clear(); }

};