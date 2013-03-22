#include "Camera.h"
#include "FBXMesh.h"

Camera::Camera(HWND g_hWnd, GraphicsEngineParams &params) : 
	params(params)
{
	this->forceBoundries = false;
	this->g_hWnd = g_hWnd;
	this->pos = D3DXVECTOR3(0, 0, 0);
	this->terrain = NULL;
	this->followTarget = NULL;
	this->moveOnlyInXZ = false;
	this->angleX = 0;
	this->angleY = 0;
	this->oldpos = this->pos;

	this->useFBXPosition = false;
	this->FBXPosition = D3DXVECTOR3(0, 0, 0);
	
	this->bone = "";
	this->speed = 1.0f;

	this->updateCamera = true;
	this->activeWindowDisabling = true;

	D3DXMatrixPerspectiveFovLH(&this->projection, this->params.FOV * 0.01745f, 
		this->params.WindowWidth / (float)this->params.WindowHeight, this->params.NearClip, this->params.FarClip);

	D3DXVECTOR3 at = this->pos + this->forward;
	D3DXMatrixLookAtLH(&view, &this->pos, &at, &this->up);

	D3DXMatrixMultiply(&this->viewProj, &this->view, &this->projection);
}

Camera::~Camera()
{

}

D3DXMATRIX& Camera::GetViewMatrix()
{
	return this->view; 
}

D3DXMATRIX& Camera::GetProjectionMatrix()
{
	return this->projection; 
}

Vector3 Camera::GetPosition()
{
	if(this->useFBXPosition)
		return Vector3(this->FBXPosition.x, this->FBXPosition.y, this->FBXPosition.z);
	else
		return Vector3(this->pos.x, this->pos.y, this->pos.z);
}

void Camera::SetPosition(Vector3 pos)
{
	this->pos = D3DXVECTOR3(pos.x, pos.y, pos.z);
}

Vector3 Camera::GetForward() const
{
	return Vector3(this->forward.x, this->forward.y, this->forward.z);
}

void Camera::SetForward(Vector3 forward)
{
	this->angleX = -atan2(forward.x * 0 - 1 * forward.z, forward.x * 1 + forward.z * 0);
	this->angleY = asin(forward.y);

	this->forward = D3DXVECTOR3(forward.x, forward.y, forward.z);
	this->forward = this->NormalizeVector(this->forward);
}

void Camera::LookAt(Vector3 target)
{
	this->forward = D3DXVECTOR3(target.x, target.y, target.z) - this->pos;
	this->forward = this->NormalizeVector(this->forward);

	this->angleX = -atan2(this->forward.x*0 - 1*this->forward.z, this->forward.x * 1 + this->forward.z * 0);
	this->angleY = asin(this->forward.y);
}

Vector3 Camera::GetUpVector() const
{
	return Vector3(this->up.x, this->up.y, this->up.z);
}

void Camera::SetUpVector(Vector3 up)
{
	this->up = D3DXVECTOR3(up.x, up.y, up.z);
}

Vector3 Camera::GetRightVector() const
{
	D3DXVECTOR3 tmp;
	D3DXVec3Cross(&tmp, &forward, &up);

	return Vector3(tmp.x, tmp.y, tmp.z);
}


void Camera::MoveToTerrain()
{
	if(this->terrain)
		this->pos.y = this->terrain->GetYPositionAt(this->pos.x, this->pos.z)/* + this->DistanceFromTarget*/;
}

void Camera::Move(Vector3 moveBy)
{
	this->pos += D3DXVECTOR3(moveBy.x, moveBy.y, moveBy.z);
	this->MoveToTerrain();
}

D3DXVECTOR3& Camera::NormalizeVector(D3DXVECTOR3& vec)
{
	float length = sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
	vec.x /= length;
	vec.y /= length;
	vec.z /= length;
	return vec;
}

void Camera::MoveFollowingMesh()
{
	if(this->followTarget)
	{
		// If it crashes here it's because the mesh has gotten removed without camera being notified.
		Vector3 pos = Vector3(this->pos.x, this->pos.y, this->pos.z) - this->distanceFromMesh;
		this->followTarget->SetPosition(pos);

		// If it's an FBX mesh and a bone is set, make the camera follow the bone, else make the mesh follow camera.
		FBXMesh* mesh = dynamic_cast<FBXMesh*>(this->followTarget);
		if(mesh && this->bone != "")
		{
			this->useFBXPosition = true;
			Vector3 pos;
			mesh->GetBoneTransformation(this->bone, &pos, 0);
			this->FBXPosition = D3DXVECTOR3(pos.x, pos.y, pos.z);
		}
		else 
			useFBXPosition = false;


		//Rotate Mesh
		Vector3 camDir = this->GetForward();
		Vector3 around = Vector3(0,1,0);
		float angle;

		camDir.y = 0;
		camDir.Normalize();

		if(mesh)	// Invert rotation for FBX due to it being inverted.
			angle = -acos(camDir.GetDotProduct(this->defaultMeshDirection));
		else
			angle = acos(camDir.GetDotProduct(this->defaultMeshDirection));

		if(camDir.x > 0.0f)
			angle *= -1;

		this->followTarget->ResetRotation();
		this->followTarget->RotateAxis(around, angle);
	}
}

void Camera::Update(float delta)
{
	this->MoveFollowingMesh();
	if(this->updateCamera)
	{
		if(activeWindowDisabling)
		{
			if(GetForegroundWindow() == this->g_hWnd)
				UpdateSpecific(delta);
		}
		else
		{
			UpdateSpecific(delta);
		}

		//this->MoveToTerrain();
		
		if(this->forceBoundries)
		{
			if(this->pos.x < this->minBoundries.x)
				this->pos.x = this->minBoundries.x;
			else if(this->pos.x > this->maxBoundries.x)
				this->pos.x = this->maxBoundries.x;

			if(this->pos.y < this->minBoundries.y)
				this->pos.y = this->minBoundries.y;
			else if(this->pos.y > this->maxBoundries.y)
				this->pos.y = this->maxBoundries.y;

			if(this->pos.z < this->minBoundries.z)
				this->pos.z = this->minBoundries.z;
			else if(this->pos.z > this->maxBoundries.z)
				this->pos.z = this->maxBoundries.z;
		}
	}

	if(this->useFBXPosition)
	{
		// Update v p matrix.
		D3DXVECTOR3 curPos = this->FBXPosition;
		D3DXVECTOR3 at = curPos + this->forward;
		D3DXMatrixLookAtLH(&view, &curPos, &at, &this->up);
		D3DXMatrixPerspectiveFovLH(&this->projection, this->params.FOV * 0.01745f, 
			this->params.WindowWidth / (float)this->params.WindowHeight, this->params.NearClip, this->params.FarClip);
		D3DXMatrixMultiply(&this->viewProj, &this->view, &this->projection);
		this->oldpos = curPos;	
	}
	else
	{
		// Update v p matrix.
		D3DXVECTOR3 curPos = this->pos;
		D3DXVECTOR3 at = curPos + this->forward;
		D3DXMatrixLookAtLH(&view, &curPos, &at, &this->up);
		D3DXMatrixPerspectiveFovLH(&this->projection, this->params.FOV * 0.01745f, 
			this->params.WindowWidth / (float)this->params.WindowHeight, this->params.NearClip, this->params.FarClip);
		D3DXMatrixMultiply(&this->viewProj, &this->view, &this->projection);
		this->oldpos = curPos;
	}
}

void Camera::SetBoundries(Vector3 minBoundries, Vector3 maxBoundries)
{
	this->minBoundries = D3DXVECTOR3(minBoundries.x, minBoundries.y, minBoundries.z);
	this->maxBoundries = D3DXVECTOR3(maxBoundries.x, maxBoundries.y, maxBoundries.z);
	this->forceBoundries = true;
}

void Camera::DisableBoundries()
{
	this->forceBoundries = false;
}

void Camera::SetMesh(iMesh* target, Vector3 distanceFromCamera, Vector3 defaultMeshDirection)
{
	this->followTarget = dynamic_cast<Mesh*>(target);
	this->distanceFromMesh = distanceFromCamera;
	this->defaultMeshDirection = defaultMeshDirection;
	this->bone = "";
	this->defaultMeshDirection.Normalize();
}

void Camera::SetMesh(iMesh* target, const char* bone, Vector3 defaultMeshDirection)
{
	this->followTarget = dynamic_cast<Mesh*>(target);
	this->distanceFromMesh = Vector3(0, 0, 0);
	this->defaultMeshDirection = defaultMeshDirection;
	this->bone = bone;
	this->defaultMeshDirection.Normalize();
}

void Camera::RecreateProjectionMatrix()
{
	D3DXMatrixPerspectiveFovLH(&this->projection, this->params.FOV * (float)D3DX_PI / 180.0f, 
		this->params.WindowWidth / (float)this->params.WindowHeight, this->params.NearClip, this->params.FarClip);
	D3DXMatrixMultiply(&this->viewProj, &this->view, &this->projection);
}

D3DXVECTOR3 Camera::GetRightVectorD3DX() const
{
	Vector3 right = this->GetRightVector();

	return D3DXVECTOR3(right.x, right.y, right.z);
}

Vector3 Camera::Get3DPickingRay()
{
	D3DXMATRIX tempView;
	D3DXMATRIX tempProj;

	D3DXVECTOR3 at = this->pos + this->forward;
	D3DXMatrixLookAtLH(&tempView, &this->pos, &at, &this->up);
	D3DXMatrixPerspectiveFovLH(&tempProj, this->params.FOV * 0.01745f, 
		this->params.WindowWidth / (float)this->params.WindowHeight, this->params.NearClip, this->params.FarClip);

	Vector3 v/* = Vector3(1, 0, 0)*/;
	POINT p;
	if(GetCursorPos(&p))
	{
		if(ScreenToClient(this->g_hWnd, &p))
		{
			v.x = (((2.0f * p.x) / this->params.WindowWidth) - 1) / tempProj._11;
			v.y = -(((2.0f * p.y) / this->params.WindowHeight) - 1) / tempProj._22;
			v.z =  1.0f;


			D3DXMATRIX m;
			D3DXVECTOR3 rayOrigin,rayDir;

			D3DXMatrixInverse(&m, NULL, &tempView);

			// Transform the screen space pick ray into 3D space
			rayDir.x = v.x * m._11 + v.y * m._21 + v.z * m._31;
			rayDir.y = v.x * m._12 + v.y * m._22 + v.z * m._32;
			rayDir.z = v.x * m._13 + v.y * m._23 + v.z * m._33;
			rayOrigin.x = m._41;
			rayOrigin.y = m._42;
			rayOrigin.z = m._43;

			rayDir = this->NormalizeVector(rayDir);

			v.x = rayDir.x;
			v.y = rayDir.y;
			v.z = rayDir.z;
		}
		else
			MaloW::Debug("Get3DPickingRay failed.");
	}
	else
		MaloW::Debug("Get3DPickingRay failed.");

	return v;
}

void Camera::MoveOnlyInXZ( bool DoItOrNotThatIsTheQuestion )
{
	this->moveOnlyInXZ = DoItOrNotThatIsTheQuestion;
}

void Camera::SetUpdateCamera( bool update )
{
	this->updateCamera = update;
}

D3DXMATRIX& Camera::GetViewProjMatrix()
{
	return this->viewProj;
}

D3DXVECTOR3 Camera::GetOldPos() const
{
	return this->oldpos;
}

