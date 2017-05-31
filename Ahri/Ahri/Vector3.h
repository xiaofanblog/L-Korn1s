#ifndef Vector3_h__
#define Vector3_h__

#include <Windows.h>
#include <cmath>
#include <limits>

static __forceinline float ToDegree(float Args)
{
	const float flPi = 3.141592654f;
	return (Args * (180.f / flPi));
}

#define D3DX_PI    ((FLOAT)  3.141592654f)
#define D3DX_1BYPI ((FLOAT)  0.318309886f)

#define D3DXToRadian( degree ) ((degree) * (D3DX_PI / 180.0f))
#define D3DXToDegree( radian ) ((radian) * (180.0f / D3DX_PI))

class Vec2
{
public:
	float x;
	float y;

	Vec2()
	{
	}

	explicit Vec2(const float x, const float y)
	{
		this->x = x;
		this->y = y;
	}

	void Set(const float x, const float y)
	{
		this->x = x;
		this->y = y;
	}

	void Zero()
	{
		this->x = 0;
		this->y = 0;
	}

	float operator[](int index) const
	{
		return (&x)[index];
	}

	float& operator[](int index)
	{
		return (&x)[index];
	}

	Vec2 operator-() const
	{
		return Vec2(-x, -y);
	}

	float operator*(Vec2& a) const
	{
		return ((x * a.x) + (y * a.y));
	}

	Vec2 operator*(const float a) const
	{
		return Vec2(x * a, y * a);
	}

	Vec2 operator/(const float a) const
	{
		float inva = 1.f / a;
		return Vec2(x * inva, y * inva);
	}

	Vec2 operator+(const Vec2& a) const
	{
		return Vec2(x + a.x, y + a.y);
	}

	Vec2 operator-(const Vec2& a) const
	{
		return Vec2(x - a.x, y - a.y);
	}

	Vec2& operator+=(const Vec2& a)
	{
		x += a.x;
		y += a.y;
		return *this;
	}

	Vec2& operator-=(const Vec2& a)
	{
		x -= a.x;
		y -= a.y;
		return *this;
	}

	Vec2& operator/=(const Vec2& a)
	{
		x /= a.x;
		y /= a.y;
		return *this;
	}

	Vec2& operator/=(const float a)
	{
		x /= a;
		y /= a;
		return *this;
	}

	Vec2& operator*=(const float a)
	{
		x *= a;
		y *= a;
		return *this;
	}

	friend Vec2 operator*(const float a, const Vec2 b)
	{
		return Vec2(b.x * a, b.y * a);
	}

	bool operator==(const Vec2& a) const
	{
		return Compare(a) == true;
	}

	bool operator!=(const Vec2& a) const
	{
		return Compare(a) == false;
	}

	bool Compare(const Vec2& a) const
	{
		return ((x == a.x) && (y == a.y));
	}

	Vec2& VectorNormalize()
	{
		float len = Length();

		*this /= len;

		return *this;
	}

	float Length()
	{
		return sqrtf(x * x + y * y);
	}

	float DotProduct(const Vec2& vecOther)
	{
		return ((x * vecOther.x) + (y * vecOther.y));
	}

	Vec2 Rotated(float Angle)
	{
		float c = cosf(Angle);
		float s = sinf(Angle);

		return Vec2(x * c - y * s, y * c + x * s);
	}

	Vec2 Perpendicular()
	{
		return Vec2(y, -x);
	}

	bool Intersects(Vec2 const& Seg1End, Vec2 const& Seg2Start, Vec2 const& Seg2End, Vec2* IntersectionOut)
	{
		double deltaACy = y - Seg2Start.y;
		double deltaDCx = Seg2End.x - Seg2Start.x;
		double deltaACx = x - Seg2Start.x;
		double deltaDCy = Seg2End.y - Seg2Start.y;
		double deltaBAx = Seg1End.x - x;
		double deltaBAy = Seg1End.y - y;

		auto denominator = deltaBAx * deltaDCy - deltaBAy * deltaDCx;
		auto numerator = deltaACy * deltaDCx - deltaACx * deltaDCy;

		if (fabs(denominator) < FLT_EPSILON)
		{
			if (fabs(numerator) < FLT_EPSILON)
			{
				// collinear. Potentially infinite intersection points.
				// Check and return one of them.
				if (x >= Seg2Start.x && x <= Seg2End.x)
				{
					if (IntersectionOut)
						*IntersectionOut = *this;

					return true;
				}

				if (Seg2Start.x >= x && Seg2Start.x <= Seg1End.x)
				{
					if (IntersectionOut)
						*IntersectionOut = Seg2Start;

					return true;
				}

				return false;
			}

			// parallel
			return false;
		}

		auto r = numerator / denominator;

		if (r < 0 || r > 1)
			return false;

		auto s = (deltaACy * deltaBAx - deltaACx * deltaBAy) / denominator;

		if (s < 0 || s > 1)
			return false;

		if (IntersectionOut)
			*IntersectionOut = Vec2(x + r * deltaBAx, y + r * deltaBAy);

		return true;
	}

	bool Close(float a, float b, float eps) const
	{
		if (fabs(eps) < FLT_EPSILON)
			eps = (float) 1e-9;

		return fabs(a - b) <= eps;
	}

	float Polar() const
	{
		if (Close(x, 0, 0))
		{
			if (y > 0)
				return 90;

			return y < 0 ? 270.f : 0.f;
		}

		float flTheta = ToDegree(atanf(y / x));

		if (x < 0)
			flTheta += 180.f;

		if (flTheta < 0)
			flTheta += 360.f;

		return flTheta;
	}

	bool IsValid()
	{
		return (Length() != 0);
	}

	float DistanceTo(Vec2 const& Other) const
	{
		return (Other - *this).Length();
	}

	float AngleBetween(Vec2 const& Other) const
	{
		float flTheta = Polar() - Other.Polar();

		if (flTheta < 0)
			flTheta += 360.f;

		if (flTheta > 180.f)
			flTheta = 360.f - flTheta;

		return flTheta;
	}

	float LengthSqr()
	{
		return (x * x + y * y);
	}

	float DistanceTo(const Vec2& vecOther)
	{
		return (vecOther - *this).Length();
	}

	float DistanceToSqr(const Vec2& vecOther)
	{
		return (vecOther - *this).LengthSqr();
	}

	Vec2 Extend(Vec2 const Other, float Len)
	{
		Vec2 vecThis = *this;
		return (vecThis + (Other - vecThis).VectorNormalize() * Len);
	}

	Vec2 Extend(Vec2 const Other, float Len) const
	{
		Vec2 vecThis = *this;
		return (vecThis + (Other - vecThis).VectorNormalize() * Len);
	}


	float Distance(Vec2 const& SegmentStart, Vec2 const& SegmentEnd, bool OnlyIfOnSegment = false, bool Squared = false);
	void GetLineFromSegment(Vec2 const& SegmentStart, Vec2 const& SegmentEnd, Vec2& Line);
};

class Vec3
{
public:
	float x;
	float y;
	float z;

	Vec3()
	{
	}

	explicit Vec3(const float x, const float y, const float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	void Set(const float x, const float y, const float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	void Zero()
	{
		this->x = 0;
		this->y = 0;
		this->z = 0;
	}

	float operator[](int index) const
	{
		return (&x)[index];
	}

	float& operator[](int index)
	{
		return (&x)[index];
	}

	Vec3 operator-() const
	{
		return Vec3(-x, -y, -z);
	}

	float operator*(Vec3& a) const
	{
		return ((x * a.x) + (y * a.y) + (z * a.z));
	}

	Vec3 operator*(const float a) const
	{
		return Vec3(x * a, y * a, z * a);
	}

	Vec3 operator/(const float a) const
	{
		float inva = 1.f / a;
		return Vec3(x * inva, y * inva, z * inva);
	}

	Vec3 operator+(const Vec3& a) const
	{
		return Vec3(x + a.x, y + a.y, z + a.z);
	}

	Vec3 operator-(const Vec3& a) const
	{
		return Vec3(x - a.x, y - a.y, z - a.z);
	}

	Vec3& operator+=(const Vec3& a)
	{
		x += a.x;
		y += a.y;
		z += a.z;
		return *this;
	}

	Vec3& operator-=(const Vec3& a)
	{
		x -= a.x;
		y -= a.y;
		z -= a.z;
		return *this;
	}

	Vec3& operator/=(const Vec3& a)
	{
		x /= a.x;
		y /= a.y;
		z /= a.z;
		return *this;
	}

	Vec3& operator/=(const float a)
	{
		x /= a;
		y /= a;
		z /= a;
		return *this;
	}

	Vec3& operator*=(const float a)
	{
		x *= a;
		y *= a;
		z *= a;
		return *this;
	}

	friend Vec3 operator*(const float a, const Vec3 b)
	{
		return Vec3(b.x * a, b.y * a, b.z * a);
	}

	bool operator==(const Vec3& a) const
	{
		return Compare(a) == true;
	}

	bool operator!=(const Vec3& a) const
	{
		return Compare(a) == false;
	}

	bool Compare(const Vec3& a) const
	{
		return ((x == a.x) && (y == a.y) && (z == a.z));
	}

	Vec3& VectorNormalize()
	{
		float len = Length();

		*this /= len;

		return *this;
	}

	float Length()
	{
		return sqrtf(x * x + y * y + z * z);
	}

	float Length2D()
	{
		return sqrtf(x * x + z * z);
	}

	float DotProduct(const Vec3& vecOther)
	{
		return ((x * vecOther.x) + (y * vecOther.y) + (z * vecOther.z));
	}

	Vec2 To2D() const
	{
		return Vec2(x, z);
	}

	Vec3 Rotated(float Angle)
	{
		float c = cosf(Angle);
		float s = sinf(Angle);

		return Vec3(x * c - z * s, y, z * c + x * s);
	}

	float Distance2D(const Vec3 vecOther)
	{
		return ((vecOther - *this).To2D().Length());
	}


	Vec3 Extend(Vec3 const Other, float Len)
	{
		Vec3 vecThis = *this;
		return (vecThis + (Other - vecThis).VectorNormalize() * Len);
	}

	Vec3 Extend(Vec3 const Other, float Len) const
	{
		Vec3 vecThis = *this;
		return (vecThis + (Other - vecThis).VectorNormalize() * Len);
	}

	float DistanceTo(const Vec3 vecOther)
	{
		return ((vecOther - *this).Length());
	}

	Vec3 Perpendicular()
	{
		return Vec3(z, y, -x);
	}
};

class Vec4
{
public:
	float x;
	float y;
	float z;
	float w;

	Vec4()
	{
	}

	explicit Vec4(const float x, const float y, const float z, const float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	void Set(const float x, const float y, const float z, const float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	void Zero()
	{
		this->x = 0;
		this->y = 0;
		this->z = 0;
		this->w = 0;
	}

	float operator[](int index) const
	{
		return (&x)[index];
	}

	float& operator[](int index)
	{
		return (&x)[index];
	}

	Vec4 operator-() const
	{
		return Vec4(-x, -y, -z, -w);
	}

	float operator*(Vec4& a) const
	{
		return ((x * a.x) + (y * a.y) + (z * a.z) + (w * a.w));
	}

	Vec4 operator*(const float a) const
	{
		return Vec4(x * a, y * a, z * a, w * a);
	}

	Vec4 operator/(const float a) const
	{
		float inva = 1.f / a;
		return Vec4(x * inva, y * inva, z * inva, w * inva);
	}

	Vec4 operator+(const Vec4& a) const
	{
		return Vec4(x + a.x, y + a.y, z + a.z, w + a.w);
	}

	Vec4 operator-(const Vec4& a) const
	{
		return Vec4(x - a.x, y - a.y, z - a.z, w - a.w);
	}

	Vec4& operator+=(const Vec4& a)
	{
		x += a.x;
		y += a.y;
		z += a.z;
		w += a.w;
		return *this;
	}

	Vec4& operator-=(const Vec4& a)
	{
		x -= a.x;
		y -= a.y;
		z -= a.z;
		w -= a.w;
		return *this;
	}

	Vec4& operator/=(const Vec4& a)
	{
		x /= a.x;
		y /= a.y;
		z /= a.z;
		w /= a.w;
		return *this;
	}

	Vec4& operator/=(const float a)
	{
		x /= a;
		y /= a;
		z /= a;
		w /= a;
		return *this;
	}

	Vec4& operator*=(const float a)
	{
		x *= a;
		y *= a;
		z *= a;
		w *= a;
		return *this;
	}

	friend Vec4 operator*(const float a, const Vec4 b)
	{
		return Vec4(b.x * a, b.y * a, b.z * a, b.w * a);
	}

	bool operator==(const Vec4& a) const
	{
		return Compare(a) == true;
	}

	bool operator!=(const Vec4& a) const
	{
		return Compare(a) == false;
	}

	bool Compare(const Vec4& a) const
	{
		return ((x == a.x) && (y == a.y) && (z == a.z) && (w == a.w));
	}

	void Negate()
	{
		this->x = -this->x;
		this->y = -this->y;
		this->z = -this->z;
	}

	Vec4& VectorNormalize()
	{
		float len = Length();

		*this /= len;

		return *this;
	}

	float Length()
	{
		return sqrtf(x * x + y * y + z * z + w * w);
	}

	float DotProduct(const Vec4& vecOther)
	{
		return ((x * vecOther.x) + (y * vecOther.y) + (z * vecOther.z) + (w * vecOther.w));
	}

	float DotProduct(const Vec3& vecOther)
	{
		return ((x * vecOther.x) + (y * vecOther.y) + (z * vecOther.z));
	}
};

struct ProjectionInfo
{
	ProjectionInfo() { }

	/// <summary>
	/// The is on segment
	/// </summary>
	bool IsOnSegment;

	/// <summary>
	/// The line point
	/// </summary>
	Vec2 LinePoint;

	/// <summary>
	/// The segment point
	/// </summary>
	Vec2 SegmentPoint;

	/// <summary>
	/// Initializes a new instance of the <see cref="ProjectionInfo"/> struct.
	/// </summary>
	/// <param name="isOnSegment">if set to <c>true</c> [is on segment].</param>
	/// <param name="segmentPoint">The segment point.</param>
	/// <param name="linePoint">The line point.</param>
	ProjectionInfo(bool isOnSegment, Vec2 segmentPoint, Vec2 linePoint)
	{
		IsOnSegment = isOnSegment;
		SegmentPoint = segmentPoint;
		LinePoint = linePoint;
	}
};
#endif // Vector3_h__