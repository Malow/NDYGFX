/*!
	Class for doing morphanimation. Morphing is done for positions, 
	texture coordinates, normals and colors of the two keyframes.
	Requirements: 
		Atleast two keyframes to morph/interpolate.
		The number of vertices must match between all keyframes.
		Same goes with the number of faces, attributes, materials, textures and shaders.
*/

#pragma once

#include "Mesh.h"
#include "iAnimatedMesh.h"


#pragma warning ( push )
#pragma warning ( disable : 4250 )

class KeyFrame
{
	public:
		unsigned int time;
		MeshStripsResource* meshStripsResource;

		KeyFrame() { this->time = 0; this->meshStripsResource = NULL; }
		virtual ~KeyFrame()
		{
			if(this->meshStripsResource)
			{
				GetResourceManager()->DeleteMeshStripsResource(this->meshStripsResource);
			}
		}
		KeyFrame(const KeyFrame* origObj)
		{
			this->time = origObj->time;
			this->meshStripsResource = new MeshStripsResource(*origObj->meshStripsResource);
		}

		/*
		MaloW::Array<MeshStrip*>* strips;

		KeyFrame() { this->time = 0; this->strips = new MaloW::Array<MeshStrip*>(); }
		virtual ~KeyFrame()
		{
			if(this->strips)
			{
				while(this->strips->size() > 0)
					delete this->strips->getAndRemove(0);

				delete this->strips;
			}
		}
		KeyFrame(const KeyFrame* origObj)
		{
			this->time = origObj->time;
			this->strips = new MaloW::Array<MeshStrip*>();
			for(int i = 0; i < origObj->strips->size(); i++)
			{
				this->strips->add(new MeshStrip(origObj->strips->get(i)));
			}
		}*/
};


class AnimatedMesh : public Mesh, public virtual iAnimatedMesh
{
	private:
		unsigned int				mNrOfTimesLooped;
		bool						mLoopNormal;
		bool						mLoopSeamless;
		float						mAnimationTime;
		MaloW::Array<KeyFrame*>*	mKeyFrames;

		string*						mSubFileNames;

	public:
		AnimatedMesh(D3DXVECTOR3 pos, string filePath, string billboardFilePath = "", float distanceToSwapToBillboard = -1);
		virtual ~AnimatedMesh();

		MaloW::Array<KeyFrame*>*	GetKeyFrames()			const { return this->mKeyFrames; }

		/*! Returns the 2 keyframes to interpolate with value t[0,1] through the parameters depending on the current time. */
		/*! Last parameter determine what keyframes and interpolation value t is returned.
		/*!	Note that currentTime is expected to be in milliseconds. Returns NULL if there's no keyframes loaded. */
		/*! Also note that this function can most likely be optimized. */
		void GetCurrentKeyFrames(KeyFrame*& one, KeyFrame*& two, float& t, float time);

		/*! Returns the strips of the first mesh. */
		virtual MaloW::Array<MeshStrip*>* GetStrips(); 
		/*! Returns the strips of the first or the second mesh currently being used. */
		virtual MaloW::Array<MeshStrip*>* GetStrips(float time, bool first = true);

		/*! Load the keyframes from file. Input is expected to be "'filename'.ani". */
		virtual bool LoadFromFile(string file);
		
		// The length of the animation in milliseconds
		virtual unsigned int GetAnimationLength() const;

		// iAnimatedMesh interface functions
		virtual unsigned int	GetNrOfTimesLooped()	const { return this->mNrOfTimesLooped; }
		virtual bool			IsLooping()				const { return this->mLoopNormal || this->mLoopSeamless; }
		virtual bool			IsLoopingNormal()		const { return this->mLoopNormal; }
		virtual bool			IsLoopingSeamless()		const { return this->mLoopSeamless; }
		
		virtual void SetAnimationTime(float animationTime);

		/*! Prevents looping. */
		virtual void NoLooping();
		/*! Loops by returning to the first keyframe when last keyframe is reached. Note that this kind of looping is not seamless. */
		virtual void LoopNormal();
		/*! Loops by adding the first keyframe as the last keyframe to prevent seamed(normal) looping */
		virtual void LoopSeamless(); 
};

#pragma warning ( pop )