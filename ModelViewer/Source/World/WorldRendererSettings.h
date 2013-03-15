#pragma once

#include "Observer.h"
#include "AnyType.h"
#include <string>
#include <map>

class WorldRendererSettings;

#pragma warning ( push )
#pragma warning ( disable : 4512 )	// Assignment operator could not be generated

class WorldRendererSettingChanging : public Event
{
	bool zAccepted;

	WorldRendererSettingChanging(const std::string& var, const AnyType& val) :
		variable(var),
		value(val),
		zAccepted(true)
	{
	}

public:
	const std::string& variable;
	const AnyType& value;

	inline void Deny() { zAccepted = false; }
	inline bool IsAccepted() const { return zAccepted; }

	friend WorldRendererSettings;
};

class WorldRendererSettingChanged : public Event
{
	WorldRendererSettingChanged(const std::string& var, const AnyType& val) :
		variable(var),
		value(val)
	{
	}
public:
	const std::string& variable;
	const AnyType& value;

	friend WorldRendererSettings;
};

class WorldRendererSettings : public Observed
{
	std::map<std::string, AnyType> zSettings;
	bool zSameAsFile;
	std::string zSettingsFile;

public:
	WorldRendererSettings(Observer* observer, const std::string& fileName="");
	virtual ~WorldRendererSettings();

	bool SetValue( const std::string& var, const AnyType& val );
	bool HasSetting( const std::string& var );
	const AnyType& GetSetting( const std::string& var );
};

#pragma warning ( pop )