/////////////////////////////////////////////////////////
//StatsFileIO.cpp
//This class is responsible for storing statistics.
//
//Manuel Madera
//11/26/09
/////////////////////////////////////////////////////////

#include "StatsFileIO.h"
#include <sstream>
#include "Framework\Debug.h"

#ifdef PrintMemoryLeak
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif
#endif

namespace MLContinuous
{
	StatsFileIO::StatsFileIO(string fileName)
	{
		this->outStream.open(fileName.c_str());
	}

	StatsFileIO::~StatsFileIO()
	{
		this->outStream.close();
	}

	void StatsFileIO::WriteHeader(list<string> headers)
	{
		list<string>::iterator iter;
		this->headersSize = headers.size();

		for (iter = headers.begin(); iter != headers.end(); iter++)
		{
			this->outStream << iter->c_str();
			this->outStream << ",";
		}

		this->outStream << "\n";
	}

	void StatsFileIO::WriteRow(list<string> elements)
	{
		if (this->headersSize == elements.size())
		{
			list<string>::iterator iter;

			for (iter = elements.begin(); iter != elements.end(); iter++)
			{
				this->outStream << iter->c_str();
				this->outStream << ",";
			}

			this->outStream << "\n";
		}
	}

	/*static*/ string StatsFileIO::ConvertToString(char c)
	{
		string outStr;
		stringstream out;

		out << c;
		outStr = out.str();

		out.str("");
		out.flush();

		return outStr;
	}

	/*static*/ string StatsFileIO::ConvertToString(unsigned num)
	{
		string number;
		stringstream out;

		out << num;
		number = out.str();

		out.str("");
		out.flush();

		return number;
	}

	/*static*/ string StatsFileIO::ConvertToString(int num)
	{
		string number;
		stringstream out;

		out << num;
		number = out.str();

		out.str("");
		out.flush();

		return number;
	}

	/*static*/ string StatsFileIO::ConvertToString(double num)
	{
		string number;
		stringstream out;

		out << num;
		number = out.str();

		out.str("");
		out.flush();

		return number;
	}
}