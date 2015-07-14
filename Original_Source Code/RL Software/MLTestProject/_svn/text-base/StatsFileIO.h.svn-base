/////////////////////////////////////////////////////////
//StatsFileIO.h
//This class is responsible for storing statistics.
//
//Manuel Madera
//11/26/09
/////////////////////////////////////////////////////////

#ifndef StatsFileIO_h
#define StatsFileIO_h

#include "MLTestProject.h"
#include "MLAgentEvents.h"
#include <fstream>
#include <list>

using namespace std;

namespace MLTest
{
	class ML_API StatsFileIO
	{
	private:
		ofstream outStream;
		unsigned headersSize;

	public:
		StatsFileIO(string fileName);
		virtual ~StatsFileIO();

		void WriteHeader(list<string> headers);
		void WriteRow(list<string> elements);

		static string ConvertToString(unsigned num);
		static string ConvertToString(char c);
		static string ConvertToString(int num);
		static string ConvertToString(double num);
	};
}

#endif	//StatsFileIO_h