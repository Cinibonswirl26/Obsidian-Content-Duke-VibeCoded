//------------------------------------------------------------------------
//  LEVEL building - DOOM format
//------------------------------------------------------------------------
//
//  OBSIDIAN Level Maker
//
//  Copyright (C) 2021-2025 The OBSIDIAN Team
//  Copyright (C) 2006-2017 Andrew Apted
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

#pragma once

#include <stdint.h>

#include <string>
#include <vector>

#include "minilua.h"

class qLump_c
{
  public:
    std::string name;

  private:
    std::vector<uint8_t> buffer;

    // when true Printf() converts '\n' to CR/LF pair
    bool crlf;

  public:
    qLump_c();
    ~qLump_c();

    void Append(const void *data, uint32_t len);
    void Append(qLump_c *other);

    void Prepend(const void *data, uint32_t len);

    void AddByte(uint8_t value);

    void Printf(const char *str, ...);
    void KeyPair(const char *key, const char *val, ...);
    void SetCRLF(bool enable);

    int            GetSize() const;
    const uint8_t *GetBuffer() const;

  private:
    void RawPrintf(const char *str);
};

qLump_c *BSP_CreateInfoLump();

/***** VARIABLES ****************/

namespace Doom
{

enum subformat_e
{
    SUBFMT_Hexen  = 1,
    SUBFMT_Strife = 2,
};

extern int sub_format;

// When a capture object is installed, the Add*() functions below do not
// write any WAD lumps, they pass the map data to the capture object
// instead.  This is used by games with a non-DOOM output format (like
// Duke Nukem 3D) which still use the DOOM CSG code to make sectors.
class map_capture_c
{
  public:
    virtual ~map_capture_c() = default;

    virtual void Vertex(int x, int y) = 0;
    virtual void Sector(int f_h, const std::string &f_tex, int c_h, const std::string &c_tex, int light, int special,
                        int tag) = 0;
    virtual void Sidedef(int sector, const std::string &l_tex, const std::string &m_tex, const std::string &u_tex,
                         int x_offset, int y_offset) = 0;
    virtual void Linedef(int vert1, int vert2, int side1, int side2, int type, int flags, int tag) = 0;
    virtual void Thing(int x, int y, int h, int type, int angle, int options) = 0;

    virtual int NumVertexes() = 0;
    virtual int NumSectors()  = 0;
    virtual int NumSidedefs() = 0;
    virtual int NumLinedefs() = 0;
    virtual int NumThings()   = 0;
};

extern map_capture_c *capture;

/***** FUNCTIONS ****************/

bool StartWAD(const std::string &filename);
bool EndWAD();

void BeginLevel();
void EndLevel(const std::string &level_name);

void WriteLump(std::string_view name, qLump_c *lump);

// the section parameter can be:
//   'P' : patches   //   'F' : flats
//   'S' : sprites   //   'C' : colormaps (Boom)
//   'T' : textures (Zdoom)
void AddSectionLump(char section, std::string_view name, qLump_c *lump);

void HeaderPrintf(const char *str, ...);

void AddVertex(int x, int y);

void AddSector(int f_h, const std::string &f_tex, int c_h, const std::string &c_tex, int light, int special, int tag);

void AddSidedef(int sector, const std::string &l_tex, const std::string &m_tex, const std::string &u_tex, int x_offset, int y_offset);

void AddLinedef(int vert1, int vert2, int side1, int side2, int type, int flags, int tag, const uint8_t *args);

void AddThing(int x, int y, int h, int type, int angle, int options, int tid, uint8_t special, const uint8_t *args);

int NumVertexes();
int NumSectors();
int NumSidedefs();
int NumLinedefs();
int NumThings();

void Send_Prog_Nodes(int progress, int num_maps);

} // namespace Doom

//--- editor settings ---
// vi:ts=4:sw=4:noexpandtab
