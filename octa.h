/**
 * @file octa.h
 * @brief Objects representing the octree and its manipulation methods.
 *
 * This file describes objects that build the octal tree structure of the world
 * (a `cubeworld` containing `cube` objects) as well as convenient representations
 * of selections of cubes (in a Cartesian sense) that allow simpler reasoning about
 * spatial relationships between nodes.
 */

#ifndef OCTA_H_
#define OCTA_H_

const int Face_MaxVerts = 15;

struct cubeext; /** internal engine data for the cube object, not accessible via external API*/

constexpr uint faceempty = 0;             /**< all edges in the range (0,0) */
constexpr uint facesolid = 0x80808080;    /**< all edges in the range (0,8) */

/**
 * @brief The fundemental building block of the octree world, representing a 3D cube.
 *
 * The main rendered object in the world is an octree (8-leaf tree) comprising of
 * objects of this type. Each cube object has its own deformation information,
 * texture information, and a handful of utility functions as well as pointing to
 * up to eight children in a fractal manner.
 *
 * Cube objects are the game-facing abstraction which the renderer itself does not
 * use; they are converted to `vertex arrays` by the engine which requires calculation
 * by itself. However, the octree structure is capable of a relatively simple and
 * effective representation of detail over several orders of magnitude.
 *
 * The integer indices for the textures are shown below.
 * ```
 *  +z
 *   |  /+x
 *   | /
 *   |/___+y
 *      ______
 *     /.    /|
 *    / . 5 / |
 *   /__.__/  |
 *   |  ...|3./
 *   | .0  | /
 *   |.    |/
 *   .-----/
 *      ______
 *     /|    .|
 *    / |  1. |
 *   /..|...  |
 *   |2 |-----/
 *   | /   . /
 *   |/   4./
 *   .-----/
 * ```
 *
 * The integer indices of the 8 children are shown below.
 * ```
 *              ^ +z
 *              |
 *            __________
 *           /  4 /  5 /
 *          /____/____/.
 *         /  6 /  7 / .
 *        /____/____/  .
 *       .    _____.___.
 *       .   /  0 /. 1 /     +x
 *       .  /____/_.__/    ->
 *       . /  2 / 3. /
 *       ./____/___./
 *
 *
 *           / +y
 *          |/
 * ```
 *
 * The integer indices of the 12 edges are shown below.
 * ```
 *              ^ +z
 *              |
 *              |
 *            _____2____
 *           /         /
 *        5 /.8      7/.
 *         / .       / .
 *        /_____3___/  .9
 *       . . ._____.___.
 *     10.   /  0  .   /     +x
 *       . 4/    11.  /    ->
 *       . /       . /6
 *       ./________./
 *            1
 *
 *           / +y
 *          |/
 * ```
 */
class cube
{
    public:
        std::array<cube, 8> *children; /**< points to 8 cube structures which are its children, or nullptr. -Z first, then -Y, -X. If children[0] == nullptr, assume there are no children.*/
        cubeext *ext;            /**< extended info for the cube */
        union
        {
            uchar edges[12];     /**< edges of the cube, each uchar is 2 4bit values denoting the range. */
                                 /**< see documentation jpgs for more info. */
            uint faces[3];       /**< 4 edges of each dimension together representing 2 perpendicular faces */
        };
        ushort texture[6];       /**< one for each face. same order as orient. */
        ushort material;         /**< empty-space material, bitmask created from available mats */
        uchar merged;            /**< merged faces of the cube */
        union
        {
            uchar escaped;       /**< mask of which children have escaped merges */
            uchar visible;       /**< visibility info for faces */
        };
        bool valid;              /**< status of the cube: */
        /**
         * @brief returns if the cube is empty (face 0 does not exist)
         *
         * note that a non-empty (but distorted) cube missing faces for an axis is impossible
         * unless there are no faces at all (impossible to construct a 3d cube otherwise)
         */
        bool isempty() const
        {
            return faces[0]==faceempty;
        }

        /**
         * @brief returns if the cube passed is entirely solid (no distortions)
         */
        bool issolid() const
        {
            return faces[0]==facesolid &&
                   faces[1]==facesolid &&
                   faces[2]==facesolid; //check all three
        }

        /**
         * @brief Sets a cube's materials, given a material & filter to use
         *
         * Modifies the material properties of a cube object, given the various
         * filtering parameters.
         *
         * @param c the cube object to use
         * @param mat material index to apply
         * @param matmask material mask
         * @param filtermat if nonzero, determines what existing mats to apply to
         * @param filtermask filter material mask
         * @param filtergeom type of geometry inside the cube (empty, solid, partially solid)
         */
        void setmat(ushort mat, ushort matmask, ushort filtermat, ushort filtermask, int filtergeom);

        /**
         * @brief discards children
         *
         * @param fixtex toggles fixing the texture of the resulting cube
         * @param depth at which to stop discarding
         */
        void discardchildren(bool fixtex = false, int depth = 0);

        /**
         * @brief Merges adjacent faces that can be losslessly merged.
         *
         * Merges adjacent faces, provided that the resulting cube created
         * is no larger than the `maxmerge` gridpower size. Faces resulting from
         * the merge process are always lossless; therefore the previously
         * existing cubes must be coplanar prior to the merge request.
         *
         * Applies to all children of the cube passed, usually the world root.
         *
         * @param root the cube object with which all children under will be merged
         */
        void calcmerges(cube * root);

        /**
         * @brief Returns whether the cube is valid.
         *
         * If the cube is not convex, this function returns true, otherwise it
         * returns false.
         */
        bool isvalidcube() const;

    private:

        struct facebounds
        {
            ushort u1, u2, v1, v2;

            bool empty() const
            {
                return u1 >= u2 || v1 >= v2;
            }
        };
        struct pvert
        {
            ushort x, y;

            pvert() {}
            pvert(ushort x, ushort y) : x(x), y(y) {}

            bool operator==(const pvert &o) const
            {
                return x == o.x && y == o.y;
            }
            bool operator!=(const pvert &o) const
            {
                return x != o.x || y != o.y;
            }
        };

        struct pedge
        {
            pvert from, to;

            pedge() {}
            pedge(const pvert &from, const pvert &to) : from(from), to(to) {}

            bool operator==(const pedge &o) const
            {
                return from == o.from && to == o.to;
            }
            bool operator!=(const pedge &o) const
            {
                return from != o.from || to != o.to;
            }
        };

        class plink : public pedge
        {
            public:
                int polys[2];

                plink()
                {
                    clear();
                }

                plink(const pedge &p) : pedge(p)
                {
                    clear();
                }
            private:
                void clear()
                {
                    polys[0] = polys[1] = -1;
                }
        };

        struct poly
        {
            cube *c;
            int numverts;
            bool merged;
            pvert verts[Face_MaxVerts];

            bool clippoly(const facebounds &b);
            bool mergepolys(std::unordered_set<plink> &links, std::deque<const plink *> &queue, int owner, poly &q, const pedge &e);
        };

        bool mincubeface(const cube &cu, int orient, const ivec &co, int size, facebounds &orig) const;
        void mincubeface(const cube &cu, int orient, const ivec &o, int size, const facebounds &orig, facebounds &cf, ushort nmat, ushort matmask) const;

        void freecubeext(cube &c);
        void genmerges(cube * root, const ivec &o = ivec(0, 0, 0), int size = 9);
        bool genpoly(int orient, const ivec &o, int size, int vis, ivec &n, int &offset, poly &p);
        void clearmerge(int orient);
        void addmerge(int orient, const ivec &n, int offset, poly &p);
        void addmerges(int orient, const ivec &n, int offset, std::deque<poly> &polys);
        void mergepolys(int orient, const ivec &n, int offset, std::deque<poly> &polys);

        struct cfpolys
        {
            std::deque<poly> polys;
        };

        struct cfkey
        {
            uchar orient;
            ushort material, tex;
            ivec n;
            int offset;
        };

        //for unordered_map
        friend std::hash<plink>;
        friend std::hash<cfkey>;
        friend bool operator==(const cfkey &x, const cfkey &y);
};

/**
 * @brief A representation of a rectangular volume of cubes.
 */
struct selinfo
{
    int corner;
    int cx, cxs, cy, cys;
    ivec o, /**< the coordinates from which the selection starts */
         s; /**< the offset vector conveying the size of the  selection*/
    int grid, /**< the gridscale of the selection */
        orient; /**< the orientation of the selection */
    selinfo() : corner(0), cx(0), cxs(0), cy(0), cys(0), o(0, 0, 0), s(0, 0, 0), grid(8), orient(0) {}

    /**
     * @brief Returns the volume of the selection.
     *
     * The volume returned is in the units of the gridpower being used, so selections
     * with the same cube dimensions but different gridpowers will have the same
     * volume.
     *
     * @return the volume of the selection (l*w*h)
     */
    int size() const
    {
        return s.x*s.y*s.z;
    }

    /**
     * @brief Returns the absolute size of the selection along the specified axis.
     *
     * @param d the dimension to query (0 for x, 1 for y, 2 for z)
     *
     * @return the absolute size of the selection along that axis
     */
    int us(int d) const
    {
        return s[d]*grid;
    }

    /**
     * @brief Returns whether the two selections occupy the same space.
     *
     * Requires that the two selections are occupying the same volume in the
     * world space, and both are of the same gridpower and orientation (selected
     * face). Otherwise, false is returned.
     */
    bool operator==(const selinfo &sel) const
    {
        return o==sel.o && s==sel.s && grid==sel.grid && orient==sel.orient;
    }

    /**
     * @brief Keeps the selection within the world.
     *
     * @return false if the selection attempts to leave the bounds of the world
     */
    bool validate();
};

/**
 * @brief A representation of a rectangular volume of cubes, with less metadata.
 *
 * A rectangular volume of cubes.
 */
struct block3
{
    ivec o, s;
    int grid, orient;
    block3() {}
    block3(const selinfo &sel) : o(sel.o), s(sel.s), grid(sel.grid), orient(sel.orient) {}

    const cube *getcube() const
    {
        return reinterpret_cast<const cube *>(this+1);
    }

    cube *c()
    {
        return reinterpret_cast<cube *>(this+1);
    }

    int size() const
    {
        return s.x*s.y*s.z;
    }
};

struct editinfo
{
    block3 *copy;
    editinfo() : copy(nullptr) {}
};

struct undoent
{
    size_t i; /**< an index in entgroup*/
    entity e;
};

struct undoblock /**< undo header, all data sits in payload */
{
    undoblock *prev, *next;
    int size,       /**< size of undo block */
        timestamp,  /**< time of creation */
        numents;    /**< if numents is 0, is a cube undo record, otherwise an entity undo record */

    block3 *block()
    {
        return reinterpret_cast<block3 *>(this + 1);
    }

    uchar *gridmap()
    {
        block3 *ub = block();
        return reinterpret_cast<uchar *>(ub->c() + ub->size());
    }

    undoent *ents()
    {
        return reinterpret_cast<undoent *>(this + 1);
    }
};

extern int selchildcount, selchildmat;

/**
 * @brief sets the faces to a given value `face` given
 */
inline void setcubefaces(cube &c, uint face)
{
    c.faces[0] = c.faces[1] = c.faces[2] = face;
}

inline int octadim(int d)
{
    return 1<<d;
}

//note that these macros actually loop in the opposite order: e.g. loopxy runs a for loop of x inside y
#define LOOP_XY(b)        for(int y = 0; y < (b).s[C[DIMENSION((b).orient)]]; ++y) for(int x = 0; x < (b).s[R[DIMENSION((b).orient)]]; ++x)
#define LOOP_XYZ(b, r, f) { for(int z = 0; z < (b).s[D[DIMENSION((b).orient)]]; ++z) LOOP_XY((b)) { cube &c = blockcube(x,y,z,b,r); f; } }
#define LOOP_SEL_XYZ(f)    { if(local) makeundo(); LOOP_XYZ(sel, sel.grid, f); rootworld.changed(sel); }
#define SELECT_CUBE(x, y, z) blockcube(x, y, z, sel, sel.grid)

// guard against subdivision
#define PROTECT_SEL(f) { undoblock *_u = newundocube(sel); f; if(_u) { pasteundoblock(_u->block(), _u->gridmap()); freeundo(_u); } }

#define DIMENSION(orient)  ((orient)>>1)
#define DIM_COORD(orient)  ((orient)&1)

extern ivec lu;
extern int lusize;
struct prefab;
struct clipplanes;

/**
 * @brief An object representing the entirety of an octree world.
 *
 * A `cubeworld` is an object storing an entire octree structure and its data;
 *  the octree is made up of `cube` objects, the topmost of which is pointed to
 * by `worldroot`.
 */
class cubeworld
{
    public:
        std::array<cube, 8> *worldroot;

        /**
         * @brief Returns the material bitmask value at the given location.
         *
         * Given a world location, returns the material bitmask at the specified location.
         * This bitmask may be the sum of multiple materials if present.
         * Returns 0 (air) if the location specified has no material or is outside the
         * world.
         *
         * @return an integer corresponding to the material(s) at the specified location
         */
        int lookupmaterial(const vec &v);

        /**
         * @brief Clears the old level and creates a new one.
         *
         * This function unloads the old map, including textures, and creates a new level
         * of the size specified.
         *
         * @param factor sets the gridscale (power of 2 size) of the map; limit 10 to 15
         * @param force if true, creates map regardless of check variables
         * @param usecfg if true, uses the default map config path
         *
         */
        bool emptymap(int factor, bool force, bool usecfg = true);

        /**
         * @brief Grows the map to an additional gridsize.
         *
         * Expands the map by placing the old map in the corner nearest the origin and
         * adding 7 old-map sized cubes to create a new largest cube.
         *
         * This moves the worldroot cube to the new parent cube of the old map.
         */
        bool enlargemap(bool force);
        bool modifyoctaent(int flags, int id, extentity &e);

        /**
         * @brief attempts to reduce the mapsize by 1 (halves all linear dimensions)
         *
         * Fails if the 7 octants not at the origin are not empty.
         * On success, the resulting map will have its maximum gridsize reduced by 1.
         */
        void shrinkmap();

        /**
         * @brief Loads a map file into the octaworld object.
         *
         * Loads a map that has been saved in .ogz format.
         * Will load the configuration file of the same name, unless cname is specified.
         *
         * @param mname the name of the map to load (the ogz)
         * @param gameident the name of the game calling (this is saved in the map)
         * @param gameinfo the string to render while the map is loading
         * @param the name of the map's configuration file (if different than the octree)
         *
         * @return true if the map was loaded
         * @return false if the map failed to load or was not found
         */
        bool load_world(const char *mname, const char *gameident, const char *gameinfo = nullptr, const char *cname = nullptr);

        /**
         * @brief Saves the current map to an ogz file.
         *
         * @param mname the name of the map file once saved
         * @param gameident the name of the game saving the file
         *
         * @return true if the map was saved
         * @return false if the map failed to save
         */
        bool save_world(const char *mname, const char *gameident);

        /**
         * @brief Removes unnecessary virtual texture slots.
         *
         * Checks and removes unused virtual texture slots (vslots) from the octree
         * world.
         */
        int compactvslots(bool cull = false);
        void genprefabmesh(prefab &p);

        /**
         * @brief Destroys vertex arrays for the octree world.
         *
         * Cleans up the geometry objects used by the renderer to render the octree
         * world.
         */
        void cleanupva();

        /**
         * @brief Returns the distance before a ray hits a cube.
         *
         * @param o      the starting location of the ray to be drawn
         * @param ray    normalized direction vector for the ray to point
         * @param radius region around ray that counts as a hit
         * @param mode   flags which determine what counts as a hit
         * @param size   size of cube which registers a hit
         * @param t      entity to check against
         */
        float raycube   (const vec &o, const vec &ray, float radius = 0, int mode = 3, int size = 0, const extentity *t = 0) const;

        /**
         * @brief Returns whether the entity passed has collided with this octaworld.
         *
         * @param d the physent to check
         * @param dir the direction at which to check for a collision
         * @param cutoff the model cutoff factor
         * @param bo the vector for the minimum position of the model
         * @param bs the vector for the maximum position of the model
         */
        bool octacollide(const physent *d, const vec &dir, float cutoff, const ivec &bo, const ivec &bs) const;

        /**
         * @brief Returns a reference to the cube at the specified world coordinates
         *
         * @param to the location to look in
         * @param tsize the size of cube to find (by gridpower)
         * @param ro the found location to be assigned by reference
         * @param rsize the found size (gridpower) of the cube to be assigned by reference
         */
        cube &lookupcube(const ivec &to, int tsize = 0, ivec &ro = lu, int &rsize = lusize);
        bool bboccluded(const ivec &bo, const ivec &br) const;
        void findtjoints();
        void allchanged(bool load = false);
        const cube &neighborcube(int orient, const ivec &co, int size, ivec &ro, int &rsize);

        /**
         * @brief Calculates normals and re-calculates geometry.
         *
         * Re-mips the cube structure of the level to remove unnecessary nodes, then
         * calculates normal maps for corners.
         */
        void calclight();
        float shadowray(const vec &o, const vec &ray, float radius, int mode, const extentity *t = nullptr);
        void changed(const ivec &bbmin, const ivec &bbmax, bool commit = true);
        void changed(const block3 &sel, bool commit = true);
        clipplanes &getclipbounds(const cube &c, const ivec &o, int size, int offset);
        void calcnormals(bool lerptjoints);

        /**
         * @brief Reduces the number of cubes on the level losslessly.
         *
         * Loops through all cube objects, dissolving child nodes where it is possible
         * to convert them to their parent nodes. The largest gridpower that can be
         * optimizied can be controlled by the maxmerge variable.
         */
        void remip();
        /**
         * @brief Returns the CRC code for the map currently loaded.
         *
         * Returns the cyclic redundancy checksum for the file currently loaded. This
         * value is unique for every revision of a map binary, and is useful to make
         * sure multiple clients have the same binary.
         *
         * @return the cyclic redundancy code of the map file currently loaded
         */
        uint getmapcrc() const;

        /**
         * @brief sets the CRC field variable to 0
         *
         * Invalidates the CRC code saved as a cubeworld field for the world, usually
         * to indicate that the CRC has become invalid as a result of modification.
         */
        void clearmapcrc();

        void entitiesinoctanodes();
        void commitchanges(bool force = false);
        void updateparticles();

        /**
         * @brief Returns the gridpower scale of the world
         *
         * Returns the size of the worldroot cube, in terms of the number of
         * half-linear-size cubes it has as descendents in the octree. This value
         * is generally between 10 and 15.
         *
         * @return the scale of the world
         */
        int mapscale() const;

        /**
         * @brief Returns the linear dimensions of the world
         *
         * Returns the size of the worldroot cube, in terms of the number of size
         * 0 cubes on each linear axis. This value is generally between 2^10 and
         * 2^15.
         *
         * @return the size of the world
         */
        int mapsize() const;
    private:
        uint mapcrc; /**< the cyclic redundancy checksum of the entire world*/
        bool haschanged;
        string ogzname, bakname, cfgname, picname;
        int worldscale; /**< should only be set at map creation/load **/


        ///@brief This is a cube() object but with a constructor that indicates nothing is in it
        struct emptycube : cube
        {
            emptycube()
            {
                children = nullptr;
                ext = nullptr;
                visible = 0;
                merged = 0;
                material = 0;
                setcubefaces(*this, faceempty);
                for(int i = 0; i < 6; ++i)
                {
                    texture[i] = 0;
                }
            }
        } emptycube;

        struct mapheader
        {
            char magic[4];              // "TMAP"
            int version;                // any >8bit quantity is little endian
            int headersize;             // sizeof(header)
            int worldsize;
            int numents;
            int numpvs;                 // no longer used, kept for backwards compatibility
            int blendmap;               // also no longer used
            int numvars;
            int numvslots;
        };

        struct octaheader
        {
            char magic[4];              // "OCTA"
            int version;                // any >8bit quantity is little endian
            int headersize;             // sizeof(header)
            int worldsize;
            int numents;
            int numvars;
            int numvslots;
        };
        /**
         * @brief Creates vertex arrays for the octree world.
         *
         * Creates vertex arrays, geometry objects used by the renderer to render
         * the world.
         */
        void octarender();
        void seedparticles();
        void makeparticles(const entity &e);

        /**
         * @brief Resets the metadata associated with a map.
         *
         */
        void resetmap();
        void resetclipplanes();

        /**
         * @brief Saves a cube and its children to the specified stream.
         *
         * Recursively saves a cube and all of its information (materials, textures, etc.) by writing it to the
         * stream object passed by const pointer.
         *
         * @param c the cube to save (usually the world root)
         * @param o the location of the cube in the world
         * @param size the gridpower of the cube to save
         * @param f the stream to save to
         */
        void savec(const std::array<cube, 8> &c, const ivec &o, int size, stream * const f);

        /**
         * @brief Loads a map header given the input stream.
         *
         * @param f the stream to decode into a map header
         * @param ogzname the name of the map file, for debug message purposes
         * @param hdr the mapheader object to be set with values from f
         * @param ohdr the octaheader object to be set with values from f
         *
         * @return true if the input stream is well formed, false otherwise
         */
        bool loadmapheader(stream *f, const char *ogzname, mapheader &hdr, octaheader &ohdr) const;

        /**
         * @brief Sets the map's save file names
         *
         * Also sets the backup names of the file in anology with `fname`.
         *
         * @param fname the octree map file (ogz) name to set
         * @param cname the config file (cfg) name to set
         */
        void setmapfilenames(const char *fname, const char *cname = nullptr);

        bool upoctree(const vec& v, int& x, int& y, int& z, const ivec& lo, int& lshift) const;
        bool checkinsideworld(const vec &invray, float radius, float &outrad, const vec &o, vec &v, const vec &ray, float &dist) const;

};

extern cubeworld rootworld;

#endif /* OCTA_H_ */
