#include "FBXScene.h"
#include "MinMax.h"
#include "Caching.h"


inline std::string GetFileFromPath( const std::string& name )
{
	std::string tempFileName = name;
	std::string pathfolder = "";
	size_t slashpos = tempFileName.find("/");
	while(slashpos != std::string::npos)
	{
		slashpos = tempFileName.find("/");
		pathfolder += tempFileName.substr(0, slashpos + 1);
		tempFileName = tempFileName.substr(slashpos + 1);
	}

	return tempFileName;
}

FBXScene::FBXScene(FbxManager* sdkManager)
	: mSdkManager(sdkManager), mScene(NULL), m_pSkeleton(NULL), m_pAnimationController(NULL)
{
	m_BoundingBox.Min = BTHFBX_VEC3(FLT_MAX, FLT_MAX, FLT_MAX);
	m_BoundingBox.Max = BTHFBX_VEC3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
}

FBXScene::~FBXScene()
{
	for(auto i = m_FBXMaterials.begin(); i != m_FBXMaterials.end(); i++)
	{
		(*i)->Destroy();
	}

	for(auto i = m_Materials.begin(); i != m_Materials.end(); i++)
	{
		delete *i, *i = 0;
	}

	m_Models.RemoveAll(true);

	if ( m_pSkeleton ) delete m_pSkeleton, m_pSkeleton = 0;
	if ( m_pAnimationController ) delete m_pAnimationController, m_pAnimationController = 0;
	if ( mScene ) mScene->Destroy();
}

bool FBXScene::Init(const char* fileName, Vector3& minPos, Vector3& maxPos)
{
    // Create the entity that will hold the scene.
    mScene = FbxScene::Create(mSdkManager,"");
	if(!mScene) return false;

	// Cache Scene
	if ( !LoadCachedScene(fileName, this, minPos, maxPos) )
	{
		std::stringstream ss;

		if ( !LoadScene(fileName, ss, minPos, maxPos) )
		{
			return false;
		}

		CacheScene(fileName, this);
	}
	else
	{
		mFilename = fileName;
	}

	return true;
}

void FBXScene::ProcessBlendWeights()
{
	for( unsigned int i = 0; i < m_Models.GetCount(); ++i )
	{
		m_Models.GetValueAt(i)->ProcessBlendWeights();
	}
}

bool FBXScene::LoadScene(const char* filename, std::ostream& output, Vector3& minPos, Vector3& maxPos)
{
    int lFileMajor, lFileMinor, lFileRevision;

    int i, lAnimStackCount;
    bool lStatus;

	FbxIOSettings *ios = FbxIOSettings::Create(mSdkManager, IOSROOT);
	mSdkManager->SetIOSettings(ios);
	FbxGeometryConverter lGConverter(mSdkManager);
	
	// Create an importer using our sdk manager.
	FbxImporter* pFBXImporter = FbxImporter::Create(mSdkManager,"");

    // Initialize the importer by providing a filename.
    const bool lImportStatus = pFBXImporter->Initialize(filename, -1, mSdkManager->GetIOSettings());
	pFBXImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

    if( !lImportStatus )
    {
		output << "FBX Importer Error: " << pFBXImporter->GetStatus().GetErrorString() << std::endl;

        if ( pFBXImporter->GetStatus() == FbxStatus::eInvalidFileVersion )
        {
			output << "FBX version number for file " << filename << " is " << lFileMajor << " " << lFileMinor << " " << lFileRevision << std::endl;
		}

        return false;
    }

    if (pFBXImporter->IsFBX())
    {
		output << "FBX version number for file " << filename << " is " << lFileMajor << " " << lFileMinor << " " << lFileRevision << std::endl;

        // From this point, it is possible to access animation stack information without
        // the expense of loading the entire file.

		output << "Animation Stack Information" << std::endl;

        lAnimStackCount = pFBXImporter->GetAnimStackCount();

		output << "    Number of Animation Stacks: " << lAnimStackCount << std::endl;
		output << "    Current Animation Stack: " << pFBXImporter->GetActiveAnimStackName().Buffer() << std::endl;
		
        for(i = 0; i < lAnimStackCount; i++)
        {
            FbxTakeInfo* lTakeInfo = pFBXImporter->GetTakeInfo(i);

			output << "    Animation Stack " << i << std::endl;
			output << "    Name: " << lTakeInfo->mName.Buffer() << std::endl;
			output << "    Description: " <<  lTakeInfo->mDescription.Buffer() << std::endl;
			output << "    Import Name: " << lTakeInfo->mImportName.Buffer() << std::endl;
			output << "    Import State: " << (lTakeInfo->mSelect ? "true" : "false") << std::endl;
        }
    }
	
    // Import the scene.
    lStatus = pFBXImporter->Import(mScene);

	if ( !lStatus )
	{
		output << "Failed Importing FBX!" << std::endl;
		output << "FBX is password protected!" << std::endl;

		if ( pFBXImporter->GetStatus() == FbxStatus::ePasswordError )
		{
			output << "FBX is password protected!" << std::endl;
		}
	}

	mFilename = pFBXImporter->GetFileName().Buffer();

    // Destroy the importer.
    pFBXImporter->Destroy();
	ios->Destroy();

	ProcessScene(mScene);

	return lStatus;
}

void FBXScene::ProcessScene(FbxScene* pScene)
{
	if ( m_pSkeleton ) delete m_pSkeleton, m_pSkeleton = 0;

	FbxAxisSystem SceneAxisSystem = pScene->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem OurAxisSystem(FbxAxisSystem::Motionbuilder);

	if( SceneAxisSystem != OurAxisSystem )
	{
		OurAxisSystem.ConvertScene(pScene);
	}
	
	ProcessMaterials(pScene);
	ProcessNode(pScene->GetRootNode(), FbxNodeAttribute::eSkeleton);
	ProcessNode(pScene->GetRootNode(), FbxNodeAttribute::eMesh);	// takes time, guesstimate 50% of it
	ProcessNode(pScene->GetRootNode(), FbxNodeAttribute::eNurbsCurve); 

	if(m_pSkeleton)
		m_pSkeleton->BuildBoneHierarchy();

	ProcessBlendWeights();
	ProcessSkeleteonBoundingBoxes();

	ProcessAnimations(pScene);	// takes time too, other 50%
}

void FBXScene::ProcessSkeleteonBoundingBoxes()
{
	if(m_pSkeleton)
	{
		for( unsigned int i = 0; i < m_Models.GetCount(); ++i )
		{
			m_Models.GetValueAt(i)->ProcessSkeleteonBoundingBoxes(m_pSkeleton);
		}
	}
}

void FBXScene::ProcessNode(FbxNode* pNode, FbxNodeAttribute::EType attributeType)
{
	if( !pNode )
		return;

	FbxNodeAttribute* pNodeAttribute = pNode->GetNodeAttribute();

	if (pNodeAttribute)
	{
		if( pNodeAttribute->GetAttributeType() == attributeType )
		{
			switch(pNodeAttribute->GetAttributeType())
			{
			case FbxNodeAttribute::eSkeleton:
				{
					ProcessSkeleton(pNode);
					break;
				}

			case FbxNodeAttribute::eMesh:
				{
					ProcessMesh(pNode);
					break;
				}

			case FbxNodeAttribute::eNurbsCurve:
				{
					ProcessCurve(pNode);
					break;
				}

			default:
				break;
			};
		}
	}

	for( int i = 0; i < pNode->GetChildCount(); ++i )
	{
		ProcessNode(pNode->GetChild(i), attributeType);
	}
}

void FBXScene::ProcessCurve(FbxNode* pNode)
{
    FbxNurbs* lNurb = (FbxNurbs*) pNode->GetNodeAttribute ();

    int lControlPointsCount = lNurb->GetControlPointsCount();

	if(lControlPointsCount > 0)
	{
		Curve c;
		FbxVector4* lControlPoints = lNurb->GetControlPoints();

		c.SetName(pNode->GetName());

		for(int i = 0; i < lControlPointsCount; i++)
		{
			c.AddControlPoint(FbxVector4ToBTHFBX_VEC3(lControlPoints[i]));
		}

		m_Curves.push_back(c);
	}
}

void FBXScene::ProcessAnimations(FbxScene* pScene)
{
	m_pAnimationController = new AnimationController();

	FbxNode* pRootNode = pScene->GetRootNode();
	if(!pRootNode)
		return;

	float fFrameRate = (float)FbxTime::GetFrameRate(pScene->GetGlobalSettings().GetTimeMode());

	FbxArray<FbxString*> takeArray;	
	FbxDocument* pDocument = FbxCast<FbxDocument>(pScene); // dynamic_cast<FbxDocument*>(pScene);
	if( pDocument )
		pDocument->FillAnimStackNameArray(takeArray);

	for( int i = 0; i < takeArray.GetCount(); ++i )
	{
		FbxString* takeName = takeArray.GetAt(i);

		if( std::string(takeName->Buffer()) != "Default" )
		{
			/// ARRRGHHH SÄTTER ALLTID FÖRSTA HÄR!!!!!!!!!!!!!!!!!!
			FbxTakeInfo* lCurrentTakeInfo = pScene->GetTakeInfo(takeName->Buffer());

			FbxAnimStack* lAnimStack = FbxCast<FbxAnimStack>(pScene->GetSrcObject<FbxAnimStack>(i));
			pScene->GetEvaluator()->SetContext(lAnimStack);

			FbxTime KStart;
			FbxTime KStop;
			if(lCurrentTakeInfo)
			{
				KStart = lCurrentTakeInfo->mLocalTimeSpan.GetStart();
				KStop = lCurrentTakeInfo->mLocalTimeSpan.GetStop();
			}
			else
			{
				// Take the time line value
				FbxTimeSpan lTimeLineTimeSpan;
				pScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(lTimeLineTimeSpan);
				KStart = lTimeLineTimeSpan.GetStart();
				KStop  = lTimeLineTimeSpan.GetStop();
			}

			float fStart = (float)KStart.GetSecondDouble();
			float fStop = (float)KStop.GetSecondDouble();

			if( fStart < fStop )
			{
				int nKeyFrames = int((fStop-fStart)*fFrameRate);

				Animation* pAnimation = new Animation(takeName->Buffer(), nKeyFrames, fFrameRate);
				m_pAnimationController->AddAnimation(pAnimation);
				
				ProcessAnimation(pRootNode, takeName->Buffer(), fFrameRate, fStart, fStop);
			}
		}

		delete takeName;
	}
	takeArray.Clear();
}

void FBXScene::ProcessAnimation(FbxNode* pNode, const char* strTakeName, float fFrameRate, float fStart, float fStop)
{
	FbxNodeAttribute* pNodeAttribute = pNode->GetNodeAttribute();
	if (pNodeAttribute)
	{
		if (pNodeAttribute->GetAttributeType() == FbxNodeAttribute::EType::eSkeleton)
		{
			if( m_pSkeleton )
			{
				SkeletonBone* pBone = m_pSkeleton->FindBone(pNode->GetName());

				if( pBone )
				{
					AnimationKeyFrames* pAnimationKeyFrames = new AnimationKeyFrames(strTakeName);

					double fTime = 0;
					while( fTime <= fStop )
					{
						FbxTime takeTime;
						takeTime.SetSecondDouble(fTime);

						FbxMatrix matAbsoluteTransform2 = GetAbsoluteTransformFromCurrentTake2(pNode, takeTime);
						FbxMatrix matParentAbsoluteTransform2 = GetAbsoluteTransformFromCurrentTake2(pNode->GetParent(), takeTime);
						FbxMatrix matInvParentAbsoluteTransform2 = matParentAbsoluteTransform2.Inverse();
						FbxMatrix matTransform2 = matInvParentAbsoluteTransform2 * matAbsoluteTransform2;

						pAnimationKeyFrames->AddKeyFrame(matTransform2);

						fTime += 1.0f / fFrameRate;
					}

					pBone->AddAnimationKeyFrames(pAnimationKeyFrames);
				}
			}
		}
		else if (pNodeAttribute->GetAttributeType() == FbxNodeAttribute::EType::eMesh)
		{
			Model* pModel = m_Models.Find(pNode->GetName(), NULL);

			if( pModel )
			{
				AnimationKeyFrames* pAnimationKeyFrames = new AnimationKeyFrames(strTakeName);

				double fTime = 0;
				while( fTime <= fStop )
				{
					FbxTime takeTime;
					takeTime.SetSecondDouble(fTime);

					FbxMatrix matAbsoluteTransform2 = GetAbsoluteTransformFromCurrentTake2(pNode, takeTime);

					pAnimationKeyFrames->AddKeyFrame(matAbsoluteTransform2);

					fTime += 1.0f/fFrameRate;
				}

				pModel->AddAnimationKeyFrames(pAnimationKeyFrames);
			}
		}
	}

	for( int i = 0; i < pNode->GetChildCount(); ++i )
	{
		ProcessAnimation(pNode->GetChild(i), strTakeName, fFrameRate, fStart, fStop);
	}
}

bool FBXScene::HasFBXAnimation(FbxNode *pNode) 
{ 
	FbxTimeSpan timeSpan;
	return pNode->GetAnimationInterval(timeSpan);
} 

void FBXScene::ProcessSkeleton(FbxNode* pNode)
{
	FbxSkeleton* pFBXSkeleton = pNode->GetSkeleton();
	if( !pFBXSkeleton )
		return;
	
	if( !m_pSkeleton )
	{
		m_pSkeleton = new Skeleton();
	}

	int nParentBoneIndex = -1;
	FbxNode* pParentNode = pNode->GetParent();
	if( pParentNode )
		nParentBoneIndex = m_pSkeleton->FindBoneIndex(pParentNode->GetName());

	SkeletonBone* pSkeletonBone = new SkeletonBone( pNode->GetName(), nParentBoneIndex, m_pSkeleton );
	m_pSkeleton->AddSkeletonBone(pSkeletonBone);
	
	int boneIndex = m_pSkeleton->FindBoneIndex(pNode->GetName());
	pSkeletonBone->SetBoneIndex(boneIndex);

	printf("BONE: %s\n", pSkeletonBone->GetName());
}

void FBXScene::UpdateBoundingBoxDataFromVertex(BTHFBX_VEC3 vertexPosition)
{
	m_BoundingBox.Min.x = Min(vertexPosition.x, m_BoundingBox.Min.x);
	m_BoundingBox.Min.y = Min(vertexPosition.y, m_BoundingBox.Min.y);
	m_BoundingBox.Min.z = Min(vertexPosition.z, m_BoundingBox.Min.z);

	m_BoundingBox.Max.x = Max(vertexPosition.x, m_BoundingBox.Max.x);
	m_BoundingBox.Max.y = Max(vertexPosition.y, m_BoundingBox.Max.y);
	m_BoundingBox.Max.z = Max(vertexPosition.z, m_BoundingBox.Max.z);
}

void FBXScene::ProcessMesh(FbxNode* pNode)
{
	FbxMesh* pFBXMesh = pNode->GetMesh();
	if( !pFBXMesh )
		return;

	if ( pFBXMesh->GetPolygonVertexCount() != pFBXMesh->GetPolygonCount() * 3 )
	{
		FbxGeometryConverter GeometryConverter(pNode->GetFbxManager());
		if( !GeometryConverter.TriangulateInPlace( pNode ) )
		{
			return;
		}
		pFBXMesh = pNode->GetMesh();
	}

	pFBXMesh->InitNormals();
	pFBXMesh->ComputeVertexNormals(true); 
	pFBXMesh->GenerateTangentsDataForAllUVSets();

	int nVertexCount = pFBXMesh->GetControlPointsCount();
	if( nVertexCount <= 0 )
		return;

	std::vector<BoneWeights> boneWeights(nVertexCount);

	ProcessBoneWeights(pFBXMesh, boneWeights);
	
	Model* pModel = new Model(pNode->GetName(), m_Models.GetCount(), false);
	FbxVector4* aControlPoints = pFBXMesh->GetControlPoints();
	for( int pi = 0; pi < pFBXMesh->GetPolygonCount(); ++pi )	// Whole for-loop takes some time too, investigate further.
	{
		// Material
		Material* pMaterial = NULL;

		for( unsigned int pvi = 0; pvi < 3; ++pvi )
		{
			int nVertexIndex = pFBXMesh->GetPolygonVertex(pi, pvi);

			if( nVertexIndex < 0 || nVertexIndex >= nVertexCount )
				continue;

			// Material
			if( pMaterial == NULL )
				pMaterial = GetMaterialLinkedWithPolygon(pFBXMesh, 0, pi, 0, nVertexIndex);

			// Position
			FbxVector4 fbxPosition = aControlPoints[nVertexIndex];

			// Normals And Tangents
			FbxVector4 fbxNormal, fbxTangent;
			fbxNormal = GetNormal(pFBXMesh, 0, pi, pvi, nVertexIndex);
			fbxTangent = GetTangent(pFBXMesh, 0, pi, pvi, nVertexIndex);

			// Add Vertex
			pModel->AddVertex(pMaterial, FbxVector4ToBTHFBX_VEC3(fbxPosition),
										 FbxVector4ToBTHFBX_VEC3(fbxNormal),
										 FbxVector4ToBTHFBX_VEC3(fbxTangent),
										 GetTexCoord(pFBXMesh, 0, pi, pvi, nVertexIndex),
										 boneWeights[nVertexIndex]);

			// Update Bounding Box
			UpdateBoundingBoxDataFromVertex(FbxVector4ToBTHFBX_VEC3(fbxPosition));
		}
	}

	// Geometric Offset
	pModel->SetGeometricOffset2(GetGeometricOffset2(pNode));

	// Insert Model
	m_Models.Add(pModel->GetName(), pModel);
}

void FBXScene::ProcessBoneWeights(FbxMesh* pFBXMesh, std::vector<BoneWeights>& meshBoneWeights)
{
	if( !m_pSkeleton )
		return;

	for( int i = 0; i < pFBXMesh->GetDeformerCount(); ++i )
	{
		FbxDeformer* pFBXDeformer = pFBXMesh->GetDeformer(i);

		if( !pFBXDeformer )
			continue;
		
		if( pFBXDeformer->GetDeformerType() == FbxDeformer::eSkin )
		{
			FbxSkin* pFBXSkin = FbxCast<FbxSkin>(pFBXDeformer); //dynamic_cast<FbxSkin*>(pFBXDeformer);

			if( !pFBXSkin )
				continue;

			ProcessBoneWeights(pFBXSkin, meshBoneWeights);
		}
	}
}

//--------------------------------------------------------------------------------------
void FBXScene::ProcessBoneWeights(FbxSkin* pFBXSkin, std::vector<BoneWeights>& meshBoneWeights)
{
	FbxCluster::ELinkMode linkMode = FbxCluster::eNormalize; //Default link mode

	std::vector<BoneWeights> skinBoneWeights(meshBoneWeights.size(), BoneWeights());
	unsigned int nClusterCount = pFBXSkin->GetClusterCount();

	for( unsigned int i = 0; i < nClusterCount; ++i )
	{
		FbxCluster* pFBXCluster = pFBXSkin->GetCluster(i);

		if( !pFBXCluster )
			continue;

		linkMode = pFBXCluster->GetLinkMode();
		FbxNode* pLinkNode = pFBXCluster->GetLink();

		if( !pLinkNode )
			continue;
		
		int nBoneIndex = m_pSkeleton->FindBoneIndex(pLinkNode->GetName());
		if( nBoneIndex < 0 )
			continue;

		SkeletonBone* pSkeletonBone = m_pSkeleton->GetSkeletonBone(nBoneIndex);

		FbxAMatrix matClusterTransformMatrix;
		FbxAMatrix matClusterLinkTransformMatrix;
		pFBXCluster->GetTransformMatrix(matClusterTransformMatrix);
		pFBXCluster->GetTransformLinkMatrix(matClusterLinkTransformMatrix);

		pSkeletonBone->SetBindPoseTransform2(matClusterLinkTransformMatrix);
		pSkeletonBone->SetBoneReferenceTransform2(matClusterTransformMatrix);

		int* indices = pFBXCluster->GetControlPointIndices();
		double* weights = pFBXCluster->GetControlPointWeights();

		for( int j = 0; j < pFBXCluster->GetControlPointIndicesCount(); ++j )
		{
			skinBoneWeights[indices[j]].AddBoneWeight(nBoneIndex, (float)weights[j]);
		}
	}

	switch(linkMode)
	{
	case FbxCluster::eNormalize: //Normalize so weight sum is 1.0.
		for( unsigned int i = 0; i < skinBoneWeights.size(); ++i )
		{
			skinBoneWeights[i].Normalize();
		}
	break;

	case FbxCluster::eAdditive:	//Not supported yet. Do nothing
	break;

	case FbxCluster::eTotalOne:
		//The weight sum should already be 1.0. Do nothing.
	break;
	}

	for( unsigned int i = 0; i < meshBoneWeights.size(); ++i )
	{
		meshBoneWeights[i].AddBoneWeights(skinBoneWeights[i]);
	}	
}

inline std::string GetFilePath(const std::string& str)
{
	std::string::size_type pos = str.find_last_of("/\\");

	if( pos != std::string::npos )
		return str.substr( 0, pos );

	return str;
}

//--------------------------------------------------------------------------------------
void FBXScene::ProcessMaterials(FbxScene* pScene)
{
	for( int i = 0; i < pScene->GetMaterialCount(); ++i )
	{
		Material* pMaterial = new Material(i);

		FbxSurfaceMaterial* pFBXMaterial = pScene->GetMaterial(i);

		FbxProperty diffuseTextureProperty = pFBXMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
		if( diffuseTextureProperty.IsValid() )
		{
			FbxFileTexture* pDiffuseTexture = diffuseTextureProperty.GetSrcObject<FbxFileTexture>(0);

			if( pDiffuseTexture )
			{
				std::string strFileName = pDiffuseTexture->GetFileName();

				if( strFileName.length() == 0 )
					strFileName = pDiffuseTexture->GetRelativeFileName();

				strFileName = GetFileFromPath(strFileName);

				pMaterial->SetDiffuseTextureName(strFileName);
			}
		}
		
		FbxProperty normalTextureProperty = pFBXMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
		if( normalTextureProperty.IsValid() )
		{
				FbxFileTexture* pNormalTexture = normalTextureProperty.GetSrcObject<FbxFileTexture>(0);

				if( pNormalTexture )
				{
					std::string strFileName = pNormalTexture->GetFileName();

					if( strFileName.length() == 0 )
						strFileName = pNormalTexture->GetRelativeFileName();
					
					strFileName = GetFileFromPath(strFileName);

					pMaterial->SetNormalTextureName(strFileName);
				}
		}

		FbxSurfaceLambert* pLambert = FbxCast<FbxSurfaceLambert>(pFBXMaterial);
		FbxSurfacePhong* pPhong = FbxCast<FbxSurfacePhong>(pFBXMaterial); 

		BTHFBX_VEC3 AmbientColor2;
		BTHFBX_VEC3 EmissiveColor2;
		BTHFBX_VEC3 DiffuseColor2;
		BTHFBX_VEC3 SpecularColor2;

		float fSpecularPower = 1.0f;
		float fTransparency = 1.0f;

		if( pLambert )
		{
			AmbientColor2 = GetMaterialColor2(pLambert->Ambient, pLambert->AmbientFactor);
			EmissiveColor2 = GetMaterialColor2(pLambert->Emissive, pLambert->EmissiveFactor);
			DiffuseColor2 = GetMaterialColor2(pLambert->Diffuse, pLambert->DiffuseFactor);

			FbxPropertyT<FbxDouble> FBXTransparencyProperty = pLambert->TransparencyFactor;

			if( FBXTransparencyProperty.IsValid() )
				fTransparency = (float)FBXTransparencyProperty.Get();
		}

		if( pPhong )
		{
			SpecularColor2 = GetMaterialColor2(pPhong->Specular, pPhong->SpecularFactor);

			FbxPropertyT<FbxDouble> FBXSpecularPowerProperty = pPhong->Shininess;

			if( FBXSpecularPowerProperty.IsValid() )
				fSpecularPower = (float)FBXSpecularPowerProperty.Get();
		}

		pMaterial->SetAmbientColor2(AmbientColor2);
		pMaterial->SetEmissiveColor2(EmissiveColor2);
		pMaterial->SetDiffuseColor2(DiffuseColor2);
		pMaterial->SetSpecularColor2(SpecularColor2);

		pMaterial->SetSpecularPower(fSpecularPower);
		pMaterial->SetTransparency(fTransparency);

		pMaterial->AddTexturePath( GetFilePath(this->mFilename) + "/" );

		m_Materials.push_back(pMaterial);
		m_FBXMaterials.push_back(pFBXMaterial);
	}
}

FbxMatrix FBXScene::GetGeometricOffset2(FbxNode* pNode)
{
	if( !pNode )
	{
		FbxAMatrix mat;
		mat.SetIdentity();
		return mat;
	}

	// CheckMe: E Source Set = E Source Pivot?
	FbxVector4 T = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	FbxVector4 R = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	FbxVector4 S = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

	FbxAMatrix matFBXGeometryOffset;
	matFBXGeometryOffset.SetIdentity();
	matFBXGeometryOffset.SetTRS(T,R,S);

	return matFBXGeometryOffset;
}

BTHFBX_VEC2 FBXScene::GetTexCoord(FbxMesh* pFBXMesh, int nLayerIndex, int nPolygonIndex, int nPolygonVertexIndex, int nVertexIndex)
{
	int nLayerCount = pFBXMesh->GetLayerCount();
	if( nLayerIndex < nLayerCount )//for( int i = 0; i < nLayerCount; ++i )
	{
		FbxLayer* pFBXLayer = pFBXMesh->GetLayer(nLayerIndex);

		if( pFBXLayer )
		{
			FbxLayerElementUV* pUVs = pFBXLayer->GetUVs(FbxLayerElement::EType::eTextureDiffuse);
			if( pUVs )
			{
				FbxLayerElement::EMappingMode mappingMode = pUVs->GetMappingMode();
				FbxLayerElement::EReferenceMode referenceMode = pUVs->GetReferenceMode();

				const FbxLayerElementArrayTemplate<FbxVector2>& pUVArray = pUVs->GetDirectArray();
				const FbxLayerElementArrayTemplate<int>& pUVIndexArray = pUVs->GetIndexArray();

				switch(mappingMode)
				{
					case FbxLayerElement::eByControlPoint:
					{
						int nMappingIndex = nVertexIndex;
						switch(referenceMode)
						{
							case FbxLayerElement::eDirect:
								if( nMappingIndex < pUVArray.GetCount() )
								{
									return FBXTexCoordToD3DTexCoord( KFbxVector2ToBTHFBX_VEC2( pUVArray.GetAt(nMappingIndex) ) );
								}
							break;
							case FbxLayerElement::eIndexToDirect:
								if( nMappingIndex < pUVIndexArray.GetCount() )
								{
									int nIndex = pUVIndexArray.GetAt(nMappingIndex);
									if( nIndex < pUVArray.GetCount() )
									{
										return FBXTexCoordToD3DTexCoord( KFbxVector2ToBTHFBX_VEC2( pUVArray.GetAt(nIndex) ) );
									}
								}
							break;
						};
					}
					break;

					case FbxLayerElement::eByPolygonVertex:
					{
						int nMappingIndex = pFBXMesh->GetTextureUVIndex(nPolygonIndex, nPolygonVertexIndex, FbxLayerElement::eTextureDiffuse);
						switch(referenceMode)
						{
						case FbxLayerElement::EReferenceMode::eDirect:
						case FbxLayerElement::EReferenceMode::eIndexToDirect: //I have no idea why the index array is not used in this case.
							if( nMappingIndex < pUVArray.GetCount() )
							{
								return FBXTexCoordToD3DTexCoord( KFbxVector2ToBTHFBX_VEC2( pUVArray.GetAt(nMappingIndex) ) );
							}
						break;
						};
					}
					break;
				};
			}
		}
	}
	return BTHFBX_VEC2();
}

FbxVector4 FBXScene::GetNormal(FbxMesh* pFBXMesh, int nLayerIndex, int nPolygonIndex, int nPolygonVertexIndex, int nVertexIndex)
{
	FbxVector4 vNormal(0, 0, 0, 0);
	int nLayerCount = pFBXMesh->GetLayerCount();
	if( nLayerIndex < nLayerCount )//for( int i = 0; i < nLayerCount; ++i )
	{
		FbxLayer* pFBXLayer = pFBXMesh->GetLayer(nLayerIndex);

		if( pFBXLayer )
		{
			FbxLayerElementNormal* pNormals = pFBXLayer->GetNormals();
		
			if( pNormals )
			{
				int nIdx = 0;
				if( pNormals->GetReferenceMode( ) == FbxLayerElement::EReferenceMode::eDirect )
				{
					nIdx = nVertexIndex;
				}

				if( pNormals->GetReferenceMode( ) == FbxLayerElement::EReferenceMode::eIndexToDirect )
				{
					nIdx = pNormals->GetIndexArray( ).GetAt( nVertexIndex );
				}

				const FbxLayerElementArrayTemplate<FbxVector4>& pNormalArray = pNormals->GetDirectArray();
				if( nIdx < pNormalArray.GetCount() )
				{					
					vNormal = pNormalArray.GetAt(nIdx);
					vNormal.Normalize();
				}
			}
		}

	}
	return vNormal;
}

FbxVector4 FBXScene::GetTangent(FbxMesh* pFBXMesh, int nLayerIndex, int nPolygonIndex, int nPolygonVertexIndex, int nVertexIndex)
{
	FbxVector4 vTangent(0,0,0,0);
	int nLayerCount = pFBXMesh->GetLayerCount();
	if( nLayerIndex < nLayerCount )//for( int i = 0; i < nLayerCount; ++i )
	{
		FbxLayer* pFBXLayer = pFBXMesh->GetLayer(nLayerIndex);

		if( pFBXLayer )
		{
			FbxLayerElementTangent* pTangents = pFBXLayer->GetTangents();

			if( pTangents )
			{
				const FbxLayerElementArrayTemplate<FbxVector4>& pTangentArray = pTangents->GetDirectArray();

				if( nVertexIndex < pTangentArray.GetCount() )
				{
					
					vTangent = pTangentArray.GetAt(nVertexIndex);
					vTangent.Normalize();
				}
			}
		}

	}
	return vTangent;
}

BTHFBX_VEC3 FBXScene::GetMaterialColor2(FbxPropertyT<FbxDouble3> FBXColorProperty, FbxPropertyT<FbxDouble> FBXFactorProperty)
{
	BTHFBX_VEC3 Color;
	
	if( FBXColorProperty.IsValid() )
	{
		FbxDouble3 FBXColor = FBXColorProperty.Get();
		Color = BTHFBX_VEC3( (float)FBXColor[0], (float)FBXColor[1], (float)FBXColor[2] );
		
		if( FBXFactorProperty.IsValid() )
		{
			float FBXFactor = (float)FBXFactorProperty.Get();
			Color.x *= FBXFactor;
			Color.y *= FBXFactor;
			Color.z *= FBXFactor;
		}
	}
	return Color;
}

Material* FBXScene::GetMaterialLinkedWithPolygon(FbxMesh* pFBXMesh, int nLayerIndex, int nPolygonIndex, int nPolygonVertexIndex, int nVertexIndex)
{
	if( nLayerIndex < 0 || nLayerIndex > pFBXMesh->GetLayerCount() )
		return NULL;

	FbxNode* pNode = pFBXMesh->GetNode();

	if( !pNode )
		return NULL;

	FbxLayerElementMaterial* pFBXMaterial = pFBXMesh->GetLayer(nLayerIndex)->GetMaterials();

	if( pFBXMaterial )
	{
		int nMappingIndex = GetMappingIndex( pFBXMaterial->GetMappingMode(), nPolygonIndex, 0, nVertexIndex );

		if( nMappingIndex < 0 )
			return NULL;

		FbxLayerElement::EReferenceMode referenceMode = pFBXMaterial->GetReferenceMode();

		
		if( referenceMode == FbxLayerElement::EReferenceMode::eDirect )
		{
			if( nMappingIndex < pNode->GetMaterialCount() )
			{
				return GetMaterial(pNode->GetMaterial(nMappingIndex));
			}
		}
		else if( referenceMode == FbxLayerElement::EReferenceMode::eIndexToDirect )
		{
			const FbxLayerElementArrayTemplate<int>& pMaterialIndexArray = pFBXMaterial->GetIndexArray();

			if( nMappingIndex < pMaterialIndexArray.GetCount() )
			{
				int nIndex = pMaterialIndexArray.GetAt(nMappingIndex);
				if( nIndex < pNode->GetMaterialCount() )
				{
					return GetMaterial(pNode->GetMaterial(nIndex));
				}
			}
		}
	}
	
	return NULL;
}

int FBXScene::GetMappingIndex(FbxLayerElement::EMappingMode MappingMode, int nPolygonIndex, int nPolygonVertexIndex, int nVertexIndex)
{
	switch(MappingMode)
	{
		case FbxLayerElement::EMappingMode::eAllSame:
			return 0;
		break;

		case FbxLayerElement::EMappingMode::eByControlPoint:
			return nVertexIndex;
		break;

		case FbxLayerElement::EMappingMode::eByPolygonVertex:
			return nPolygonIndex*3 + nPolygonVertexIndex;
		break;

		case FbxLayerElement::EMappingMode::eByPolygon:
			return nPolygonIndex;
		break;

		case FbxLayerElement::EMappingMode::eNone:
		case FbxLayerElement::EMappingMode::eByEdge:
		break;
	}
	return -1;
}

Material* FBXScene::GetMaterial(FbxSurfaceMaterial* pFBXMaterial)
{
	for( size_t i = 0; i < m_FBXMaterials.size(); ++i )
	{
		if( m_FBXMaterials[i] == pFBXMaterial )
		{
			return m_Materials[i];
		}
	}
	return NULL;
}

FbxMatrix FBXScene::GetAbsoluteTransformFromCurrentTake2(FbxNode* pNode, FbxTime time)
{
	if( !pNode )
	{
		FbxMatrix mat;
		mat.SetIdentity();
		return mat;
	}
	
	FbxAMatrix matFBXGlobal = mScene->GetEvaluator()->GetNodeGlobalTransform(pNode, time);

	return matFBXGlobal;
}

void FBXScene::DisplayMetaData()
{
    FbxDocumentInfo* sceneInfo = mScene->GetSceneInfo();
    if (sceneInfo)
    {
        printf("\n\n--------------------\nMeta-Data\n--------------------\n\n");
        printf("    Title: %s\n", sceneInfo->mTitle.Buffer());
        printf("    Subject: %s\n", sceneInfo->mSubject.Buffer());
        printf("    Author: %s\n", sceneInfo->mAuthor.Buffer());
        printf("    Keywords: %s\n", sceneInfo->mKeywords.Buffer());
        printf("    Revision: %s\n", sceneInfo->mRevision.Buffer());
        printf("    Comment: %s\n", sceneInfo->mComment.Buffer());

        FbxThumbnail* thumbnail = sceneInfo->GetSceneThumbnail();
        if (thumbnail)
        {
            printf("    Thumbnail:\n");

            switch (thumbnail->GetDataFormat())
            {
            case FbxThumbnail::eRGB_24:
                printf("        Format: RGB\n");
                break;
            case FbxThumbnail::eRGBA_32:
                printf("        Format: RGBA\n");
                break;
            }

            switch (thumbnail->GetSize())
            {
			case FbxThumbnail::eNotSet:
                printf("        Size: no dimensions specified (%ld bytes)\n", thumbnail->GetSizeInBytes());
                break;
            case FbxThumbnail::e64x64:
                printf("        Size: 64 x 64 pixels (%ld bytes)\n", thumbnail->GetSizeInBytes());
                break;
            case FbxThumbnail::e128x128:
                printf("        Size: 128 x 128 pixels (%ld bytes)\n", thumbnail->GetSizeInBytes());
            }
        }
    }
}