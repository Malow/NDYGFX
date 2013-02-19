#include "Caching.h"
#include <fstream>
#include <set>


bool LoadCachedBoneWeights( const std::string& fileName, std::vector<BoneWeights>& weights )
{
	std::ifstream file(fileName+".cached.weights", std::ios::binary);

	// Failed Opening File
	if ( !file.good() ) return false;

	// Read Number of vertices
	unsigned int numVertices;
	file.read( reinterpret_cast<char*>(&numVertices), sizeof(size_t) );
	weights.resize(numVertices);

	for( unsigned int v = 0; v < numVertices; ++v )
	{
		// Read Number of Weights
		unsigned int numWeights;
		file.read( reinterpret_cast<char*>(&numWeights), sizeof(size_t) );
		for( unsigned int w = 0; w < numWeights; ++w )
		{
			std::pair<int, float> data;
			file.read( reinterpret_cast<char*>(&data.first), sizeof(int) );
			file.read( reinterpret_cast<char*>(&data.second), sizeof(float) );
			weights[v].AddBoneWeight(data);
		}
	}

	// Close File
	file.close();

	return true;
}

bool CacheBoneWeights( const std::string& fileName, const std::vector<BoneWeights>& weights )
{	
	// Create File
	std::ofstream file(fileName+".cached.weights", std::ios::trunc | std::ios::binary);

	// Failed Opening File
	if ( !file.good() ) return false;

	// Store Number of Vertices
	unsigned int numVertices = weights.size();
	file.write( reinterpret_cast<const char*>(&numVertices), sizeof(size_t) );

	for( unsigned int v = 0; v < numVertices; ++v )
	{
		// Store Number of Weights
		unsigned int numWeights = weights[v].GetBoneWeightCount();
		file.write( reinterpret_cast<const char*>(&numWeights), sizeof(size_t) );

		// Store Weight Data
		for( unsigned int w = 0; w < numWeights; ++w )
		{
			auto data = weights[v].GetBoneWeight(w);
			file.write( reinterpret_cast<const char*>(&data.first), sizeof(int) );
			file.write( reinterpret_cast<const char*>(&data.second), sizeof(float) );
		}
	}

	// Close File
	file.close();

	return true;
}

bool LoadCachedScene(const std::string& fileName, FBXScene* scene)
{
	// TODO: Make Cache Loading
	return false;

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

			// Read Number of Vertices
			// Read Positions
			// Read Normals
			// Read UV
			// Read Tangents
			// Read Bone Weights
			// Read Index Count
			// Read Indices


			// Model Part
			ModelPart* modelPart = new ModelPart(model, x, materialMap[matIdentifier]);

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
		file.read(reinterpret_cast<char*>(matIdentifier), sizeof(IBTHFbxMaterial*));
		
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
		i->second->SetDiffuseTextureName(normalMapName);
		
		// Read Specular Multiplier
		BTHFBX_VEC3 specular;
		file.read(reinterpret_cast<char*>(&specular), sizeof(BTHFBX_VEC3));
		i->second->SetSpecularColor2(specular);

		// Insert Material
		scene->getMaterials().push_back(i->second);
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
		Model* m = scene->GetModel(x);
		
		// Write Model Name
		unsigned int nameLength = strlen(m->GetName());
		file.write(reinterpret_cast<const char*>(&nameLength), sizeof(unsigned int));
		file.write(m->GetName(), nameLength);

		// Write Model Parts
		unsigned int modelPartCount = m->GetModelPartCount();
		file.write(reinterpret_cast<const char*>(&modelPartCount), sizeof(unsigned int));
		for( unsigned int y = 0; y < modelPartCount; ++y )
		{
			ModelPart* mp = dynamic_cast<ModelPart*>(m->GetModelPart(y));
			
			// Write Material Index
			IBTHFbxMaterial* matPtr = mp->GetMaterial();
			materials.insert(matPtr);
			file.write(reinterpret_cast<const char*>(&matPtr), sizeof(IBTHFbxMaterial*));

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
		file.write(reinterpret_cast<const char*>(*i), sizeof(IBTHFbxMaterial*));
		
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

	// Close File
	file.close();

	return true;
}