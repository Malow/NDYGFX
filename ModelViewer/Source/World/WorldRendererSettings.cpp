#include "WorldRendererSettings.h"
#include <fstream>


WorldRendererSettings::WorldRendererSettings(Observer* observer, const std::string& fileName) :
	Observed(observer),
	zSettingsFile(fileName),
	zSameAsFile(true)
{
	// Grass Default Values
	zSettings["GrassDensity"] = 2500u;
	zSettings["GrassNearDistance"] = 2.0f;
	zSettings["GrassFarDistance"] = 64.0f;
	zSettings["GrassUpdateDistance"] = 1.0f;
	zSettings["GrassWidthMin"] = 0.5f;
	zSettings["GrassWidthMax"] = 1.0f;
	zSettings["GrassHeightMin"] = 0.25f;
	zSettings["GrassHeightMax"] = 0.5f;
	zSettings["GrassMinNeightbourDistance"] = 0.2f;

	// LOD Default Settings
	zSettings["LODUpdateDistance"] = 10.0f;

	// Read From File
	if( !zSettingsFile.empty() )
	{
		// Try Reading From File
		std::ifstream settingsFile(zSettingsFile);

		// Read Values
		while ( settingsFile.good() )
		{
			// Line
			std::string line;

			// Read Line
			std::getline(settingsFile, line);

			// Find = Sign
			unsigned int equalPos = line.find('=');
			if ( equalPos != std::string::npos )
			{
				std::string var = line.substr(0, equalPos-1);
				std::string val = line.substr(equalPos+2, line.size()-(equalPos+1));

				if ( *val.rbegin() == 'f' )
				{
					float fVal;
					if ( sscanf_s(val.c_str(), "%f", &fVal) )
					{
						SetValue(var, fVal);
					}
				}
				else if ( *val.rbegin() == 'u' )
				{
					unsigned int uVal;
					if ( sscanf_s(val.c_str(), "%u", &uVal) )
					{
						SetValue(var, uVal);
					}
				}
				else
				{
					int sVal;
					if ( sscanf_s(val.c_str(), "%d", &sVal) )
					{
						SetValue(var, sVal);
					}
				}
			}
		}

		zSameAsFile = true;
		settingsFile.close();
	}
}

WorldRendererSettings::~WorldRendererSettings()
{
	if ( !zSettingsFile.empty() && !zSameAsFile )
	{
		// Try Reading From File
		std::ofstream settingsFile(zSettingsFile);

		// Write Values
		if ( settingsFile.good() )
		{
			for( auto i = zSettings.cbegin(); i != zSettings.cend(); ++i )
			{
				if ( i->second.curType != ANY_TYPE_NONE )
				{
					settingsFile << i->first << " = ";

					if ( i->second.curType == ANY_TYPE_FLOAT )
					{
						settingsFile << i->second.value.f << "f";
					}
					
					else if ( i->second.curType == ANY_TYPE_UINT )
					{
						settingsFile << i->second.value.uint << "u";
					}
					else if ( i->second.curType == ANY_TYPE_SINT )
					{
						settingsFile << i->second.value.sint;
					}

					settingsFile << std::endl;
				}
			}
		}

		settingsFile.close();
	}
}

bool WorldRendererSettings::SetValue( const std::string& var, const AnyType& val )
{
	// Check if same value as before
	if ( HasSetting(var) && GetSetting(var) == val ) return true;
	
	// Ask if Ok
	WorldRendererSettingChanging okToChange(var, val);
	NotifyObservers(&okToChange);

	// All Good
	if ( okToChange.IsAccepted() )
	{
		// Set Value
		zSettings[var] = val;
		zSameAsFile = false;

		// Notify Change
		WorldRendererSettingChanged changed(var, val);
		NotifyObservers(&changed);

		return true;
	}
	else
	{
		return false;
	}
}

bool WorldRendererSettings::HasSetting( const std::string& var )
{
	auto i = zSettings.find(var);
	return ( i != zSettings.cend() );
}

const AnyType& WorldRendererSettings::GetSetting( const std::string& var )
{
	auto i = zSettings.find(var);
	return i->second;
}