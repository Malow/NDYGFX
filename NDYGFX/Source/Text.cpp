#include "Text.h"

using namespace std;

Text::Text(string text, D3DXVECTOR2 position, float size)
{
	this->text = text;
	this->ToUpper();

	this->position = position;
	this->size = size;
	this->strata = 0.5f;
	this->color = D3DXVECTOR3(0, 0, 0);
	this->font = new Font();
	this->font->textureResource = NULL;
	for(int i = 0; i < 256; i++)
		this->font->charTexCoords[i] = 0;
}

void Text::ToUpper()
{
	for(unsigned int i = 0; i < this->text.size(); i++)
	{
		this->text[i] = (char)toupper(this->text[i]);
	}
}

void Text::DeleteFromEnd(unsigned int CharsToDel) 
{
	if(CharsToDel <= this->text.size())
		this->text = this->text.substr(0, this->text.size() - CharsToDel);
}

Text::~Text()
{
	if(this->font->textureResource) GetResourceManager()->DeleteTextureResource(this->font->textureResource);
	if(this->font)
		delete this->font;
}

void Text::SetText( const char* text )
{
	this->text = string(text);
}

const char* Text::GetText() const
{
	return this->text.c_str();
}

void Text::SetPosition( Vector2 pos )
{
	this->position = D3DXVECTOR2(pos.x, pos.y);
}

Vector2 Text::GetPosition() const
{
	return Vector2(this->position.x, this->position.y);
}

void Text::SetSize( float size )
{
	this->size = size;
}

float Text::GetSize() const
{
	return this->size;
}

void Text::AppendText( const char* app )
{
	this->text += app;
}

void Text::SetColor( Vector3 color )
{
	this->color = D3DXVECTOR3(color.x / 255.0f, color.y / 255.0f, color.z / 255.0f);
}

void Text::SetStrata( float strata )
{
	this->strata = strata / 1000.0f;
}

float Text::GetStrata() const
{
	return this->strata * 1000.0f;
}
