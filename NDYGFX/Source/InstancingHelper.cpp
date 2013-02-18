#include "InstancingHelper.h"

//PRIVATE
void InstancingHelper::ExpandBillboardDataAndBuffer()
{
	this->zBillboardDataCapacity *= 2;

	//Resize vector holding billboard data
	this->zBillboardData.resize(this->zBillboardDataCapacity);

	//Resize(recreate) buffer
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
		MaloW::Debug("ERROR: InstancingHelper: ExpandBillboards(): Failed to create buffer for instance billboard.");
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

	MaloW::Debug("INFO: InstancingHelper: ExpandBillboards(): Resizing billboard data and buffer. Number of billboards: '" + MaloW::convertNrToString(this->zNrOfBillboards) + "'."
		+ "New capacity: '" + MaloW::convertNrToString(this->zBillboardDataCapacity) + "'."
		);
}



//PUBLIC
InstancingHelper::InstancingHelper()
{
	//Counters
	this->zNrOfBillboards = 0; 
	this->zNrOfBillboardInstanceGroups = 0;
	this->zBillboardDataCapacity = 200;
	this->zBillboardInstanceGroupCapacity = 10;
	//Instance data
	this->zBillboardData.resize(this->zBillboardDataCapacity);
	//Instance group 
	this->zBillboardInstanceGroups.resize(this->zBillboardInstanceGroupCapacity);
	//Buffer
	this->zBillboardInstanceBuffer = NULL; 
}
HRESULT InstancingHelper::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	this->g_Device = device;
	this->g_DeviceContext = deviceContext;

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
		MaloW::Debug("ERROR: InstancingHelper: Init(): Failed to create buffer.");
	}

	return hr;
}
InstancingHelper::~InstancingHelper()
{
	if(this->zBillboardInstanceBuffer) this->zBillboardInstanceBuffer->Release(); this->zBillboardInstanceBuffer = NULL;
}

void InstancingHelper::AddBillboard( const Billboard* const billboard )
{
	if(billboard->GetTextureResource() != NULL)
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
}

void InstancingHelper::PreRender()
{
	//Sort the data by shader resource view
	int counter = 0;
	Vertex tempVertex; //**TILLMAN OPT sorting algorithm
	ID3D11ShaderResourceView* tempSRV;
	for(int i = 1; i < this->zNrOfBillboards; i++)
	{
		tempVertex = this->zBillboardData[i].s_Vertex;
		tempSRV = this->zBillboardData[i].s_SRV;

		counter = i - 1;

		while(this->zBillboardData[counter].s_SRV > tempSRV && counter >= 0)
		{
			this->zBillboardData[counter + 1] = this->zBillboardData[counter];
			//this->instanceSRVsBillboard[counter + 1] = this->instanceSRVsBillboard[counter];
			counter--;
			if(counter < 0)
			{
				break;
			}
		}

		this->zBillboardData[counter + 1].s_Vertex = tempVertex;
		this->zBillboardData[counter + 1].s_SRV = tempSRV;
	}



	//Now that the data is sorted, create instance groups.
	if(this->zNrOfBillboards > 1)
	{
		//Reset instance counter
		this->zNrOfBillboardInstanceGroups = 0;
		int groupCounter = 1; //**
		//Add a NULL-pointer at the end as a separator. 
		this->zBillboardData[this->zNrOfBillboards++].s_SRV = NULL;
		//First group's start location/index is always 0.
		this->zBillboardInstanceGroups[0].s_StartLocation = 0;
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
				this->zNrOfBillboardInstanceGroups++;
				//Resize vector if necessary.
				if(this->zNrOfBillboardInstanceGroups >= this->zBillboardInstanceGroupCapacity)
				{
					this->zBillboardInstanceGroupCapacity *= 2;
					this->zBillboardInstanceGroups.resize(this->zBillboardInstanceGroupCapacity);
					MaloW::Debug("INFO: InstancingHelper: PreRender(): Resizing instance group vector. Number of instance groups: '" + MaloW::convertNrToString(this->zNrOfBillboardInstanceGroups) + "'."
						+ "New capacity: '" + MaloW::convertNrToString(this->zBillboardInstanceGroupCapacity) + "'."
						);
				}

				//Wrap up current instance group.
				//Save groupCounter (size of group).
				this->zBillboardInstanceGroups[this->zNrOfBillboardInstanceGroups - 1].s_Size = groupCounter; 
				//Save shader resource view for instance group to use.
				this->zBillboardInstanceGroups[this->zNrOfBillboardInstanceGroups - 1].s_SRV = this->zBillboardData[i].s_SRV;

				//Start new instance group.
				//Set group counter to 1.
				groupCounter = 1; 
				//groupCounter = 0;  //**
				//Save index (startLocation) of the NEW group (+1 on indices).
				this->zBillboardInstanceGroups[this->zNrOfBillboardInstanceGroups].s_StartLocation = i + 1;
			}
		}
		//There is no need to wrap up last instance group since we put a NULL pointer there.
	}
	//Handle the special case of 1 billboard.
	else if(this->zNrOfBillboards == 1)
	{
		//Increase the number of instance groups.
		this->zNrOfBillboardInstanceGroups = 1;
		//Set size of group (1)
		this->zBillboardInstanceGroups[0].s_Size = 1; 
		//Save shader resource view for instance group to use.
		this->zBillboardInstanceGroups[0].s_SRV = this->zBillboardData[0].s_SRV;

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

