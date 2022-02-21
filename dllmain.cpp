#include <Windows.h>
#include <detours.h>

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cstdio>

#include "loader.h"

#include "address.h"
#include "pointer.h"
#include "hook.h"
#include "structure.h"

namespace pkodev
{
    // Load mod settings
    void load_settings(const std::string& path, std::vector<pkodev::map>& maps);

    // List of maps
    std::vector<pkodev::map> maps;
}


// Entry point
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    // Nothing to do . . .
    return TRUE;
}

// Get mod information
void GetModInformation(mod_info& info)
{
    strcpy_s(info.name, TOSTRING(MOD_NAME));
    strcpy_s(info.version, TOSTRING(MOD_VERSION));
    strcpy_s(info.author, TOSTRING(MOD_AUTHOR));
    info.exe_version = MOD_EXE_VERSION;
}

// Start the mod
void Start(const char* path)
{
    // Build path to the mod settings file
    char buf[MAX_PATH]{ 0x00 };
    sprintf_s(buf, sizeof(buf), "%s\\%s.cfg", path, TOSTRING(MOD_NAME));
    
    // Load the list of maps
    pkodev::load_settings(std::string(buf), pkodev::maps);

    // Print settings
    if (pkodev::maps.size() > 0)
    {
        // Print a log
        std::cout << "[" << TOSTRING(MOD_NAME) << "] (" << pkodev::maps.size() << ") maps found!" << std::endl;

        // Print the list of maps
        for (const auto& map : pkodev::maps)
        {
            // Print a log
            std::cout << "[" << TOSTRING(MOD_NAME) << "] * Minimap added: " << map.name << " (" << map.width << " x " << map.height << ")" << std::endl;
        }
    }
    else
    {
        // Print a log
        std::cout << "[" << TOSTRING(MOD_NAME) << "] No maps found!" << std::endl;
    }

    // Enable hooks
    DetourRestoreAfterWith();
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)pkodev::pointer::CPlayer__CPlayer, pkodev::hook::CPlayer__CPlayer);
    DetourTransactionCommit();
}

// Stop the mod
void Stop()
{
    // Disable hooks
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourDetach(&(PVOID&)pkodev::pointer::CPlayer__CPlayer, pkodev::hook::CPlayer__CPlayer);
    DetourTransactionCommit();
}

// CPlayer::CPlayer()
void __fastcall pkodev::hook::CPlayer__CPlayer(void* This, void* NotUsed)
{
    // Call the original method CPlayer::CPlayer()
    pkodev::pointer::CPlayer__CPlayer(This);

    // Get pointer to the CMaskData object
    void* CMaskData__Ptr = reinterpret_cast<void*>(
        reinterpret_cast<unsigned int>(This) + 0x10A4
    );

    // Add minimaps
    for (const auto& map : pkodev::maps)
    {
        // Add minimap
        pkodev::pointer::CMaskData__AddMap(CMaskData__Ptr, 
            map.name.c_str(), map.width, map.height);
    }
}

// Load mod settings
void pkodev::load_settings(const std::string& path, 
    std::vector<pkodev::map>& maps)
{
    // Buffer for map paths
    char buf[MAX_PATH]{ 0x00 };

    // Clear the old list with maps
    maps.clear();

    // Open the file
    std::ifstream file(path, std::ios::in);

    // Check that file is open
    if (file.is_open() == false)
    {
        // Error
        return;
    }

    // Read file line-by-line
    for (std::string line; std::getline(file, line); )
    {
        // Remove spaces from the line
        line.erase(std::remove_if(line.begin(), line.end(), std::isspace), line.end());

        // Check that line is not empty
        if (line.empty() == true)
        {
            // Skip line
            continue;
        }

        // Search the map in the list
        const auto it = std::find_if(
            pkodev::maps.begin(),
            pkodev::maps.end(),
            [&line](const pkodev::map& map_) -> bool
            {
                return line == map_.name;
            }
        );

        // Check that the map is exist in the list
        if (it != pkodev::maps.end())
        {
            // Skip the map
            continue;
        }

        // Build the map name
        sprintf_s(buf, sizeof(buf), "resource\\%s\\%s.atr", 
            line.c_str(), line.c_str());

        // Open the map file
        std::ifstream map(buf, std::ios::in | std::ios::binary);

        // Check that file is open
        if (map.is_open() == false)
        {
            // Skip the map
            continue;
        }

        // Read map size
        unsigned int width = 0;
        unsigned int height = 0;
        map.read(reinterpret_cast<char*>(&width), sizeof(width));
        map.read(reinterpret_cast<char*>(&height), sizeof(height));

        // Close the map file
        map.close();

        // Add the map to the maps list
        maps.push_back({ line, width, height });
    }

    // Close the settings file
    file.close();
}