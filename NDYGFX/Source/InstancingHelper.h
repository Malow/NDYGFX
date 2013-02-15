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


struct BillboardData
{
	//std::vector<Vertex>							billboardInstanceDataVertex;
	//std::vector<ID3D11ShaderResourceView*>		billboardInstanceDataSRVsBillboard; 

	Vertex							s_Vertex;
	ID3D11ShaderResourceView*		s_SRV; 
	
	
	BillboardData() : s_Vertex(), s_SRV(NULL) {}
	BillboardData(Vertex vertex, ID3D11ShaderResourceView* srv) : s_Vertex(vertex), s_SRV(srv) {}
};
struct BillboardInstanceGroup
{
	//std::vector<unsigned int>				billboardInstanceGroupSize; 
	//std::vector<unsigned int>				billboardInstanceGroupStartLocation;
	//std::vector<ID3D11ShaderResourceView*>	billboardInstanceGroupSRV;
	unsigned int				s_Size; 
	unsigned int				s_StartLocation;
	ID3D11ShaderResourceView*	s_SRV;
	
	BillboardInstanceGroup() : s_Size(0), s_StartLocation(0), s_SRV(NULL) {}
	BillboardInstanceGroup(unsigned int size, unsigned int startLocation, ID3D11ShaderResourceView* srv)
		: s_Size(size), s_StartLocation(startLocation), s_SRV(srv) {}
};



class InstancingHelper
{
	private:
		ID3D11Device*			g_Device;
		ID3D11DeviceContext*	g_DeviceContext;

	private:
		//BILLBOARD
		//Counters
		unsigned int zNrOfBillboards; 
		unsigned int zNrOfBillboardInstanceGroups;
		unsigned int zBillboardDataCapacity; //Also capacity of buffer
		unsigned int zBillboardInstanceGroupCapacity;
		//Instance data
		std::vector<BillboardData> zBillboardData;
		//Instance group 
		std::vector<BillboardInstanceGroup> zBillboardInstanceGroups;
		//Buffer
		ID3D11Buffer* zBillboardInstanceBuffer; //Shall contain vertex data of billboardData.

	public:
		InstancingHelper();
		virtual~InstancingHelper();

		HRESULT Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext);


		unsigned int GetNrOfBillboards() { return this->zNrOfBillboards; }
		unsigned int GetNrOfBillboardInstanceGroups() { return this->zNrOfBillboardInstanceGroups; }
		unsigned int GetBillboardDataCapacity() { return this->zBillboardDataCapacity; }
		unsigned int GetBillboardInstanceGroupCapacity() { return this->zBillboardInstanceGroupCapacity; }

		BillboardData GetBillboardData(unsigned int index) { return this->zBillboardData[index]; }
		BillboardInstanceGroup GetBillboardInstanceGroup(unsigned int index) { return this->zBillboardInstanceGroups[index]; }
		
		ID3D11Buffer* GetBillboardInstanceBuffer() { return this->zBillboardInstanceBuffer; }  


		void AddBillboard(const Billboard* const billboard);
		/*	Sorts, creates instance groups and updates the instance buffer.	*/
		void PreRender();

		/*	Resets zNrOfBillboards to allow overwriting of old data.	*/
		void PostRender() { this->zNrOfBillboards = 0; }
		

};