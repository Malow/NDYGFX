#include "InstancingHelper.h"
#include <algorithm>

//GLOBAL
bool SortBillboardData(BillboardData billboardLeft, BillboardData billboardRight)
{
	return billboardLeft.s_SRV > billboardRight.s_SRV;
}
bool SortMeshData(MeshData meshLeft, MeshData meshRight)
{
	return meshLeft.s_MeshStripsResource > meshRight.s_MeshStripsResource;
}



//PRIVATE
void InstancingHelper::ExpandBillboardDataAndBuffer()
{
	this->zBillboardDataCapacity *= 2;

	//Resize vector holding billboard data
	this->zBillboardData.resize(this->zBillboardDataCapacity);

	//Resize(recreate) instance buffer
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(Vertex) * this->zBillboardDataCapacity;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	//Create new, temporary buffer with increased size.
	ID3D11Buffer* temporaryNewBuffer = NULL;
	HRESULT hr = this->g_Device->CreateBuffer(&vbd, 0, &temporaryNewBuffer);
	if(FAILED(hr))
	{
		MaloW::Debug("ERROR: InstancingHelper: ExpandBillboardDataAndBuffer(): Failed to create buffer for instance billboard.");
	}
						
	//Copy over data from the old buffer to the new buffer.
	D3D11_MAPPED_SUBRESOURCE mappedSubResourceNew;
	D3D11_MAPPED_SUBRESOURCE mappedSubResourceOld;
	this->g_DeviceContext->Map(temporaryNewBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResourceNew);
	this->g_DeviceContext->Map(this->zBillboardInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResourceOld);
	unsigned int oldSize = this->zBillboardDataCapacity / 2; //**TILLMAN - annat sätt? bör finnas i mappedsubres?
	Vertex* dataViewNew = reinterpret_cast<Vertex*>(mappedSubResourceNew.pData);
	Vertex* dataViewOld = reinterpret_cast<Vertex*>(mappedSubResourceOld.pData);
	for(unsigned int i = 0; i < oldSize; ++i)
	{
		dataViewNew[i] = dataViewOld[i];
	}
	this->g_DeviceContext->Unmap(this->zBillboardInstanceBuffer, 0);
	this->g_DeviceContext->Unmap(temporaryNewBuffer, 0);
					
						
	//Release old buffer.
	this->zBillboardInstanceBuffer->Release();
	//Set old buffer to point to the new one.
	this->zBillboardInstanceBuffer = temporaryNewBuffer;
	//The temporary pointer is no longer needed, so set to NULL.
	temporaryNewBuffer = NULL;

	MaloW::Debug("INFO: InstancingHelper: ExpandBillboardDataAndBuffer(): Resizing billboard data and buffer. Number of billboards: '" + MaloW::convertNrToString(this->zNrOfBillboards) + "'."
		+ "New capacity: '" + MaloW::convertNrToString(this->zBillboardDataCapacity) + "'."
		);
}
void InstancingHelper::ExpandMeshDataAndBuffer()
{
	this->zMeshDataCapacity *= 2;
	//Resize vector holding billboard data
	this->zMeshData.resize(this->zBillboardDataCapacity);



	//Resize(recreate) instance buffer
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(MeshData::InstancedDataStruct) * this->zMeshDataCapacity;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	//Create new, temporary buffer with increased size.
	ID3D11Buffer* temporaryNewBuffer = NULL;
	HRESULT hr = this->g_Device->CreateBuffer(&vbd, 0, &temporaryNewBuffer);
	if(FAILED(hr))
	{
		MaloW::Debug("ERROR: InstancingHelper: ExpandMeshDataAndBuffer(): Failed to create buffer for instance meshes.");
	}
						
	//Copy over data from the old buffer to the new buffer.
	D3D11_MAPPED_SUBRESOURCE mappedSubResourceNew;
	D3D11_MAPPED_SUBRESOURCE mappedSubResourceOld;
	this->g_DeviceContext->Map(temporaryNewBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResourceNew);
	this->g_DeviceContext->Map(this->zMeshInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResourceOld);
	unsigned int oldSize = this->zMeshDataCapacity / 2; 
	Vertex* dataViewNew = reinterpret_cast<Vertex*>(mappedSubResourceNew.pData);
	Vertex* dataViewOld = reinterpret_cast<Vertex*>(mappedSubResourceOld.pData);
	for(unsigned int i = 0; i < oldSize; ++i)
	{
		dataViewNew[i] = dataViewOld[i];
	}
	this->g_DeviceContext->Unmap(this->zMeshInstanceBuffer, 0);
	this->g_DeviceContext->Unmap(temporaryNewBuffer, 0);
					
						
	//Release old buffer.
	this->zMeshInstanceBuffer->Release();
	//Set old buffer to point to the new one.
	this->zMeshInstanceBuffer = temporaryNewBuffer;
	//The temporary pointer is no longer needed, so set to NULL.
	temporaryNewBuffer = NULL;

	MaloW::Debug("INFO: InstancingHelper: ExpandMeshDataAndBuffer(): Resizing mesh data and buffer. Number of meshes: '" + MaloW::convertNrToString(this->zNrOfMeshes) + "'."
		+ "New capacity: '" + MaloW::convertNrToString(this->zMeshDataCapacity) + "'."
		);

}

//PUBLIC
InstancingHelper::InstancingHelper()
{
	//BILLBOARDS
	//Counters
	this->zNrOfBillboards = 0; 
	this->zNrOfBillboardGroups = 0;
	this->zBillboardDataCapacity = 200;
	this->zBillboardGroupCapacity = 10;
	//Instance data
	this->zBillboardData.resize(this->zBillboardDataCapacity);
	//Instance group 
	this->zBillboardGroups.resize(this->zBillboardGroupCapacity);
	//Buffer
	this->zBillboardInstanceBuffer = NULL; 


	//MESHES
	//Counters
	this->zNrOfMeshes = 0; 
	this->zNrOfMeshGroups = 0;
	this->zMeshDataCapacity = 200; 
	this->zMeshGroupCapacity = 10;
	//Mesh data (with instance data)
	this->zMeshData.resize(this->zMeshDataCapacity);
	//Instance group
	this->zMeshGroups.resize(this->zMeshGroupCapacity);
	//Instance Buffer (containing all instanced data)
	this->zMeshInstanceBuffer = NULL; 
}
HRESULT InstancingHelper::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	this->g_Device = device;
	this->g_DeviceContext = deviceContext;

	//BILLBOARD
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(Vertex) * this->zBillboardDataCapacity; 
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	HRESULT hr = this->g_Device->CreateBuffer(&bufferDesc, 0, &this->zBillboardInstanceBuffer);
	if(FAILED(hr))
	{
		MaloW::Debug("ERROR: InstancingHelper: Init(): Failed to create billboard mesh buffer.");
	}
	
	
	//MESH
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(MeshData::InstancedDataStruct) * this->zMeshDataCapacity; 
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	hr = this->g_Device->CreateBuffer(&bufferDesc, 0, &this->zMeshInstanceBuffer);
	if(FAILED(hr))
	{
		MaloW::Debug("ERROR: InstancingHelper: Init(): Failed to create mesh instance buffer.");
	}

	return hr;
}
InstancingHelper::~InstancingHelper()
{
	if(this->zBillboardInstanceBuffer) this->zBillboardInstanceBuffer->Release(); this->zBillboardInstanceBuffer = NULL;
}

void InstancingHelper::AddBillboard( const Billboard* const billboard )
{
	this->zNrOfBillboards++;
	if(this->zNrOfBillboards >= this->zBillboardDataCapacity)
	{
		this->ExpandBillboardDataAndBuffer();
	}

	this->zBillboardData[this->zNrOfBillboards - 1] = 
		BillboardData(Vertex(	billboard->GetPositionD3DX(),
								billboard->GetSizeD3DX(), 
								D3DXVECTOR3(), //dummy **TILLMAN
								D3DXVECTOR3(billboard->GetColorD3DX())
							)
							, billboard->GetTextureResource()->GetSRVPointer());
	
}

void InstancingHelper::PreRenderBillboards()
{
	//Sort the data by shader resource view
	std::sort(this->zBillboardData.begin(), this->zBillboardData.end(), SortBillboardData);



	//Now that the data is sorted, create instance groups.
	if(this->zNrOfBillboards > 1)
	{
		ID3D11ShaderResourceView* tempSRV;
		//Reset instance counter
		this->zNrOfBillboardGroups = 0;
		int groupCounter = 1; //**
		//Add a NULL-pointer at the end as a separator. 
		this->zBillboardData[this->zNrOfBillboards++].s_SRV = NULL;
		//First group's start location/index is always 0.
		this->zBillboardGroups[0].s_StartLocation = 0;
		for(unsigned int i = 0; i < this->zNrOfBillboards - 1; i++)
		{
			//Save current element.
			tempSRV = this->zBillboardData[i].s_SRV;

			//groupCounter++; //**

			//As long as the next element is the same as the current element, increase groupCounter. 
			//(The number of instances in the current group).
			if(tempSRV == this->zBillboardData[i + 1].s_SRV)
			{
				groupCounter++;
			}
			//If the next element is NOT the same as the current element, we have found a new instance group.
			else
			{
				//Increase the number of instance groups.
				this->zNrOfBillboardGroups++;
				//Resize vector if necessary.
				if(this->zNrOfBillboardGroups >= this->zBillboardGroupCapacity)
				{
					this->zBillboardGroupCapacity *= 2;
					this->zBillboardGroups.resize(this->zBillboardGroupCapacity);
					MaloW::Debug("INFO: InstancingHelper: PreRenderBillboards(): Resizing instance group vector. Number of instance groups: '" + MaloW::convertNrToString(this->zNrOfBillboardGroups) + "'."
						+ "New capacity: '" + MaloW::convertNrToString(this->zBillboardGroupCapacity) + "'."
						);
				}

				//Wrap up current instance group.
				//Save groupCounter (size of group).
				this->zBillboardGroups[this->zNrOfBillboardGroups - 1].s_Size = groupCounter; 
				//Save shader resource view for instance group to use.
				this->zBillboardGroups[this->zNrOfBillboardGroups - 1].s_SRV = this->zBillboardData[i].s_SRV;

				//Start new instance group.
				//Set group counter to 1.
				groupCounter = 1; 
				//groupCounter = 0;  //**
				//Save index (startLocation) of the NEW group (+1 on indices).
				this->zBillboardGroups[this->zNrOfBillboardGroups].s_StartLocation = i + 1;
			}
		}
		//There is no need to wrap up last instance group since we put a NULL pointer there.
	}
	//Handle the special case of 1 billboard.
	else if(this->zNrOfBillboards == 1)
	{
		//Increase the number of instance groups.
		this->zNrOfBillboardGroups = 1;
		//Set size of group (1)
		this->zBillboardGroups[0].s_Size = 1; 
		//Save shader resource view for instance group to use.
		this->zBillboardGroups[0].s_SRV = this->zBillboardData[0].s_SRV;

	}


	//Update buffer 
	D3D11_MAPPED_SUBRESOURCE mappedSubResource; 
	
	//Map to access data //**TILLMAN OPTIMERING: uppdatera endast de som lagts till/tagits bort(array med index(i))**
	this->g_DeviceContext->Map(this->zBillboardInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);

	Vertex* dataView = reinterpret_cast<Vertex*>(mappedSubResource.pData);
	//Copy over all vertex data
	for(UINT i = 0; i < this->zNrOfBillboards; ++i)
	{
		dataView[i] = this->zBillboardData[i].s_Vertex;
	}
	//Unmap so the GPU can have access
	this->g_DeviceContext->Unmap(this->zBillboardInstanceBuffer, 0);
}





void InstancingHelper::AddMesh(Mesh* mesh)
{
	//Expand vector and buffer holding mesh(instance data)
	this->zNrOfMeshes++;
	if(this->zNrOfMeshes >= this->zMeshDataCapacity)
	{
		this->ExpandMeshDataAndBuffer();
	}


	this->zMeshData[this->zNrOfMeshes - 1].s_MeshStripsResource = mesh->GetMeshStripsResourcePointer();
	this->zMeshData[this->zNrOfMeshes - 1].InstancedData.s_WorldMatrix = mesh->GetWorldMatrix();
	D3DXMATRIX worldInverseTranspose;
	D3DXMatrixIdentity(&worldInverseTranspose);
	D3DXMatrixInverse(&worldInverseTranspose, NULL, &mesh->GetWorldMatrix());
	D3DXMatrixTranspose(&worldInverseTranspose, &worldInverseTranspose);
	this->zMeshData[this->zNrOfMeshes - 1].InstancedData.s_WorldInverseTransposeMatrix = worldInverseTranspose;
}
void InstancingHelper::PreRenderMeshes()
{
	//Sort the data by MeshStripsResource
	std::sort(this->zMeshData.begin(), this->zMeshData.end(), SortMeshData);



	//Now that the data is sorted, create instance groups.
	if(this->zNrOfMeshes > 1)
	{
		MeshStripsResource* tempPtr = NULL;
		//Reset instance counter
		this->zNrOfMeshGroups = 0;
		int groupCounter = 1; //**
		//Add a NULL-pointer at the end as a separator. 
		this->zMeshData[this->zNrOfMeshes++].s_MeshStripsResource = NULL;
		//First group's start location/index is always 0.
		this->zMeshGroups[0].s_StartLocation = 0;

		for(unsigned int i = 0; i < this->zNrOfMeshes - 1; i++)
		{
			//Save current element.
			tempPtr = this->zMeshData[i].s_MeshStripsResource;

			//groupCounter++; //**

			//As long as the next element is the same as the current element, increase groupCounter. 
			//(The number of instances in the current group).
			if(tempPtr == this->zMeshData[i + 1].s_MeshStripsResource)
			{
				groupCounter++;
			}
			//If the next element is NOT the same as the current element, we have found a new instance group.
			else
			{
				//Increase the number of instance groups.
				this->zNrOfMeshGroups++;
				//Resize vector if necessary.
				if(this->zNrOfMeshGroups >= this->zMeshGroupCapacity)
				{
					this->zMeshGroupCapacity *= 2;
					this->zMeshGroups.resize(this->zMeshGroupCapacity);
					MaloW::Debug("INFO: InstancingHelper: PreRenderMeshes(): Resizing instance group vector. Number of instance groups: '" + MaloW::convertNrToString(this->zNrOfMeshGroups) + "'."
						+ "New capacity: '" + MaloW::convertNrToString(this->zMeshGroupCapacity) + "'."
						);
				}

				//Wrap up current instance group.
				//Save groupCounter (size of group).
				this->zMeshGroups[this->zNrOfMeshGroups - 1].s_Size = groupCounter; 
				//Save mesh strip resource for instance group to use.
				this->zMeshGroups[this->zNrOfMeshGroups - 1].s_MeshStripsResource = this->zMeshData[i].s_MeshStripsResource;

				//Start new instance group.
				//Set group counter to 1.
				groupCounter = 1; 
				//groupCounter = 0;  //**
				//Save index (startLocation) of the NEW group (+1 on indices).
				this->zMeshGroups[this->zNrOfMeshGroups].s_StartLocation = i + 1;
			}
		}
		//"Remove" the separator at the end
		this->zNrOfMeshes--;
		//There is no need to wrap up last instance group since we put a NULL pointer there.
	}
	//Handle the special case of 1 mesh.
	else if(this->zNrOfMeshes == 1)
	{
		//Increase the number of instance groups.
		this->zNrOfMeshGroups = 1;
		//Set size of group (1)
		this->zMeshGroups[0].s_Size = 1; 
		//Save shader resource view for instance group to use.
		this->zMeshGroups[0].s_MeshStripsResource = this->zMeshData[0].s_MeshStripsResource;
	}


	//Update buffer 
	D3D11_MAPPED_SUBRESOURCE mappedSubResource;
	//Map to access data //**TILLMAN OPTIMERING: uppdatera endast de som lagts till/tagits bort(array med index(i))**
	this->g_DeviceContext->Map(this->zMeshInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
	MeshData::InstancedDataStruct* dataView = reinterpret_cast<MeshData::InstancedDataStruct*>(mappedSubResource.pData);
	//Copy over all instance data
	for(UINT i = 0; i < this->zNrOfMeshes; ++i)
	{
		dataView[i] = this->zMeshData[i].InstancedData;
	}
	//Unmap so the GPU can have access
	this->g_DeviceContext->Unmap(this->zMeshInstanceBuffer, 0);
}