//--------------------------------------------------------------------------------------------------
//
//	Written by Markus Tillman for project "Not dead yet" at Blekinge Tekniska Högskola.
// 
//	Manager for handling resources. Resources include (but are not limited to):
//	Vertex buffers and textures.
//--------------------------------------------------------------------------------------------------
#include "Texture.h"
#include "ReferenceCounted.h"

class ResourceManager 
{
public:
	unsigned int zNrOfResources;
	//unsi
public:
	ResourceManager();
/*#include <map>
#include <iostream>
#include <string>

	typedef unsigned int texture;

	int main()
	{
		std::map< std::string, texture > textures;

		auto tex = textures.find("Hello.png");
		if ( tex == textures.end() )
		{
			textures["Hello.png"] = 2;
		}

		std::cout << textures["Hello.png"] << std::endl;

		return textures["Hello.png"];
	}
	*/
};
