#ifndef Polygons_h__
#define Polygons_h__

#include "PluginSDK.h"
#include "Clipper.h"

#include <algorithm>

#define M_PI static_cast<float>(3.141592)


static Vec3 Get3DPoint(Vec2 const& Position)
{
	return Vec3(Position.x, GNavMesh->GetHeightForPoint(Position), Position.y);
}
static void DrawLine(Vec2 const Start, Vec2 const End, Vec4 const Color)
{
	Vec3 vecStart3D = Get3DPoint(Start);
	Vec3 vecEnd3D = Get3DPoint(End);

	Vec3 vecOut1;
	GGame->Projection(vecStart3D, &vecOut1);

	Vec3 vecOut2;
	GGame->Projection(vecEnd3D, &vecOut2);

	if (vecOut1.z < 1.f && vecOut2.z < 1.f)
		GRender->DrawLine(Vec2(vecOut1.x, vecOut1.y), Vec2(vecOut2.x, vecOut2.y), Color);
}
namespace Geometry
{
	static std::vector<Vec2> CircleCircleIntersection(Vec2 center1, Vec2 center2, float radius1, float radius2)
	{
		std::vector<Vec2> vecOut;
		auto D = (center2 - center1).Length();

		//The Circles dont intersect:
		if (D > radius1 + radius2 || (D <= fabs(radius1 - radius2)))
			return vecOut;

		auto A = (radius1 * radius1 - radius2 * radius2 + D * D) / (2 * D);
		auto H = (float)sqrt(radius1 * radius1 - A * A);
		auto Direction = (center2 - center1).VectorNormalize();
		auto PA = center1 + A * Direction;
		auto S1 = PA + H * Direction.Perpendicular();
		auto S2 = PA - H * Direction.Perpendicular();

		vecOut.push_back(S1);
		vecOut.push_back(S2);

		return vecOut;
	}

	struct IPolygon
	{
		std::vector<Vec2> Points;

		void Clear()
		{
			Points.clear();
		}

		void Add(Vec2 Point)
		{
			Points.push_back(Point);
		}

		void Add(Vec3 Point)
		{
			Points.push_back(Point.To2D());
		}

		void Add(IPolygon Other)
		{
			for (auto i : Other.Points)
				Points.push_back(i);
		}

		void Draw(Vec4 Color, int Width = 1)
		{
			for (size_t i = 0; i <= Points.size() - 1; i++)
			{
				auto iNextIdx = (Points.size() - 1 == i) ? 0 : (i + 1);

				DrawLine(Points[i], Points[iNextIdx], Color);
			}
		}

		bool IsInside(Vec2 point)
		{
			return !IsOutside(point);
		}

		bool IsInside(Vec3 point)
		{
			return !IsOutside(point.To2D());
		}

		bool IsInside(IUnit* point)
		{
			if (point == nullptr)
				return false;

			return !IsOutside(point->GetPosition().To2D());
		}


		bool IsOutside(Vec2 point)
		{
			auto p = ClipperLib::IntPoint(static_cast<ClipperLib::long64>(point.x), static_cast<ClipperLib::long64>(point.y));
			auto v = ToClipperPath();

			return (ClipperLib::PointInPolygon(p, v) == 0);
		}

		std::vector<ClipperLib::IntPoint> ToClipperPath()
		{
			std::vector<ClipperLib::IntPoint> result;

			for (auto i : Points)
				result.push_back(ClipperLib::IntPoint(static_cast<ClipperLib::long64>(i.x), static_cast<ClipperLib::long64>(i.y)));

			return result;
		}
	};

	static std::vector<std::vector<ClipperLib::IntPoint>> ClipPolygons(std::vector<IPolygon> polygons)
	{
		auto subj = std::vector<std::vector<ClipperLib::IntPoint>>(polygons.size());
		auto clip = std::vector<std::vector<ClipperLib::IntPoint>>(polygons.size());

		for (auto i : polygons)
		{
			subj.push_back(i.ToClipperPath());
			clip.push_back(i.ToClipperPath());
		}

		auto solution = std::vector<std::vector<ClipperLib::IntPoint>>();

		ClipperLib::Clipper c;

		c.AddPaths(subj, ClipperLib::PolyType::ptSubject, true);
		c.AddPaths(clip, ClipperLib::PolyType::ptClip, true);
		c.Execute(ClipperLib::ClipType::ctUnion, solution, ClipperLib::pftPositive, ClipperLib::pftEvenOdd);

		return solution;
	}

	static IPolygon ToPolygon(std::vector<ClipperLib::IntPoint> Args)
	{
		IPolygon out;

		for (auto i : Args)
			out.Add(Vec2(i.X, i.Y));

		return out;
	}

	static std::vector<IPolygon> ToPolygons(std::vector<std::vector<ClipperLib::IntPoint>> Args)
	{
		std::vector<IPolygon> out;

		for (auto i : Args)
			out.push_back(ToPolygon(i));

		return out;
	}

	struct Arc : public IPolygon
	{
		float Angle;
		Vec2 EndPos;
		float Radius;
		Vec2 StartPos;
		int _quality;

		Arc() { }

		Arc(Vec2 start, Vec2 direction, float angle, float radius, int quality = 20)
		{
			StartPos = start;
			EndPos = (direction - start).VectorNormalize();
			Angle = angle;
			Radius = radius;
			_quality = quality;

			UpdatePolygon();
		}

		void UpdatePolygon(int offset = 0)
		{
			Clear();

			auto outRadius = (Radius + offset) / cosf(2 * M_PI / _quality);
			auto side1 = EndPos.Rotated(-Angle * 0.5f);

			for (auto i = 0; i <= _quality; i++)
			{
				auto cDirection = side1.Rotated(i * Angle / _quality).VectorNormalize();
				Add(Vec2(StartPos.x + outRadius * cDirection.x, StartPos.y + outRadius * cDirection.y));
			}
		}
	};

	struct Circle : public IPolygon
	{
		Vec2 Center;
		float Radius;
		int _quality;

		Circle() { }

		Circle(Vec2 center, float radius, int quality = 20)
		{
			Center = center;
			Radius = radius;
			_quality = quality;

			UpdatePolygon();
		}

		void UpdatePolygon(int offset = 0, float overrideWidth = -1)
		{
			Clear();

			auto outRadius = (overrideWidth > 0 ? overrideWidth : (offset + Radius) / cosf(2 * M_PI / _quality));

			for (auto i = 1; i <= _quality; i++)
			{
				auto angle = i * 2 * M_PI / _quality;
				auto point = Vec2(Center.x + outRadius * cosf(angle), Center.y + outRadius * sinf(angle));

				Add(point);
			}
		}
	};

	struct Line : public IPolygon
	{
		Vec2 LineEnd;
		Vec2 LineStart;

		Line() { }

		Line(Vec2 start, Vec2 end, float length = -1)
		{
			LineStart = start;
			LineEnd = end;

			if (length > 0)
				SetLength(length);

			UpdatePolygon();
		}

		float GetLength()
		{
			return (LineEnd - LineStart).Length();
		}

		void SetLength(float Length)
		{
			LineEnd = (LineEnd - LineStart).VectorNormalize() * Length + LineStart;
		}

		void UpdatePolygon()
		{
			Clear();
			Add(LineStart);
			Add(LineEnd);
		}
	};

	struct Rectangle : public IPolygon
	{
		Vec2 End;
		Vec2 Start;
		float Width;

		Rectangle() { }

		Rectangle(Vec2 start, Vec2 end, float width)
		{
			Start = start;
			End = end;
			Width = width;

			UpdatePolygon();
		}

		Vec2 GetDirection()
		{
			return (End - Start).VectorNormalize();
		}

		Vec2 GetPerpendicular()
		{
			return GetDirection().Perpendicular();
		}

		void UpdatePolygon(int offset = 0, float overrideWidth = -1)
		{
			Clear();

			float flOffset = static_cast<float>(offset);

			Add(Start + (overrideWidth > 0 ? overrideWidth : Width + flOffset) * GetPerpendicular() - flOffset * GetDirection());
			Add(Start - (overrideWidth > 0 ? overrideWidth : Width + flOffset) * GetPerpendicular() - flOffset * GetDirection());
			Add(End - (overrideWidth > 0 ? overrideWidth : Width + flOffset) * GetPerpendicular() + flOffset * GetDirection());
			Add(End + (overrideWidth > 0 ? overrideWidth : Width + flOffset) * GetPerpendicular() + flOffset * GetDirection());
		}
	};

	struct Ring : public IPolygon
	{
		Vec2 Center;
		float InnerRadius;
		float OuterRadius;
		int _quality;

		Ring() { }

		Ring(Vec2 center, float innerRadius, float outerRadius, int quality = 20)
		{
			Center = center;
			InnerRadius = innerRadius;
			OuterRadius = outerRadius;
			_quality = quality;

			UpdatePolygon();
		}

		void UpdatePolygon(int offset = 0)
		{
			Points.clear();

			auto outRadius = (offset + InnerRadius + OuterRadius) / cosf(2 * M_PI / _quality);
			auto innerRadius = InnerRadius - OuterRadius - offset;

			for (auto i = 0; i <= _quality; i++)
			{
				auto angle = i * 2 * M_PI / _quality;
				auto point = Vec2(Center.x - outRadius * cosf(angle), Center.y - outRadius * sinf(angle));

				Add(point);
			}

			for (auto i = 0; i <= _quality; i++)
			{
				auto angle = i * 2 * M_PI / _quality;
				auto point = Vec2(Center.x + innerRadius * cosf(angle), Center.y - innerRadius * sinf(angle));

				Add(point);
			}
		}
	};

	struct Sector : public IPolygon
	{
		float Angle;
		Vec2 Center;
		Vec2 Direction;
		float Radius;
		int _quality;

		Sector() { }

		Sector(Vec2 center, Vec2 direction, float angle, float radius, int quality = 20)
		{
			Center = center;
			Direction = (direction - center).VectorNormalize();
			Angle = angle;
			Radius = radius;
			_quality = quality;

			UpdatePolygon();
		}

		Vec2 RotateLineFromPoint(Vec2 point1, Vec2 point2, float value, bool radian = true)
		{
			auto angle = !radian ? value * M_PI / 180 : value;
			auto line = (point2 - point1);

			auto newline = Vec2(line.x * cosf(angle) - line.y * sinf(angle), line.x * sinf(angle) + line.y * cosf(angle));

			return (newline + point1);
		}

		void UpdatePolygon(int offset = 0)
		{
			Clear();

			auto outRadius = (Radius + offset) / cosf(2 * M_PI / _quality);
			auto side1 = Direction.Rotated(Angle * 0.5f);

			Add(Center);

			for (auto i = 0; i <= _quality; i++)
			{
				auto cDirection = side1.Rotated(i * Angle / _quality).VectorNormalize();

				Add(Center + outRadius * cDirection);
			}
		}
	};
}

namespace Polygons
{
	static const int Quality = 22;

	static std::vector<Vec2> GetIntersectPointsLine(Geometry::IPolygon poly, Vec2 start, Vec2 end)
	{
		std::vector<Vec2> points;

		for (size_t i = 0; i < poly.Points.size(); i++)
		{
			Vec2 vecIntersection;

			if (poly.Points[i].Intersects(poly.Points[i != poly.Points.size() - 1 ? i + 1 : 0], start, end, &vecIntersection))
				points.push_back(vecIntersection);
		}

		return points;
	}

	struct Arc
	{
		float Distance;
		Vec2 End;
		int Radius;
		Vec2 Start;

		Arc() { }

		Arc(Vec2 start, Vec2 end, int radius)
		{
			Start = start;
			End = end;
			Radius = radius;
			Distance = (End - Start).Length();
		}

		Geometry::IPolygon ToPolygon(int extraRadius = 0)
		{
			extraRadius += Radius;

			Geometry::IPolygon result;

			auto outerRadius = (0.35256f * Distance + 133.f) / cos(2 * M_PI / Quality);
			auto innerRadius = -0.1562f * Distance + 687.31f;
			auto outerCenter = Geometry::CircleCircleIntersection(Start, End, outerRadius, outerRadius)[0];
			auto innerCenter = Geometry::CircleCircleIntersection(Start, End, innerRadius, innerRadius)[0];

			auto dir = (End - outerCenter).VectorNormalize();
			auto step = -(float)(dir.AngleBetween((Start - outerCenter).VectorNormalize()) * M_PI / 180) / Quality;

			for (auto i = 0; i < Quality; i++)
				result.Add(outerCenter + (outerRadius + 15 + extraRadius) * dir.Rotated(step * i));

			dir = (Start - innerCenter).VectorNormalize();
			step = (float)(dir.AngleBetween((End - innerCenter).VectorNormalize()) * M_PI / 180) / Quality;

			for (auto i = 0; i < Quality; i++)
				result.Add(innerCenter + std::max<float>(0.0, innerRadius - extraRadius - 100.f) * dir.Rotated(step * i));

			return result;
		}
	};

	struct Circle
	{
		Vec2 Center;
		int Radius;

		Circle() { }

		Circle(Vec2 center, int radius)
		{
			Center = center;
			Radius = radius;
		}

		Geometry::IPolygon ToPolygon(int extraRadius = 0, float overrideRadius = -1)
		{
			Geometry::IPolygon result;

			const double Step = 2.f * M_PI / static_cast<float>(Quality);
			auto radius = overrideRadius > 0 ? overrideRadius : (extraRadius + Radius) / cos(Step);
			auto angle = (double)Radius;

			for (auto i = 0; i <= Quality; i++)
			{
				angle += Step;

				result.Add(Vec2(Center.x + radius * cosf(angle), Center.y + radius * sinf(angle)));
			}

			return result;
		}
	};

	struct Cone
	{
		Vec2 Center;
		Vec2 Direction;
		float Radius;
		int Range;

		Cone() { }

		Cone(Vec2 center, Vec2 direction, int radius, int range)
		{
			Center = center;
			Direction = direction;
			Radius = radius * M_PI / 180;
			Range = range;
		}

		Geometry::IPolygon ToPolygon(int extraRadius = 0)
		{
			Geometry::IPolygon result;

			result.Add(Center);

			auto radius = (Range + extraRadius) / cos(2 * M_PI / Quality);
			auto side = Direction.Rotated(-Radius * 0.5f);

			for (auto i = 0; i <= Quality; i++)
			{
				auto dir = side.Rotated(i * Radius / Quality).VectorNormalize();
				result.Add(Vec2(Center.x + radius * dir.x, Center.y + radius * dir.y));
			}

			return result;
		}
	};

	struct Line
	{
		Vec2 Direction;
		Vec2 Perpendicular;
		Vec2 End;
		int Radius;
		Vec2 Start;

		Line() { }

		Line(Vec2 start, Vec2 end, int radius)
		{
			Start = start;
			End = end;
			Radius = radius;
			Direction = (End - Start).VectorNormalize();
			Perpendicular = Direction.Perpendicular();
		}

		Geometry::IPolygon ToPolygon(int extraRadius = 0, float overrideRadius = -1)
		{
			Geometry::IPolygon result;
			auto radius = (overrideRadius > 0 ? overrideRadius : Radius + extraRadius) * Perpendicular;
			auto dir = static_cast<float>(extraRadius) * Direction;
			result.Add(Start + radius - dir);
			result.Add(Start - radius - dir);
			result.Add(End - radius + dir);
			result.Add(End + radius + dir);

			return result;
		}
	};

	struct Ring
	{
		Vec2 Center;
		int InnerRadius;
		int OuterRadius;

		Ring() { }

		Ring(Vec2 center, int innerRadius, int outerRadius)
		{
			Center = center;
			InnerRadius = innerRadius;
			OuterRadius = outerRadius;
		}

		Geometry::IPolygon ToPolygon(int extraRadius = 0)
		{
			Geometry::IPolygon result;

			const double Step = 2 * M_PI / Quality;
			auto outerRadius = (extraRadius + OuterRadius) / cos(Step);
			auto innerRadius = (InnerRadius - extraRadius) / cos(Step);

			auto outerAngle = (double)OuterRadius;

			for (auto i = 0; i <= Quality; i++)
			{
				outerAngle += Step;
				result.Add(Vec2(Center.x + outerRadius * cos(outerAngle), Center.y + outerRadius * sin(outerAngle)));
			}

			auto innerAngle = (double)InnerRadius;

			for (auto i = 0; i <= Quality; i++)
			{
				innerAngle += Step;
				result.Add(Vec2(Center.x + innerRadius * cos(innerAngle), Center.y + innerRadius * sin(innerAngle)));
			}

			return result;
		}
	};
}

#endif // Polygons_h__