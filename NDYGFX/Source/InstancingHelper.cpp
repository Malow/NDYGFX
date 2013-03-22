#include "InstancingHelper.h"

#include "StaticMesh.h"
#include "AnimatedMesh.h"

#include <algorithm>

//GLOBAL
bool SortBillboardData(BillboardData &billboardLeft, BillboardData &billboardRight)
{
	return billboardLeft.s_SRV > billboardRight.s_SRV;
}
bool SortStripData(StripData &stripLeft, StripData &stripRight)
{
	return stripLeft.s_MeshStrip > stripRight.s_MeshStrip;
}
bool SortAnimatedStripData(AnimatedStripData &stripLeft, AnimatedStripData &stripRight)
{
	return stripLeft.s_MeshStripOne > stripRight.s_MeshStripOne; //**räcker med 1 jämförelse?**
}

//PRIVATE
void InstancingHelper::ExpandBillboardInstanceBuffer()
{
	unsigned int oldSize = this->zBillboardInstanceBufferSize;
	this->zBillboardInstanceBufferSize = this->zBillboardData.size() * 2;

	//Resize(recreate) instance buffer
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(VertexBillboard1) * this->zBillboardInstanceBufferSize;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	//Create new, temporary buffer with increased size.
	ID3D11Buffer* temporaryNewBuffer = NULL;
	HRESULT hr = this->g_Device->CreateBuffer(&vbd, 0, &temporaryNewBuffer);
	if(FAILED(hr))
	{
		MaloW::Debug("ERROR: InstancingHelper: ExpandBillboardBuffer(): Failed to create buffer for instance billboard. HRESULT error msg: '"
			+ MaloW::GetHRESULTErrorCodeString(hr) + "'. HRESULT #" + MaloW::convertNrToString(hr) + ".");

		return;
	}
						
	//Copy over data from the old buffer to the new buffer.
	D3D11_MAPPED_SUBRESOURCE mappedSubResourceNew;
	D3D11_MAPPED_SUBRESOURCE mappedSubResourceOld;
	this->g_DeviceContext->Map(temporaryNewBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResourceNew);
	this->g_DeviceContext->Map(this->zBillboardInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResourceOld);
	
	VertexBillboard1* dataViewNew = reinterpret_cast<VertexBillboard1*>(mappedSubResourceNew.pData);
	VertexBillboard1* dataViewOld = reinterpret_cast<VertexBillboard1*>(mappedSubResourceOld.pData);
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

	MaloW::Debug("INFO: InstancingHelper: ExpandBillboardBuffer(): Resizing billboard instance buffer. Number of billboards: '" + MaloW::convertNrToString(this->zBillboardData.size()) + "'."
		+ "New BUFFER size '" + MaloW::convertNrToString(this->zBillboardInstanceBufferSize) + "'."
		);
}
void InstancingHelper::ExpandStripInstanceBuffer()
{
	unsigned int oldSize = this->zStripInstanceBufferSize;
	this->zStripInstanceBufferSize = this->zStripData.size() * 2;


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

void InstancingHelper::ExpandAnimatedStripInstanceBuffer()
{
	unsigned int oldSize = this->zAnimatedStripInstanceBufferSize;
	this->zAnimatedStripInstanceBufferSize = this->zAnimatedStripData.size() * 2;

	//Resize(recreate) instance buffer
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(AnimatedStripData::AnimatedInstancedDataStruct) * this->zAnimatedStripInstanceBufferSize; 
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	//Create new, temporary buffer with increased size.
	ID3D11Buffer* temporaryNewBuffer = NULL;
	HRESULT hr = this->g_Device->CreateBuffer(&vbd, 0, &temporaryNewBuffer);
	if(FAILED(hr))
	{
		MaloW::Debug("ERROR: InstancingHelper: ExpandAnimatedStripDataAndBuffer(): Failed to create buffer for instance Strips.");
	}

	//Copy over data from the old buffer to the new buffer.
	D3D11_MAPPED_SUBRESOURCE mappedSubResourceNew;
	D3D11_MAPPED_SUBRESOURCE mappedSubResourceOld;
	this->g_DeviceContext->Map(temporaryNewBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResourceNew);
	this->g_DeviceContext->Map(this->zAnimatedStripInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResourceOld);

	AnimatedStripData::AnimatedInstancedDataStruct* dataViewNew = reinterpret_cast<AnimatedStripData::AnimatedInstancedDataStruct*>(mappedSubResourceNew.pData);
	AnimatedStripData::AnimatedInstancedDataStruct* dataViewOld = reinterpret_cast<AnimatedStripData::AnimatedInstancedDataStruct*>(mappedSubResourceOld.pData);
	for(unsigned int i = 0; i < oldSize; ++i)
	{
		dataViewNew[i] = dataViewOld[i];
	}
	this->g_DeviceContext->Unmap(this->zAnimatedStripInstanceBuffer, 0);
	this->g_DeviceContext->Unmap(temporaryNewBuffer, 0);


	//Release old buffer.
	this->zAnimatedStripInstanceBuffer->Release();
	//Set old buffer to point to the new one.
	this->zAnimatedStripInstanceBuffer = temporaryNewBuffer;
	//The temporary pointer is no longer needed, so set to NULL.
	temporaryNewBuffer = NULL;

	MaloW::Debug("INFO: InstancingHelper: ExpandStripDataAndBuffer(): Resizing Animated Strip instance buffer. Number of Aniamted Strips: '" + MaloW::convertNrToString(this->zAnimatedStripData.size()) + "'."
		+ "New BUFFER size: '" + MaloW::convertNrToString(this->zAnimatedStripInstanceBufferSize) + "'."
		);
}

//PUBLIC
InstancingHelper::InstancingHelper()
{
	//Billboards
	this->zRenderGrassFlag = true;
	this->zGrassFilePath = "";
	this->zBillboardInstanceBufferSize = 200;
	this->zBillboardInstanceBuffer = NULL; 

	//Strips
	this->zStripInstanceBufferSize = 200; 
	this->zStripInstanceBuffer = NULL; 

	//Animated strips
	this->zAnimatedStripInstanceBufferSize = 200; 
	this->zAnimatedStripInstanceBuffer = NULL; 
}
HRESULT InstancingHelper::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	this->g_Device = device;
	this->g_DeviceContext = deviceContext;

	//Billboard
	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(VertexBillboard1) * this->zBillboardInstanceBufferSize; 
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
	bufferDesc.ByteWidth = sizeof(StripData::InstancedDataStruct) * this->zStripInstanceBufferSize;
	hr = this->g_Device->CreateBuffer(&bufferDesc, 0, &this->zStripInstanceBuffer);
	if(FAILED(hr))
	{
		MaloW::Debug("ERROR: InstancingHelper: Init(): Failed to create Strip instance buffer.");
	}

	//Animated strips
	bufferDesc.ByteWidth = sizeof(AnimatedStripData::AnimatedInstancedDataStruct) * this->zAnimatedStripInstanceBufferSize;
	hr = this->g_Device->CreateBuffer(&bufferDesc, 0, &this->zAnimatedStripInstanceBuffer);
	if(FAILED(hr))
	{
		MaloW::Debug("ERROR: InstancingHelper: Init(): Failed to create Animated Strip instance buffer.");
	}

	return hr;
}
InstancingHelper::~InstancingHelper()
{
	if(this->zBillboardInstanceBuffer) this->zBillboardInstanceBuffer->Release(); this->zBillboardInstanceBuffer = NULL;
}



void InstancingHelper::AddBillboardsReference(const MaloW::Array<Billboard*>& billboards)
{
	this->zBillboardsReference = &billboards;
}
void InstancingHelper::AddBillboardCollectionsReference(const MaloW::Array<BillboardCollection*>& billboardCollections)
{
	this->zBillboardCollectionsReference = &billboardCollections;
}

void InstancingHelper::AddBillboard(Billboard* billboard)
{
	//Expand buffer if necessary
	if(this->zBillboardData.size() > this->zBillboardInstanceBufferSize)
	{
		this->ExpandBillboardInstanceBuffer();
	}

	//Set data
	BillboardData billboardData;
	billboardData.s_Vertex = billboard->GetVertex();
	if(billboard->GetTextureResource() != NULL)
	{
		billboardData.s_SRV = billboard->GetTextureResource()->GetSRVPointer();		
	}
	else
	{
		billboardData.s_SRV = NULL;		
	}	

	//Add billboard data
	this->zBillboardData.push_back(billboardData);
	
}
void InstancingHelper::AddBillboard( Mesh* meshWithBillboard )
{
	//Expand buffer if necessary
	if(this->zBillboardData.size() > this->zBillboardInstanceBufferSize)
	{
		this->ExpandBillboardInstanceBuffer();
	}

	//Calculate billboard data (sometimes it has not been precalculated)
	//Calculate billboard position(this needs to be updated as the mesh position changes).(don't forget to include the scale).
	float halfHeightScaled = meshWithBillboard->GetHeight() * 0.5f * meshWithBillboard->GetScaling().y; //(yOffset)
	D3DXVECTOR3 billboardPos = meshWithBillboard->GetPosition();
	billboardPos.y += halfHeightScaled;
	//Calculate the size using Pythagoras theorem (don't forget to include the scale).
	//Note that this returns the half of the half size, so multiply by 4.
	float billboardSize = sqrtf(powf(halfHeightScaled, 2.0f) * 0.5f) * 4.0f;
	BillboardData billboardData;
	D3DXVECTOR3 billboardColor = meshWithBillboard->GetBillboardGFX()->GetVertex().GetColor();
	
	//Set data
	billboardData.s_Vertex = VertexBillboard1(	billboardPos, 
												D3DXVECTOR2(billboardSize, billboardSize),
												billboardColor);
	/*billboardData.zVertex->posAndSizeX.x = billboardPos.x;
	billboardData.zVertex->posAndSizeX.x = billboardPos.y;
	billboardData.zVertex->posAndSizeX.x = billboardPos.z;*/
	if(meshWithBillboard->GetBillboardGFX()->GetTextureResource() != NULL)
	{
		billboardData.s_SRV = meshWithBillboard->GetBillboardGFX()->GetTextureResource()->GetSRVPointer();		
	}

	//Add billboard data
	this->zBillboardData.push_back(billboardData);
}

void InstancingHelper::PreRenderBillboards(bool shadowmap)
{
	//Add (standalone) billboards
	if(shadowmap)
	{
		for(unsigned int i = 0; i < this->zBillboardsReference->size(); ++i)
		{
			Billboard* billboard = this->zBillboardsReference->get(i);
			//Check if it shall be rendered
			if(billboard->GetRenderShadowFlag())
			{
				//Check if it has been culled
				if(!billboard->IsShadowCulled())
				{
					this->AddBillboard(billboard);
				}
			}
		}
	}
	else
	{
		for(unsigned int i = 0; i < this->zBillboardsReference->size(); ++i)
		{
			Billboard* billboard = this->zBillboardsReference->get(i);
			//Check if it has been culled
			if(!billboard->IsCameraCulled()) 
			{
				this->AddBillboard(billboard);
			}
		}
	}
	


	//Sort the data by shader resource view
	std::sort(this->zBillboardData.begin(), this->zBillboardData.end(), SortBillboardData);



	//Now that the data is sorted, create instance groups.
	//Clear any old groups
	this->zBillboardGroups.clear();
	if(this->zBillboardData.size() > 1)
	{
		ID3D11ShaderResourceView* tempSRV;

		int groupCounter = 1; 
		unsigned int nrOfGroups = 0;
		//Add a seperator at the end of billboard data.
		BillboardData seperator;
		seperator.s_SRV = (ID3D11ShaderResourceView*)0xFFFFFFFF;
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


	//Add billboard collections
	if(shadowmap) //If for shadow map...
	{
		for(unsigned int i = 0; i < this->zBillboardCollectionsReference->size(); ++i)
		{
			BillboardCollection* billboardCollection = this->zBillboardCollectionsReference->get(i);
			//... check if the billboard collection should cast a shadow.
			if(billboardCollection->GetRenderShadowFlag())
			{
				//Add to groups if it hasn't been culled
				if(!billboardCollection->IsShadowCulled())
				{
					unsigned int prevGroupStart = 0;
					unsigned int prevGroupSize = 0;
					//If there's a group, start where it ends.
					if(this->zBillboardGroups.size() > 0)
					{
						prevGroupStart = this->zBillboardGroups[this->zBillboardGroups.size() - 1].s_StartLocation;
						prevGroupSize = this->zBillboardGroups[this->zBillboardGroups.size() - 1].s_Size;
					}
					BillboardGroup newBBGroup;
					if(billboardCollection->GetTextureResource() != NULL)
					{
						newBBGroup = BillboardGroup(billboardCollection->GetNrOfVertices(), prevGroupStart + prevGroupSize, 
													billboardCollection->GetTextureResource()->GetSRVPointer(),
													billboardCollection->GetCullNearDistance(), 
													billboardCollection->GetCullFarDistance());
					}
					else
					{
						newBBGroup = BillboardGroup(billboardCollection->GetNrOfVertices(), prevGroupStart + prevGroupSize, 
													NULL, 
													billboardCollection->GetCullNearDistance(), 
													billboardCollection->GetCullFarDistance());
					}
					this->zBillboardGroups.push_back(newBBGroup);

					//Add billboard data
					this->zBillboardData.insert(
						this->zBillboardData.end(), 
						billboardCollection->GetBillboardData().cbegin(),
						billboardCollection->GetBillboardData().cend());

					//Expand buffer if necessary.
					if(this->zBillboardData.size() > this->zBillboardInstanceBufferSize)
					{
						this->ExpandBillboardInstanceBuffer();
					}
				}
			}
		}
	}
	else
	{
		for(unsigned int i = 0; i < this->zBillboardCollectionsReference->size(); ++i)
		{
			BillboardCollection* billboardCollection = this->zBillboardCollectionsReference->get(i);
			//Check if billboardCollection is grass and if it should be added.
			bool add = true;
			if(!this->zRenderGrassFlag)
			{
				if(std::strcmp(billboardCollection->GetTextureResource()->GetName().c_str(), this->zGrassFilePath.c_str()) == 0)
				{
					add = false;
				}
			}

			//Add to groups if it hasn't been culled.
			if(!billboardCollection->IsCameraCulled())
			{
				if(add)
				{
					unsigned int prevGroupStart = 0;
					unsigned int prevGroupSize = 0;
					//If there's a group, start where it ends.
					if(this->zBillboardGroups.size() > 0)
					{
						prevGroupStart = this->zBillboardGroups[this->zBillboardGroups.size() - 1].s_StartLocation;
						prevGroupSize = this->zBillboardGroups[this->zBillboardGroups.size() - 1].s_Size;
					}
					BillboardGroup newBBGroup;
					if(billboardCollection->GetTextureResource() != NULL)
					{
						newBBGroup = BillboardGroup(billboardCollection->GetNrOfVertices(), prevGroupStart + prevGroupSize, 
													billboardCollection->GetTextureResource()->GetSRVPointer(),
													billboardCollection->GetCullNearDistance(), 
													billboardCollection->GetCullFarDistance());
					}
					else
					{
						newBBGroup = BillboardGroup(billboardCollection->GetNrOfVertices(), prevGroupStart + prevGroupSize,
													NULL, 
													billboardCollection->GetCullNearDistance(), 
													billboardCollection->GetCullFarDistance());
					}
					this->zBillboardGroups.push_back(newBBGroup);
			
					//Add billboard data
					this->zBillboardData.insert(
						this->zBillboardData.end(), 
						billboardCollection->GetBillboardData().cbegin(),
						billboardCollection->GetBillboardData().cend());

					//Expand buffer if necessary.
					if(this->zBillboardData.size() > this->zBillboardInstanceBufferSize)
					{
						this->ExpandBillboardInstanceBuffer();
					}
				}
			}
		}
	}
	


	//Update buffer 
	D3D11_MAPPED_SUBRESOURCE mappedSubResource;
	//Map to access data 
	this->g_DeviceContext->Map(this->zBillboardInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
	VertexBillboard1* dataView = reinterpret_cast<VertexBillboard1*>(mappedSubResource.pData);
	//Copy over all vertex data
	for(UINT i = 0; i < this->zBillboardData.size(); ++i)
	{
		dataView[i] = this->zBillboardData[i].s_Vertex;
	}
	//Unmap so the GPU can have access
	this->g_DeviceContext->Unmap(this->zBillboardInstanceBuffer, 0);
}


void InstancingHelper::AddStaticMesh(StaticMesh* staticMesh)
{
	//Expand buffer if necessary
	if(this->zStripData.size() > this->zStripInstanceBufferSize)
	{
		this->ExpandStripInstanceBuffer();
	}
	else
	{
		//Mesh & instance data
		/*D3DXMATRIX worldInverseTranspose;
		D3DXMatrixIdentity(&worldInverseTranspose);
		D3DXMatrixInverse(&worldInverseTranspose, NULL, &staticMesh->GetWorldMatrix());
		D3DXMatrixTranspose(&worldInverseTranspose, &worldInverseTranspose);*/

		//Add/save strips data
		for(unsigned int i = 0; i < staticMesh->GetStrips()->size(); ++i)
		{
			StripData stripData;

			stripData.InstancedData.s_WorldMatrix = staticMesh->GetWorldMatrix();
			//stripData.InstancedData.s_WorldInverseTransposeMatrix = worldInverseTranspose;
			stripData.s_MeshStrip = staticMesh->GetMeshStripsResourcePointer()->GetMeshStripsPointer()->get(i);

			this->zStripData.push_back(stripData);
		}
	}
}
void InstancingHelper::PreRenderStrips()
{
	//Sort the data by meshStrip
	std::sort(this->zStripData.begin(), this->zStripData.end(), SortStripData);


	//Now that the data is sorted, create instance groups.
	//Clear any old groups
	this->zStripGroups.clear();
	if(this->zStripData.size() > 1)
	{
		MeshStrip* tempPtr = NULL;

		int groupCounter = 1; //**
		unsigned int nrOfGroups = 0;

		//Add a seperator.
		StripData seperator;
		seperator.s_MeshStrip = (MeshStrip*)0xFFFFFFFF;
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

void InstancingHelper::AddAnimatedMesh(AnimatedMesh* animatedMesh, float timer)
{
	//Expand buffer if necessary
	if(this->zAnimatedStripData.size() > this->zAnimatedStripInstanceBufferSize)
	{
		this->ExpandAnimatedStripInstanceBuffer();
	}
	else
	{
		//Mesh & instance data
		/*D3DXMATRIX worldInverseTranspose;
		D3DXMatrixIdentity(&worldInverseTranspose);
		D3DXMatrixInverse(&worldInverseTranspose, NULL, &animatedMesh->GetWorldMatrix());
		D3DXMatrixTranspose(&worldInverseTranspose, &worldInverseTranspose);*/

		KeyFrame* one = NULL;
		KeyFrame* two = NULL;
		float interpolationValue = 0.0f;
		animatedMesh->GetCurrentKeyFrames(one, two, interpolationValue, timer * 1000.0f); //timer is in seconds.
		MaloW::Array<MeshStrip*>* stripsOne = one->meshStripsResource->GetMeshStripsPointer();
		MaloW::Array<MeshStrip*>* stripsTwo = two->meshStripsResource->GetMeshStripsPointer();

		//Add/save strips data
		for(unsigned int i = 0; i < stripsOne->size(); ++i) //(stripsOne and stripsTwo are of the same size)
		{
			//Add strip data
			AnimatedStripData animatedStripData;
			animatedStripData.InstancedData.s_WorldMatrix = animatedMesh->GetWorldMatrix();
			animatedStripData.InstancedData.s_WorldMatrix._44 = interpolationValue;
			animatedStripData.s_MeshStripOne = stripsOne->get(i);
			animatedStripData.s_MeshStripTwo = stripsTwo->get(i);
			
			this->zAnimatedStripData.push_back(animatedStripData);
		}
	}
}
void InstancingHelper::PreRenderAnimatedStrips()
{
	//Sort the data by meshStrip
	std::sort(this->zAnimatedStripData.begin(), this->zAnimatedStripData.end(), SortAnimatedStripData);



	//Now that the data is sorted, create instance groups.
	//Clear any old groups
	this->zAnimatedStripGroups.clear();
	if(this->zAnimatedStripData.size() > 1)
	{
		MeshStrip* tempPtr = NULL;

		int groupCounter = 1; //**
		unsigned int nrOfGroups = 0;

		//Add a seperator.
		AnimatedStripData seperator;
		seperator.s_MeshStripOne = (MeshStrip*)0xFFFFFFFF; 
		this->zAnimatedStripData.push_back(seperator);

		//First group's start location/index is always 0.
		AnimatedStripGroup firstAnimatedStripGroup;
		firstAnimatedStripGroup.s_StartLocation = 0;
		this->zAnimatedStripGroups.push_back(firstAnimatedStripGroup);

		for(unsigned int i = 0; i < this->zAnimatedStripData.size() - 1; i++)
		{
			//Save current element. (Checking the first strip is enough).
			tempPtr = this->zAnimatedStripData[i].s_MeshStripOne;

			//groupCounter++; //**

			//As long as the next element is the same as the current element, increase groupCounter. 
			//(The number of instances in the current group).
			if(tempPtr == this->zAnimatedStripData[i + 1].s_MeshStripOne)
			{
				groupCounter++;
			}
			//If the next element is NOT the same as the current element, we have found a new instance group.
			else
			{
				//Wrap up current instance group.
				nrOfGroups++;
				//Save groupCounter (size of group).
				this->zAnimatedStripGroups[nrOfGroups - 1].s_Size = groupCounter; 
				//Save the strips for instance group to use.
				this->zAnimatedStripGroups[nrOfGroups - 1].s_MeshStripOne = this->zAnimatedStripData[i].s_MeshStripOne;
				this->zAnimatedStripGroups[nrOfGroups - 1].s_MeshStripTwo = this->zAnimatedStripData[i].s_MeshStripTwo;

				//Start new instance group.
				//Set group counter to 1.
				groupCounter = 1; 
				//groupCounter = 0;  //**
				AnimatedStripGroup newAnimatedStripGroup;
				//Save index (startLocation) of the NEW group (+1 on indices).
				newAnimatedStripGroup.s_StartLocation = i + 1;
				this->zAnimatedStripGroups.push_back(newAnimatedStripGroup);
			}
		}
		//Remove seperator and the group it's in
		this->zAnimatedStripData.pop_back();
		this->zAnimatedStripGroups.pop_back();
	}
	//Handle the special case of 1 Strip.
	else if(this->zAnimatedStripData.size() == 1)
	{
		AnimatedStripGroup singleAnimatedStrip;

		singleAnimatedStrip.s_StartLocation = 0;
		singleAnimatedStrip.s_Size = 1;
		singleAnimatedStrip.s_MeshStripOne = this->zAnimatedStripData[0].s_MeshStripOne;
		singleAnimatedStrip.s_MeshStripTwo = this->zAnimatedStripData[0].s_MeshStripTwo;

		this->zAnimatedStripGroups.push_back(singleAnimatedStrip);
	}
	


	//Update buffer 
	D3D11_MAPPED_SUBRESOURCE mappedSubResource;
	//Map to access data
	this->g_DeviceContext->Map(this->zAnimatedStripInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
	AnimatedStripData::AnimatedInstancedDataStruct* dataView = reinterpret_cast<AnimatedStripData::AnimatedInstancedDataStruct*>(mappedSubResource.pData);
	//Copy over all instance data
	for(UINT i = 0; i < this->zAnimatedStripData.size(); ++i)
	{
		dataView[i] = this->zAnimatedStripData[i].InstancedData;
	}
	//Unmap so the GPU can have access
	this->g_DeviceContext->Unmap(this->zAnimatedStripInstanceBuffer, 0);
}
