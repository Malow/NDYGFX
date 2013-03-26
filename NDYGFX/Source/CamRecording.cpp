//Written by Markus Tillman.

#include "CamRecording.h"

#include "iCamera.h"
#include "TCBSpline.h"
#include <string>
#include "MaloWFileDebug.h"

#define PI (3.14159265358979323846f)

//private
void CamRecording::DeletePreviousRecording()
{
	this->mCamAtSpline->Clear();
	this->mCamPosSpline->Clear();
}

//de/con-structors, init, other
CamRecording::CamRecording(int interval, bool seamLessPath)
{
	this->gCamera = NULL;

	//this->gDevice = NULL;
	//this->gDeviceContext = NULL;
	//this->gShader = NULL;

	this->mIsLoopingSeamless = false;
	this->mIsRecording = false;
	this->mHasRecorded = false;
	this->mIsPlaying = false;
	this->mInterval = interval;
	this->mPlayTime = 0.0f;
	this->mCurrentPlayTime = 0.0f;
	this->mPlaySpeed = 1.0f;
	this->mPathOffset = Vector3(0.0f, 0.0f, 0.0f);
	this->mCamPosSpline = new TCBSpline(seamLessPath);
	this->mCamAtSpline = new TCBSpline(seamLessPath);
	
	this->mNrOfVertices = 0;
	this->mVertices = NULL;
	//this->mVertexBuffer = NULL;
}
CamRecording::~CamRecording()
{
	if( this->mCamPosSpline ) delete this->mCamPosSpline, this->mCamPosSpline = 0;
	if( this->mCamAtSpline ) delete this->mCamAtSpline, this->mCamAtSpline = 0;

	if(this->mVertices)
	{
		for(int i = 0; i < this->mNrOfVertices; i++)
		{
			if( this->mVertices[i] ) delete this->mVertices[i], this->mVertices[i] = 0;
		}
		delete [] mVertices;
		this->mVertices=0;
	}
	//SAFE_RELEASE(this->mVertexBuffer) ;
}

void CamRecording::Init(iCamera* camera)//, ID3D11Device* device, ID3D11DeviceContext* deviceContext, Shader* shader)
{
	this->gCamera = camera;
	//this->gDevice = device;
	//this->gDeviceContext = deviceContext;
	//this->gShader = shader;
}

//get
bool CamRecording::IsLooping() const
{
	return this->mIsLoopingSeamless; 
}
bool CamRecording::IsRecording() const
{
	return this->mIsRecording;
}
bool CamRecording::HasRecorded() const
{
	return this->mHasRecorded;
}
bool CamRecording::IsPlaying() const
{
	return this->mIsPlaying;
}
int CamRecording::GetInterval() const
{
	return this->mInterval;
}
float CamRecording::GetPlayTime() const
{
	return this->mPlayTime;
}
float CamRecording::GetCurrentPlayTime() const
{
	return this->mCurrentPlayTime;
}
float CamRecording::GetPlaySpeed() const
{
	return this->mPlaySpeed;
}
Vector3 CamRecording::GetPathOffset() const
{
	return this->mPathOffset;
}

//set
void CamRecording::LoopSeamLess()
{
	this->mIsLoopingSeamless = true;
}
void CamRecording::StopLooping()
{
	this->mIsLoopingSeamless = false;
}
void CamRecording::SetInterval(int interval)
{
	this->mInterval = interval;
}
void CamRecording::SetPlaySpeed(float playSpeed)
{
	this->mPlaySpeed = playSpeed;
}
void CamRecording::SetPathOffset(Vector3 pathOffset)
{
	this->mPathOffset = pathOffset;
}

// other
void CamRecording::AddCameraWaypoint(Vector3 position, Vector3 lookAt)
{
	this->mCamPosSpline->AddControlPoint(position);
	this->mCamAtSpline->AddControlPoint(lookAt);
	this->mHasRecorded = true;
}

void CamRecording::Record(bool record)
{
	this->mIsRecording = record;
	if(record) //check if something has already been recorded and delete is if that's the case
	{
		if(this->mHasRecorded)
		{
			CamRecording::DeletePreviousRecording();
		}
	}
	else
	{
		//initialize the splines when recording is done
		bool success = false;
		success = this->mCamPosSpline->Init();
		if(success)
		{
			success = this->mCamAtSpline->Init();
		}
		if(success)
		{
			MaloW::Debug("CamRecording: Error: Failed to initialize spline(s)");
		}

		this->mHasRecorded = true;
	}
}
void CamRecording::Play()
{
	if(this->mHasRecorded) 
	{
		this->mIsPlaying = true;
		this->mPlayTime = (this->mCamPosSpline->GetNrOfControlPoints() - 1) * this->mInterval * 0.001f * this->mPlaySpeed;
	}
	else
	{
		MaloW::Debug("CamRecording: Warning: Tried to play when there's nothing to play.");
	}
}
void CamRecording::Save(const char* fileName)
{
	std::string pos(fileName);
	std::string at(fileName);
	pos += "_Pos";
	at += "_At";
	this->mCamPosSpline->WriteControlPointsToFile(pos.c_str());
	this->mCamAtSpline->WriteControlPointsToFile(at.c_str());
}
void CamRecording::Open(const char* fileName)
{
	std::string pos(fileName);
	std::string at(fileName);
	pos += "_Pos";
	at += "_At";
	if(this->mHasRecorded)
	{
		CamRecording::DeletePreviousRecording();
	}
	bool success = false;
	success = this->mCamPosSpline->ReadControlPointsFromFile(pos.c_str());
	if(!success)
	{
		MaloW::Debug("CamRecording: Warning: Failed to read control points from file.");
	}
	success = this->mCamAtSpline->ReadControlPointsFromFile(at.c_str());
	if(!success)
	{
		MaloW::Debug("CamRecording: Warning: Failed to read control points from file.");
	}
	else //if succesful read, initialize splines
	{
		HRESULT hr = S_OK;
		hr = this->mCamPosSpline->Init();
		if(FAILED(hr))
		{
			MaloW::Debug("CamRecording: Error: Failed to initialize splines from file.");
			return;
		}
		hr = this->mCamAtSpline->Init();
		if(FAILED(hr))
		{
			MaloW::Debug("CamRecording: Error: Failed to initialize splines from file.");
			return;
		}

		this->mHasRecorded = true;
	}
	

	//if(this->gDevice) //if rendering is used, initialize rendering
	//{
	//	//CamRecording::InitDrawing(this->gDevice); todo**
	//}
}/*
void CamRecording::CircleAround(bool loop, unsigned int interval, unsigned int nrOfPoints, unsigned int nrOfRotations, Vector3 startPos, D3DXVECTOR3 lookAt)
{
	//remove previous recording, if any
	if(this->mHasRecorded)
	{
		CamRecording::DeletePreviousRecording();
	}

	this->mIsLoopingSeamless = loop;
	this->mInterval = interval;

	float dAngle = 0.0f;
	if(loop)
	{
		dAngle = ((2 * PI) / nrOfPoints);
	}
	else
	{
		dAngle = ((2 * PI) / nrOfPoints) * nrOfRotations;
	}
	float currentAngle = dAngle;
	Vector3 currentPos = startPos;
	
	Vector3 mat;
	Vector3 vecIn, vecOut;
	vecIn = currentPos - lookAt;
		
	for(unsigned int i = 0; i < nrOfPoints; i++)
	{
		this->AddCameraWaypoint(currentPos, lookAt);

		D3DXMatrixRotationY(&mat, currentAngle);
		D3DXVec3TransformNormal(&vecOut, &vecIn, &mat);
		currentPos = lookAt + vecOut; 
		currentAngle += dAngle;
	}
}*/
/*
void CamRecording::Load(CAMERA_PATH camPath)
{
	D3DXVECTOR3 pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 at = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vecIn = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vecOut = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	D3DXMATRIX mat;
	unsigned int nrOfPoints = 0;
	unsigned int nrOfRotations = 0;

	//remove previous recording, if any
	if(this->mHasRecorded)
	{
		CamRecording::DeletePreviousRecording();
	}

	if(camPath == SPIRAL_DOWN) 
	{
		this->mInterval = 1000;
		nrOfPoints = 10;
		nrOfRotations = 1;
		float startX = 30.0f;
		float startY = 70.0f;
		float startZ = 30.0f;
		float endY = 20.0f;
		float dY = (startY - endY) / nrOfPoints;
		float dAngle = ((2 * PI) / nrOfPoints) * nrOfRotations;
		float angle = dAngle;
		pos = D3DXVECTOR3(startX, startY, startZ);
		vecIn = pos - at;
		
		for(unsigned int i = 0; i < nrOfPoints; i++)
		{
			pos.y = startY - i * dY;

			this->AddCameraWaypoint(pos, at);

			D3DXMatrixRotationY(&mat, angle);
			D3DXVec3TransformNormal(&vecOut, &vecIn, &mat);
			pos = vecOut;
			angle += dAngle;
		}
	}
	else if(camPath == CIRCLE_AROUND)
	{
		this->mIsLoopingSeamless = true;
		this->mInterval = 50;
		nrOfPoints = 1000;
		nrOfRotations = 1;
		float startX = 30.0f;
		float startY = 50.0f;
		float startZ = 30.0f;
		float dAngle = ((2 * PI) / nrOfPoints) * nrOfRotations;
		float angle = dAngle;
		pos = D3DXVECTOR3(startX, startY, startZ);
		vecIn = pos - at;
		
		for(unsigned int i = 0; i < nrOfPoints; i++)
		{
			this->AddCameraWaypoint(pos, at);

			D3DXMatrixRotationY(&mat, angle);
			D3DXVec3TransformNormal(&vecOut, &vecIn, &mat);
			pos = vecOut;
			angle += dAngle;
		}
	}
}
*/
void CamRecording::Update(float deltaTime)
{
	//milliseconds per frame for 10 fps (1 / 10fps). used for when deltatime > 1. 
	//Ex: interval = 1000. time = 999. (int)deltaTime = 2. time += (int)deltatime = 1001. 
	static int intervalEpsilon = 100; 
	static float time = 0.0f;

	if(this->mIsRecording)
	{
		time += deltaTime;
		int intervalTime = (int)time % (this->mInterval + intervalEpsilon); //+ intervalEpsilon because of "time += intervalEpsilon"

		if(intervalTime >= 0 && intervalTime < intervalEpsilon) 
		{
			this->mCamPosSpline->AddControlPoint(this->gCamera->GetPosition());
			this->mCamAtSpline->AddControlPoint(this->gCamera->GetPosition() + this->gCamera->GetForward());
			time += intervalEpsilon;
		}
	}
	else if(this->mIsPlaying) //todo: disable camera input**
	{
		this->mCurrentPlayTime += deltaTime * 0.001f; 

		if(this->mCurrentPlayTime < this->mPlayTime)
		{
			float t = (float)this->mCurrentPlayTime / (float)this->mPlayTime;
			Vector3 pos, at;
			
			pos = this->mCamPosSpline->GetPoint(t) + this->mPathOffset;
			at = this->mCamAtSpline->GetPoint(t) + this->mPathOffset;
			
			this->gCamera->SetPosition(pos);
			this->gCamera->LookAt(at);
		}
		else
		{
			if(!this->IsLooping())
			{
				this->mIsPlaying = false;
				this->mCurrentPlayTime = 0;
			}
			else
			{
				this->mCurrentPlayTime = 0;
			}
		}
	}
}

/*
void CamRecording::Render(int nrOfPoints = -1)
{
	if(this->mNrOfVertices > 1)
	{
		UINT stride = 12;
		UINT offset = 0;
		this->mEffect->GetConstantBufferByName("cbPerFrame")->GetMemberByName("pf_WVP")->AsMatrix()->SetMatrix(viewProj);
		this->gDevice->IASetInputLayout(this->mInputLayout);
		this->gDevice->IASetVertexBuffers(0, 1, &this->mVertexBuffer, &stride, &offset);
		this->gDevice->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
		this->mEffectTech->GetPassByIndex(0)->Apply(0);
		this->gDevice->Draw(this->mNrOfVertices, 0);
	}
}*/