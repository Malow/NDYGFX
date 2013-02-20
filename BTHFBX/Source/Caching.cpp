#include "Caching.h"
#include <fstream>
#include <set>


bool LoadCachedScene(const std::string& fileName, FBXScene* scene)
{
	std::ifstream file(fileName+".cached", std::ios::binary);

	// Failed Opening File
	if ( !file.good() ) return false;

	// Temp Variables
	std::map< unsigned int, Material* > materialMap;

	// Read Number of Models
	unsigned int modelCount = 0;
	file.read(reinterpret_cast<char*>(&modelCount), sizeof(unsigned int));
	for( unsigned int x=0; x<modelCount; ++x )
	{		
		// Read Model Name
		unsigned int nameLength = 0;
		std::string modelName;
		file.read(reinterpret_cast<char*>(&nameLength), sizeof(unsigned int));
		modelName.resize(nameLength);
		file.read(&modelName[0], nameLength);

		// Model Object
		Model* model = new Model(modelName, x, false);

		// Read Geometric Offset
		file.read(reinterpret_cast<char*>(model->GetGeometricOffset()), sizeof(BTHFBX_MATRIX));

		// Read Animation
		unsigned int animationCount;
		file.read(reinterpret_cast<char*>(&animationCount), sizeof(unsigned int));
		for( unsigned int y=0; y < animationCount; ++y )
		{
			// Read Name
			unsigned int animationNameLength;
			file.read(reinterpret_cast<char*>(&animationNameLength), sizeof(unsigned int));
			std::string animationName;
			animationName.resize(animationNameLength);
			file.read(&animationName[0], animationNameLength);

			// Read Num Key Frames
			unsigned int numFrames = 0;
			file.read(reinterpret_cast<char*>(&numFrames), sizeof(unsigned int));

			// Key Frames
			AnimationKeyFrames* keyFrames = new AnimationKeyFrames(animationName, numFrames);

			// Read Key Frames
			file.read(reinterpret_cast<char*>(&keyFrames->GetKeyFrameTransform2(0)), sizeof(FbxMatrix) * numFrames);

			// Add Animation To Model
			model->AddAnimationKeyFrames(keyFrames);
		}

		// Model Parts
		unsigned int modelPartCount = 0;
		file.read(reinterpret_cast<char*>(&modelPartCount), sizeof(unsigned int));
		for( unsigned int y = 0; y < modelPartCount; ++y )
		{
			// Read Material Index
			unsigned int matIdentifier = 0;
			file.read(reinterpret_cast<char*>(&matIdentifier), sizeof(unsigned int));
			
			// Try Finding Identifier
			auto i = materialMap.find(matIdentifier);
			if ( i == materialMap.cend() )
			{
				unsigned int newMatIndex = materialMap.size();
				materialMap[matIdentifier] = new Material(newMatIndex);
			}

			// Model Part
			ModelPart* modelPart = new ModelPart(model, x, materialMap[matIdentifier]);

			// Read Number of Vertices
			unsigned int numVertices;
			file.read(reinterpret_cast<char*>(&numVertices), sizeof(unsigned int));
			modelPart->GetPositions().resize(numVertices);
			modelPart->GetNormals().resize(numVertices);
			modelPart->GetTexCoords().resize(numVertices);
			modelPart->GetTangents().resize(numVertices);
			modelPart->GetBoneWeights().resize(numVertices);

			// Read Positions
			file.read(reinterpret_cast<char*>(&modelPart->GetPositions()[0]), sizeof(BTHFBX_VEC3)*numVertices);

			// Read Normals
			file.read(reinterpret_cast<char*>(&modelPart->GetNormals()[0]), sizeof(BTHFBX_VEC3)*numVertices);
			
			// Read UV
			file.read(reinterpret_cast<char*>(&modelPart->GetTexCoords()[0]), sizeof(BTHFBX_VEC2)*numVertices);
			
			// Read Tangents
			file.read(reinterpret_cast<char*>(&modelPart->GetTangents()[0]), sizeof(BTHFBX_VEC3)*numVertices);
			
			// Read Bone Weights
			file.read(reinterpret_cast<char*>(&modelPart->GetBoneWeights()[0]), sizeof(BTHFBX_BLEND_WEIGHT_DATA)*numVertices);

			// Read Index Count
			unsigned int indexCount;
			file.read(reinterpret_cast<char*>(&indexCount), sizeof(unsigned int));
			modelPart->GetIndices().resize(indexCount);

			// Read Indices
			file.read(reinterpret_cast<char*>(&modelPart->GetIndices()[0]), sizeof(unsigned int)*indexCount);

			// Insert To Model
			model->GetModelParts().push_back( modelPart );
		}

		// Add To Scene
		scene->getModels().Add( model->GetName(), model );
	}

	// Read Material Count
	unsigned int numMaterials = 0;
	file.read(reinterpret_cast<char*>(&numMaterials), sizeof(unsigned int));

	// Read Materials
	for( unsigned int x=0; x<numMaterials; ++x )
	{
		// Read Material Identifier
		unsigned int matIdentifier = 0;
		file.read(reinterpret_cast<char*>(&matIdentifier), sizeof(unsigned int));
		
		// Find Material
		auto i = materialMap.find(matIdentifier);
		if ( i == materialMap.cend() ) continue;

		// Read Ambient Multiplier
		BTHFBX_VEC3 ambient;
		file.read(reinterpret_cast<char*>(&ambient), sizeof(BTHFBX_VEC3));
		i->second->SetAmbientColor2(ambient);

		// Read Diffuse Multiplier
		BTHFBX_VEC3 diffuse;
		file.read(reinterpret_cast<char*>(&diffuse), sizeof(BTHFBX_VEC3));
		i->second->SetDiffuseColor2(diffuse);

		// Read Diffuse Texture
		unsigned int diffuseNameLength = 0;
		file.read(reinterpret_cast<char*>(&diffuseNameLength), sizeof(unsigned int));
		std::string diffuseName;
		diffuseName.resize(diffuseNameLength);
		file.read(&diffuseName[0], diffuseNameLength);
		i->second->SetDiffuseTextureName(diffuseName);

		// Read Emissive Multiplier
		BTHFBX_VEC3 emissive;
		file.read(reinterpret_cast<char*>(&emissive), sizeof(BTHFBX_VEC3));
		i->second->SetEmissiveColor2(emissive);

		// Read Normal Map
		unsigned int normalMapLength = 0;
		file.read(reinterpret_cast<char*>(&normalMapLength), sizeof(unsigned int));
		std::string normalMapName;
		normalMapName.resize(normalMapLength);
		file.read(&normalMapName[0], normalMapLength);
		i->second->SetNormalTextureName(normalMapName);
		
		// Read Specular Multiplier
		BTHFBX_VEC3 specular;
		file.read(reinterpret_cast<char*>(&specular), sizeof(BTHFBX_VEC3));
		i->second->SetSpecularColor2(specular);

		// Insert Material
		scene->GetMaterials().push_back(i->second);
	}

	// Load Skeleton
	if ( !file.eof() )
	{
		// Read Bones Count
		unsigned int numBones;
		file.read(reinterpret_cast<char*>(&numBones), sizeof(unsigned int));

		// Skeleton
		Skeleton* skeleton = new Skeleton();

		// Read Bones
		for( unsigned int x=0; x < numBones; ++x )
		{
			// Read Bone Name
			unsigned int boneNameLength;
			file.read(reinterpret_cast<char*>(&boneNameLength), sizeof(unsigned int));
			std::string boneName;
			boneName.resize(boneNameLength);
			file.read(&boneName[0], boneNameLength);

			// Read Bone Parent Name
			unsigned int parentBoneNameLength;
			file.read(reinterpret_cast<char*>(&parentBoneNameLength), sizeof(unsigned int));
			std::string parentBoneName;
			parentBoneName.resize(parentBoneNameLength);
			file.read(&parentBoneName[0], parentBoneNameLength);

			// Find Parent
			int parentIndex = -1;
			if ( !parentBoneName.empty() )
			{
				parentIndex = skeleton->FindBoneIndex(parentBoneName);
			}

			// Bone
			SkeletonBone* bone = new SkeletonBone(boneName, parentIndex, skeleton);

			// Read Animations
			unsigned int numAnimations = 0;
			file.read(reinterpret_cast<char*>(&numAnimations), sizeof(unsigned int));
			for( unsigned int y = 0; y < numAnimations; ++y )
			{
				// Read Animation Name
				unsigned int animationNameSize;
				file.read(reinterpret_cast<char*>(&animationNameSize), sizeof(unsigned int));
				std::string animationName;
				animationName.resize(animationNameSize);
				if ( animationNameSize ) file.read(&animationName[0], animationNameSize);

				// Read Number Of KeyFrames
				unsigned int numKeyFrames;
				file.read(reinterpret_cast<char*>(&numKeyFrames), sizeof(unsigned int));

				// Animation KeyFrames
				AnimationKeyFrames* keyFrames = new AnimationKeyFrames(animationName, numKeyFrames);

				// Read Transformations
				if ( numKeyFrames ) file.read( reinterpret_cast<char*>(&keyFrames->GetKeyFrameTransform2(0)), sizeof(FbxMatrix) * numKeyFrames );
				
				// Animation
				bone->AddAnimationKeyFrames(keyFrames);
			}

			// Insert Bone Into Skeleton
			skeleton->AddSkeletonBone(bone);
		}

		// Build Bone Hierarchy
		skeleton->BuildBoneHierarchy();

		// Set Scene Skeleton
		scene->SetSkeleton(skeleton);

		// Read Animation Count
		unsigned int numAnimations;
		file.read(reinterpret_cast<char*>(&numAnimations), sizeof(unsigned int));
		if ( numAnimations ) 
		{
			// Animation Controller
			AnimationController* animations = new AnimationController();

			// Read Animations
			for( unsigned int x=0; x<numAnimations; ++x )
			{
				// Read Animation Name
				unsigned int animationNameLength;
				file.read(reinterpret_cast<char*>(&animationNameLength), sizeof(unsigned int));
				std::string animationName;
				animationName.resize(animationNameLength);
				if ( animationNameLength ) file.read(&animationName[0], animationNameLength);

				// Read Default Framerate
				float defaultFrameRate;
				file.read(reinterpret_cast<char*>(&defaultFrameRate), sizeof(float));

				// Read Animation KeyFrame Length
				unsigned int numKeyFrames;
				file.read(reinterpret_cast<char*>(&numKeyFrames), sizeof(unsigned int));

				// Animation
				animations->AddAnimation(new Animation(animationName, numKeyFrames, defaultFrameRate));
			}

			scene->SetAnimationController( animations );
		}
	}

	// Close File
	file.close();

	return true;
}

bool CacheScene(const std::string& fileName, FBXScene* scene)
{
	// Create File
	std::ofstream file(fileName+".cached", std::ios::trunc | std::ios::binary);

	// Temporary Data
	std::set< IBTHFbxMaterial* > materials; 

	// Failed Opening File
	if ( !file.good() ) return false;
	
	// Write Model Count
	unsigned int modelCount = scene->GetModelCount();
	file.write(reinterpret_cast<const char*>(&modelCount), sizeof(unsigned int));
	for( unsigned int x = 0; x < modelCount; ++x )
	{
		// Model
		Model* model = scene->GetModel(x);
		
		// Write Model Name
		unsigned int nameLength = strlen(model->GetName());
		file.write(reinterpret_cast<const char*>(&nameLength), sizeof(unsigned int));
		file.write(model->GetName(), nameLength);

		// Write Geometric Offset
		file.write(reinterpret_cast<const char*>(model->GetGeometricOffset()), sizeof(BTHFBX_MATRIX));

		// Write Animation
		unsigned int animationCount = model->GetAnimationCount();
		file.write(reinterpret_cast<const char*>(&animationCount), sizeof(unsigned int));
		for( unsigned int y=0; y < animationCount; ++y )
		{
			// Key Frames
			AnimationKeyFrames* keyFrames = model->GetAnimationKeyFrames(y);

			// Write Name
			unsigned int animationNameLength = keyFrames->GetAnimationName().length();
			file.write(reinterpret_cast<const char*>(&animationNameLength), sizeof(unsigned int));
			file.write(&keyFrames->GetAnimationName()[0], animationNameLength);

			// Write Key Frames
			unsigned int numFrames = keyFrames->GetFrameCount();
			file.write(reinterpret_cast<const char*>(&numFrames), sizeof(unsigned int));
			file.write(reinterpret_cast<const char*>(&keyFrames->GetKeyFrameTransform2(0)), sizeof(FbxMatrix) * numFrames);
		}

		// Write Model Parts
		unsigned int modelPartCount = model->GetModelPartCount();
		file.write(reinterpret_cast<const char*>(&modelPartCount), sizeof(unsigned int));
		for( unsigned int y = 0; y < modelPartCount; ++y )
		{
			ModelPart* mp = dynamic_cast<ModelPart*>(model->GetModelPart(y));
			
			// Write Material Index
			unsigned int matPtr = (unsigned int)mp->GetMaterial();
			file.write(reinterpret_cast<const char*>(&matPtr), sizeof(unsigned int));
			materials.insert(mp->GetMaterial());

			// Write Number of Vertices
			unsigned int vertexCount = mp->GetVertexCount();
			file.write(reinterpret_cast<const char*>(&vertexCount), sizeof(unsigned int) );

			// Write Positions
			file.write(reinterpret_cast<const char*>(mp->GetVertexPositionData()), sizeof(BTHFBX_VEC3) * vertexCount);

			// Write Normals
			file.write(reinterpret_cast<const char*>(mp->GetVertexNormalData()), sizeof(BTHFBX_VEC3) * vertexCount);

			// Write UV
			file.write(reinterpret_cast<const char*>(mp->GetVertexTexCoordData()), sizeof(BTHFBX_VEC2) * vertexCount);
			
			// Write Tangents
			file.write(reinterpret_cast<const char*>(mp->GetVertexTangentData()), sizeof(BTHFBX_VEC3) * vertexCount);
			
			// Write Bone Weights
			file.write(reinterpret_cast<const char*>(mp->GetVertexBoneWeightData()), sizeof(BTHFBX_BLEND_WEIGHT_DATA) * vertexCount);
		
			// Write Index Count
			unsigned int indexCount = mp->GetIndexCount();
			file.write(reinterpret_cast<const char*>(&indexCount), sizeof(unsigned int));

			// Write Indices
			file.write(reinterpret_cast<const char*>(mp->GetIndexData()), sizeof(unsigned int) * indexCount);
		}
	}

	// Write Material Count
	unsigned int numMaterials = materials.size();
	file.write(reinterpret_cast<const char*>(&numMaterials), sizeof(unsigned int));

	// Write Materials
	for( auto i = materials.begin(); i != materials.end(); ++i )
	{
		// Write Material Identifier
		unsigned int matIdentifier = (unsigned int)(*i);
		file.write(reinterpret_cast<const char*>(&matIdentifier), sizeof(unsigned int));
		
		// Write Ambient Multiplier
		BTHFBX_VEC3 ambient = (*i)->GetAmbientColor();
		file.write(reinterpret_cast<const char*>(&ambient), sizeof(BTHFBX_VEC3));

		// Write Diffuse Multiplier
		BTHFBX_VEC3 diffuse = (*i)->GetDiffuseColor();
		file.write(reinterpret_cast<const char*>(&diffuse), sizeof(BTHFBX_VEC3));

		// Write Diffuse Texture
		unsigned int diffuseNameLength = strlen((*i)->GetDiffuseTextureFilename());
		file.write(reinterpret_cast<const char*>(&diffuseNameLength), sizeof(unsigned int));
		file.write((*i)->GetDiffuseTextureFilename(), diffuseNameLength);

		// Write Emissive Multiplier
		BTHFBX_VEC3 emissive = (*i)->GetEmissiveColor();
		file.write(reinterpret_cast<const char*>(&emissive), sizeof(BTHFBX_VEC3));

		// Write Normal Map
		unsigned int normalMapLength = strlen((*i)->GetNormalTextureFilename());
		file.write(reinterpret_cast<const char*>(&normalMapLength), sizeof(unsigned int));
		file.write((*i)->GetNormalTextureFilename(), normalMapLength);

		// Write Specular Multiplier
		BTHFBX_VEC3 specular = (*i)->GetSpecularColor();
		file.write(reinterpret_cast<const char*>(&specular), sizeof(BTHFBX_VEC3));
	}

	if ( Skeleton* skeleton = scene->GetSkeleton() )
	{
		// Write Bones Count
		unsigned int numBones = skeleton->GetBoneCount();
		file.write(reinterpret_cast<const char*>(&numBones), sizeof(unsigned int));

		// Write Bones
		for( unsigned int x=0; x < numBones; ++x )
		{
			// Bone
			SkeletonBone* bone = skeleton->GetSkeletonBone(x);

			// Write Bone Name
			unsigned int boneNameLength = strlen(bone->GetName());
			file.write(reinterpret_cast<const char*>(&boneNameLength), sizeof(unsigned int));
			if ( boneNameLength ) file.write(bone->GetName(), boneNameLength);

			// Parent Bone
			int parentID = bone->GetParentBoneIndex();
			SkeletonBone* parentBone = skeleton->GetSkeletonBone(parentID);

			// Find Bone Parent Name
			// Note: Root Node has ParentID of -1
			unsigned int parentNameLength = 0;
			if ( parentBone )
			{
				parentNameLength = strlen(parentBone->GetName());
			}

			// Write Bone Parent Name
			file.write(reinterpret_cast<const char*>(&parentNameLength), sizeof(unsigned int));
			if ( parentNameLength ) file.write(parentBone->GetName(), parentNameLength);

			// Animations
			auto animations = bone->GetAnimations();
			unsigned int numAnimations = animations.size();
			file.write(reinterpret_cast<const char*>(&numAnimations), sizeof(unsigned int));
			for( auto i = animations.begin(); i != animations.end(); ++i )
			{
				// Current Animation
				auto animation = i->second;

				// Write Animation Name
				unsigned int animationNameSize = animation->GetAnimationName().size();
				file.write(reinterpret_cast<const char*>(&animationNameSize), sizeof(unsigned int));
				if ( animationNameSize ) file.write(&animation->GetAnimationName()[0], animationNameSize);

				// Write KeyFrames
				unsigned int keyFrames = animation->GetFrameCount();
				file.write(reinterpret_cast<const char*>(&keyFrames), sizeof(unsigned int));
				if ( keyFrames ) file.write( reinterpret_cast<const char*>(&animation->GetKeyFrameTransform2(0)), sizeof(FbxMatrix) * keyFrames );
			}
		}

		// Animation Controller
		AnimationController* animations = scene->GetAnimationController();

		// Write Animation Count
		unsigned int numAnimations = animations->GetAnimationCount();
		file.write(reinterpret_cast<const char*>(&numAnimations), sizeof(unsigned int));

		// Write Animations
		for( unsigned int x=0; x<numAnimations; ++x )
		{
			// Animation
			Animation* animation = animations->GetAnimation(x);

			// Write Animation Name
			unsigned int nameLength = animation->GetName().length();
			file.write(reinterpret_cast<const char*>(&nameLength), sizeof(unsigned int));
			if ( nameLength ) file.write(&animation->GetName()[0], nameLength);

			// Write Default Framerate
			float defaultFrameRate = animation->GetDefaultFrameRate();
			file.write(reinterpret_cast<const char*>(&defaultFrameRate), sizeof(float));

			// Write Animation KeyFrame Length
			unsigned int numKeyFrames = animation->GetKeyFrames();
			file.write(reinterpret_cast<const char*>(&numKeyFrames), sizeof(unsigned int));
		}
	}

	// Close File
	file.close();

	return true;
}