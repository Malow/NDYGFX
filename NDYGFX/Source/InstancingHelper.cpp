#include "InstancingHelper.h"
#include <algorithm>

//GLOBAL
bool SortBillboardData(BillboardData billboardLeft, BillboardData billboardRight)
{
	return billboardLeft.s_SRV > billboardRight.s_SRV;
}
/*bool SortMeshData(MeshData meshLeft, MeshData meshRight)
{
	return meshLeft.s_MeshStripsResource > meshRight.s_MeshStripsResource;
}*/
bool SortStripData(StripData stripLeft, StripData stripRight)
{
	return stripLeft.s_MeshStrip > stripRight.s_MeshStrip;
}


//PRIVATE
void InstancingHelper::ExpandBillboardInstanceBuffer()
{
	unsigned int oldSize = this->zBillboardInstanceBufferSize;
	this->zBillboardInstanceBufferSize *= 2;

	//Resize(recreate) instance buffer
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(Vertex) * this->zBillboardInstanceBufferSize;
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

	MaloW::Debug("INFO: InstancingHelper: ExpandBillboardDataAndBuffer(): Resizing billboard instance buffer. Number of billboards: '" + MaloW::convertNrToString(this->zBillboardData.size()) + "'."
		+ "New BUFFER size '" + MaloW::convertNrToString(this->zBillboardInstanceBufferSize) + "'."
		);
}
void InstancingHelper::ExpandStripInstanceBuffer()
{
	unsigned int oldSize = this->zStripInstanceBufferSize;
	this->zStripInstanceBufferSize *= 2;


	//Resize(recreate) instance buffer
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(StripData::InstancedDataStruct) * this->zStripInstanceBufferSize; 
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	//Create new, temporary buffer with increased size.
	ID3D11Buffer* temporaryNewBuffer = NULL;
	HRESULT hr = this->g_Device->CreateBuffer(&vbd, 0, &temporaryNewBuffer);
	if(FAILED(hr))
	{
		MaloW::Debug("ERROR: InstancingHelper: ExpandStripDataAndBuffer(): Failed to create buffer for instance Strips.");
	}
						
	//Copy over data from the old buffer to the new buffer.
	D3D11_MAPPED_SUBRESOURCE mappedSubResourceNew;
	D3D11_MAPPED_SUBRESOURCE mappedSubResourceOld;
	this->g_DeviceContext->Map(temporaryNewBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResourceNew);
	this->g_DeviceContext->Map(this->zStripInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResourceOld);
	
	StripData::InstancedDataStruct* dataViewNew = reinterpret_cast<StripData::InstancedDataStruct*>(mappedSubResourceNew.pData);
	StripData::InstancedDataStruct* dataViewOld = reinterpret_cast<StripData::InstancedDataStruct*>(mappedSubResourceOld.pData);
	for(unsigned int i = 0; i < oldSize; ++i)
	{
		dataViewNew[i] = dataViewOld[i];
	}
	this->g_DeviceContext->Unmap(this->zStripInstanceBuffer, 0);
	this->g_DeviceContext->Unmap(temporaryNewBuffer, 0);
					
						
	//Release old buffer.
	this->zStripInstanceBuffer->Release();
	//Set old buffer to point to the new one.
	this->zStripInstanceBuffer = temporaryNewBuffer;
	//The temporary pointer is no longer needed, so set to NULL.
	temporaryNewBuffer = NULL;

	MaloW::Debug("INFO: InstancingHelper: ExpandStripDataAndBuffer(): Resizing Strip instance buffer. Number of Strips: '" + MaloW::convertNrToString(this->zStripData.size()) + "'."
		+ "New BUFFER size: '" + MaloW::convertNrToString(this->zStripInstanceBufferSize) + "'."
		);
}

//PUBLIC
InstancingHelper::InstancingHelper()
{
	//BILLBOARDS
	this->zBillboardInstanceBufferSize = 200;
	this->zBillboardInstanceBuffer = NULL; 


	//STRIPS
	this->zStripInstanceBufferSize = 200; 
	this->zStripInstanceBuffer = NULL; 
}
HRESULT InstancingHelper::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	this->g_Device = device;
	this->g_DeviceContext = deviceContext;

	//BILLBOARD
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(Vertex) * this->zBillboardInstanceBufferSize; 
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	HRESULT hr = this->g_Device->CreateBuffer(&bufferDesc, 0, &this->zBillboardInstanceBuffer);
	if(FAILED(hr))
	{
		MaloW::Debug("ERROR: InstancingHelper: Init(): Failed to create billboard Strip buffer.");
	}
	
	
	//Strip
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(StripData::InstancedDataStruct) * this->zStripInstanceBufferSize;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	hr = this->g_Device->CreateBuffer(&bufferDesc, 0, &this->zStripInstanceBuffer);
	if(FAILED(hr))
	{
		MaloW::Debug("ERROR: InstancingHelper: Init(): Failed to create Strip instance buffer.");
	}

	twice = false;
	test = false;

	return hr;
}
InstancingHelper::~InstancingHelper()
{
	if(this->zBillboardInstanceBuffer) this->zBillboardInstanceBuffer->Release(); this->zBillboardInstanceBuffer = NULL;
}

void InstancingHelper::AddBillboard( Mesh* meshWithBillboard )
{
	//Expand buffer if necessary
	if(this->zBillboardData.size() >= this->zBillboardInstanceBufferSize)
	{
		this->ExpandBillboardInstanceBuffer();
	}

	//Calculate billboard data (sometimes it has not been precalculated)**Tillman
	//Calculate billboard position(this needs to be updated as the mesh position changes).(don't forget to include the scale).
	float halfHeightScaled = meshWithBillboard->GetHeight() * 0.5f * meshWithBillboard->GetScaling().y; //(yOffset)
	D3DXVECTOR3 billboardPos = meshWithBillboard->GetPosition();
	billboardPos.y += halfHeightScaled;
	//Calculate the size using Pythagoras theorem (don't forget to include the scale).
	//Note that this returns the half of the half size, so multiply by 4.
	float billboardSize = sqrtf(powf(halfHeightScaled, 2.0f) * 0.5f) * 4.0f;
	BillboardData billboardData;
	D3DXVECTOR3 billboardColor = D3DXVECTOR3(meshWithBillboard->GetBillboardGFX()->GetColorD3DX().x, meshWithBillboard->GetBillboardGFX()->GetColorD3DX().y, meshWithBillboard->GetBillboardGFX()->GetColorD3DX().z);
	//Set data
	billboardData.s_Vertex = Vertex(billboardPos,
									D3DXVECTOR2(billboardSize, billboardSize),
									D3DXVECTOR3(), //dummy **TILLMAN
									billboardColor);//dummy? **TILLMAN
	billboardData.s_SRV = meshWithBillboard->GetBillboardGFX()->GetTextureResource()->GetSRVPointer();				

	//Add billboard data
	this->zBillboardData.push_back(billboardData);
}

void InstancingHelper::PreRenderBillboards()
{
	//Sort the data by shader resource view
	std::sort(this->zBillboardData.begin(), this->zBillboardData.end(), SortBillboardData);



	//Now that the data is sorted, create instance groups.
	//Clear any old groups
	this->zBillboardGroups.clear();
	if(this->zBillboardData.size() > 1)
	{
		ID3D11ShaderResourceView* tempSRV;

		int groupCounter = 1; //**TILLMAN
		unsigned int nrOfGroups = 0;
		//Add a seperator with an SRV pointing NULL at the end of billboard data.
		BillboardData seperator;
		seperator.s_SRV = NULL;
		this->zBillboardData.push_back(seperator);

		//First group's start location/index is always 0.
		BillboardGroup firstBBGroup;
		firstBBGroup.s_StartLocation = 0;
		this->zBillboardGroups.push_back(firstBBGroup);
		
		for(unsigned int i = 0; i < this->zBillboardData.size() - 1; i++)
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
				//Wrap up current instance group.
				nrOfGroups++;
				//Save groupCounter (size of group).
				this->zBillboardGroups[nrOfGroups - 1].s_Size = groupCounter; 
				//Save shader resource view for instance group to use.
				this->zBillboardGroups[nrOfGroups - 1].s_SRV = this->zBillboardData[i].s_SRV;

				//Start new instance group.
				//Set group counter to 1.
				groupCounter = 1; 
				//groupCounter = 0;  //**
				//Save index (startLocation) of the NEW group (+1 on indices).
				BillboardGroup newBBGroup;
				newBBGroup.s_StartLocation = i + 1;
				this->zBillboardGroups.push_back(newBBGroup);
			}
		}
		//Remove seperator and last, invalid group
		this->zBillboardData.pop_back();
		this->zBillboardGroups.pop_back();
	}
	//Handle the special case of 1 billboard.
	else if(this->zBillboardData.size() == 1)
	{
		BillboardGroup singleBB;
		singleBB.s_StartLocation = 0;
		singleBB.s_Size = 1;
		singleBB.s_SRV = this->zBillboardData[0].s_SRV;
		this->zBillboardGroups.push_back(singleBB);
	}



	//Update buffer 
	D3D11_MAPPED_SUBRESOURCE mappedSubResource; 
	//Map to access data 
	this->g_DeviceContext->Map(this->zBillboardInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
	Vertex* dataView = reinterpret_cast<Vertex*>(mappedSubResource.pData);
	//Copy over all vertex data
	for(UINT i = 0; i < this->zBillboardData.size(); ++i)
	{
		dataView[i] = this->zBillboardData[i].s_Vertex;
	}
	//Unmap so the GPU can have access
	this->g_DeviceContext->Unmap(this->zBillboardInstanceBuffer, 0);
}


void InstancingHelper::AddMesh(Mesh* mesh)
{
	if(!twice)
	{

	
	//if(!test)
	{

	
	//Expand buffer if necessary
	if(this->zStripData.size() >= this->zStripInstanceBufferSize)
	{
		this->ExpandStripInstanceBuffer();
	}
	else
	{
		//Mesh & instance data
		D3DXMATRIX worldInverseTranspose;
		D3DXMatrixIdentity(&worldInverseTranspose);
		D3DXMatrixInverse(&worldInverseTranspose, NULL, &mesh->GetWorldMatrix());
		D3DXMatrixTranspose(&worldInverseTranspose, &worldInverseTranspose);

		//Add/save strips data
		for(unsigned int i = 0; i < mesh->GetStrips()->size(); ++i)
		{
			StripData StripData;

			//StripData.InstancedData.s_WorldMatrix = mesh->GetWorldMatrix();
			//StripData.InstancedData.x 
			//StripData.InstancedData.s_WorldInverseTransposeMatrix = worldInverseTranspose;
			//D3DXMATRIX leWhy = mesh->GetWorldMatrix();
			//StripData.InstancedData.x = D3DXVECTOR4(leWhy._11, leWhy._12, leWhy._13, leWhy._14);
			//StripData.InstancedData.y = D3DXVECTOR4(leWhy._21, leWhy._22, leWhy._23, leWhy._24);
			//StripData.InstancedData.z = D3DXVECTOR4(leWhy._31, leWhy._32, leWhy._33, leWhy._34);
			//StripData.InstancedData.w = D3DXVECTOR4(leWhy._41, leWhy._42, leWhy._43, leWhy._44);
				


			StripData.s_MeshStrip = mesh->GetMeshStripsResourcePointer()->GetMeshStripsPointer()->get(i);

			this->zStripData.push_back(StripData);
		}
	}
	if(this->zStripData.size() >= 1)
	{

		//twice = true; //**TILLMAN TEST
	}
	}
	}
}
void InstancingHelper::PreRenderStrips()
{
	twice = false;
	//Sort the data by meshStrip
	std::sort(this->zStripData.begin(), this->zStripData.end(), SortStripData);



	//Now that the data is sorted, create instance groups.
	//Clear any old groups
	/*this->zStripGroups.clear();
	if(this->zStripData.size() > 1)
	{
		MeshStrip* tempPtr = NULL;

		int groupCounter = 1; //**
		unsigned int nrOfGroups = 0;

		//Add a seperator with an SRV pointing to NULL.
		StripData seperator;
		seperator.s_MeshStrip = NULL;
		this->zStripData.push_back(seperator);

		//First group's start location/index is always 0.
		StripGroup firstStripGroup;
		firstStripGroup.s_StartLocation = 0;
		this->zStripGroups.push_back(firstStripGroup);

		for(unsigned int i = 0; i < this->zStripData.size() - 1; i++)
		{
			//Save current element.
			tempPtr = this->zStripData[i].s_MeshStrip;

			//groupCounter++; //**

			//As long as the next element is the same as the current element, increase groupCounter. 
			//(The number of instances in the current group).
			if(tempPtr == this->zStripData[i + 1].s_MeshStrip)
			{
				groupCounter++;
			}
			//If the next element is NOT the same as the current element, we have found a new instance group.
			else
			{
				//Wrap up current instance group.
				nrOfGroups++;
				//Save groupCounter (size of group).
				this->zStripGroups[nrOfGroups - 1].s_Size = groupCounter; 
				//Save the strip for instance group to use.
				this->zStripGroups[nrOfGroups - 1].s_MeshStrip = this->zStripData[i].s_MeshStrip;
				
				//Start new instance group.
				//Set group counter to 1.
				groupCounter = 1; 
				//groupCounter = 0;  //**
				StripGroup newStripGroup;
				//Save index (startLocation) of the NEW group (+1 on indices).
				newStripGroup.s_StartLocation = i + 1;
				this->zStripGroups.push_back(newStripGroup);
			}
		}
		//Remove seperator and the group it's in
		this->zStripData.pop_back();
		this->zStripGroups.pop_back();
	}
	//Handle the special case of 1 Strip.
	else if(this->zStripData.size() == 1)
	{
		StripGroup singleStrip;
		singleStrip.s_StartLocation = 0;
		singleStrip.s_Size = 1;
		singleStrip.s_MeshStrip = this->zStripData[0].s_MeshStrip;
		this->zStripGroups.push_back(singleStrip);
	}*/
	this->zStripGroups.clear(); //**TILLMAN TEST
	for(int testI = 0; testI < this->zStripData.size(); ++testI)
	{
		StripGroup testGroup;
		testGroup.s_StartLocation = testI;
		testGroup.s_Size = 1;
		testGroup.s_MeshStrip = this->zStripData[testI].s_MeshStrip;
		this->zStripGroups.push_back(testGroup);
	}

	//Update buffer 
	D3D11_MAPPED_SUBRESOURCE mappedSubResource;
	//Map to access data
	this->g_DeviceContext->Map(this->zStripInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
	StripData::InstancedDataStruct* dataView = reinterpret_cast<StripData::InstancedDataStruct*>(mappedSubResource.pData);
	//Copy over all instance data
	for(UINT i = 0; i < this->zStripData.size(); ++i)
	{
		dataView[i] = this->zStripData[i].InstancedData;
	}
	//Unmap so the GPU can have access
	this->g_DeviceContext->Unmap(this->zStripInstanceBuffer, 0);
}

