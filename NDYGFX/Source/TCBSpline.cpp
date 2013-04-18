//Written by Markus Tillman.

#include "TCBSpline.h"
// #include "MaloWFileDebug.h"
#include "Vector.h"
#include <fstream>
#include <string>
using namespace std;

//private
void TCBSpline::Expand()
{
	//control points
	this->mControlCap += this->mControlCap;
	Vector3** temp = new Vector3*[this->mControlCap];
	for(int i = 0; i < this->mNrOfControlPoints; i++)
	{
		temp[i] = this->mControlPoints[i];
	}
	delete [] this->mControlPoints;
	this->mControlPoints = temp;
	temp = 0;
}

void TCBSpline::CalculateTangents(int i)
{
	Vector3 sv1, sv2, dv1, dv2, src, dst;
	float s1, s2;
	float d1, d2;
	
	s1 = ((1 - this->mTension) * (1 + this->mBias) * (1 - this->mContinuity)) * 0.5f;
	s2 = ((1 - this->mTension) * (1 - this->mBias) * (1 + this->mContinuity)) * 0.5f;
	d1 = ((1 - this->mTension) * (1 + this->mBias) * (1 + this->mContinuity)) * 0.5f;
	d2 = ((1 - this->mTension) * (1 - this->mBias) * (1 - this->mContinuity)) * 0.5f;

	if(i == 0) //first control point
	{
		if(this->mEndsAreConnected)
		{
			sv1 = (*this->mControlPoints[0] - *this->mControlPoints[this->mNrOfControlPoints - 1]) * s1; 
			dv1 = (*this->mControlPoints[0] - *this->mControlPoints[this->mNrOfControlPoints - 1]) * d1;
		}
		else
		{
			sv1 = (*this->mControlPoints[0] - Vector3(0.0f, 0.0f, 0.0f)) * s1;
			dv1 = (*this->mControlPoints[0] - Vector3(0.0f, 0.0f, 0.0f)) * d1;
		}
		//source-tangent
		sv2 = (*this->mControlPoints[1] - *this->mControlPoints[0]) * s2;
		src = sv1 + sv2; 
		this->mSource[0] = new Vector3(src);
		//destination tangent
		dv2 = (*this->mControlPoints[1] - *this->mControlPoints[0]) * d2;
		src = dv1 + dv2;
		this->mDestination[0] = new Vector3(src);
	}
	else if(i == (this->mNrOfControlPoints - 1)) //last control point
	{
		if(this->mEndsAreConnected)
		{
			sv2 = (*this->mControlPoints[0] - *this->mControlPoints[this->mNrOfControlPoints - 1]) * s2;
			dv2 = (*this->mControlPoints[0] - *this->mControlPoints[this->mNrOfControlPoints - 1]) * d2;
		}
		else
		{
			sv2 = (Vector3(0.0f, 0.0f, 0.0f) - *this->mControlPoints[this->mNrOfControlPoints - 1]) * s2;
			dv2 = (Vector3(0.0f, 0.0f, 0.0f) - *this->mControlPoints[this->mNrOfControlPoints - 1]) * d2;
		}
		//source-tangent
		sv1 = (*this->mControlPoints[this->mNrOfControlPoints - 1] - *this->mControlPoints[this->mNrOfControlPoints - 2]) * s1;
		src = sv1 + sv2;
		this->mSource[this->mNrOfControlPoints - 1] = new Vector3(src);
		//destination-tangent
		dv1 = (*this->mControlPoints[this->mNrOfControlPoints - 1] - *this->mControlPoints[this->mNrOfControlPoints - 2]) * d1;
		src = dv1 + dv2;
		this->mDestination[this->mNrOfControlPoints - 1] = new Vector3(src);
	}
	else //rest
	{
		//source(incoming) tangents
		sv1 = (*this->mControlPoints[i] - *this->mControlPoints[i - 1]) * s1;
		sv2 = (*this->mControlPoints[i + 1] - *this->mControlPoints[i]) * s2;
		src = sv1 + sv2;
		this->mSource[i] = new Vector3(src);
		//destination(outgoing) tangents
		dv1 = (*this->mControlPoints[i] - *this->mControlPoints[i - 1]) * d1;
		dv2 = (*this->mControlPoints[i + 1] - *this->mControlPoints[i]) * d2;
		dst = dv1 + dv2;
		this->mDestination[i] = new Vector3(dst);
	}
}

//de/con-structors, init, other
TCBSpline::TCBSpline(bool connectEnds, float tension, float bias, float continuity)
{
	this->mTension = tension;
	this->mBias = bias;
	this->mContinuity = continuity;

	this->mEndsAreConnected = connectEnds;
	this->mNrOfControlPoints = 0;
	this->mControlCap = 10;
	this->mControlPoints = new Vector3*[this->mControlCap];
	for(unsigned int i = 0; i < this->mControlCap; ++i)
	{
		this->mControlPoints[i] = 0;
	}
	this->mSource = 0;
	this->mDestination = 0;
}
TCBSpline::~TCBSpline()
{
	//control points
	for(int i = 0; i < this->mNrOfControlPoints; i++)
	{
		delete this->mControlPoints[i];
		this->mControlPoints[i] = 0;
	}
	delete [] this->mControlPoints;
	this->mControlPoints = NULL;
	//source & destination tangents
	if(this->mSource)
	{
		for(int i = 0; i < this->mNrOfControlPoints; i++)
		{	
			delete this->mSource[i];
			delete this->mDestination[i];
			this->mSource[i] = 0;
			this->mDestination[i] = 0;
		}
		delete [] this->mSource;
		delete [] this->mDestination;
		this->mSource =  0;
		this->mDestination = 0;

		this->mNrOfControlPoints = 0;
	}
}

bool TCBSpline::Init()
{
	if(this->mNrOfControlPoints > 1) //the number of control points has to be 2 or greater to create line(s)
	{
		this->mSource = new Vector3*[this->mNrOfControlPoints];
		this->mDestination = new Vector3*[this->mNrOfControlPoints];
		for(unsigned int i = 0; i < this->mNrOfControlPoints; ++i)
		{
			this->mSource[i] = 0;
			this->mDestination[i] = 0;
		}

		//calculate source & destination-tangents of the control points
		for(int i = 0; i < this->mNrOfControlPoints; i++)
		{
			TCBSpline::CalculateTangents(i);
		}
	}
	else
	{
		//remove control point if there is one
		if(this->mNrOfControlPoints == 1)
		{
			delete this->mControlPoints[0];
			this->mControlPoints[0] = 0;
			this->mNrOfControlPoints--;
		}
		//MaloW::Debug("TCBSpline: Warning: Failed to initilize spline: Not enough control points");

		return false;
	}
	
	return true;
}

//get
bool TCBSpline::AreEndsConnected() const
{
	return this->mEndsAreConnected;
}
int TCBSpline::GetNrOfControlPoints() const
{
	return this->mNrOfControlPoints;
}
float TCBSpline::GetTension() const
{
	return this->mTension;
}
float TCBSpline::GetBias() const
{
	return this->mBias;
}
float TCBSpline::GetContinuity() const
{
	return this->mContinuity;
}

//set
void TCBSpline::SetTension(float tension)
{
	this->mTension = tension;
}
void TCBSpline::SetBias(float bias)
{
	this->mBias = bias;
}
void TCBSpline::SetContinuity(float continuity) 
{
	this->mContinuity = continuity;
}

//other
void TCBSpline::AddControlPoint(Vector3& controlPoint)
{
	if(this->mNrOfControlPoints == this->mControlCap)
	{
		TCBSpline::Expand();
	}
	this->mControlPoints[this->mNrOfControlPoints++] = new Vector3(controlPoint);
}


Vector3 TCBSpline::GetPoint(float t) const
{
	if(t != 1.0f)
	{
		//Split up the curves into 2 curves with local t value and create the points on the current curve using the local t value
		float tmp = (this->mNrOfControlPoints - 1) * t;
		int cpIndex = (int)tmp;
		float localT = tmp - cpIndex;
		float a0, a1, a2, a3;
		Vector3 cp0, cp1, d0, d1, p;
	
		cp0 = *this->mControlPoints[cpIndex];	
		cp1 = *this->mControlPoints[cpIndex + 1];	
		d0 = *this->mDestination[cpIndex];			
		d1 = *this->mDestination[cpIndex + 1];		

		a0 = 2.0f * powf(localT, 3.0f) - 3.0f * powf(localT, 2.0f) + 1;
		a1 = powf(localT, 3.0f) - 2.0f * powf(localT, 2.0f) + localT;
		a2 = powf(localT, 3.0f) - powf(localT, 2.0f);
		a3 = -2.0f * powf(localT, 3.0f) + 3.0f * powf(localT, 2.0f);

		p = cp0 * a0 +
			d0  * a1 +
			d1  * a2 +
			cp1 * a3;

		return p;
	}
	else
	{
		return *this->mControlPoints[this->mNrOfControlPoints - 1];
	}
}

Vector3** TCBSpline::CalculatePoints(int nrOfPoints)
{	
	Vector3** temp = new Vector3*[nrOfPoints];

	float t = 0.0f;
	for(int i = 0; i < nrOfPoints; i++)
	{
		t = (float)i / float(nrOfPoints);
		temp[i] = new Vector3(TCBSpline::GetPoint(t));
	}

	return temp;
}

void TCBSpline::WriteControlPointsToFile(const char* fileName)
{
	string fileNameOpen = string(fileName);
	fileNameOpen += ".txt";
	std::ofstream out;
	out.open(fileNameOpen, std::ios_base::out);

	int index = 0;
	char buffer[1000];
	if(out)
	{
		_itoa_s(this->mNrOfControlPoints, buffer, 10);
		while(buffer[index] != '\0')
		{
			index++;
		}
		buffer[index++] = '\n';
		out.write(buffer, index); 
		for(int i = 0; i < this->mNrOfControlPoints; i++)
		{
			//x-pos
			sprintf_s(buffer, "%f", this->mControlPoints[i]->x);
			index = 0;
			while(buffer[index] != '\0')
			{
				index++;
			}
			buffer[index++] = ' ';
			out.write(buffer, index);
			//y-pos
			sprintf_s(buffer, "%f", this->mControlPoints[i]->y);
			index = 0;
			while(buffer[index] != '\0')
			{
				index++;
			}
			buffer[index++] = ' ';
			out.write(buffer, index);
			//z-pos
			sprintf_s(buffer, "%f", this->mControlPoints[i]->z);
			index = 0;
			while(buffer[index] != '\0')
			{
				index++;
			}
			buffer[index++] = ' ';
			out.write(buffer, index);
			//new line
			buffer[0] = '\n';
			out.write(buffer, 1);
		}
		out.close();
	}
}


bool TCBSpline::ReadControlPointsFromFile(const char* fileName)
{
	string fileNameOpen = string(fileName);
	fileNameOpen += ".txt";
	std::ifstream in;
	in.open(fileNameOpen);
	//in.open(fileName);
	if(in)
	{
		/*if(this->mControlPoints)
		{
			for(unsigned int i = 0; i < this->mNrOfControlPoints; ++i)
			{
				delete this->mControlPoints[i];
			}
			delete [] this->mControlPoints;
		}*/

		const int bufferSize = 512;
		char buffer[bufferSize];

		//get number of control points to read
		in.getline(buffer, bufferSize);
		sscanf_s(buffer, "%d", &this->mNrOfControlPoints);
		this->mControlCap = this->mNrOfControlPoints;
		
		this->mControlPoints = new Vector3*[this->mNrOfControlPoints];
		//read values
		for(int i = 0; i < this->mNrOfControlPoints; i++)
		{
			in.getline(buffer, bufferSize);
			this->mControlPoints[i] = new Vector3();
			sscanf_s(buffer, "%f %f %f", &this->mControlPoints[i]->x, &this->mControlPoints[i]->y, &this->mControlPoints[i]->z);
		}

		in.close();
		return true;
	}

	return false;
}
	

void TCBSpline::Clear()
{
	if(this->mControlPoints)
	{
		for(int i = 0; i < this->mNrOfControlPoints; i++)
		{
			if ( this->mControlPoints[i] ) delete this->mControlPoints[i], this->mControlPoints[i] = 0;
			//SAFE_DELETE(this->mSource[i]); **
			//SAFE_DELETE(this->mDestination[i]); **
		}
		this->mNrOfControlPoints = 0;
	}
}