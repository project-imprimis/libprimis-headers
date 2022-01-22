/**
 * @file octa.h
 * @brief
 */

#ifndef OCTA_H_
#define OCTA_H_

const int Face_MaxVerts = 15;

struct facebounds
{
    ushort u1, u2, v1, v2;

    bool empty() const { return u1 >= u2 || v1 >= v2; }
};

struct cubeext;

constexpr uint faceempty = 0;             /**< all edges in the range (0,0) */
constexpr uint facesolid = 0x80808080;    /**< all edges in the range (0,8) */

class cube
{
    public:
        cube *children;          /**< points to 8 cube structures which are its children, or nullptr. -Z first, then -Y, -X */
        cubeext *ext;            /**< extended info for the cube */
        union
        {
            uchar edges[12];     /**< edges of the cube, each uchar is 2 4bit values denoting the range. */
                                 /**< see documentation jpgs for more info. */
            uint faces[3];       /**< 4 edges of each dimension together representing 2 perpendicular faces */
        };
        ushort texture[6];       /**< one for each face. same order as orient. */
        ushort material;         /**< empty-space material */
        uchar merged;            /**< merged faces of the cube */
        union
        {
            uchar escaped;       /**< mask of which children have escaped merges */
            uchar visible;       /**< visibility info for faces */
        };
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

        void setmat(ushort mat, ushort matmask, ushort filtermat, ushort filtermask, int filtergeom);
        void discardchildren(bool fixtex = false, int depth = 0);
        void calcmerges();
        bool isvalidcube();

    private:

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
            bool mergepolys(int orient, hashset<plink> &links, std::vector<plink *> &queue, int owner, poly &q, const pedge &e);
        };

        void freecubeext(cube &c);
        void genmerges(const ivec &o = ivec(0, 0, 0), int size = 9);
        bool genpoly(int orient, const ivec &o, int size, int vis, ivec &n, int &offset, poly &p);
        void clearmerge(int orient);
        void addmerge(int orient, const ivec &co, const ivec &n, int offset, poly &p);
        void addmerges(int orient, const ivec &co, const ivec &n, int offset, std::vector<poly> &polys);
        void mergepolys(int orient, const ivec &co, const ivec &n, int offset, std::vector<poly> &polys);

        struct cfpolys
        {
            std::vector<poly> polys;
        };

        struct cfkey
        {
            uchar orient;
            ushort material, tex;
            ivec n;
            int offset;
        };

        static hashtable<cube::cfkey, cube::cfpolys> cpolys;

        //need htcmp to be free functions to work with tools.h
        //but nothing else needs it
        friend bool htcmp(const cube::cfkey &x, const cube::cfkey &y);
        friend uint hthash(const cube::cfkey &k);

        friend uint hthash(const cube::pedge &x);
        friend bool htcmp(const cube::pedge &x, const cube::pedge &y);
};

struct selinfo
{
    int corner;
    int cx, cxs, cy, cys;
    ivec o, s; //two corners of the selection (s is an _offset_ vector
    int grid, orient;
    selinfo() : corner(0), cx(0), cxs(0), cy(0), cys(0), o(0, 0, 0), s(0, 0, 0), grid(8), orient(0) {}
    int size() const
    {
        return s.x*s.y*s.z;
    }

    int us(int d) const
    {
        return s[d]*grid;
    }

    bool operator==(const selinfo &sel) const
    {
        return o==sel.o && s==sel.s && grid==sel.grid && orient==sel.orient;
    }

    bool validate()
    {
        extern int worldsize;
        if(grid <= 0 || grid >= worldsize)
        {
            return false;
        }
        if(o.x >= worldsize || o.y >= worldsize || o.z >= worldsize)
        {
            return false;
        }
        if(o.x < 0)
        {
            s.x -= (grid - 1 - o.x)/grid;
            o.x = 0;
        }
        if(o.y < 0)
        {
            s.y -= (grid - 1 - o.y)/grid;
            o.y = 0;
        }
        if(o.z < 0)
        {
            s.z -= (grid - 1 - o.z)/grid;
            o.z = 0;
        }
        s.x = std::clamp(s.x, 0, (worldsize - o.x)/grid);
        s.y = std::clamp(s.y, 0, (worldsize - o.y)/grid);
        s.z = std::clamp(s.z, 0, (worldsize - o.z)/grid);
        return s.x > 0 && s.y > 0 && s.z > 0;
    }
};

struct block3
{
    ivec o, s;
    int grid, orient;
    block3() {}
    block3(const selinfo &sel) : o(sel.o), s(sel.s), grid(sel.grid), orient(sel.orient) {}

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
    int i;
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

extern cube *worldroot;             /**< the world data. only a ptr to 8 cubes (ie: like cube.children above) */

extern int selchildcount, selchildmat;

/**
 * @brief sets the faces to a given value `face` given
 */
inline void setcubefaces(cube &c, uint face)
{
    c.faces[0] = c.faces[1] = c.faces[2] = face;
}

#define EDGE_GET(edge, coord) ((coord) ? (edge)>>4 : (edge)&0xF)
#define EDGE_SET(edge, coord, val) ((edge) = ((coord) ? ((edge)&0xF)|((val)<<4) : ((edge)&0xF0)|(val)))

#define CUBE_EDGE(c, d, x, y) ((c).edges[(((d)<<2)+((y)<<1)+(x))])

inline int octadim(int d)
{
    return 1<<d;
}

//note that these macros actually loop in the opposite order: e.g. loopxy runs a for loop of x inside y
#define LOOP_XY(b)        for(int y = 0; y < (b).s[C[DIMENSION((b).orient)]]; ++y) for(int x = 0; x < (b).s[R[DIMENSION((b).orient)]]; ++x)
#define LOOP_XYZ(b, r, f) { for(int z = 0; z < (b).s[D[DIMENSION((b).orient)]]; ++z) LOOP_XY((b)) { cube &c = blockcube(x,y,z,b,r); f; } }
#define LOOP_SEL_XYZ(f)    { if(local) makeundo(); LOOP_XYZ(sel, sel.grid, f); changed(sel); }
#define SELECT_CUBE(x, y, z) blockcube(x, y, z, sel, sel.grid)

// guard against subdivision
#define PROTECT_SEL(f) { undoblock *_u = newundocube(sel); f; if(_u) { pasteundoblock(_u->block(), _u->gridmap()); freeundo(_u); } }

#define DIMENSION(orient)  ((orient)>>1)
#define DIM_COORD(orient)  ((orient)&1)
#define OPPOSITE(orient)   ((orient)^1)

struct undolist
{
    undoblock *first, *last;

    undolist() : first(nullptr), last(nullptr) {}

    bool empty() { return !first; }

    void add(undoblock *u)
    {
        u->next = nullptr;
        u->prev = last;
        if(!first)
        {
            first = last = u;
        }
        else
        {
            last->next = u;
            last = u;
        }
    }

    undoblock *popfirst()
    {
        undoblock *u = first;
        first = first->next;
        if(first)
        {
            first->prev = nullptr;
        }
        else
        {
            last = nullptr;
        }
        return u;
    }

    undoblock *poplast()
    {
        undoblock *u = last;
        last = last->prev;
        if(last)
        {
            last->next = nullptr;
        }
        else
        {
            first = nullptr;
        }
        return u;
    }
};

#endif /* OCTA_H_ */
