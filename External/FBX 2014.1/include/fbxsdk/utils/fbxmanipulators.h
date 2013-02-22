/****************************************************************************************
 
   Copyright (C) 2013 Autodesk, Inc.
   All rights reserved.
 
   Use of this software is subject to the terms of the Autodesk license agreement
   provided at the time of installation or download, or which otherwise accompanies
   this software in either electronic or hard copy form.
 
****************************************************************************************/

//! \file fbxmanipulators.h
#ifndef _FBXSDK_UTILS_MANIPULATORS_H_
#define _FBXSDK_UTILS_MANIPULATORS_H_

#include <fbxsdk/fbxsdk_def.h>

#include <fbxsdk/core/fbxobject.h>
#include <fbxsdk/core/math/fbxvector2.h>
#include <fbxsdk/core/math/fbxvector4.h>
#include <fbxsdk/scene/geometry/fbxcamera.h>

#include <fbxsdk/fbxsdk_nsbegin.h>

class FbxCameraManipulationState;

/** This class can be used to provide basic camera manipulation in any program using this library.
  * \nosubgrouping
  */
class FBXSDK_DLL FbxCameraManipulator : public FbxObject
{
	FBXSDK_OBJECT_DECLARE(FbxCameraManipulator, FbxObject);

public:
	//! All possible manipulation actions that can be performed on a camera using this manipulator.
	enum EAction
	{
		eNone,		//!< No action.
		eOrbit,		//!< Orbiting camera around LootAt/Interest position.
		eDolly,		//!< Moving camera closer or away from its LookAt/Intest position.
		ePan,		//!< Panning camera up, down and sideways.
		eFreePan	//!< Panning and dollying all at once.
	};

	/** Set the camera used for the manipulation.
	* \param pCamera Camera that will be used for the manipulation.
	* \param pRestore If true the pCamera revert back to the initial position upon destruction of this object.
	* \remark The camera current rotation, LookAt and node position are changed by this manipulator but restored when this object is destroyed. */
	void Initialize(FbxCamera* pCamera, bool pRestore=true);

	/** Begin manipulation of the camera.
	* \param pAction The action performed for this manipulation scope.
	* \param pX Begin horizontal position of the manipulation, in pixels.
	* \param pY Begin vertical position of the manipulation, in pixels. */
	void Begin(EAction pAction, float pX, float pY);

	/** Notify manipulation of latest input.
	* \param pX Horizontal position of the manipulation, in pixels.
	* \param pY Vertical position of the manipulation, in pixels.
	* \param pZ Depth position of the manipulation, in pixels. Only used by eDollyPan action. */
	void Notify(float pX, float pY, float pZ=0);

	//! End current manipulation.
	void End();

	/** Change camera position, rotation and LookAt node to frame all objects.
	* \param pTime Current time.
	* \param pOnAnimLayer Specify which animation layer to use for the evaluation.
	* \return True if the FrameAll action was successfully performed. */
	bool FrameAll(const FbxTime& pTime=FBXSDK_TIME_INFINITE, int pOnAnimLayer=0);

	/** Change camera position, rotation and LookAt to frame all selected objects.
	* \param pTime Current time.
	* \param pOnAnimLayer Specify which animation layer to use for the evaluation.
	* \return True if the FrameSelected action was successfully performed. */
	bool FrameSelected(const FbxTime& pTime=FBXSDK_TIME_INFINITE, int pOnAnimLayer=0);

	/** Retrieve the camera used by this manipulator.
	* \return The camera currently in use. */
	FbxCamera* GetCamera() const;

	/** Retrieve current manipulation action.
	* \return The action currently performed by the camera manipulator. */
	EAction GetCurrentAction() const;

/*****************************************************************************************************************************
** WARNING! Anything beyond these lines is for internal use, may not be documented and is subject to change without notice! **
*****************************************************************************************************************************/
#ifndef DOXYGEN_SHOULD_SKIP_THIS
	virtual FbxObject& Copy(const FbxObject& pObject);

protected:
	virtual void Construct(const FbxCameraManipulator* pFrom);
	virtual void Destruct(bool pRecursive);
	virtual void ConstructProperties(bool pForceSet);

private:
	FbxCameraManipulationState* mState;
	FbxPropertyT<FbxReference> Camera;

	bool Frame(int pOnAnimLayer, const FbxTime& pTime, bool pSelected);

#endif /* !DOXYGEN_SHOULD_SKIP_THIS *****************************************************************************************/
};

#include <fbxsdk/fbxsdk_nsend.h>

#endif /* _FBXSDK_UTILS_MANIPULATORS_H_ */
