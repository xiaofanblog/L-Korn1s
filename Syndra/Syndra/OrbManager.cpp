#include "OrbManager.h"
#include "PluginSDK.h"
#include "Vector3.h"

//CREDITS TO : IJAVA C:

static int NetworkId = -1;
static int tempQOrbTime;
static Vec3 tempQOrbPosition;
static int tempWOrbTime;
static Vec3 tempWOrbPosition;

int OrbManager::GetNetworkId()
{
	if (GEntityList->Player()->GetSpellBook()->GetToggleState(kSlotW) == 1)
		return -1;

	return NetworkId;
}

void OrbManager::OnPauseAnimation(IUnit* source)
{
	for (auto unit : GEntityList->GetAllUnits())
	{
		if (unit != nullptr && source == unit && !source->IsHero())
		{
			NetworkId = unit->GetNetworkId();
		}
	}
}

void OrbManager::OnProcessSpellCast(CastedSpell const& args)
{
	if (args.Caster_ == GEntityList->Player())
	{
		if (GEntityList->Player() && std::string(args.Name_) == "SyndraQ")
		{
			tempQOrbTime = GGame->TickCount();
			tempQOrbPosition = args.EndPosition_;
		}

		if (GEntityList->Player() && GetWObject(true) != nullptr && (std::string(args.Name_) == "SyndraW" || std::string(args.Name_) == "SyndraW2"))
		{
			tempWOrbTime = GGame->TickCount() + 250;
			tempWOrbPosition = args.EndPosition_;
		}
	}
}

IUnit* OrbManager::GetWObject(bool orbsOnly)
{
	if (GetNetworkId() == -1)
		return nullptr;

	auto unit = GEntityList->GetEntityByNetworkId(GetNetworkId());

	if (unit == nullptr)
		return nullptr;

	if (orbsOnly && std::string(unit->GetObjectName()) == "Seed" || !orbsOnly && unit->IsValidObject())
	{
		return unit;
	}

	return nullptr;
}

std::vector<Vec3> OrbManager::GetOrbs(bool toGrab)
{
	std::vector<Vec3> result;

	for (auto orb : GEntityList->GetAllUnits())
	{
		if (orb != nullptr && orb->GetTeam() == GEntityList->Player()->GetTeam() && !orb->IsDead() && std::string(orb->GetObjectName()) == "Seed")
		{
			auto valid = false;
			if (orb->GetNetworkId() != GetNetworkId())
			{
				for (auto b : GEntityList->GetAllUnits())
					if (b->IsValidObject() && (orb->GetPosition() - b->GetPosition()).Length() < 50
						&& std::string(b->GetObjectName()).find("Syndra_")
						&& std::string(b->GetObjectName()).find("_Q_")
						&& std::string(b->GetObjectName()).find("idle"))
						valid = true;
			}
			if (valid && (!toGrab || !orb->IsMoving()))
			{
				result.push_back(orb->ServerPosition());
			}
		}
	}

	if (GGame->TickCount() - tempQOrbTime < 400)
	{
		result.push_back(tempQOrbPosition);
	}
	if (GGame->TickCount() - tempWOrbTime < 400 && GGame->TickCount() - tempWOrbTime > 0)
	{
		result.push_back(tempWOrbPosition);
	}
	return result;
}

void OrbManager::ProjectOn(Vec2 const& Point, Vec2 const& SegmentStart, Vec2 const& SegmentEnd, ProjectionInfo& Out)
{
	auto cx = Point.x;
	auto cy = Point.y;
	auto ax = SegmentStart.x;
	auto ay = SegmentStart.y;
	auto bx = SegmentEnd.x;
	auto by = SegmentEnd.y;
	auto rL = ((cx - ax) * (bx - ax) + (cy - ay) * (by - ay)) /
		(powf(bx - ax, 2) + powf(by - ay, 2));
	auto pointLine = Vec2(ax + rL * (bx - ax), ay + rL * (by - ay));
	float rS;
	if (rL < 0)
	{
		rS = 0;
	}
	else if (rL > 1)
	{
		rS = 1;
	}
	else
	{
		rS = rL;
	}

	auto isOnSegment = (rS == rL);
	auto pointSegment = isOnSegment ? pointLine : Vec2(ax + rS * (bx - ax), ay + rS * (by - ay));

	Out = ProjectionInfo(isOnSegment, pointSegment, pointLine);
}

float OrbManager::GetDistance(Vec3 from, Vec3 to)
{
	auto x1 = from.x;
	auto x2 = to.x;
	auto y1 = from.y;
	auto y2 = to.y;
	auto z1 = from.z;
	auto z2 = to.z;
	return static_cast<float>(sqrt(pow((x2 - x1), 2.0) + pow((y2 - y1), 2.0) + pow((z2 - z1), 2.0)));
}

float OrbManager::GetDistanceV2(Vec2 from, Vec2 to)
{
	auto x1 = from.x;
	auto x2 = to.x;
	auto y1 = from.y;
	auto y2 = to.y;
	return static_cast<float>(sqrt(pow((x2 - x1), 2.0) + pow((y2 - y1), 2.0) + pow(GNavMesh->GetHeightForPoint(to) - GNavMesh->GetHeightForPoint(from), 2.0)));
}

Vec3 OrbManager::GetOrbToGrab(int range)
{
	auto orbList = GetOrbs(true);
	std::vector<Vec3> result;

	if (orbList.size() > 0)
	{
		for (auto orb : orbList)
		{
			if (GetDistance(GEntityList->Player()->ServerPosition(), orb) <= range)
				result.push_back(orb);
		}
	}
	return result.size() > 0 ? result[0] : Vec3(0, 0, 0);
}

float OrbManager::Distance(Vec2 point, Vec2 start, Vec2 End, bool onlyOnSegment)
{
	ProjectionInfo projection_info;
	ProjectOn(point, start, End, projection_info);

	if (projection_info.IsOnSegment || onlyOnSegment == false)
	{
		return projection_info.SegmentPoint.Distance(point);
	}

	return FLT_MAX;
}