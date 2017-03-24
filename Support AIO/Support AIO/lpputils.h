#pragma once
#include "PluginSDK.h"
#include "EventManager.h"
#include <algorithm>
#include <unordered_map>
#include <windows.h>
#include <locale>
#include <codecvt>
#include <fstream>
#include <unordered_set>

#define M_PI 3.14159265358979323846

#pragma comment(lib, "urlmon")

class LPPUtils
{
public:
    static double AngleBetween(Vec3 a, Vec3 b, bool degrees = true)
    {
        return (acos((a.DotProduct(b)) / (a.Length() * b.Length()))) * (degrees ? (180.f / M_PI) : 1);
    }
    static void TrimString(std::string &str, char c)
    {
        str.erase(std::remove(str.begin(), str.end(), c), str.end());
    }
    static void TrimString(std::string &str, const std::vector<char> &chars)
    {
        for (auto c : chars)
        {
            LPPUtils::TrimString(str, c);
        }
    }
    static void DelayAction(IPluginSDK *sdk, std::function<void()> action, unsigned long ticks)
    {
        auto startTick = GetTickCount();
        eventmanager::GameEventManager::RegisterUpdateEvent([=](event_id_t id) -> void {

            if ((GetTickCount() - startTick) > ticks)
            {
                action();
                eventmanager::GameEventManager::UnregisterUpdateEvent(id);
                return;
            }
        });
    }
    static void DelayActionGame(IPluginSDK *sdk, std::function<void()> action, float interval)
    {
        static auto game = sdk->GetGame();

        auto startTime = game->Time();
        eventmanager::GameEventManager::RegisterUpdateEvent([=](event_id_t id) -> void {

            if ((game->Time() - startTime) > interval)
            {
                action();
                eventmanager::GameEventManager::UnregisterUpdateEvent(id);
                return;
            }
        });
    }
    static bool DownloadFile(IPluginSDK *sdk, std::string name, std::string subpath, std::string urlString, bool downloadLocal = false)
    {
        if (LPPUtils::FileExists(sdk, name, subpath))
        {
            if (!downloadLocal)
            {
                return true;
            }
        }

        std::string basePath;
        sdk->GetBaseDirectory(basePath);

        basePath += "\\" + subpath + "\\" + name;

        auto url = std::wstring(urlString.begin(), urlString.end());
        auto path = std::wstring(basePath.begin(), basePath.end());

        auto urlC = url.c_str();
        auto pathC = path.c_str();

        if (downloadLocal)
        {
            ::DeleteFile(pathC);
        }

        auto result = ::URLDownloadToFile(nullptr, urlC, pathC, 0, nullptr);

        if (result == S_OK)
        {
            return true;
        }

        return false;
    }

    static bool FileExists(IPluginSDK *sdk, std::string file, std::string subPath)
    {
        std::string finalPath;
        sdk->GetBaseDirectory(finalPath);

        finalPath += "\\" + subPath + "\\" + file;

        auto hFile = CreateFileA(finalPath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

        if (hFile != INVALID_HANDLE_VALUE)
        {
            CloseHandle(hFile);
            return true;
        }

        return false;
    }

    static ITexture *GetTextureForChampion(IPluginSDK *sdk, IUnit *unit)
    {
        static auto map = std::unordered_map<std::string, ITexture *>();
        auto name = std::string(unit->ChampionName()) + std::string(".png");

        if (map.count(name))
        {
            return map[name];
        }

        static auto renderer = sdk->GetRenderer();

        if (LPPUtils::DownloadFile(sdk, name, "Textures\\Utility\\ChampionIcons", std::string("http://sk2.op.gg/images/lol/champion/" + name)))
        {
            auto texture = renderer->CreateTextureFromFile((std::string("Utility\\ChampionIcons\\") + name).c_str());
            map[name] = texture;

            return texture;
        }

        auto texture = nullptr;
        map[name] = texture;

        return texture;
    }

    static ITexture *GetTextureForSpell(IPluginSDK *sdk, IUnit *unit, int slot)
    {
        static auto map = std::unordered_map<std::string, ITexture *>();
        auto name = std::string(unit->GetSpellName(slot)) + std::string(".png");

        if (map.count(name))
        {
            return map[name];
        }

        static auto renderer = sdk->GetRenderer();

        if (LPPUtils::DownloadFile(sdk, name, "Textures\\Utility\\SpellIcons", std::string("http://sk2.op.gg/images/lol/spell/" + name)))
        {
            auto texture = renderer->CreateTextureFromFile((std::string("Utility\\SpellIcons\\") + name).c_str());
            map[name] = texture;

            return texture;
        }

        auto texture = nullptr;
        map[name] = texture;

        return texture;
    }

    static bool DownloadFileToString(IPluginSDK *sdk, std::string urlString, std::string &content)
    {
        wchar_t path[MAX_PATH];

        auto url = std::wstring(urlString.begin(), urlString.end());

        auto result = ::URLDownloadToCacheFile(nullptr, url.c_str(), path, sizeof(path), 0, nullptr);

        if (result == S_OK)
        {
            auto file = ::CreateFile(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_HIDDEN, nullptr);

            if (file != INVALID_HANDLE_VALUE)
            {
                auto size = ::GetFileSize(file, nullptr);
                auto fileContents = new char[size];
                DWORD count;

                if (::ReadFile(file, fileContents, size, &count, nullptr))
                {
                    content = std::string(fileContents);

                    delete[] fileContents;
                    CloseHandle(file);

                    return true;
                }
                delete[] fileContents;

                CloseHandle(file);
            }
        }

        return false;
    }
    static bool ChampionInGame(IPluginSDK *sdk, std::string championName)
    {
        static auto champions = sdk->GetEntityList()->GetAllHeros(true, true);

        for (auto champion : champions)
        {
            if (std::string(champion->ChampionName()) == championName)
            {
                return true;
            }
        }

        return false;
    }
    static Vec3 GetSpawnPos(IPluginSDK *sdk, int team)
    {
        static auto game = sdk->GetGame();

        if (game->GetMapId() == kMapSummonersRift)
        {
            switch (team)
            {
            case kTeam1:
                return Vec3(408.108887f, 182.849243f, 423.148804f);
            case kTeam2:
                return Vec3(14298.314453f, 171.977539f, 14378.306641f);
            }

            return Vec3(0.f, 0.f, 0.f);
        }

        static auto entityList = sdk->GetEntityList();
        static auto player = entityList->Player();

        return player->GetTeam() == team ? entityList->GetTeamNexus()->GetPosition() : entityList->GetEnemyNexus()->GetPosition();
    }

    static eSpellSlot GetSummonerSpellByName(IPluginSDK *sdk, std::string name)
    {
        static auto player = sdk->GetEntityList()->Player();

        auto spellBook = player->GetSpellBook();

        if (std::string(spellBook->GetName(kSummonerSlot1)) == name)
        {
            return kSummonerSlot1;
        }

        if (std::string(spellBook->GetName(kSummonerSlot2)) == name)
        {
            return kSummonerSlot2;
        }

        return kSlotUnknown;
    }

    template<typename T>
    static void RemoveIfExists(std::vector<T> vec, T e)
    {
        vec.erase(std::remove(vec.begin(), vec.end(), e), vec.end());
    }

    static int CountEnemiesNearMe(IPluginSDK *sdk, float range)
    {
        static auto entityList = sdk->GetEntityList();
        static auto player = entityList->Player();

        auto count = 0;

        for (auto unit : entityList->GetAllHeros(false, true))
        {
            if (player->IsValidTarget(unit, range))
            {
                count++;
            }
        }

        return count;
    }

    static int CountEnemiesInAttackRange(IPluginSDK *sdk)
    {
        static auto entityList = sdk->GetEntityList();
        static auto player = entityList->Player();

        auto count = 0;

        for (auto unit : entityList->GetAllHeros(false, true))
        {
            if (player->IsValidTarget(unit, player->GetRealAutoAttackRange(unit)))
            {
                count++;
            }
        }

        return count;
    }

    static bool InFountain(IPluginSDK *sdk, IUnit *unit)
    {
        static auto utility = sdk->GetUtility();

        return utility->IsPositionInFountain(unit->GetPosition());
    }

    static bool IsTeleporting(IUnit *unit, IPluginSDK *sdk)
    {
        static auto buffReader = sdk->GetBuffDataReader();

        std::vector<void *> dataVector;
        unit->GetAllBuffsData(dataVector);

        for (auto data : dataVector)
        {
            auto name = std::string(buffReader->GetBuffName(data));
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);

            if (name.find("recall") != std::string::npos || name.find("teleport") != std::string::npos)
            {
                return true;
            }
        }

        return false;
    }

    static Vec2 To2D(Vec3 vec)
    {
        return Vec2(vec.x, vec.z);
    }

    static Vec2 V2E(Vec3 from, Vec3 direction, float distance)
    {
        return LPPUtils::To2D(from) + LPPUtils::To2D(distance * (direction - from).VectorNormalize());
    }

    static Vec2 Extend(const Vec2 &from, const Vec2 &to, float distance)
    {
        return from + (distance * (to - from).VectorNormalize());
    }

    static Vec3 Extend(const Vec3 &from, const Vec3 &to, float distance)
    {
        return from + (distance * (to - from).VectorNormalize());
    }

    static Vec3 To3D(IPluginSDK *sdk, Vec2 v)
    {
        static auto player = sdk->GetEntityList()->Player();

        return Vec3(v.x, player->GetPosition().y, v.y);
    }

    static void DrawHpBarDamage(IRender *renderer, IUnit *hero, float damage, Vec4 &color)
    {
        //Credit to xSalice
        static auto font = renderer->CreateFontW("monospace", 12.f, kFontWeightThin);

        const auto xOffset = 10;
        const auto yOffset = 20;
        const auto width = 103;
        const auto height = 8;

        Vec2 barPos;
        if (hero->GetHPBarPosition(barPos))
        {
            auto hp = hero->GetHealth();
            auto maxHp = hero->GetMaxHealth();
            auto percentHealthAfterDamage = max(0, hp - damage) / maxHp;
            auto yPos = barPos.y + yOffset;
            auto xPosDamage = barPos.x + xOffset + (width * percentHealthAfterDamage);
            auto xPosCurrentHp = barPos.y + xOffset + ((width * hp) / maxHp);

            if (damage > hp)
            {
                font->Render((barPos.x + xOffset), (barPos.y + yOffset - 13.f), "Killable: %d", (hp - damage));
            }

            renderer->DrawLine(Vec2(xPosDamage, yPos), Vec2(xPosDamage, yPos + height), color);

            auto differenceInHp = xPosCurrentHp - xPosDamage;
            auto pos1 = barPos.x + 9 + (107 * percentHealthAfterDamage);

            for (auto i = 0; i < differenceInHp; i++)
            {
                renderer->DrawLine(Vec2(pos1 + i, yPos), Vec2(pos1 + i, yPos + height), color);
            }
        }
    }

    static void DrawLFCircle(IRender *renderer, IGame *game, const Vec3 &position, const Vec4 &color, float radius, int quality = 30)
    {
        //Credits to L#.Common

        auto points = std::vector<Vec3>();
        for (auto i = 0; i < quality; i++)
        {
            auto angle = i * M_PI * 2 / quality;
            points.push_back(
                Vec3(
                    position.x + static_cast<float>(radius * cos(angle)),
                    position.y,
                    position.z + static_cast<float>(radius * sin(angle))
                )
            );
        }

        for (size_t i = 0; i < points.size(); i++)
        {
            auto a = points[i];
            auto b = points[i == points.size() - 1 ? 0 : i + 1];

            Vec3 aonScreen, bonScreen;
            game->Projection(a, &aonScreen);
            game->Projection(b, &bonScreen);

            if (aonScreen.z < 1.f && bonScreen.z < 1.f)
            {
                renderer->DrawLine(Vec2(aonScreen.x, aonScreen.y), Vec2(bonScreen.x, bonScreen.y), color);
            }
        }
    }
    static void DrawCircleMinimap(IRender *renderer, IGame *game, const Vec3 &position, const Vec4 &color, float radius, int quality = 30)
    {
        //Credits to L#.Common
        auto points = std::vector<Vec3>();
        for (auto i = 0; i < quality; i++)
        {
            auto angle = i * M_PI * 2 / quality;
            points.push_back(
                Vec3(
                    position.x + static_cast<float>(radius * cos(angle)),
                    position.y,
                    position.z + static_cast<float>(radius * sin(angle))
                )
            );
        }

        for (size_t i = 0; i < points.size(); i++)
        {
            auto a = points[i];
            auto b = points[i == points.size() - 1 ? 0 : i + 1];

            Vec2 aonScreen, bonScreen;
            game->WorldToMinimap(a, aonScreen);
            game->WorldToMinimap(b, bonScreen);

            renderer->DrawLine(aonScreen, bonScreen, color);
        }

    }

    static bool IsKeyDown(IMenuOption *menuOption)
    {
        return GetAsyncKeyState(menuOption->GetInteger()) & 0x8000;
    }

    static IMenuOption *AddStringList(IMenu *menu, std::string name, std::vector<std::string> options, int defaultIndex = 0)
    {
        auto optionsCount = options.size();

        auto menuOption = menu->AddInteger(name.c_str(), 0, optionsCount, defaultIndex);

        auto checkBoxes = std::vector<IMenuOption *>();
        auto map = std::unordered_map<int, bool>();

        for (size_t i = 0; i < optionsCount; i++)
        {
            checkBoxes.push_back(menu->CheckBox(options[i].c_str(), false));
            map[i] = false;
        }

        eventmanager::GameEventManager::RegisterUpdateEvent([=](event_id_t id) mutable -> void {
            for (size_t i = 0; i < optionsCount; i++)
            {
                auto enabled = checkBoxes[i]->Enabled();

                if (!map[i] && enabled)
                {
                    menuOption->UpdateInteger(i);
                }
                map[i] = enabled;

                if (menuOption->GetInteger() != i && checkBoxes[i]->Enabled())
                {
                    checkBoxes[i]->UpdateInteger(0);
                }
                else if (menuOption->GetInteger() == i && !checkBoxes[i]->Enabled())
                {
                    checkBoxes[i]->UpdateInteger(1);
                }
            }
        });

        return menuOption;
    }

    static IMenuOption *AddKeyBindToggle(IMenu *menu, std::string name, char key, bool enabled)
    {
        auto menuOption = menu->CheckBox(name.c_str(), enabled);
        auto toggleOption = menu->AddKey((name + " Toggle").c_str(), key);

        eventmanager::GameEventManager::RegisterUpdateEvent([=](event_id_t id) mutable -> void {
            static auto previousState = false;

            auto currentState = LPPUtils::IsKeyDown(toggleOption);

            if (previousState && !currentState)
            {
                menuOption->UpdateInteger(!menuOption->GetInteger());
            }

            previousState = currentState;
        });

        return menuOption;
    }

    /// <summary>
    ///     Defines whether a Potion is running.
    /// </summary>
    static bool IsPotRunning(IPluginSDK *sdk)
    {
        static auto player = sdk->GetEntityList()->Player();
        return player->HasBuff("ItemCrystalFlask") || player->HasBuff("RegenerationPotion")
            || player->HasBuff("ItemMiniRegenPotion")
            || player->HasBuff("ItemDarkCrystalFlask")
            || player->HasBuff("ItemCrystalFlaskJungle");
    }

    /// <summary>
    ///     Gets a value indicating whether BuffType is worth cleansing.
    /// </summary>
    static bool ShouldCleanse(IPluginSDK *sdk)
    {
        static auto player = sdk->GetEntityList()->Player();
        auto enemyInRange = false;
        for (auto enemy : GEntityList->GetAllHeros(false, true))
        {
            if (enemy == nullptr) return false;
            if (player->IsValidTarget(enemy, 1500.f))
            {
                enemyInRange = true;
                break;
            }
        }
        if (!player->IsInvulnerable() && enemyInRange
            &&  player->HasBuffOfType(BUFF_Stun) || player->HasBuffOfType(BUFF_Snare) || player->HasBuffOfType(BUFF_Flee)
            || player->HasBuffOfType(BUFF_Charm) || player->HasBuffOfType(BUFF_Taunt)
            || player->HasBuffOfType(BUFF_Polymorph) || player->HasBuff("ThreshQ")
            || player->HasBuff("SummonerDot"))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    /// <summary>
    ///     Defines whether the player should use a cleanser.
    /// </summary>
    static bool ShouldUseCleanser(IPluginSDK *sdk)
    {
        static auto player = sdk->GetEntityList()->Player();

        auto enemyInRange = false;
        for (auto enemy : GEntityList->GetAllHeros(false, true))
        {
            if (enemy == nullptr) return false;
            enemyInRange = player->IsValidTarget(enemy, 1500.f) ? true : false;
        }
        return !player->IsInvulnerable()
            && enemyInRange
            && (player->HasBuffOfType(BUFF_Suppression)
                || player->HasBuff("ThreshQ")
                || player->HasBuff("summonerexhaust")
                || player->HasBuff("vladimirhemoplague")
                || player->HasBuff("mordekaiserchildrenofthegrave"));
    }

    static std::string ReadDataFile(IPluginSDK *sdk, std::string name, std::string subPath, std::string url)
    {
        auto pathFromLoader = "Data\\" + subPath;

        if (LPPUtils::DownloadFile(sdk, name, pathFromLoader, url, true))
        {
            std::string basePath;
            sdk->GetBaseDirectory(basePath);

            basePath += std::string("\\") + pathFromLoader + std::string("\\") + name;

            auto t = std::ifstream(basePath);
            auto str = std::string((std::istreambuf_iterator<char>(t)),
                std::istreambuf_iterator<char>());

            return str;
        }

        return std::string();
    };
};