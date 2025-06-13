#pragma once
#include <string>
#include <vector>
#include <map>

// --- Minimal forward declarations of core GameGuru structures ---
// These would normally be defined in actual GameGuru headers like "globstruct.h" or "gameguru.h"

// Typedefs for basic types often used in GameGuru (assuming 32-bit compatibility for some if needed)
typedef unsigned long DWORD;
typedef char* LPSTR;

// Forward declaration for sObject to be used in other structs if necessary
struct sObject;

// Simplified representation of GameGuru's global 'g' structure
// Only include members relevant to AI obstacle generation based on previous analysis.
struct SGLOBALS_Minimal
{
    // Paths
    std::string fpscrootdir_s;          // Root path of GameGuru files
    std::string mysystem_levelBankTestMap_s; // Path to the current level's folder (e.g., "levelbank\\testmap\\")

    // Entity Data
    int entityelementlist;          // Total number of entities in the current map
    // t.entityelement would be a std::vector<sEntityElement_Minimal> accessed via MinimalEngine::g_entityelement_sim
    // t.entityprofile would be a std::vector<sEntityProfile_Minimal> accessed via MinimalEngine::g_entityprofile_sim

    // Object Management
    int guishadereffectindex;       // Example: a shader index AIStart might need
    int entityviewstartobj;         // Start object ID for general entities
    int entityviewendobj;           // End object ID for general entities

    // Waypoint Data
    int waypointmax;                // Maximum number of waypoints/zones
    // t.waypoint would be std::vector<WaypointItem_Minimal>
    // t.waypointcoord would be std::vector<WaypointCoord_Minimal>

    // AI System related (mirroring parts of t.aisystem)
    float aisystem_obstacleradius;
    int aisystem_terrainobsnum;     // Object number for terrain obstacles
    bool aisystem_generateobs;      // Flag to control runtime obstacle generation

    // Constructor to provide some default values
    SGLOBALS_Minimal() : entityelementlist(0), guishadereffectindex(0),
                         entityviewstartobj(0), entityviewendobj(0),
                         waypointmax(0), aisystem_obstacleradius(50.0f),
                         aisystem_terrainobsnum(0), aisystem_generateobs(true) {}
};

// Simplified representation of GameGuru's temporary 't' structure
struct STEMPS_Minimal
{
    // Entity context for darkai_setup_entity/tree (normally set in a loop)
    int e;                          // Current entity element index
    int entid;                      // Current entity profile bank ID
    int tobj;                       // Current entity's object ID
    int ttreemode;                  // For darkai_setup_tree
    int tfullheight;                // For darkai_setup_entity/tree
    int tcontainerid;               // Current AI container ID for obstacle

    // Terrain Data (mirroring parts of t.terrain)
    int terrain_TerrainID;          // ID of the terrain object
    // float terrain_playerx_f, terrain_playerz_f; // Player position on terrain (if needed by BT_GetGroundHeight indirectly)

    // Waypoint context
    int twaypointindex;             // Current waypoint index for path/zone creation

    STEMPS_Minimal() : e(0), entid(0), tobj(0), ttreemode(0), tfullheight(0), tcontainerid(0),
                       terrain_TerrainID(0), twaypointindex(0) {}
};

// Minimal sObject structure (actual one is very large)
struct sObject_Minimal {
    int iID;
    bool bExists; // To simulate ObjectExist
    // Collision geometry data (simplified)
    struct CollisionData {
        float fX, fY, fZ; // Center of collision box
        float fRadiusX, fRadiusY, fRadiusZ; // Half-sizes of collision box
        float fObjectSizeX, fObjectSizeY, fObjectSizeZ; // Overall size
        float fColRadius; // Overall collision radius
    } collision;

    // Position and rotation (simplified)
    float X, Y, Z;
    float AngX, AngY, AngZ;

    sObject_Minimal(int id) : iID(id), bExists(false), X(0), Y(0), Z(0), AngX(0), AngY(0), AngZ(0) {
        collision.fX = 0; collision.fY = 0; collision.fZ = 0;
        collision.fRadiusX = 0; collision.fRadiusY = 0; collision.fRadiusZ = 0;
        collision.fObjectSizeX = 0; collision.fObjectSizeY = 0; collision.fObjectSizeZ = 0;
        collision.fColRadius = 0;
    }
};

// Minimal sEntityElement structure
struct sEntityElement_Minimal {
    int bankindex;
    int obj; // Object ID
    int staticflag;
    float x, y, z, rx, ry, rz; // Position and rotation
    // Add other eleprof members if needed by generator logic, e.g. from t.entityelement[e].eleprof
    struct EleprofMinimal {
        // ... relevant FPE properties
        int collisionmode;
        int ismarker;
        int forcesimpleobstacle; // 0=default, 1=simplebox, 2=complex shape, 3=slice
        float forceobstaclesliceheight;
        float forceobstaclesliceminsize;
        // ...
        EleprofMinimal() : collisionmode(0), ismarker(0), forcesimpleobstacle(0),
                           forceobstaclesliceheight(0), forceobstaclesliceminsize(0) {}
    } eleprof;


    sEntityElement_Minimal() : bankindex(0), obj(0), staticflag(0),
                               x(0), y(0), z(0), rx(0), ry(0), rz(0) {}
};

// Minimal sEntityProfile structure
struct sEntityProfile_Minimal {
    int ismarker;
    int collisionmode;
    int forcesimpleobstacle;
    float forceobstaclesliceheight;
    float forceobstaclesliceminsize;
    // ... other profile fields if needed
    sEntityProfile_Minimal() : ismarker(0), collisionmode(0), forcesimpleobstacle(0),
                               forceobstaclesliceheight(0), forceobstaclesliceminsize(0) {}
};

// Minimal Waypoint structures
struct WaypointItem_Minimal {
    int style; // 1 for path, 3 for zone
    int count;
    int start;
    int finish;
    WaypointItem_Minimal() : style(0), count(0), start(0), finish(0) {}
};

struct WaypointCoord_Minimal {
    float x, y, z;
    WaypointCoord_Minimal() : x(0),y(0),z(0) {}
};


namespace MinimalEngine {

    // --- Simulated Globals (accessible as MinimalEngine::g and MinimalEngine::t) ---
    extern SGLOBALS_Minimal g;
    extern STEMPS_Minimal t;

    // Simulated entity and profile lists (normally part of 'g' or 't' or managed by entity system)
    extern std::vector<sEntityElement_Minimal> entityelements_sim;
    extern std::map<int, sEntityProfile_Minimal> entityprofiles_sim; // Map by bankindex
    extern std::vector<sObject_Minimal> objects_sim; // Simulate object store
    extern std::vector<WaypointItem_Minimal> waypoints_sim;
    extern std::vector<WaypointCoord_Minimal> waypointcoords_sim;


    // --- Simulated Engine Functions ---
    void CoreSystem_SetRootPath(const std::string& rootPath);
    void CoreSystem_SetLevelPath(const std::string& levelPath);

    bool Level_Load(const std::string& levelFPMPath); // Simulates loading .fpm
                                                      // Populates entityelements_sim, entityprofiles_sim,
                                                      // waypoints_sim, waypointcoords_sim,
                                                      // and relevant fields in MinimalEngine::g and MinimalEngine::t
                                                      // Also simulates creation of objects in objects_sim

    // Object related functions
    bool ObjectExist(int objID);
    sObject_Minimal* GetObjectDataPtr(int objID); // Returns pointer to our minimal object
    float ObjectPositionX(int objID);
    float ObjectPositionY(int objID);
    float ObjectPositionZ(int objID);
    float ObjectAngleX(int objID);
    float ObjectAngleY(int objID);
    float ObjectAngleZ(int objID);
    float ObjectSizeX(int objID, bool bScaled = false); // bScaled might not be fully simulated here
    float ObjectSizeY(int objID, bool bScaled = false);
    float ObjectSizeZ(int objID, bool bScaled = false);
    float GetObjectCollisionCenterX(int objID); // Needs to use sObject_Minimal::collision
    float GetObjectCollisionCenterY(int objID);
    float GetObjectCollisionCenterZ(int objID);
    float GetObjectCollisionRadius(int objID); // Needs to use sObject_Minimal::collision.fColRadius
    void SimulateObjectCreation(int objID, float x, float y, float z, float angX, float angY, float angZ, float sizeX, float sizeY, float sizeZ, float colRad);


    // Terrain functions (simulated from BlitzTerrain.h or similar)
    // These would need to load or use pre-loaded terrain heightmap data.
    // For this stub, they might return fixed values or values derived from g.terrain_...
    void Terrain_Load(const std::string& terrainHeightMapPath); // Or get data from FPM
    float BT_GetTerrainSizeX(int terrainID);
    float BT_GetTerrainSizeZ(int terrainID);
    float BT_GetGroundHeight(int terrainID, float x, float z);
    void obs_fillterraindot(); // Placeholder
    void ode_doterraindotwork_loop(); // Placeholder for the loop calling ode_doterraindotwork

    // DarkAI SDK function stubs (if not directly linking the SDK)
    // These are just declarations; implementations would be in MinimalEngine.cpp or linked
    void darkai_setup_tree_logic();    // Simulates the logic within darkai_setup_tree
    void darkai_setup_entity_logic();  // Simulates the logic within darkai_setup_entity
    void darkai_addobstoallneededcontainers_logic ( int iType, int iObj, int iFullHeight, float fMinHeight, float fSliceHeight, float fSliceMinSize );


} // namespace MinimalEngine
