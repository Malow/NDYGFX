#pragma once

#include <string>

void LoadEntList( const std::string& fileName ) throw(...);
const std::string& GetEntName( unsigned int entIndex ) throw(...);
const std::string& GetEntModel( unsigned int entIndex, float distance=0 ) throw(...);
const std::string& GetEntBillboard( unsigned int entIndex ) throw(...);
const float& GetEntBillboardDistance( unsigned int entIndex ) throw(...);
const float& GetEntBlockRadius( unsigned int entIndex ) throw(...);
unsigned int GetEntListSize();