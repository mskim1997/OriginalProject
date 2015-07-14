/////////////////////////////////////////////////////////
//Position.h
//Defines the Position class.  This class holds an xy-coordinate
//
//Manuel Madera
//7/25/09
/////////////////////////////////////////////////////////

#ifndef Position_h
#define Position_h

#include "Framework.h"

namespace Framework
{
	template<class T>
	class Position
	{
	private:
		T xCoor;
		T yCoor;
	public:
		Position() : xCoor(0), yCoor(0)
		{
		}

		Position(T newXCoor, T newYCoor) : xCoor(newXCoor), yCoor(newYCoor)
		{
		}

		Position(Position<T> &rhs)
		{
			this->xCoor = rhs.xCoor;
			this->yCoor = rhs.yCoor;
		}

		virtual ~Position()
		{
		}

		T GetXCoor() const
		{
			return this->xCoor;
		}

		T GetYCoor() const
		{
			return this->yCoor;
		}

		void SetXCoor(T newXCoor)
		{
			this->xCoor = newXCoor;
		}

		void SetYCoor(T newYCoor)
		{
			this->yCoor = newYCoor;
		}

		Position<T> &operator=(const Position<T> &rhs)
		{
			if (this != &rhs)
			{
				this->xCoor = rhs.GetXCoor();
				this->yCoor = rhs.GetYCoor();
			}

			return *this;
		}

		Position<T> operator+(const Position<T> &rhs)
		{
			Position<T> result;

			result.xCoor = this->xCoor + rhs.GetXCoor();
			result.yCoor = this->yCoor + rhs.GetYCoor();

			return result;
		}

		Position<T> operator+(const T &rhs)
		{
			Position<T> temp(rhs, rhs);

			return *this + temp;
		}

		Position<T> operator-(const Position<T> &rhs)
		{
			Position<T> result;

			result.xCoor = this->xCoor - rhs.GetXCoor();
			result.yCoor = this->yCoor - rhs.GetYCoor();

			return result;
		}

		Position<T> operator-(const T &rhs)
		{
			Position<T> temp(rhs, rhs);

			return *this - temp;
		}

		Position<T> operator*(const Position<T> &rhs)
		{
			Position<T> result;

			result.xCoor = this->xCoor * rhs.GetXCoor();
			result.yCoor = this->yCoor * rhs.GetYCoor();

			return result;
		}

		Position<T> operator*(const T &rhs)
		{
			Position<T> temp(rhs, rhs);

			return *this * temp;
		}
	};
}

#endif	//Position_h