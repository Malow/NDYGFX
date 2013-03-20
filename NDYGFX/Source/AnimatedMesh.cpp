#include "AnimatedMesh.h"
#include "MaloWFileDebug.h"


AnimatedMesh::AnimatedMesh(D3DXVECTOR3 pos, string filePath, string billboardFilePath, float distanceToSwapToBillboard) : Mesh(pos, filePath, billboardFilePath, distanceToSwapToBillboard)
{
	this->mNrOfTimesLooped = 0;
	this->mLoopNormal = false;
	this->mLoopSeamless = true;
	this->mAnimationTime = 0.0f;
	this->mKeyFrames = NULL;
	this->mSubFileNames = NULL;
}

AnimatedMesh::~AnimatedMesh()
{
	if(this->mKeyFrames)
	{
		while(this->mKeyFrames->size() > 0)
		{
			delete this->mKeyFrames->getAndRemove(0);
		}

		delete this->mKeyFrames;
		delete[] this->mSubFileNames;
	}
}


void AnimatedMesh::GetCurrentKeyFrames(KeyFrame*& one, KeyFrame*& two, float& t, float time)
{
	if(this->mKeyFrames->size() > 1) //if 2 or more keyframes
	{
		if(this->IsLooping() || !this->mNrOfTimesLooped) //is looping or still haven't reach 0 on the loop count
		{
			if(this->mLoopSeamless) //if looping seamlessly, add first keyframe as last keyframe & return the 2 current keyframes
			{
				unsigned int diff = this->mKeyFrames->get(this->mKeyFrames->size() - 1)->time - this->mKeyFrames->get(this->mKeyFrames->size() - 2)->time;
				unsigned int newEndTime = this->mKeyFrames->get(this->mKeyFrames->size() - 1)->time + diff;

				this->mNrOfTimesLooped = (unsigned int)((int)(time + this->mAnimationTime) / newEndTime);
			
				//compute the indices for the keyframes to interpolate
				unsigned int currentPlayTimeMillis = (int)(time + this->mAnimationTime)  % newEndTime;
				unsigned int firstIndex = 0;
				unsigned int lastIndex = 1;
				bool foundIndex = false;
				while(!foundIndex)
				{
					if(firstIndex == this->mKeyFrames->size() - 1)
					{
						lastIndex = 0;
						foundIndex = true;
					}
					else if(this->mKeyFrames->get(firstIndex + 1)->time > currentPlayTimeMillis)
					{
						foundIndex = true;
					}
					else
					{
						firstIndex++;
					}
				}

				//get previous and next keyframes
				one = this->mKeyFrames->get(firstIndex);
				if(!lastIndex)
				{
					two = this->mKeyFrames->get(lastIndex);
				}
				else
				{
					two = this->mKeyFrames->get(firstIndex + 1);
				}

				//compute interpolation value t
				int newTimeTwo = two->time - one->time; //can also be seen as the time between keyframe one & two. (new time for keyframe one is 0.)
				//if this time is negative, then we're at the end of the looping, and therefore have to compute the time
				//between the first and second keyframes. (newTimeTwo = time of two = too long, t won't become [0,1])
				if(newTimeTwo < 0)
				{
					newTimeTwo = this->mKeyFrames->get(1)->time;
				}
				int newCurrentTimeMillis = currentPlayTimeMillis - one->time;
				//convert to range [0,1]
				t = ((float)newCurrentTimeMillis / (float)newTimeTwo);
			}
			else //if not (normal looping), return the 2 current keyframes
			{
				int endTime = this->mKeyFrames->get(this->mKeyFrames->size() - 1)->time;
				this->mNrOfTimesLooped = (unsigned int)((int)time / endTime);

				//compute the indices for the keyframes to interpolate
				int currentPlayTimeMillis = (int)(time + this->mAnimationTime) % endTime;
				int firstIndex = 0; 
				bool foundIndex = false;
				while(!foundIndex)
				{
					if(this->mKeyFrames->get(firstIndex + 1)->time > currentPlayTimeMillis)
					{
						foundIndex = true;
					}
					else
					{
						firstIndex++;
					}
				}

				//get previous and next keyframes
				one = this->mKeyFrames->get(firstIndex);
				two = this->mKeyFrames->get(firstIndex + 1);

				//compute interpolation value t
				int newTimeTwo = two->time - one->time; //can also be seen as the time between keyframe one & two. (new time for keyframe one is 0.)
				int newCurrentTimeMillis = currentPlayTimeMillis - one->time;
				//convert to range [0,1]
				t = ((float)newCurrentTimeMillis / (float)newTimeTwo);
			}
		}
		else //if no looping, return last key frame for both
		{
			one = this->mKeyFrames->get(this->mKeyFrames->size() - 1);
			two = one;
			t = 0.0f;
		}
	}
	else if(this->mKeyFrames->size() == 1) //if only 1 keyframe, return it for both
	{
		one = this->mKeyFrames->get(this->mKeyFrames->size() - 1);
		two = one;
		t = 0.0f;
	}
	else //if no keyframes, return null
	{
		one = two = NULL;
		t = 0.0f;
	}
}

MaloW::Array<MeshStrip*>* AnimatedMesh::GetStrips() 
{
	if ( mKeyFrames == NULL ) return 0;
	return this->mKeyFrames->get(0)->meshStripsResource->GetMeshStripsPointer();
}

MaloW::Array<MeshStrip*>* AnimatedMesh::GetStrips(float time, bool first) 
{
	KeyFrame* one; 
	KeyFrame* two;
	float t; 
	this->GetCurrentKeyFrames(one, two, t, time);

	if(first)
	{
		return one->meshStripsResource->GetMeshStripsPointer();
	}

	return two->meshStripsResource->GetMeshStripsPointer();
}


void AnimatedMesh::SetAnimationTime(float animationTime)
{
	this->mAnimationTime = animationTime;
}
void AnimatedMesh::NoLooping()
{
	this->mLoopNormal = false;
	this->mLoopSeamless = false;
}
void AnimatedMesh::LoopNormal()
{
	this->mLoopNormal = true;
	this->mLoopSeamless = false;
}
void AnimatedMesh::LoopSeamless()
{
	this->mLoopNormal = false;
	this->mLoopSeamless = true;
}

bool AnimatedMesh::LoadFromFile(string file)
{
	this->filePath = file;

	// if substr of the last 4 = .obj do this:    - else load other format / print error
	ObjLoader oj;

	// Get the directory correct
	string tempFilename = file;
	string pathfolder = "";
	size_t slashpos = tempFilename.find("/");
	while(slashpos != string::npos)
	{
		slashpos = tempFilename.find("/");
		pathfolder += tempFilename.substr(0, slashpos + 1);
		tempFilename = tempFilename.substr(slashpos + 1);
	}


	bool doOnce = false;

	ifstream anifile;
	anifile.open(file);
	if(anifile)
	{
		string line = "";
		getline(anifile, line);
		int nrOfKeyframes = atoi(line.c_str());

		//Create array to hold filenames.
		this->mSubFileNames = new string[nrOfKeyframes];

		auto keyFrames = mKeyFrames = new MaloW::Array<KeyFrame*>();

		for(int a = 0; a < nrOfKeyframes; a++)
		{
			int time = 0;
			string path = "";
			getline(anifile, line);
			time = atoi(line.c_str());
			getline(anifile, path);

			//Store the subfilename (with path folder)
			this->mSubFileNames[a] = pathfolder + path;

			KeyFrame* frame = new KeyFrame();
			frame->time = time;

			//The code for loading the object(+/data) files
			{
				string tmpPath = pathfolder + path;
			
				frame->meshStripsResource = GetResourceManager()->CreateMeshStripsResourceFromFile(tmpPath.c_str(), this->height);
				
				if(!doOnce)
				{
					//Create arrays for culling
					this->isStripCulled = new bool[frame->meshStripsResource->GetMeshStripsPointer()->size()];
					this->isStripShadowCulled = new bool[frame->meshStripsResource->GetMeshStripsPointer()->size()];
					for(int i = 0; i < frame->meshStripsResource->GetMeshStripsPointer()->size(); i++)
					{
						this->isStripCulled[i] = false;
						this->isStripShadowCulled[i] = false;
					}
					doOnce = true;
				}
			}

			keyFrames->add(frame);
		}
		
		this->mKeyFrames = keyFrames;

		//Set Billboard values
		//Calculate billboard position(this needs to be updated as the mesh position changes).(don't forget to include the scale).
		float halfHeightScaled = this->height * 0.5f * this->scale.y; //(yOffset)
		D3DXVECTOR3 billboardPos = this->pos;
		billboardPos.y += halfHeightScaled;
		//Calculate the size using Pythagoras theorem (don't forget to include the scale).
		//Note that this returns the half size, so multiply by 2.
		float size = sqrtf(powf(halfHeightScaled, 2.0f) * 0.5f) * 4.0f;
		this->billboard = new Billboard(billboardPos, D3DXVECTOR2(size, size), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

		return true;
	}
	else
	{
		MaloW::Debug("Failed to open AnimatedMesh: " + file);
		return false;
	}
}

unsigned int AnimatedMesh::GetAnimationLength() const 
{
	if ( !this->mKeyFrames || !this->mKeyFrames->size() )
	{
		return 0;
	}

	if ( this->mKeyFrames->size() == 1 )
	{
		return 0;
	}

	return this->mKeyFrames->get(this->mKeyFrames->size() - 1)->time + this->mKeyFrames->get(1)->time; 
}
