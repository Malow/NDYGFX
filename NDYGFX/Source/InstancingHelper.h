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
#include "Mesh.h"


struct BillboardData
{
	Vertex							s_Vertex;
	ID3D11ShaderResourceView*		s_SRV; 
	
	BillboardData() : s_Vertex(), s_SRV(NULL) {}
	BillboardData(Vertex vertex, ID3D11ShaderResourceView* srv) : s_Vertex(vertex), s_SRV(srv) {}
};
struct BillboardGroup
{
	unsigned int				s_Size; 
	unsigned int				s_StartLocation;
	ID3D11ShaderResourceView*	s_SRV;
	
	BillboardGroup() : s_Size(0), s_StartLocation(0), s_SRV(NULL) {}
	BillboardGroup(unsigned int size, unsigned int startLocation, ID3D11ShaderResourceView* srv)
		: s_Size(size), s_StartLocation(startLocation), s_SRV(srv) {}
};


/*struct MeshData
{
	struct StripData
	{
		ID3D11ShaderResourceView*	s_SRV;
		ID3D11Buffer*				s_VertexBuffer;

		StripData() : s_SRV(NULL), s_VertexBuffer(NULL) {}
		StripData(ID3D11ShaderResourceView*	srv, ID3D11Buffer* vertexBuffer) : s_SRV(srv), s_VertexBuffer(vertexBuffer) {}

	};
	
	struct InstancedData
	{
		D3DXMATRIX				s_WorldMatrix; //translation, rotation, scale
	};

	std::vector<StripData>	s_StripData;
	InstancedData			s_InstancedData;

	MeshData() : s_StripData(), s_InstancedData() {}
	MeshData(std::vector<StripData>	stripData, InstancedData instancedData) 
		: s_StripData(stripData), s_InstancedData(instancedData) {}
};*/
struct MeshData
{
	struct InstancedDataStruct
	{
		D3DXMATRIX	s_WorldMatrix; //translation, rotation, scale
		D3DXMATRIX	s_WorldInverseTransposeMatrix; 

		InstancedDataStruct() 
		{
			D3DXMatrixIdentity(&s_WorldMatrix);
			s_WorldInverseTransposeMatrix = s_WorldMatrix;
		}
	} InstancedData;

	
	MeshStripsResource*	 s_MeshStripsResource;

	MeshData() : s_MeshStripsResource(NULL) {}
	MeshData(MeshStripsResource* meshStripsResource) : s_MeshStripsResource(meshStripsResource) {}
};


struct MeshGroup 
{
	unsigned int				s_StartLocation;
	unsigned int				s_Size; 
	MeshStripsResource*			s_MeshStripsResource;

	MeshGroup() : s_StartLocation(0), s_Size(0), s_MeshStripsResource(NULL) {}
	MeshGroup(unsigned int startLocation, unsigned int size, D3DXMATRIX* worldMatrices, MeshStripsResource* meshStripsResource)
		: s_StartLocation(startLocation), s_Size(size), s_MeshStripsResource(meshStripsResource) {}
};

bool SortBillboardData(BillboardData billboardLeft, BillboardData billboardRight);
bool SortMeshData(MeshData meshLeft, MeshData meshRight);

class InstancingHelper
{
	private:
		ID3D11Device*			g_Device;
		ID3D11DeviceContext*	g_DeviceContext;

	private:
		//BILLBOARD
		//Counters
		unsigned int zNrOfBillboards; 
		unsigned int zNrOfBillboardGroups;
		unsigned int zBillboardDataCapacity; //Also size of buffer
		unsigned int zBillboardGroupCapacity;
		//Billboard data
		std::vector<BillboardData> zBillboardData;
		//Instance group 
		std::vector<BillboardGroup> zBillboardGroups;
		//Instance Buffer
		ID3D11Buffer* zBillboardInstanceBuffer; //Shall contain vertex data of billboardData.



		//MESHES(MESHSTRIPS)
		//Counters
		unsigned int zNrOfMeshes;
		unsigned int zNrOfMeshGroups;
		unsigned int zMeshDataCapacity;
		unsigned int zMeshGroupCapacity;

		//Vector containing Mesh data of all meshes that are added each frame.
		std::vector<MeshData> zMeshData;
		
		//Vertex buffers containing instance data for each mesh group.
		//ID3D11Buffer** zVertexBuffers;

		//Instance group (groups of meshes that share the same texture)
		std::vector<MeshGroup> zMeshGroups;

		//Instance Buffer (containing all instanced data)
		ID3D11Buffer* zMeshInstanceBuffer; 

		
		


    private:
	    void ExpandBillboardDataAndBuffer();
		void ExpandMeshDataAndBuffer();

	public:
		InstancingHelper();
		virtual~InstancingHelper();

		HRESULT Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

		//BILLBOARD
		unsigned int GetNrOfBillboards() { return this->zNrOfBillboards; }
		unsigned int GetNrOfBillboardGroups() { return this->zNrOfBillboardGroups; }
		unsigned int GetBillboardDataCapacity() { return this->zBillboardDataCapacity; }
		unsigned int GetBillboardGroupCapacity() { return this->zBillboardGroupCapacity; }
		BillboardData GetBillboardData(unsigned int index) { return this->zBillboardData[index]; }
		BillboardGroup GetBillboardGroup(unsigned int index) { return this->zBillboardGroups[index]; }
		ID3D11Buffer* GetBillboardInstanceBuffer() { return this->zBillboardInstanceBuffer; }  

		void AddBillboard(const Billboard* const billboard);
		/*	Sorts, creates instance groups and updates the instance buffer.	*/
		void PreRenderBillboards();
		/*	Reset zNrOfBillboards to allow overwriting of old data.	*/
		void PostRenderBillboards() { this->zNrOfBillboards = 0; }
		


		//MESH
		unsigned int GetNrOfMeshes() { return this->zNrOfMeshes; }
		unsigned int GetNrOfMeshGroups() { return this->zNrOfMeshGroups; }
		unsigned int GetMeshDataCapacity() { return this->zMeshDataCapacity; }
		unsigned int GetMeshGroupCapacity() { return this->zMeshGroupCapacity; }
		MeshData GetMeshData(unsigned int index) { return this->zMeshData[index]; }
		MeshGroup GetMeshGroup(unsigned int index) { return this->zMeshGroups[index]; }
		ID3D11Buffer* GetMeshInstanceBuffer() { return this->zMeshInstanceBuffer; }  

		void AddMesh(Mesh* mesh);
		void PreRenderMeshes();
		/*	Reset zNrOfMeshes to allow overwriting of old data.	*/
		void PostRenderMeshes() { this->zNrOfMeshes = 0; }

};