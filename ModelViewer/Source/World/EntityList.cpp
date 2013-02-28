#include "EntityList.h"
#include <vector>
#include <string>
#include <fstream>


struct EntInfo
{
	std::string entName;
	float blockRadius;
	float billboardDistance;
	std::vector< std::string > billboards;
	std::vector< std::string > models;
};

static std::vector<EntInfo> entities;

void LoadEntList( const std::string& fileName ) throw(...)
{
	// Clear Old Info
	entities.clear();

	std::ifstream file(fileName);
	if ( !file.is_open() ) throw("File Not Found!");

	unsigned int curNumber = 0;
	while( !file.eof() )
	{
		std::string curLine;
		std::getline(file, curLine);
		
		// Force lowercase
		for( unsigned int x=0; x<curLine.size(); ++x )
		{
			curLine[x] = (char)tolower(curLine[x]);
		}

		if ( curLine.find("number:") == 0 )
		{
			curNumber = atoi(curLine.substr(7, curLine.length() - 7).c_str());
			if ( entities.size() < curNumber ) entities.resize(curNumber);
		}
		else if ( curLine.find("name:") == 0 )
		{
			entities[curNumber-1].entName = curLine.substr(5, curLine.length() - 5);
		}
		else if ( curLine.find("model:") == 0 )
		{
			entities[curNumber-1].models.push_back(curLine.substr(6, curLine.length() - 6));
		}
		else if ( curLine.find("billboarddistance:") == 0 )
		{
			sscanf_s(curLine.c_str(), "billboarddistance:%f", &entities[curNumber-1].billboardDistance);
		}
		else if ( curLine.find("billboard:") == 0 )
		{
			entities[curNumber-1].billboards.push_back(curLine.substr(10, curLine.length() - 10));
		}
		else if ( curLine.find("blockradius:") == 0 )
		{
			sscanf_s(curLine.c_str(), "blockradius:%f", &entities[curNumber-1].blockRadius);
		}
	}

	file.close();
}

const std::string& GetEntName( unsigned int entIndex ) throw(...)
{
	if ( entIndex-1 >= entities.size() ) throw("Index Out Of Bounds!");
	return entities[entIndex-1].entName;
}

const std::string& GetEntModel( unsigned int entIndex ) throw(...)
{
	if ( entIndex-1 >= entities.size() ) throw("Index Out Of Bounds!");
	
	static const std::string noModel = "";
	if ( entities[entIndex-1].models.empty() )
		return noModel;

	unsigned int r = rand()%entities[entIndex-1].models.size();
	return entities[entIndex-1].models[r];
}

const float& GetEntBlockRadius( unsigned int entIndex ) throw(...)
{
	if ( entIndex-1 >= entities.size() ) throw("Index Out Of Bounds!");
	return entities[entIndex-1].blockRadius;
}

const float& GetEntBillboardDistance( unsigned int entIndex ) throw(...)
{
	if ( entIndex-1 >= entities.size() ) throw("Index Out Of Bounds!");
	return entities[entIndex-1].billboardDistance;
}

unsigned int GetEntListSize()
{
	return entities.size();
}

const std::string& GetEntBillboard( unsigned int entIndex ) throw(...)
{
	if ( entIndex-1 >= entities.size() ) throw("Index Out Of Bounds!");

	static const std::string emptyString = "";
	if ( entities[entIndex-1].billboards.empty() )
		return emptyString;

	unsigned int r = rand()%entities[entIndex-1].billboards.size();
	return entities[entIndex-1].billboards[r];
}
