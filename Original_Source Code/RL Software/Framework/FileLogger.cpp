/////////////////////////////////////////////////////////
//FileLogger.h
//Defines the FileLogger class.  This object is responsible
//for storing log messages into a file.
//
//Manuel Madera
//1/25/10
/////////////////////////////////////////////////////////

#include "FileLogger.h"

using namespace std;

namespace Framework
{
	namespace Logging
	{
		FileLogger::FileLogger(string fileName)
		{
			this->outStream.open(fileName.c_str());
		}

		FileLogger::~FileLogger()
		{
			this->outStream.close();
		}

		void FileLogger::Log(string message)
		{
			this->outStream << message;
		}
	}
}