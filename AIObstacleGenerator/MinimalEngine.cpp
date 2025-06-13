#include "MinimalEngine.h"
#include <iostream> // For std::cout in stubs
#include <cmath>    // For std::fabs, std::sqrt

// Define the global g and t instances for the MinimalEngine namespace
namespace MinimalEngine {
    SGLOBALS_Minimal g;
    STEMPS_Minimal t;
    std::vector<sEntityElement_Minimal> entityelements_sim;
    std::map<int, sEntityProfile_Minimal> entityprofiles_sim;
    std::vector<sObject_Minimal> objects_sim;
    std::vector<WaypointItem_Minimal> waypoints_sim;
    std::vector<WaypointCoord_Minimal> waypointcoords_sim;

    // Simple heightmap for terrain simulation
    std::vector<std::vector<float>> terrain_heightmap_sim;
    float terrain_cellsize_sim = 50.0f; // Example cell size
    int terrain_width_sim = 0; // In cells
    int terrain_depth_sim = 0; // In cells
}

void MinimalEngine::CoreSystem_SetRootPath(const std::string& rootPath) {
    g.fpscrootdir_s = rootPath;
    std::cout << "MinimalEngine: Root path set to " << rootPath << std::endl;
}

void MinimalEngine::CoreSystem_SetLevelPath(const std::string& levelPath) {
    g.mysystem_levelBankTestMap_s = levelPath; // This should be relative like "levelbank\\testmap\\"
    std::cout << "MinimalEngine: Level bank path set to " << levelPath << std::endl;
}

bool MinimalEngine::Level_Load(const std::string& levelFPMPath) {
    std::cout << "MinimalEngine: Simulating FPM load for '" << levelFPMPath << "'" << std::endl;
    // This is a major simplification. A real FPM loader is very complex.
    // We'll just populate with some dummy data for demonstration.

    // Simulate some entities
    entityelements_sim.clear();
    entityprofiles_sim.clear();
    objects_sim.clear();

    // Make sure vectors are large enough (1-based indexing often used in GG)
    entityelements_sim.resize(10);
    // entityprofiles_sim does not need resize, it's a map

    // Entity 1: Static Box
    entityelements_sim[1].bankindex = 101; // Profile ID
    entityelements_sim[1].obj = 1;        // Object ID
    entityelements_sim[1].staticflag = 1;
    entityelements_sim[1].x = 100; entityelements_sim[1].y = 0; entityelements_sim[1].z = 100;
    entityprofiles_sim[101].ismarker = 0;
    entityprofiles_sim[101].collisionmode = 1; // Normal box
    entityprofiles_sim[101].forcesimpleobstacle = 1; // Force simple box
    SimulateObjectCreation(1, 100,0,100, 0,0,0, 50,50,50, 50);


    // Entity 2: Static Tree
    entityelements_sim[2].bankindex = 102;
    entityelements_sim[2].obj = 2;
    entityelements_sim[2].staticflag = 1;
    entityelements_sim[2].x = 200; entityelements_sim[2].y = 0; entityelements_sim[2].z = 150;
    entityprofiles_sim[102].ismarker = 0;
    entityprofiles_sim[102].collisionmode = 51; // Tree mode 1
    SimulateObjectCreation(2, 200,0,150, 0,0,0, 30,100,30, 30);

    // Entity 3: Another static box
    entityelements_sim[3].bankindex = 103;
    entityelements_sim[3].obj = 3;
    entityelements_sim[3].staticflag = 1;
    entityelements_sim[3].x = 50; entityelements_sim[3].y = 0; entityelements_sim[3].z = 200;
    entityprofiles_sim[103].ismarker = 0;
    entityprofiles_sim[103].collisionmode = 40; // Slice obstacle
    entityprofiles_sim[103].forcesimpleobstacle = 3;
    entityprofiles_sim[103].forceobstaclesliceheight = 25.0f;
    entityprofiles_sim[103].forceobstaclesliceminsize = 5.0f;
    SimulateObjectCreation(3, 50,0,200, 0,0,0, 80,80,80, 80);

    g.entityelementlist = 3; // We simulated 3 entities

    // Simulate some terrain
    g.terrain_TerrainID = 1; // Mark terrain as existing
    Terrain_Load("dummy_terrain.raw"); // Simulate loading heightmap

    // Simulate some waypoints for paths and zones
    waypoints_sim.resize(3); // 1-based index
    waypointcoords_sim.resize(10);

    // Path 1
    waypoints_sim[1].style = 1; // Path
    waypoints_sim[1].count = 3;
    waypoints_sim[1].start = 1;
    waypoints_sim[1].finish = 3;
    waypointcoords_sim[1] = {10,0,10};
    waypointcoords_sim[2] = {100,0,10};
    waypointcoords_sim[3] = {200,0,10};

    // Zone 1 (Container)
    waypoints_sim[2].style = 3; // Zone
    waypoints_sim[2].count = 4;
    waypoints_sim[2].start = 4;
    waypoints_sim[2].finish = 7;
    waypointcoords_sim[4] = {500,0,500};
    waypointcoords_sim[5] = {600,0,500};
    waypointcoords_sim[6] = {600,0,600};
    waypointcoords_sim[7] = {500,0,600};
    g.waypointmax = 2;

    std::cout << "MinimalEngine: Dummy level data populated." << std::endl;
    return true;
}

// --- Object Functions ---
bool MinimalEngine::ObjectExist(int objID) {
    if (objID > 0 && objID < objects_sim.size()) {
        return objects_sim[objID].bExists;
    }
    return false;
}

sObject_Minimal* MinimalEngine::GetObjectDataPtr(int objID) {
    if (objID > 0 && objID < objects_sim.size() && objects_sim[objID].bExists) {
        return &objects_sim[objID];
    }
    return nullptr;
}

void MinimalEngine::SimulateObjectCreation(int objID, float x, float y, float z, float angX, float angY, float angZ, float sizeX, float sizeY, float sizeZ, float colRad) {
    if (objID >= objects_sim.size()) {
        objects_sim.resize(objID + 1);
    }
    objects_sim[objID].iID = objID;
    objects_sim[objID].bExists = true;
    objects_sim[objID].X = x; objects_sim[objID].Y = y; objects_sim[objID].Z = z;
    objects_sim[objID].AngX = angX; objects_sim[objID].AngY = angY; objects_sim[objID].AngZ = angZ;
    objects_sim[objID].collision.fObjectSizeX = sizeX;
    objects_sim[objID].collision.fObjectSizeY = sizeY;
    objects_sim[objID].collision.fObjectSizeZ = sizeZ;
    // Assuming collision box is centered and half-sizes are half of overall size for simplicity
    objects_sim[objID].collision.fX = x;
    objects_sim[objID].collision.fY = y + sizeY / 2.0f; // Center Y might be at half height
    objects_sim[objID].collision.fZ = z;
    objects_sim[objID].collision.fRadiusX = sizeX / 2.0f;
    objects_sim[objID].collision.fRadiusY = sizeY / 2.0f;
    objects_sim[objID].collision.fRadiusZ = sizeZ / 2.0f;
    objects_sim[objID].collision.fColRadius = colRad;
}


float MinimalEngine::ObjectPositionX(int objID) { return GetObjectDataPtr(objID) ? GetObjectDataPtr(objID)->X : 0.0f; }
float MinimalEngine::ObjectPositionY(int objID) { return GetObjectDataPtr(objID) ? GetObjectDataPtr(objID)->Y : 0.0f; }
float MinimalEngine::ObjectPositionZ(int objID) { return GetObjectDataPtr(objID) ? GetObjectDataPtr(objID)->Z : 0.0f; }
float MinimalEngine::ObjectAngleY(int objID) { return GetObjectDataPtr(objID) ? GetObjectDataPtr(objID)->AngY : 0.0f; }
float MinimalEngine::ObjectSizeX(int objID, bool bScaled) { return GetObjectDataPtr(objID) ? GetObjectDataPtr(objID)->collision.fObjectSizeX : 0.0f; }
float MinimalEngine::ObjectSizeY(int objID, bool bScaled) { return GetObjectDataPtr(objID) ? GetObjectDataPtr(objID)->collision.fObjectSizeY : 0.0f; }
float MinimalEngine::ObjectSizeZ(int objID, bool bScaled) { return GetObjectDataPtr(objID) ? GetObjectDataPtr(objID)->collision.fObjectSizeZ : 0.0f; }
float MinimalEngine::GetObjectCollisionCenterX(int objID) { return GetObjectDataPtr(objID) ? GetObjectDataPtr(objID)->collision.fX - ObjectPositionX(objID) : 0.0f; } // Relative to obj pos
float MinimalEngine::GetObjectCollisionCenterY(int objID) { return GetObjectDataPtr(objID) ? GetObjectDataPtr(objID)->collision.fY - ObjectPositionY(objID) : 0.0f; }
float MinimalEngine::GetObjectCollisionCenterZ(int objID) { return GetObjectDataPtr(objID) ? GetObjectDataPtr(objID)->collision.fZ - ObjectPositionZ(objID) : 0.0f; }
float MinimalEngine::GetObjectCollisionRadius(int objID) { return GetObjectDataPtr(objID) ? GetObjectDataPtr(objID)->collision.fColRadius : 0.0f; }


// --- Terrain Functions ---
void MinimalEngine::Terrain_Load(const std::string& terrainHeightMapPath) {
    std::cout << "MinimalEngine: Simulating Terrain Load: " << terrainHeightMapPath << std::endl;
    // Simulate a 10x10 terrain grid (in cells)
    terrain_width_sim = 100;
    terrain_depth_sim = 100;
    terrain_heightmap_sim.resize(terrain_width_sim, std::vector<float>(terrain_depth_sim, 0.0f));
    // Example: create a simple ramp
    for (int x = 0; x < terrain_width_sim; ++x) {
        for (int z = 0; z < terrain_depth_sim; ++z) {
            terrain_heightmap_sim[x][z] = (x + z) * 0.5f; // Simple ramp
        }
    }
    MinimalEngine::g.terrain_TerrainID = 1; // Mark terrain as existing and loaded
}

float MinimalEngine::BT_GetTerrainSizeX(int terrainID) {
    if (terrainID == MinimalEngine::g.terrain_TerrainID) return terrain_width_sim * terrain_cellsize_sim;
    return 0;
}
float MinimalEngine::BT_GetTerrainSizeZ(int terrainID) {
    if (terrainID == MinimalEngine::g.terrain_TerrainID) return terrain_depth_sim * terrain_cellsize_sim;
    return 0;
}
float MinimalEngine::BT_GetGroundHeight(int terrainID, float x, float z) {
    if (terrainID != MinimalEngine::g.terrain_TerrainID) return 0.0f;
    int cellX = static_cast<int>(x / terrain_cellsize_sim);
    int cellZ = static_cast<int>(z / terrain_cellsize_sim);
    if (cellX >= 0 && cellX < terrain_width_sim && cellZ >= 0 && cellZ < terrain_depth_sim) {
        return terrain_heightmap_sim[cellX][cellZ];
    }
    return 0.0f; // Default height if out of bounds
}

void MinimalEngine::obs_fillterraindot() {
    std::cout << "MinimalEngine: Simulating obs_fillterraindot()" << std::endl;
    // Actual logic would analyze terrain_heightmap_sim
}

void MinimalEngine::ode_doterraindotwork_loop() {
    std::cout << "MinimalEngine: Simulating ode_doterraindotwork_loop()" << std::endl;
    // Actual logic would iterate and call a simulated ode_doterraindotwork()
    // For now, we assume it completes and populates some internal list that
    // AIStartNewObstacle etc. would use.
}

// --- DarkAI SDK Stubs/Wrappers ---
// These would call the actual DarkAI functions if linked, or provide minimal behavior.
// For now, they just print messages.

void MinimalEngine::darkai_setup_tree_logic() {
    std::cout << "MinimalEngine: darkai_setup_tree_logic for obj: " << t.tobj
              << ", entID: " << t.entid << ", treeMode: " << t.ttreemode << std::endl;
    // AIAddObstacleFromLevel(t.tobj, t.tcontainerid, 1, ObjectPositionY(t.tobj) + t.tworldy_f, 0, ...);
}

void MinimalEngine::darkai_setup_entity_logic() {
    std::cout << "MinimalEngine: darkai_setup_entity_logic for obj: " << t.tobj
              << ", entID: " << t.entid << std::endl;
    // ... logic to determine obstype ...
    // darkai_addobstoallneededcontainers_logic(obstype, t.tobj, t.tfullheight, ...);
}

void MinimalEngine::darkai_addobstoallneededcontainers_logic ( int iType, int iObjID, int iFullHeight, float fMinHeight, float fSliceHeight, float fSliceMinSize ) {
    std::cout << "MinimalEngine: darkai_addobstoallneededcontainers_logic for type " << iType << ", obj: " << iObjID << std::endl;
    // ... logic to iterate containers and call AIAddStaticObstacle or AIAddObstacleFromLevel ...
    // For global container 0:
    // if (iType == 3) AIAddObstacleFromLevel(iObjID, 0, iFullHeight, ObjectPositionY(iObjID)+fSliceHeight, fSliceMinSize, fMinHeight, 0 );
    // else AIAddStaticObstacle(iObjID, iFullHeight, 0);
}


void MinimalEngine::InitializeCoreSystems() {
    std::cout << "MinimalEngine: Core systems initialized (simulated)." << std::endl;
    // Setup default paths, load configs, etc.
    CoreSystem_SetRootPath(".\\"); // Default, should be set by user
    // Initialize any other necessary subsystems
}

void MinimalEngine::ShutdownCoreSystems() {
    std::cout << "MinimalEngine: Core systems shut down (simulated)." << std::endl;
}


} // namespace MinimalEngine
