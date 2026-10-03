//------------------------------------------------------------------------
//  LEVEL building - Duke Nukem 3D (Build engine) format
//------------------------------------------------------------------------
//
//  OBSIDIAN Level Maker
//
//  Copyright (C) 2021-2025 The OBSIDIAN Team
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//------------------------------------------------------------------------
//
//  The level is built with the normal DOOM CSG code, but instead of
//  writing WAD lumps the resulting vertices, linedefs, sidedefs,
//  sectors and things are captured and converted to Build engine
//  sectors, walls and sprites.  Each level is stored as a version 7
//  MAP file (version 8 when the vanilla limits are exceeded), and all
//  the levels are packed into a GRP file.
//
//  Coordinates are scaled by 16 horizontally (one DOOM unit is 16 Build
//  units) and by 256 vertically, the Y axis is flipped and Z grows
//  downwards, as the Build engine expects.
//
//  DOOM line and sector specials are translated to their nearest Duke
//  equivalents:
//
//    manual doors          ->  sector lotag 20 (ceiling door)
//    locked doors          ->  ACTIVATOR + ACCESSSWITCH with key palette
//    remote doors / floors ->  ACTIVATOR sprite + switch or TOUCHPLATE
//    lifts                 ->  sector lotag 17 (player operated)
//    exit switches         ->  wall lotag 65535 (ends the level)
//    walk-over exits       ->  sector lotag 65535
//    secret sectors        ->  sector lotag 32767
//
//------------------------------------------------------------------------

#include "g_duke.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <string>
#include <unordered_map>
#include <vector>

#include "g_doom.h"
#include "lib_util.h"
#include "m_cookie.h"
#include "m_lua.h"
#include "m_trans.h"
#include "main.h"
#include "sys_assert.h"
#include "sys_debug.h"
#include "sys_macro.h"

#ifndef OBSIDIAN_CONSOLE_ONLY
#include "ui_window.h"
#endif

extern void CSG_DOOM_Write();
#ifndef OBSIDIAN_CONSOLE_ONLY
extern std::string DLG_OutputFilename(const char *ext, const char *preset);
#endif

extern int ef_solid_type;
extern int ef_liquid_type;
extern int ef_thing_mode;

namespace Duke
{

// one DOOM unit is this many Build units (horizontally)
constexpr int XY_SCALE = 16;
// ...and this many Build Z units (vertically)
constexpr int Z_SCALE = 256;

// Build engine limits (vanilla Duke Nukem 3D v1.3D / v1.5)
constexpr int MAXSECTORS_V7 = 1024;
constexpr int MAXWALLS_V7   = 8192;
constexpr int MAXSPRITES_V7 = 4096;

// EDuke32 limits for map version 8
constexpr int MAXSECTORS_V8 = 4096;
constexpr int MAXWALLS_V8   = 16384;
constexpr int MAXSPRITES_V8 = 16384;

// the player's eye height above the floor (PHEIGHT in the Duke source)
constexpr int PLAYER_EYE_Z = 38 << 8;

// the big red button which ends a level (secret exit when it has a palette)
constexpr int NUKEBUTTON = 142;
constexpr int PAL_SECRET = 14;

// special lotag values
constexpr int LOTAG_END_LEVEL   = 65535;
constexpr int LOTAG_SECRET_ROOM = 32767;
constexpr int LOTAG_CEIL_DOOR   = 20;
constexpr int LOTAG_ELEV_DOWN   = 16;
constexpr int LOTAG_ELEV_UP     = 17;

// key palettes (blue keycards use the normal palette)
constexpr int PAL_BLUE_KEY   = 0;
constexpr int PAL_RED_KEY    = 21;
constexpr int PAL_YELLOW_KEY = 23;

/* ----- captured DOOM data ----- */

struct cap_vertex_t
{
    int x, y;
};

struct cap_sector_t
{
    int         f_h, c_h;
    std::string f_tex, c_tex;
    int         light, special, tag;
};

struct cap_side_t
{
    int         sector;
    std::string lower, mid, upper;
    int         x_offset, y_offset;
};

struct cap_line_t
{
    int v1, v2;
    int side1, side2;
    int special, flags, tag;
};

struct cap_thing_t
{
    int x, y, h;
    int type, angle, options;
};

class capture_c : public Doom::map_capture_c
{
  public:
    std::vector<cap_vertex_t> vertices;
    std::vector<cap_sector_t> sectors;
    std::vector<cap_side_t>   sides;
    std::vector<cap_line_t>   lines;
    std::vector<cap_thing_t>  things;

  public:
    void Clear()
    {
        vertices.clear();
        sectors.clear();
        sides.clear();
        lines.clear();
        things.clear();
    }

    void Vertex(int x, int y) override
    {
        vertices.push_back({x, y});
    }

    void Sector(int f_h, const std::string &f_tex, int c_h, const std::string &c_tex, int light, int special,
                int tag) override
    {
        sectors.push_back({f_h, c_h, f_tex, c_tex, light, special, tag});
    }

    void Sidedef(int sector, const std::string &l_tex, const std::string &m_tex, const std::string &u_tex, int x_offset,
                 int y_offset) override
    {
        sides.push_back({sector, l_tex, m_tex, u_tex, x_offset, y_offset});
    }

    void Linedef(int vert1, int vert2, int side1, int side2, int type, int flags, int tag) override
    {
        lines.push_back({vert1, vert2, side1, side2, type, flags, tag});
    }

    void Thing(int x, int y, int h, int type, int angle, int options) override
    {
        things.push_back({x, y, h, type, angle, options});
    }

    int NumVertexes() override
    {
        return (int)vertices.size();
    }
    int NumSectors() override
    {
        return (int)sectors.size();
    }
    int NumSidedefs() override
    {
        return (int)sides.size();
    }
    int NumLinedefs() override
    {
        return (int)lines.size();
    }
    int NumThings() override
    {
        return (int)things.size();
    }
};

/* ----- Build engine structures ----- */

struct build_sector_t
{
    int wallptr = 0, wallnum = 0;
    int ceilingz = 0, floorz = 0;
    int ceilingstat = 0, floorstat = 0;
    int ceilingpicnum = 0, ceilingheinum = 0;
    int ceilingshade = 0, ceilingpal = 0, ceilingxpanning = 0, ceilingypanning = 0;
    int floorpicnum = 0, floorheinum = 0;
    int floorshade = 0, floorpal = 0, floorxpanning = 0, floorypanning = 0;
    int visibility = 0;
    int lotag = 0, hitag = 0, extra = 0;

    // index into the captured DOOM sectors
    int doom_sec = -1;
};

struct build_wall_t
{
    int x = 0, y = 0;
    int point2 = -1, nextwall = -1, nextsector = -1;
    int cstat  = 0;
    int picnum = 0, overpicnum = 0;
    int shade = 0, pal = 0;
    int xrepeat = 8, yrepeat = 8;
    int xpanning = 0, ypanning = 0;
    int lotag = 0, hitag = 0, extra = -1;
};

struct build_sprite_t
{
    int x = 0, y = 0, z = 0;
    int cstat = 0, picnum = 0;
    int shade = 0, pal = 0, clipdist = 32;
    int xrepeat = 32, yrepeat = 32;
    int xoffset = 0, yoffset = 0;
    int sectnum = 0, statnum = 0;
    int ang = 0, owner = -1;
    int xvel = 0, yvel = 0, zvel = 0;
    int lotag = 0, hitag = 0, extra = -1;
};

// a directed edge of a DOOM sector boundary (one side of a linedef)
struct edge_t
{
    int  v1, v2; // canonical vertex numbers
    int  line;
    int  side;   // 0 = front sidedef, 1 = back sidedef
    bool used;
};

// a closed loop of edges
struct loop_t
{
    std::vector<int> edges;
    double           area; // signed area in DOOM coordinates
};

/* ----- state ----- */

static capture_c *cap;

static std::string level_name;

static int sky_tile;

// level number of the secret level (0 = none), and whether this is the
// episode's boss level (killing the boss ends the episode)
static int  secret_target;
static bool boss_level;

// finished MAP files (name + data), written into the GRP at the end
static std::vector<std::pair<std::string, std::vector<uint8_t>>> map_files;

// per-level conversion state
static std::vector<int>          sector_rep;  // merged DOOM sector for each sector
static std::vector<cap_vertex_t> canon_verts; // canonical vertices (DOOM coords)
static std::vector<int>          vert_canon;  // captured vertex -> canonical
static std::vector<edge_t>       edges;

static std::vector<build_sector_t> b_sectors;
static std::vector<build_wall_t>   b_walls;
static std::vector<build_sprite_t> b_sprites;

// for each linedef: wall index of the front [0] and back [1] side
static std::vector<std::pair<int, int>> line_walls;

// for each DOOM sector : list of Build sectors made from it
static std::vector<std::vector<int>> doom_to_build;

static int center_x, center_y;

static int next_channel;

static bool have_start;
static int  start_x, start_y, start_z, start_ang, start_sec;

/* ----- helpers ----- */

static inline int ToBuildX(int x)
{
    return (x - center_x) * XY_SCALE;
}

static inline int ToBuildY(int y)
{
    return (center_y - y) * XY_SCALE;
}

static inline int ToBuildZ(int h)
{
    return -h * Z_SCALE;
}

static int ParseTile(const std::string &tex, bool *is_sky = nullptr)
{
    if (is_sky)
    {
        *is_sky = false;
    }

    if (tex.empty() || tex[0] == '-')
    {
        return -1;
    }

    // invisible blocking lines (used to stop the player falling off ledges)
    if (StringCaseCompare(tex, "O_INVIST") == 0)
    {
        return -1;
    }

    if (StringPrefixCaseCompare(tex, "SKY") == 0 || StringCaseCompare(tex, "F_SKY1") == 0)
    {
        if (is_sky)
        {
            *is_sky = true;
        }
        return sky_tile;
    }

    if (isdigit(tex[0]))
    {
        return StringToInt(tex);
    }

    static std::vector<std::string> warned;

    if (std::find(warned.begin(), warned.end(), tex) == warned.end())
    {
        LogPrint("WARNING: Duke: texture '%s' is not a tile number\n", tex.c_str());
        warned.push_back(tex);
    }

    return 0;
}

static int LightToShade(int light)
{
    // DOOM light levels go 0 (black) to 255 (full bright), Duke shades
    // go -128 (very bright) through 0 (normal) to 31 (almost black).
    // (tuned to give a similar spread of shades as the original levels)
    int shade = (224 - light) / 6;

    return std::clamp(shade, -4, 26);
}

static bool IsSwitchTile(int pic)
{
    switch (pic)
    {
    case DIPSWITCH:
    case DIPSWITCH + 1:
    case DIPSWITCH2:
    case DIPSWITCH2 + 1:
    case TECHSWITCH:
    case TECHSWITCH + 1:
    case DIPSWITCH3:
    case DIPSWITCH3 + 1:
    case ACCESSSWITCH:
    case ACCESSSWITCH2:
    case HANDSWITCH:
    case HANDSWITCH + 1:
    case PULLSWITCH:
    case PULLSWITCH + 1:
    case ALIENSWITCH:
    case ALIENSWITCH + 1:
        return true;

    default:
        return false;
    }
}

static bool IsMonsterTile(int pic)
{
    switch (pic)
    {
    case 1550: // SHARK
    case 1680: // LIZTROOP
    case 1681:
    case 1682:
    case 1744:
    case 1820: // OCTABRAIN
    case 1821:
    case 1880: // DRONE
    case 1920: // COMMANDER
    case 1921:
    case 1960: // RECON
    case 1975: // TANK
    case 2000: // PIGCOP
    case 2001:
    case 2045:
    case 2120: // LIZMAN
    case 2121:
    case 2360: // ROTATEGUN
    case 2370: // GREENSLIME
    case 2630: // BOSS1
    case 2631:
    case 2710: // BOSS2
    case 2760: // BOSS3
    case 4610: // NEWBEAST
    case 4611:
    case 4740: // BOSS4
        return true;

    default:
        return false;
    }
}

static bool IsItemTile(int pic)
{
    return (pic >= 21 && pic <= 61 && pic != ACCESSCARD) || pic == 100 /* ATOMICHEALTH */ || pic == 1348 /* HOLODUKE */;
}

static bool IsSolidDecor(int pic)
{
    switch (pic)
    {
    case 908:  // TREE1
    case 910:  // TREE2
    case 911:  // CACTUS
    case 916:  // FIREEXT
    case 981:  // HYDRENT
    case 1227: // NUKEBARREL
    case 1238: // EXPLODINGBARREL
    case 1239: // EXPLODINGBARREL2
    case 1240: // FIREBARREL
    case 1247: // SEENINE
    case 1079: // OOZFILTER
    case 977:  // GENERICPOLE
        return true;

    default:
        return false;
    }
}

/* ----- geometry ----- */

static void CanonicalizeVertices()
{
    std::unordered_map<int64_t, int> coord_map;

    canon_verts.clear();
    vert_canon.clear();

    for (const auto &V : cap->vertices)
    {
        int64_t key = ((int64_t)V.x << 32) ^ (int64_t)(uint32_t)V.y;

        auto it = coord_map.find(key);

        if (it != coord_map.end())
        {
            vert_canon.push_back(it->second);
        }
        else
        {
            int idx = (int)canon_verts.size();
            canon_verts.push_back(V);
            coord_map[key] = idx;
            vert_canon.push_back(idx);
        }
    }

    // center the map on the origin, Build editors are happiest that way
    if (!canon_verts.empty())
    {
        int min_x = canon_verts[0].x, max_x = min_x;
        int min_y = canon_verts[0].y, max_y = min_y;

        for (const auto &V : canon_verts)
        {
            min_x = std::min(min_x, V.x);
            max_x = std::max(max_x, V.x);
            min_y = std::min(min_y, V.y);
            max_y = std::max(max_y, V.y);
        }

        center_x = (min_x + max_x) / 2;
        center_y = (min_y + max_y) / 2;

        if ((max_x - min_x) * XY_SCALE / 2 > 131072 || (max_y - min_y) * XY_SCALE / 2 > 131072)
        {
            LogPrint("WARNING: Duke: map is larger than the Build editor boundaries\n");
        }
    }
}

static int LineSector(const cap_line_t &L, int side)
{
    int sd = (side == 0) ? L.side1 : L.side2;

    if (sd < 0 || sd >= (int)cap->sides.size())
    {
        return -1;
    }

    int sec = cap->sides[sd].sector;

    if (sec >= 0 && sec < (int)sector_rep.size())
    {
        sec = sector_rep[sec];
    }

    return sec;
}

static int FindRep(int sec)
{
    while (sector_rep[sec] != sec)
    {
        sector_rep[sec] = sector_rep[sector_rep[sec]];
        sec             = sector_rep[sec];
    }
    return sec;
}

// The DOOM CSG code splits rooms into many sectors which only differ in
// their light level.  Build levels have much lower limits on sectors and
// walls, so neighboring sectors which only differ in lighting are merged
// (keeping the brightest light level).
static void MergeLightSectors()
{
    int num_sec = (int)cap->sectors.size();

    sector_rep.resize(num_sec);

    for (int i = 0; i < num_sec; i++)
    {
        sector_rep[i] = i;
    }

    for (const auto &L : cap->lines)
    {
        if (L.side1 < 0 || L.side2 < 0 || L.special != 0 || (L.flags & 1))
        {
            continue;
        }

        const cap_side_t &F = cap->sides[L.side1];
        const cap_side_t &B = cap->sides[L.side2];

        // keep railings and other middle textures
        if (ParseTile(F.mid) >= 0 || ParseTile(B.mid) >= 0)
        {
            continue;
        }

        int s1 = FindRep(F.sector);
        int s2 = FindRep(B.sector);

        if (s1 == s2)
        {
            continue;
        }

        const cap_sector_t &A = cap->sectors[s1];
        const cap_sector_t &C = cap->sectors[s2];

        if (A.f_h != C.f_h || A.c_h != C.c_h || A.special != C.special || A.tag != C.tag || A.f_tex != C.f_tex ||
            A.c_tex != C.c_tex)
        {
            continue;
        }

        int light = std::max(A.light, C.light);

        sector_rep[s2]         = s1;
        cap->sectors[s1].light = light;
    }

    int merged = 0;

    for (int i = 0; i < num_sec; i++)
    {
        sector_rep[i] = FindRep(i);

        if (sector_rep[i] != i)
        {
            merged++;
        }
    }

    LogPrint("Duke: merged %d sectors which only differed in lighting\n", merged);
}

static const cap_side_t *LineSide(const cap_line_t &L, int side)
{
    int sd = (side == 0) ? L.side1 : L.side2;

    if (sd < 0 || sd >= (int)cap->sides.size())
    {
        return nullptr;
    }

    return &cap->sides[sd];
}

static double EdgeAngle(const edge_t &E)
{
    const cap_vertex_t &A = canon_verts[E.v1];
    const cap_vertex_t &B = canon_verts[E.v2];

    return atan2((double)(B.y - A.y), (double)(B.x - A.x));
}

static double LoopArea(const loop_t &loop)
{
    double area = 0;

    for (int e : loop.edges)
    {
        const cap_vertex_t &A = canon_verts[edges[e].v1];
        const cap_vertex_t &B = canon_verts[edges[e].v2];

        area += (double)A.x * (double)B.y - (double)B.x * (double)A.y;
    }

    return area / 2.0;
}

// point-in-polygon test (DOOM coordinates) using the crossing rule
static bool PointInLoop(double px, double py, const loop_t &loop)
{
    bool inside = false;

    for (int e : loop.edges)
    {
        const cap_vertex_t &A = canon_verts[edges[e].v1];
        const cap_vertex_t &B = canon_verts[edges[e].v2];

        if ((A.y > py) != (B.y > py))
        {
            double ix = A.x + (py - A.y) * (double)(B.x - A.x) / (double)(B.y - A.y);

            if (px < ix)
            {
                inside = !inside;
            }
        }
    }

    return inside;
}

static void CollectEdges()
{
    edges.clear();

    for (int i = 0; i < (int)cap->lines.size(); i++)
    {
        const cap_line_t &L = cap->lines[i];

        int v1 = vert_canon[L.v1];
        int v2 = vert_canon[L.v2];

        if (v1 == v2)
        {
            continue;
        }

        int front = LineSector(L, 0);
        int back  = LineSector(L, 1);

        // lines with the same sector on both sides are not needed
        if (front == back)
        {
            continue;
        }

        if (front >= 0)
        {
            edges.push_back({v1, v2, i, 0, false});
        }
        if (back >= 0)
        {
            edges.push_back({v2, v1, i, 1, false});
        }
    }
}

// trace all the boundary loops of every DOOM sector
static void TraceLoops(std::vector<std::vector<loop_t>> &sector_loops)
{
    int num_sec = (int)cap->sectors.size();

    sector_loops.assign(num_sec, {});

    // group the edges by sector
    std::vector<std::vector<int>> sec_edges(num_sec);

    for (int i = 0; i < (int)edges.size(); i++)
    {
        const cap_line_t &L = cap->lines[edges[i].line];

        int sec = LineSector(L, edges[i].side);

        if (sec >= 0 && sec < num_sec)
        {
            sec_edges[sec].push_back(i);
        }
    }

    int broken     = 0;
    int degenerate = 0;

    for (int sec = 0; sec < num_sec; sec++)
    {
        std::unordered_multimap<int, int> outgoing;

        for (int e : sec_edges[sec])
        {
            outgoing.insert({edges[e].v1, e});
        }

        for (int first : sec_edges[sec])
        {
            if (edges[first].used)
            {
                continue;
            }

            loop_t loop;
            loop.edges.push_back(first);
            edges[first].used = true;

            int  cur    = first;
            bool closed = false;

            for (int guard = 0; guard < 100000; guard++)
            {
                // the interior lies on the right side of every edge.
                // at a vertex shared by several edges of this sector, the
                // next edge is the first one counter-clockwise from the
                // reverse of the incoming direction.
                double back_ang = EdgeAngle(edges[cur]) + M_PI;

                // candidates which can continue this loop : the first
                // edge (closing the loop) or edges not used yet.  Used
                // edges are only considered when nothing else is left.
                int    best       = -1;
                double best_delta = 1e9;
                int    used_best  = -1;
                double used_delta = 1e9;

                auto range = outgoing.equal_range(edges[cur].v2);

                for (auto it = range.first; it != range.second; ++it)
                {
                    int cand = it->second;

                    // never go straight back along the same linedef
                    if (edges[cand].line == edges[cur].line)
                    {
                        continue;
                    }

                    double delta = EdgeAngle(edges[cand]) - back_ang;

                    while (delta <= 1e-9)
                    {
                        delta += 2 * M_PI;
                    }
                    while (delta > 2 * M_PI + 1e-9)
                    {
                        delta -= 2 * M_PI;
                    }

                    if (cand == first || !edges[cand].used)
                    {
                        if (delta < best_delta)
                        {
                            best_delta = delta;
                            best       = cand;
                        }
                    }
                    else if (delta < used_delta)
                    {
                        used_delta = delta;
                        used_best  = cand;
                    }
                }

                if (best < 0)
                {
                    best = used_best;
                }

                if (best < 0)
                {
                    break;
                }

                if (best == first)
                {
                    closed = true;
                    break;
                }

                if (edges[best].used)
                {
                    // the trace started on a dangling spike : the loop
                    // proper begins where we have come back to.
                    auto pos = std::find(loop.edges.begin(), loop.edges.end(), best);

                    if (pos != loop.edges.end())
                    {
                        loop.edges.erase(loop.edges.begin(), pos);
                        closed = true;
                    }
                    break;
                }

                edges[best].used = true;
                loop.edges.push_back(best);
                cur = best;
            }

            if (!closed)
            {
                // a dangling spike of zero width (two lines between the
                // same vertices hanging off a real boundary) : ignore it
                // area of the chain, closed back to its start
                const cap_vertex_t &C1 = canon_verts[edges[loop.edges.back()].v2];
                const cap_vertex_t &C2 = canon_verts[edges[loop.edges.front()].v1];

                double chain_area = LoopArea(loop) + ((double)C1.x * C2.y - (double)C2.x * C1.y) / 2.0;

                if (fabs(chain_area) < 0.5)
                {
                    degenerate++;
                    continue;
                }

                broken++;

                const cap_vertex_t &V = canon_verts[edges[first].v1];
                LogPrint("WARNING: Duke: open loop in sector %d near (%d %d)\n", sec, V.x, V.y);

                for (int e : sec_edges[sec])
                {
                    const cap_vertex_t &A = canon_verts[edges[e].v1];
                    const cap_vertex_t &B = canon_verts[edges[e].v2];

                    if (abs(A.x - V.x) < 40 && abs(A.y - V.y) < 40)
                    {
                        DebugPrint("   edge %d: (%d %d) -> (%d %d) line %d side %d inloop %d\n", e, A.x, A.y, B.x, B.y,
                                   edges[e].line, edges[e].side,
                                   std::find(loop.edges.begin(), loop.edges.end(), e) != loop.edges.end() ? 1 : 0);
                    }
                }
                continue;
            }

            // slivers with no area (e.g. two lines between the same two
            // vertices) are simply dropped.
            loop.area = LoopArea(loop);

            if (loop.edges.size() < 3 || fabs(loop.area) < 0.5)
            {
                degenerate++;
                continue;
            }

            sector_loops[sec].push_back(loop);
        }
    }

    if (broken > 0)
    {
        LogPrint("WARNING: Duke: %d sector boundary loops could not be closed\n", broken);
    }

    if (degenerate > 0)
    {
        LogPrint("Duke: dropped %d sector slivers without any area\n", degenerate);
    }
}

// split each DOOM sector into connected pieces (an outer loop with its
// holes) and create the Build sectors and walls.
static void CreateSectorsAndWalls()
{
    std::vector<std::vector<loop_t>> sector_loops;

    TraceLoops(sector_loops);

    b_sectors.clear();
    b_walls.clear();

    line_walls.assign(cap->lines.size(), {-1, -1});
    doom_to_build.assign(cap->sectors.size(), {});

    int orphan_holes = 0;

    for (int sec = 0; sec < (int)cap->sectors.size(); sec++)
    {
        std::vector<loop_t> &loops = sector_loops[sec];

        if (loops.empty())
        {
            continue;
        }

        // with the interior on the right, outer loops are clockwise
        // (negative area) and holes are anti-clockwise.
        std::vector<int> outers;
        std::vector<int> holes;

        for (int k = 0; k < (int)loops.size(); k++)
        {
            if (loops[k].area < 0)
            {
                outers.push_back(k);
            }
            else
            {
                holes.push_back(k);
            }
        }

        // largest outer loop first
        std::sort(outers.begin(), outers.end(),
                  [&](int a, int b) { return fabs(loops[a].area) > fabs(loops[b].area); });

        std::vector<std::vector<int>> groups(outers.size());

        for (size_t k = 0; k < outers.size(); k++)
        {
            groups[k].push_back(outers[k]);
        }

        for (int h : holes)
        {
            // find smallest outer loop which contains this hole
            int    best      = -1;
            double best_area = 1e30;

            for (size_t k = 0; k < outers.size(); k++)
            {
                const loop_t &O = loops[outers[k]];

                if (fabs(O.area) <= fabs(loops[h].area))
                {
                    continue;
                }

                // test the midpoints of the hole's edges, a vertex may be
                // shared with the outer loop.
                int inside_count = 0;
                int tests        = 0;

                for (int e : loops[h].edges)
                {
                    const cap_vertex_t &A = canon_verts[edges[e].v1];
                    const cap_vertex_t &B = canon_verts[edges[e].v2];

                    if (PointInLoop((A.x + B.x) / 2.0 + 0.01, (A.y + B.y) / 2.0 + 0.013, O))
                    {
                        inside_count++;
                    }

                    if (++tests >= 8)
                    {
                        break;
                    }
                }

                if (inside_count * 2 > tests && fabs(O.area) < best_area)
                {
                    best      = (int)k;
                    best_area = fabs(O.area);
                }
            }

            if (best < 0)
            {
                orphan_holes++;
                continue;
            }

            groups[best].push_back(h);
        }

        const cap_sector_t &DS = cap->sectors[sec];

        for (auto &group : groups)
        {
            build_sector_t BS;

            BS.doom_sec = sec;
            BS.wallptr  = (int)b_walls.size();

            for (int k : group)
            {
                const loop_t &loop = loops[k];

                int loop_start = (int)b_walls.size();

                for (size_t n = 0; n < loop.edges.size(); n++)
                {
                    const edge_t &E = edges[loop.edges[n]];

                    build_wall_t W;

                    W.x      = ToBuildX(canon_verts[E.v1].x);
                    W.y      = ToBuildY(canon_verts[E.v1].y);
                    W.point2 = (n + 1 < loop.edges.size()) ? (int)b_walls.size() + 1 : loop_start;

                    if (E.side == 0)
                    {
                        line_walls[E.line].first = (int)b_walls.size();
                    }
                    else
                    {
                        line_walls[E.line].second = (int)b_walls.size();
                    }

                    b_walls.push_back(W);
                }
            }

            BS.wallnum = (int)b_walls.size() - BS.wallptr;

            // heights and textures
            BS.floorz   = ToBuildZ(DS.f_h);
            BS.ceilingz = ToBuildZ(DS.c_h);

            bool f_sky = false;
            bool c_sky = false;

            BS.floorpicnum   = std::max(0, ParseTile(DS.f_tex, &f_sky));
            BS.ceilingpicnum = std::max(0, ParseTile(DS.c_tex, &c_sky));

            if (f_sky)
            {
                BS.floorstat |= 1;
            }
            if (c_sky)
            {
                BS.ceilingstat |= 1;
            }

            BS.floorshade = BS.ceilingshade = LightToShade(DS.light);

            // secret areas
            if (DS.special == 9)
            {
                BS.lotag = LOTAG_SECRET_ROOM;
            }

            doom_to_build[sec].push_back((int)b_sectors.size());
            b_sectors.push_back(BS);
        }
    }

    if (orphan_holes > 0)
    {
        LogPrint("WARNING: Duke: %d holes without an enclosing loop\n", orphan_holes);
    }
}

static int WallSector(int w)
{
    // walls are stored contiguously, so a binary search would work,
    // but the sector lookup table is cheap to build.
    static std::vector<int> table;

    if (w < 0)
    {
        table.clear();
        return -1;
    }

    if (table.size() != b_walls.size())
    {
        table.assign(b_walls.size(), -1);

        for (int s = 0; s < (int)b_sectors.size(); s++)
        {
            for (int k = 0; k < b_sectors[s].wallnum; k++)
            {
                table[b_sectors[s].wallptr + k] = s;
            }
        }
    }

    return table[w];
}

static void SetupWalls()
{
    WallSector(-1);

    for (int i = 0; i < (int)cap->lines.size(); i++)
    {
        const cap_line_t &L = cap->lines[i];

        int wf = line_walls[i].first;
        int wb = line_walls[i].second;

        if (wf >= 0 && wb >= 0)
        {
            b_walls[wf].nextwall   = wb;
            b_walls[wf].nextsector = WallSector(wb);
            b_walls[wb].nextwall   = wf;
            b_walls[wb].nextsector = WallSector(wf);
        }

        double len =
            ComputeDist(cap->vertices[L.v1].x, cap->vertices[L.v1].y, cap->vertices[L.v2].x, cap->vertices[L.v2].y);

        for (int side = 0; side < 2; side++)
        {
            int w = (side == 0) ? wf : wb;

            if (w < 0)
            {
                continue;
            }

            build_wall_t     &W  = b_walls[w];
            const cap_side_t *SD = LineSide(L, side);

            if (!SD)
            {
                continue;
            }

            const cap_sector_t &PS = cap->sectors[sector_rep[SD->sector]];

            int upper = ParseTile(SD->upper);
            int mid   = ParseTile(SD->mid);
            int lower = ParseTile(SD->lower);

            int pic = -1;

            if (W.nextwall < 0)
            {
                pic = mid;

                // lower unpegged means aligned with the floor
                if (L.flags & 16)
                {
                    W.cstat |= 4;
                }
            }
            else
            {
                const cap_side_t   *OD = LineSide(L, 1 - side);
                const cap_sector_t &OS = cap->sectors[sector_rep[OD->sector]];

                int upper_h = PS.c_h - OS.c_h;
                int lower_h = OS.f_h - PS.f_h;

                // nothing visible above when both ceilings are sky
                bool p_sky = false, o_sky = false;
                ParseTile(PS.c_tex, &p_sky);
                ParseTile(OS.c_tex, &o_sky);
                if (p_sky && o_sky)
                {
                    upper_h = 0;
                }

                if (upper_h > 0 && upper >= 0 && (lower_h <= 0 || lower < 0 || upper_h >= lower_h))
                {
                    pic = upper;
                }
                else if (lower_h > 0 && lower >= 0)
                {
                    pic = lower;
                }
                else if (upper >= 0)
                {
                    pic = upper;
                }
                else if (lower >= 0)
                {
                    pic = lower;
                }

                // railings, grates, fences...
                if (mid >= 0)
                {
                    W.overpicnum = mid;
                    W.cstat |= 16;
                }

                if (L.flags & 1)
                {
                    W.cstat |= 1;
                }
            }

            W.picnum = std::max(0, pic);

            int length_units = OBSIDIAN_I_ROUND(len / 8.0);
            W.xrepeat        = std::clamp(length_units, 1, 255);
            W.yrepeat        = 8;

            W.xpanning = SD->x_offset & 255;

            W.shade = LightToShade(PS.light);
        }
    }
}

/* ----- point location ----- */

// the Build engine's inside() function
static bool InsideSector(int x, int y, int sec)
{
    const build_sector_t &S = b_sectors[sec];

    unsigned int cnt = 0;

    for (int k = 0; k < S.wallnum; k++)
    {
        const build_wall_t &W1 = b_walls[S.wallptr + k];
        const build_wall_t &W2 = b_walls[W1.point2];

        int64_t y1 = W1.y - y;
        int64_t y2 = W2.y - y;

        if ((y1 ^ y2) < 0)
        {
            int64_t x1 = W1.x - x;
            int64_t x2 = W2.x - x;

            if ((x1 ^ x2) >= 0)
            {
                cnt ^= (x1 < 0) ? 1 : 0;
            }
            else
            {
                cnt ^= (((x1 * y2 - x2 * y1) ^ y2) < 0) ? 1 : 0;
            }
        }
    }

    return (cnt & 1) != 0;
}

static int FindSector(int x, int y)
{
    static const int offsets[][2] = {{0, 0}, {3, 3}, {-3, 3}, {3, -3}, {-3, -3}, {16, 0}, {-16, 0}, {0, 16}, {0, -16}};

    for (const auto &off : offsets)
    {
        for (int s = 0; s < (int)b_sectors.size(); s++)
        {
            if (InsideSector(x + off[0], y + off[1], s))
            {
                return s;
            }
        }
    }

    return -1;
}

// get a Build position which is inside the given Build sector
static bool SectorInteriorPoint(int sec, int *bx, int *by)
{
    const build_sector_t &S = b_sectors[sec];

    for (int dist : {48, 16, 128, 4})
    {
        for (int k = 0; k < S.wallnum; k++)
        {
            const build_wall_t &W1 = b_walls[S.wallptr + k];
            const build_wall_t &W2 = b_walls[W1.point2];

            double dx  = W2.x - W1.x;
            double dy  = W2.y - W1.y;
            double len = sqrt(dx * dx + dy * dy);

            if (len < 4)
            {
                continue;
            }

            // in Build coordinates the interior is on the right side,
            // which (with Y pointing down) is the (-dy, dx) direction.
            int px = OBSIDIAN_I_ROUND((W1.x + W2.x) / 2.0 - dy / len * dist);
            int py = OBSIDIAN_I_ROUND((W1.y + W2.y) / 2.0 + dx / len * dist);

            if (InsideSector(px, py, sec))
            {
                *bx = px;
                *by = py;
                return true;
            }
        }
    }

    return false;
}

static build_sprite_t &AddMarker(int sec, int picnum, int lotag, int hitag)
{
    build_sprite_t SP;

    SP.picnum  = picnum;
    SP.sectnum = sec;
    SP.lotag   = lotag;
    SP.hitag   = hitag;
    SP.xrepeat = SP.yrepeat = 64;
    SP.z                    = b_sectors[sec].floorz;

    if (!SectorInteriorPoint(sec, &SP.x, &SP.y))
    {
        SP.x = b_walls[b_sectors[sec].wallptr].x;
        SP.y = b_walls[b_sectors[sec].wallptr].y;
    }

    b_sprites.push_back(SP);

    return b_sprites.back();
}

/* ----- specials ----- */

enum trigger_e
{
    TRIG_Manual = 0,
    TRIG_Switch,
    TRIG_Walk,
    TRIG_Gun,
};

enum action_e
{
    ACT_None = 0,
    ACT_Door,
    ACT_Lower,
    ACT_Raise,
    ACT_Lift,
    ACT_Stairs,
    ACT_Exit,
};

enum key_e
{
    KEY_None = 0,
    KEY_Blue,
    KEY_Yellow,
    KEY_Red,
};

struct special_info_t
{
    int  special;
    int  trigger;
    int  action;
    int  key;
    bool once;
};

// DOOM linedef specials which have a Duke equivalent
static const special_info_t special_table[] = {
    // manual doors
    {1, TRIG_Manual, ACT_Door, KEY_None, false},
    {26, TRIG_Manual, ACT_Door, KEY_Blue, false},
    {27, TRIG_Manual, ACT_Door, KEY_Yellow, false},
    {28, TRIG_Manual, ACT_Door, KEY_Red, false},
    {31, TRIG_Manual, ACT_Door, KEY_None, true},
    {32, TRIG_Manual, ACT_Door, KEY_Blue, true},
    {33, TRIG_Manual, ACT_Door, KEY_Red, true},
    {34, TRIG_Manual, ACT_Door, KEY_Yellow, true},
    {117, TRIG_Manual, ACT_Door, KEY_None, false},
    {118, TRIG_Manual, ACT_Door, KEY_None, true},

    // switches
    {7, TRIG_Switch, ACT_Stairs, KEY_None, true},
    {11, TRIG_Switch, ACT_Exit, KEY_None, true},
    {14, TRIG_Switch, ACT_Raise, KEY_None, true},
    {15, TRIG_Switch, ACT_Raise, KEY_None, true},
    {18, TRIG_Switch, ACT_Raise, KEY_None, true},
    {20, TRIG_Switch, ACT_Raise, KEY_None, true},
    {21, TRIG_Switch, ACT_Lift, KEY_None, true},
    {23, TRIG_Switch, ACT_Lower, KEY_None, true},
    {29, TRIG_Switch, ACT_Door, KEY_None, true},
    {45, TRIG_Switch, ACT_Lower, KEY_None, false},
    {51, TRIG_Switch, ACT_Exit, KEY_None, true},
    {55, TRIG_Switch, ACT_Raise, KEY_None, true},
    {60, TRIG_Switch, ACT_Lower, KEY_None, false},
    {61, TRIG_Switch, ACT_Door, KEY_None, false},
    {62, TRIG_Switch, ACT_Lift, KEY_None, false},
    {63, TRIG_Switch, ACT_Door, KEY_None, false},
    {64, TRIG_Switch, ACT_Raise, KEY_None, false},
    {65, TRIG_Switch, ACT_Raise, KEY_None, false},
    {66, TRIG_Switch, ACT_Raise, KEY_None, false},
    {67, TRIG_Switch, ACT_Raise, KEY_None, false},
    {68, TRIG_Switch, ACT_Raise, KEY_None, false},
    {69, TRIG_Switch, ACT_Raise, KEY_None, false},
    {70, TRIG_Switch, ACT_Lower, KEY_None, false},
    {71, TRIG_Switch, ACT_Lower, KEY_None, true},
    {99, TRIG_Switch, ACT_Door, KEY_Blue, false},
    {101, TRIG_Switch, ACT_Raise, KEY_None, true},
    {102, TRIG_Switch, ACT_Lower, KEY_None, true},
    {103, TRIG_Switch, ACT_Door, KEY_None, true},
    {111, TRIG_Switch, ACT_Door, KEY_None, true},
    {112, TRIG_Switch, ACT_Door, KEY_None, true},
    {114, TRIG_Switch, ACT_Door, KEY_None, false},
    {115, TRIG_Switch, ACT_Door, KEY_None, false},
    {122, TRIG_Switch, ACT_Lift, KEY_None, true},
    {123, TRIG_Switch, ACT_Lift, KEY_None, false},
    {127, TRIG_Switch, ACT_Stairs, KEY_None, true},
    {131, TRIG_Switch, ACT_Raise, KEY_None, true},
    {132, TRIG_Switch, ACT_Raise, KEY_None, false},
    {133, TRIG_Switch, ACT_Door, KEY_Blue, true},
    {134, TRIG_Switch, ACT_Door, KEY_Red, false},
    {135, TRIG_Switch, ACT_Door, KEY_Red, true},
    {136, TRIG_Switch, ACT_Door, KEY_Yellow, false},
    {137, TRIG_Switch, ACT_Door, KEY_Yellow, true},
    {140, TRIG_Switch, ACT_Raise, KEY_None, true},

    // walk-over lines
    {2, TRIG_Walk, ACT_Door, KEY_None, true},
    {4, TRIG_Walk, ACT_Door, KEY_None, true},
    {5, TRIG_Walk, ACT_Raise, KEY_None, true},
    {8, TRIG_Walk, ACT_Stairs, KEY_None, true},
    {10, TRIG_Walk, ACT_Lift, KEY_None, true},
    {19, TRIG_Walk, ACT_Lower, KEY_None, true},
    {22, TRIG_Walk, ACT_Raise, KEY_None, true},
    {30, TRIG_Walk, ACT_Raise, KEY_None, true},
    {36, TRIG_Walk, ACT_Lower, KEY_None, true},
    {37, TRIG_Walk, ACT_Lower, KEY_None, true},
    {38, TRIG_Walk, ACT_Lower, KEY_None, true},
    {52, TRIG_Walk, ACT_Exit, KEY_None, true},
    {56, TRIG_Walk, ACT_Raise, KEY_None, true},
    {58, TRIG_Walk, ACT_Raise, KEY_None, true},
    {59, TRIG_Walk, ACT_Raise, KEY_None, true},
    {82, TRIG_Walk, ACT_Lower, KEY_None, false},
    {83, TRIG_Walk, ACT_Lower, KEY_None, false},
    {86, TRIG_Walk, ACT_Door, KEY_None, false},
    {88, TRIG_Walk, ACT_Lift, KEY_None, false},
    {90, TRIG_Walk, ACT_Door, KEY_None, false},
    {91, TRIG_Walk, ACT_Raise, KEY_None, false},
    {92, TRIG_Walk, ACT_Raise, KEY_None, false},
    {93, TRIG_Walk, ACT_Raise, KEY_None, false},
    {95, TRIG_Walk, ACT_Raise, KEY_None, false},
    {96, TRIG_Walk, ACT_Raise, KEY_None, false},
    {98, TRIG_Walk, ACT_Lower, KEY_None, false},
    {100, TRIG_Walk, ACT_Stairs, KEY_None, true},
    {105, TRIG_Walk, ACT_Door, KEY_None, false},
    {106, TRIG_Walk, ACT_Door, KEY_None, false},
    {108, TRIG_Walk, ACT_Door, KEY_None, true},
    {109, TRIG_Walk, ACT_Door, KEY_None, true},
    {119, TRIG_Walk, ACT_Raise, KEY_None, true},
    {120, TRIG_Walk, ACT_Lift, KEY_None, false},
    {121, TRIG_Walk, ACT_Lift, KEY_None, true},
    {124, TRIG_Walk, ACT_Exit, KEY_None, true},
    {128, TRIG_Walk, ACT_Raise, KEY_None, false},
    {129, TRIG_Walk, ACT_Raise, KEY_None, false},
    {130, TRIG_Walk, ACT_Raise, KEY_None, true},

    // BOOM extended types
    {219, TRIG_Walk, ACT_Lower, KEY_None, true},
    {220, TRIG_Walk, ACT_Lower, KEY_None, false},
    {221, TRIG_Switch, ACT_Lower, KEY_None, true},
    {222, TRIG_Switch, ACT_Lower, KEY_None, false},

    // shoot-able lines
    {24, TRIG_Gun, ACT_Raise, KEY_None, true},
    {46, TRIG_Gun, ACT_Door, KEY_None, false},
    {47, TRIG_Gun, ACT_Raise, KEY_None, true},

    // the end
    {0, 0, 0, 0, false},
};

// decode a BOOM generalized linedef type
static const special_info_t *DecodeGeneralized(int special)
{
    static special_info_t info;

    // trigger types: W1 WR S1 SR G1 GR D1 DR
    static const int trig_types[8] = {TRIG_Walk, TRIG_Walk, TRIG_Switch, TRIG_Switch,
                                      TRIG_Gun,  TRIG_Gun,  TRIG_Manual, TRIG_Manual};

    info.special = special;
    info.trigger = trig_types[special & 7];
    info.once    = (special & 1) == 0;
    info.key     = KEY_None;
    info.action  = ACT_None;

    if (special >= 0x6000 && special < 0x8000)
    {
        // floors : direction bit
        info.action = (special & 0x40) ? ACT_Raise : ACT_Lower;
    }
    else if (special >= 0x4000 && special < 0x6000)
    {
        // ceilings : not supported
        return nullptr;
    }
    else if (special >= 0x3C00 && special < 0x4000)
    {
        // doors : only the opening kinds
        int kind = (special >> 5) & 3;

        if (kind >= 2)
        {
            return nullptr;
        }

        info.action = ACT_Door;
    }
    else if (special >= 0x3800 && special < 0x3C00)
    {
        // locked doors
        static const int key_types[8] = {KEY_Blue, KEY_Red,  KEY_Blue,   KEY_Yellow,
                                         KEY_Red,  KEY_Blue, KEY_Yellow, KEY_Blue};

        info.action = ACT_Door;
        info.key    = key_types[(special >> 6) & 7];
    }
    else if (special >= 0x3400 && special < 0x3800)
    {
        info.action = ACT_Lift;
    }
    else if (special >= 0x3000 && special < 0x3400)
    {
        info.action = ACT_Stairs;
    }
    else
    {
        return nullptr;
    }

    // generalized types never apply manual actions to anything but doors
    if (info.trigger == TRIG_Manual && info.action != ACT_Door)
    {
        return nullptr;
    }

    return &info;
}

static const special_info_t *LookupSpecial(int special)
{
    for (const special_info_t *info = special_table; info->special; info++)
    {
        if (info->special == special)
        {
            return info;
        }
    }

    if (special >= 0x3000)
    {
        return DecodeGeneralized(special);
    }

    return nullptr;
}

static int KeyPalette(int key)
{
    switch (key)
    {
    case KEY_Red:
        return PAL_RED_KEY;
    case KEY_Yellow:
        return PAL_YELLOW_KEY;
    default:
        return PAL_BLUE_KEY;
    }
}

static void SetSectorLotag(int sec, int lotag)
{
    build_sector_t &S = b_sectors[sec];

    // don't clobber exits and other important stuff
    if (S.lotag == 0 || S.lotag == LOTAG_SECRET_ROOM)
    {
        S.lotag = lotag;
    }
}

// find the Build sector on one side of a linedef, which works even when
// the linedef was dropped (same sector on both sides).
static int LineBuildSector(int line, int side)
{
    int w = (side == 0) ? line_walls[line].first : line_walls[line].second;

    if (w >= 0)
    {
        return WallSector(w);
    }

    const cap_line_t &L = cap->lines[line];

    int dsec = LineSector(L, side);

    if (dsec < 0)
    {
        return -1;
    }

    // use the midpoint of the line, nudged towards the given side
    const cap_vertex_t &A = cap->vertices[L.v1];
    const cap_vertex_t &B = cap->vertices[L.v2];

    double dx  = B.x - A.x;
    double dy  = B.y - A.y;
    double len = std::max(1.0, sqrt(dx * dx + dy * dy));

    double nx = dy / len * (side == 0 ? 4 : -4);
    double ny = -dx / len * (side == 0 ? 4 : -4);

    int bx = ToBuildX(OBSIDIAN_I_ROUND((A.x + B.x) / 2.0 + nx));
    int by = ToBuildY(OBSIDIAN_I_ROUND((A.y + B.y) / 2.0 + ny));

    for (int s : doom_to_build[dsec])
    {
        if (InsideSector(bx, by, s))
        {
            return s;
        }
    }

    return doom_to_build[dsec].empty() ? -1 : doom_to_build[dsec][0];
}

static void MakeSwitchWall(int w, int lotag, int key)
{
    if (w < 0)
    {
        return;
    }

    build_wall_t &W = b_walls[w];

    if (key != KEY_None)
    {
        W.picnum = ACCESSSWITCH;
        W.pal    = KeyPalette(key);
    }
    else if (!IsSwitchTile(W.picnum))
    {
        W.picnum = HANDSWITCH;
    }

    W.lotag = lotag;
}

static int LongestSolidWall(int sec)
{
    const build_sector_t &S = b_sectors[sec];

    int    best     = -1;
    double best_len = 0;

    for (int k = 0; k < S.wallnum; k++)
    {
        const build_wall_t &W1 = b_walls[S.wallptr + k];
        const build_wall_t &W2 = b_walls[W1.point2];

        double len = ComputeDist(W1.x, W1.y, W2.x, W2.y);

        // prefer solid walls
        if (W1.nextwall < 0)
        {
            len += 1e6;
        }

        if (len > best_len)
        {
            best     = S.wallptr + k;
            best_len = len;
        }
    }

    return best;
}

// place a NUKEBUTTON sprite on a wall, which takes the player to the
// secret level.
static void AddSecretButton(int w)
{
    const build_wall_t &W1 = b_walls[w];
    const build_wall_t &W2 = b_walls[W1.point2];

    int sec = WallSector(w);

    double dx  = W2.x - W1.x;
    double dy  = W2.y - W1.y;
    double len = std::max(1.0, sqrt(dx * dx + dy * dy));

    // normal pointing into the sector (interior is on the right side)
    double nx = -dy / len;
    double ny = dx / len;

    build_sprite_t SP;

    SP.picnum  = NUKEBUTTON;
    SP.pal     = PAL_SECRET;
    SP.lotag   = secret_target;
    SP.sectnum = sec;
    SP.cstat   = 16 | 64; // wall aligned, one sided
    SP.xrepeat = SP.yrepeat = 16;
    SP.x                    = OBSIDIAN_I_ROUND((W1.x + W2.x) / 2.0 + nx * 4);
    SP.y                    = OBSIDIAN_I_ROUND((W1.y + W2.y) / 2.0 + ny * 4);
    SP.z                    = b_sectors[sec].floorz - (40 << 8);
    SP.ang                  = (int)(atan2(ny, nx) * 1024.0 / M_PI) & 2047;
    SP.shade                = b_sectors[sec].floorshade;

    // keep the button below the ceiling
    if (SP.z < b_sectors[sec].ceilingz + (16 << 8))
    {
        SP.z = (b_sectors[sec].floorz + b_sectors[sec].ceilingz) / 2;
    }

    b_sprites.push_back(SP);

    LogPrint("Duke: secret exit to level %d\n", secret_target);
}

static void ProcessSpecials()
{
    // build list of Build sectors for each DOOM tag
    std::unordered_map<int, std::vector<int>> tag_sectors;

    for (int s = 0; s < (int)b_sectors.size(); s++)
    {
        int tag = cap->sectors[b_sectors[s].doom_sec].tag;

        if (tag > 0)
        {
            tag_sectors[tag].push_back(s);
        }
    }

    // channels used by locked doors, keyed by Build sector
    std::unordered_map<int, int> door_channels;
    // channels used by remote actions, keyed by DOOM tag
    std::unordered_map<int, int> tag_channels;
    // sectors which already got an ACTIVATOR for a channel
    std::unordered_map<int64_t, bool> have_activator;

    auto add_activator = [&](int sec, int channel) {
        int64_t key = ((int64_t)sec << 32) | (uint32_t)channel;

        if (!have_activator[key])
        {
            AddMarker(sec, ACTIVATOR, channel, 0);
            have_activator[key] = true;
        }
    };

    int unknown = 0;

    bool have_secret_button = false;

    std::vector<int> unknown_list;

    for (int i = 0; i < (int)cap->lines.size(); i++)
    {
        const cap_line_t &L = cap->lines[i];

        if (L.special <= 0)
        {
            continue;
        }

        const special_info_t *info = LookupSpecial(L.special);

        if (!info)
        {
            unknown++;

            if (std::find(unknown_list.begin(), unknown_list.end(), L.special) == unknown_list.end())
            {
                unknown_list.push_back(L.special);
            }
            continue;
        }

        int wf = line_walls[i].first;
        int wb = line_walls[i].second;

        /* exits */

        if (info->action == ACT_Exit)
        {
            bool secret = (L.special == 51 || L.special == 124);

            if (secret && secret_target > 0)
            {
                // one button is enough (prefabs often have several lines)
                if (!have_secret_button)
                {
                    int w = -1;

                    if (info->trigger == TRIG_Switch && (wf >= 0 || wb >= 0))
                    {
                        w = (wf >= 0) ? wf : wb;
                    }
                    else
                    {
                        // walk-over secret exit (or a switch line which was
                        // removed) : put the button on the longest solid
                        // wall of the room
                        int sec = LineBuildSector(i, 1);
                        if (sec < 0)
                        {
                            sec = LineBuildSector(i, 0);
                        }
                        if (sec >= 0)
                        {
                            w = LongestSolidWall(sec);
                        }
                    }

                    if (w >= 0)
                    {
                        AddSecretButton(w);
                        have_secret_button = true;
                        continue;
                    }
                }
                else
                {
                    continue;
                }
            }

            if (info->trigger == TRIG_Switch)
            {
                MakeSwitchWall(wf >= 0 ? wf : wb, LOTAG_END_LEVEL, KEY_None);
            }
            else
            {
                int sec = LineBuildSector(i, 1);
                if (sec < 0)
                {
                    sec = LineBuildSector(i, 0);
                }
                if (sec >= 0)
                {
                    b_sectors[sec].lotag = LOTAG_END_LEVEL;
                }
            }
            continue;
        }

        /* manual doors */

        if (info->trigger == TRIG_Manual)
        {
            int door = (wb >= 0) ? WallSector(wb) : -1;

            if (door < 0)
            {
                continue;
            }

            SetSectorLotag(door, LOTAG_CEIL_DOOR);

            if (info->key != KEY_None)
            {
                if (door_channels.find(door) == door_channels.end())
                {
                    door_channels[door] = next_channel++;
                }

                int channel = door_channels[door];

                add_activator(door, channel);

                MakeSwitchWall(wf, channel, info->key);
            }
            continue;
        }

        /* remote actions */

        if (L.tag <= 0)
        {
            continue;
        }

        const std::vector<int> &targets = tag_sectors[L.tag];

        // lifts are operated by the player standing on them, there is
        // no need for switches or touch plates.
        if (info->action == ACT_Lift)
        {
            for (int sec : targets)
            {
                SetSectorLotag(sec, LOTAG_ELEV_UP);
            }
            continue;
        }

        int lotag = LOTAG_ELEV_UP;

        if (info->action == ACT_Door)
        {
            lotag = LOTAG_CEIL_DOOR;
        }
        else if (info->action == ACT_Lower)
        {
            lotag = LOTAG_ELEV_DOWN;
        }

        // a switch on the edge of the sector it moves (e.g. a pedestal
        // which lowers when used) : in Duke the player can simply use
        // the sector itself.
        if (info->trigger == TRIG_Switch && info->key == KEY_None)
        {
            int s1 = (wf >= 0) ? WallSector(wf) : -1;
            int s2 = (wb >= 0) ? WallSector(wb) : -1;

            bool borders = false;

            for (int sec : targets)
            {
                if (sec == s1 || sec == s2)
                {
                    borders = true;
                }
            }

            if (borders)
            {
                for (int sec : targets)
                {
                    SetSectorLotag(sec, lotag);
                }
                continue;
            }
        }

        if (tag_channels.find(L.tag) == tag_channels.end())
        {
            tag_channels[L.tag] = next_channel++;
        }

        int channel = tag_channels[L.tag];

        for (int sec : targets)
        {
            SetSectorLotag(sec, lotag);
            add_activator(sec, channel);
        }

        if (info->trigger == TRIG_Switch)
        {
            MakeSwitchWall(wf >= 0 ? wf : wb, channel, info->key);
        }
        else
        {
            // walk-over and shoot-able lines become touch plates in the
            // sectors on both sides of the line
            int s1 = LineBuildSector(i, 0);
            int s2 = LineBuildSector(i, 1);

            if (s1 >= 0)
            {
                AddMarker(s1, TOUCHPLATE, channel, info->once ? 1 : 0);
            }
            if (s2 >= 0 && s2 != s1)
            {
                AddMarker(s2, TOUCHPLATE, channel, info->once ? 1 : 0);
            }
        }
    }

    if (unknown > 0)
    {
        std::string list;

        for (int sp : unknown_list)
        {
            list += " " + std::to_string(sp);
        }

        LogPrint("Duke: %d linedef specials have no Duke equivalent:%s\n", unknown, list.c_str());
    }
}

/* ----- sprites ----- */

static void ConvertThings()
{
    have_start = false;

    int lost = 0;

    for (const auto &T : cap->things)
    {
        // skip things which only appear in multiplayer
        if (T.options & 16)
        {
            continue;
        }

        int picnum = -1;
        int pal    = 0;
        int lotag  = 0;

        bool is_start = false;

        if (T.type >= ENTITY_BASE)
        {
            int rest = T.type - ENTITY_BASE;

            picnum = rest % 10000;
            pal    = rest / 10000;
        }
        else if (T.type >= 1 && T.type <= 4)
        {
            // co-operative player starts
            picnum   = APLAYER;
            lotag    = 1;
            is_start = (T.type == 1);
        }
        else if (T.type == 11)
        {
            // deathmatch start
            picnum = APLAYER;
            lotag  = 0;
        }
        else
        {
            static std::vector<int> warned;

            if (std::find(warned.begin(), warned.end(), T.type) == warned.end())
            {
                LogPrint("WARNING: Duke: unknown thing type %d\n", T.type);
                warned.push_back(T.type);
            }
            continue;
        }

        int bx = ToBuildX(T.x);
        int by = ToBuildY(T.y);

        int sec = FindSector(bx, by);

        if (sec < 0)
        {
            lost++;
            continue;
        }

        build_sprite_t SP;

        SP.x       = bx;
        SP.y       = by;
        SP.z       = b_sectors[sec].floorz - T.h * Z_SCALE;
        SP.picnum  = picnum;
        SP.pal     = pal;
        SP.sectnum = sec;
        SP.shade   = b_sectors[sec].floorshade;
        SP.ang     = (2048 - (T.angle * 2048 / 360)) & 2047;
        SP.lotag   = lotag;

        if (IsMonsterTile(picnum))
        {
            SP.xrepeat = SP.yrepeat = 40;
        }

        // skill levels: in Duke a monster or item with a lotag higher
        // than the current skill (1..4) is removed.
        if (IsMonsterTile(picnum) || IsItemTile(picnum))
        {
            if (T.options & 1)
            {
                SP.lotag = 0;
            }
            else if (T.options & 2)
            {
                SP.lotag = 2;
            }
            else
            {
                SP.lotag = 3;
            }
        }

        if (IsSolidDecor(picnum))
        {
            SP.cstat |= 1;
        }

        if (picnum == APLAYER)
        {
            SP.z       = b_sectors[sec].floorz - PLAYER_EYE_Z;
            SP.xrepeat = 42;
            SP.yrepeat = 36;
        }

        if (is_start && !have_start)
        {
            have_start = true;
            start_x    = SP.x;
            start_y    = SP.y;
            start_z    = SP.z;
            start_ang  = SP.ang;
            start_sec  = sec;
        }

        b_sprites.push_back(SP);
    }

    if (lost > 0)
    {
        LogPrint("WARNING: Duke: %d things were outside of every sector\n", lost);
    }

    if (!have_start && !b_sectors.empty())
    {
        LogPrint("WARNING: Duke: no player start, using the first sector\n");

        have_start = true;
        start_sec  = 0;
        start_ang  = 0;

        if (!SectorInteriorPoint(0, &start_x, &start_y))
        {
            start_x = b_walls[0].x;
            start_y = b_walls[0].y;
        }

        start_z = b_sectors[0].floorz - PLAYER_EYE_Z;
    }
}

/* ----- MAP file writing ----- */

class map_writer_c
{
  public:
    std::vector<uint8_t> data;

    void U8(int v)
    {
        data.push_back((uint8_t)(v & 0xFF));
    }

    void S16(int v)
    {
        U8(v);
        U8(v >> 8);
    }

    void S32(int v)
    {
        uint32_t u = (uint32_t)v;
        U8(u);
        U8(u >> 8);
        U8(u >> 16);
        U8(u >> 24);
    }
};

static std::vector<uint8_t> WriteMapData()
{
    int version = 7;

    if ((int)b_sectors.size() > MAXSECTORS_V7 || (int)b_walls.size() > MAXWALLS_V7 ||
        (int)b_sprites.size() > MAXSPRITES_V7)
    {
        version = 8;

        LogPrint("Duke: level exceeds the vanilla limits, writing a version 8 map (EDuke32)\n");
    }

    if ((int)b_sectors.size() > MAXSECTORS_V8 || (int)b_walls.size() > MAXWALLS_V8 ||
        (int)b_sprites.size() > MAXSPRITES_V8)
    {
        LogPrint("WARNING: Duke: level exceeds the EDuke32 limits!\n");
    }

    map_writer_c M;

    M.S32(version);

    M.S32(start_x);
    M.S32(start_y);
    M.S32(start_z);
    M.S16(start_ang);
    M.S16(start_sec);

    M.S16((int)b_sectors.size());

    for (const auto &S : b_sectors)
    {
        M.S16(S.wallptr);
        M.S16(S.wallnum);
        M.S32(S.ceilingz);
        M.S32(S.floorz);
        M.S16(S.ceilingstat);
        M.S16(S.floorstat);
        M.S16(S.ceilingpicnum);
        M.S16(S.ceilingheinum);
        M.U8(S.ceilingshade);
        M.U8(S.ceilingpal);
        M.U8(S.ceilingxpanning);
        M.U8(S.ceilingypanning);
        M.S16(S.floorpicnum);
        M.S16(S.floorheinum);
        M.U8(S.floorshade);
        M.U8(S.floorpal);
        M.U8(S.floorxpanning);
        M.U8(S.floorypanning);
        M.U8(S.visibility);
        M.U8(0); // filler
        M.S16(S.lotag);
        M.S16(S.hitag);
        M.S16(S.extra);
    }

    M.S16((int)b_walls.size());

    for (const auto &W : b_walls)
    {
        M.S32(W.x);
        M.S32(W.y);
        M.S16(W.point2);
        M.S16(W.nextwall);
        M.S16(W.nextsector);
        M.S16(W.cstat);
        M.S16(W.picnum);
        M.S16(W.overpicnum);
        M.U8(W.shade);
        M.U8(W.pal);
        M.U8(W.xrepeat);
        M.U8(W.yrepeat);
        M.U8(W.xpanning);
        M.U8(W.ypanning);
        M.S16(W.lotag);
        M.S16(W.hitag);
        M.S16(W.extra);
    }

    M.S16((int)b_sprites.size());

    for (const auto &SP : b_sprites)
    {
        M.S32(SP.x);
        M.S32(SP.y);
        M.S32(SP.z);
        M.S16(SP.cstat);
        M.S16(SP.picnum);
        M.U8(SP.shade);
        M.U8(SP.pal);
        M.U8(SP.clipdist);
        M.U8(0); // filler
        M.U8(SP.xrepeat);
        M.U8(SP.yrepeat);
        M.U8(SP.xoffset);
        M.U8(SP.yoffset);
        M.S16(SP.sectnum);
        M.S16(SP.statnum);
        M.S16(SP.ang);
        M.S16(SP.owner);
        M.S16(SP.xvel);
        M.S16(SP.yvel);
        M.S16(SP.zvel);
        M.S16(SP.lotag);
        M.S16(SP.hitag);
        M.S16(SP.extra);
    }

    return M.data;
}

// on a boss level, killing the boss ends the episode (the boss actors
// do this when their palette is 0), so the normal exits are removed.
static void RemoveNormalExits()
{
    bool have_boss = false;

    for (const auto &SP : b_sprites)
    {
        if ((SP.picnum == 2630 || SP.picnum == 2710 || SP.picnum == 2760 || SP.picnum == 4740) && SP.pal == 0)
        {
            have_boss = true;
        }
    }

    if (!have_boss)
    {
        LogPrint("WARNING: Duke: boss level without a boss, keeping the exit\n");
        return;
    }

    for (auto &W : b_walls)
    {
        if (W.lotag == LOTAG_END_LEVEL)
        {
            W.lotag = 0;
        }
    }

    for (auto &S : b_sectors)
    {
        if (S.lotag == LOTAG_END_LEVEL)
        {
            S.lotag = 0;
        }
    }

    LogPrint("Duke: boss level, the episode ends when the boss is killed\n");
}

static void ConvertLevel()
{
    CanonicalizeVertices();
    MergeLightSectors();
    CollectEdges();
    CreateSectorsAndWalls();
    SetupWalls();

    b_sprites.clear();
    next_channel = 1;

    ProcessSpecials();
    ConvertThings();

    if (boss_level)
    {
        RemoveNormalExits();
    }

    LogPrint("Duke: %d sectors, %d walls, %d sprites\n", (int)b_sectors.size(), (int)b_walls.size(),
             (int)b_sprites.size());
}

static void FreeLevel()
{
    sector_rep.clear();
    canon_verts.clear();
    vert_canon.clear();
    edges.clear();
    b_sectors.clear();
    b_walls.clear();
    b_sprites.clear();
    line_walls.clear();
    doom_to_build.clear();
    WallSector(-1);
}

/* ----- GRP file writing ----- */

static bool WriteGRP(const std::string &filename)
{
    FILE *fp = FileOpen(filename, "wb");

    if (!fp)
    {
        LogPrint("Duke: cannot create file: %s\n", filename.c_str());
        return false;
    }

    map_writer_c M;

    const char *sig = "KenSilverman";

    for (int i = 0; i < 12; i++)
    {
        M.U8(sig[i]);
    }

    M.S32((int)map_files.size());

    for (const auto &entry : map_files)
    {
        char name[12];
        memset(name, 0, sizeof(name));
        memcpy(name, entry.first.data(), std::min<size_t>(12, entry.first.size()));

        for (int i = 0; i < 12; i++)
        {
            M.U8(name[i]);
        }

        M.S32((int)entry.second.size());
    }

    for (const auto &entry : map_files)
    {
        M.data.insert(M.data.end(), entry.second.begin(), entry.second.end());
    }

    bool ok = (fwrite(M.data.data(), M.data.size(), 1, fp) == 1);

    fclose(fp);

    LogPrint("Duke: wrote %d maps to %s\n", (int)map_files.size(), filename.c_str());

    return ok;
}

/* ----- game interface ----- */

class game_interface_c : public ::game_interface_c
{
  private:
    std::string filename;

  public:
    game_interface_c() : filename("")
    {
    }

    bool        Start(const char *preset) override;
    bool        Finish(bool build_ok) override;
    void        BeginLevel() override;
    void        EndLevel() override;
    void        Property(std::string key, std::string value) override;
    std::string Filename() override
    {
        return filename;
    }
};

bool game_interface_c::Start(const char *preset)
{
    ef_solid_type  = 0;
    ef_liquid_type = 0;
    ef_thing_mode  = 0;

    Doom::sub_format = 0;

    sky_tile = LA_SKY;

    map_files.clear();

    ob_invoke_hook("pre_setup");

    if (batch_mode)
    {
        if (IsPathAbsolute(batch_output_file))
        {
            filename = batch_output_file;
        }
        else
        {
            filename = PathAppend(CurrentDirectoryGet(), batch_output_file);
        }
    }
    else
    {
#ifndef OBSIDIAN_CONSOLE_ONLY
        std::string grp_preset = preset ? preset : "";
        ReplaceExtension(grp_preset, ".grp");
        filename = DLG_OutputFilename("grp", grp_preset.c_str());
#endif
    }

    if (filename.empty())
    {
        ProgStatus("%s", _("Cancelled"));
        return false;
    }

    ReplaceExtension(filename, ".grp");

    if (create_backups)
    {
        Main::BackupFile(filename);
    }

    // make sure we can create the file
    FILE *fp = FileOpen(filename, "wb");

    if (!fp)
    {
        ProgStatus("%s", _("Error (create file)"));
        return false;
    }

    fclose(fp);

    cap = new capture_c();

#ifndef OBSIDIAN_CONSOLE_ONLY
    if (main_win)
    {
        main_win->build_box->Prog_Init(20, _("CSG"));
    }
#endif

    return true;
}

bool game_interface_c::Finish(bool build_ok)
{
    Doom::capture = nullptr;

    delete cap;
    cap = nullptr;

    if (build_ok)
    {
        build_ok = WriteGRP(filename);
    }

    map_files.clear();

    if (!build_ok)
    {
        FileDelete(filename);
    }
    else
    {
        Recent_AddFile(RECG_Output, filename);
    }

    return build_ok;
}

void game_interface_c::BeginLevel()
{
    secret_target = 0;
    boss_level    = false;

    cap->Clear();
    Doom::capture = cap;
}

void game_interface_c::Property(std::string key, std::string value)
{
    if (StringCompare(key, "level_name") == 0)
    {
        level_name = value;
    }
    else if (StringCompare(key, "sky_tile") == 0)
    {
        sky_tile = StringToInt(value);
    }
    else if (StringCompare(key, "secret_target") == 0)
    {
        secret_target = StringToInt(value);
    }
    else if (StringCompare(key, "boss_level") == 0)
    {
        boss_level = (StringToInt(value) != 0);
    }
#ifndef OBSIDIAN_CONSOLE_ONLY
    else if (StringCompare(key, "description") == 0 && main_win)
    {
        main_win->build_box->name_disp->copy_label(value.c_str());
        main_win->build_box->name_disp->redraw();
    }
#endif
    else
    {
        // most DOOM-specific properties are simply not needed
        DebugPrint("Duke: ignoring property %s=%s\n", key.c_str(), value.c_str());
    }
}

void game_interface_c::EndLevel()
{
    if (level_name.empty())
    {
        FatalError("Script problem: did not set level name!\n");
    }

#ifndef OBSIDIAN_CONSOLE_ONLY
    if (main_win)
    {
        main_win->build_box->Prog_Step("CSG");
    }
#endif

    LogPrint("Duke: converting level %s\n", level_name.c_str());

    // the DOOM CSG code makes the sectors, which are captured
    CSG_DOOM_Write();

    Doom::capture = nullptr;

    ConvertLevel();

    std::string map_name = level_name;
    for (auto &ch : map_name)
    {
        ch = ToUpperASCII(ch);
    }
    map_name += ".MAP";

    map_files.push_back({map_name, WriteMapData()});

    FreeLevel();
    cap->Clear();

    level_name.clear();
}

} // namespace Duke

game_interface_c *Duke_GameObject()
{
    return new Duke::game_interface_c();
}

//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab
