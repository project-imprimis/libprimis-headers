/**
 * @file slot.h
 * @brief Texture slot objects
 *
 * The texture slot objects expose the data structures used to organize geometry
 * texture information and encode it in a level. A vector containing a series of
 * slot objects is associated with every cube object, allowing every cube face
 * to reference a slot "palette" containing many texture + shader combinations
 * without requiring much data stored on the client.
 *
 * Furthermore, a `VSlot` object is defined, which defines a virtual texture slot.
 * These are derived from a "real" texture slot, and can have one or more shader
 * modifiers applied to it: color, scale, rotation etc.
 */

#ifndef SLOT_H_
#define SLOT_H_

enum
{
    VSlot_ShParam = 0,
    VSlot_Scale,
    VSlot_Rotation,
    VSlot_Offset,
    VSlot_Scroll,
    VSlot_Layer,
    VSlot_Alpha,
    VSlot_Color,
    VSlot_Reserved, // used by RE
    VSlot_Refract,
    VSlot_Detail,
    VSlot_Angle,
    VSlot_Num
};

struct SlotShaderParam
{
    enum
    {
        REUSE = 1<<0
    };

    const char *name;
    int loc, flags;
    float val[4];
};

struct Texture;
struct Shader;
struct VSlot; //both slot and vslot depend on each other

struct Slot
{
    enum
    {
        SlotType_Octa,
        SlotType_Material,
        SlotType_Decal
    };

    struct Tex
    {
        int type;
        Texture *t;
        string name;
        int combined;

        Tex() : t(nullptr), combined(-1) {}
    };

    int index, smooth;
    vector<Tex> sts;
    Shader *shader;
    vector<SlotShaderParam> params;
    VSlot *variants;
    bool loaded;
    uint texmask;
    char *grass;
    Texture *grasstex, *thumbnail;

    Slot(int index = -1) : index(index), variants(nullptr), grass(nullptr) { reset(); }
    virtual ~Slot() {}

    virtual int type() const
    {
        return SlotType_Octa;
    }

    virtual const char *name() const;
    virtual const char *texturedir() const
    {
        return "media/texture";
    }

    virtual VSlot &emptyvslot();

    virtual int cancombine(int type)const;
    virtual bool shouldpremul(int) const
    {
        return false;
    }

    int findtextype(int type, int last = -1) const;

    void load(int index, Slot::Tex &t);
    void load();

    Texture *loadthumbnail();

    void reset()
    {
        smooth = -1;
        sts.setsize(0);
        shader = nullptr;
        params.setsize(0);
        loaded = false;
        texmask = 0;
        DELETEA(grass);
        grasstex = nullptr;
        thumbnail = nullptr;
    }

    void cleanup()
    {
        loaded = false;
        grasstex = nullptr;
        thumbnail = nullptr;
        for(int i = 0; i < sts.length(); i++)
        {
            Tex &t = sts[i];
            t.t = nullptr;
            t.combined = -1;
        }
    }
};

struct VSlot
{
    Slot *slot;
    VSlot *next;
    int index, changed;
    vector<SlotShaderParam> params;
    bool linked;
    float scale;
    int rotation;
    vec angle;
    ivec2 offset;
    vec2 scroll;
    int layer;
    float alphafront, alphaback;
    vec colorscale;
    vec glowcolor;
    float refractscale;
    vec refractcolor;

    VSlot(Slot *slot = nullptr, int index = -1) : slot(slot), next(nullptr), index(index), changed(0)
    {
        reset();
        if(slot)
        {
            addvariant(slot);
        }
    }

    void addvariant(Slot *slot);

    void reset()
    {
        params.setsize(0);
        linked = false;
        scale = 1;
        rotation = 0;
        angle = vec(0, sinf(0), cosf(0));
        offset = ivec2(0, 0);
        scroll = vec2(0, 0);
        layer = 0;
        alphafront = 0.5f;
        alphaback = 0;
        colorscale = vec(1, 1, 1);
        glowcolor = vec(1, 1, 1);
        refractscale = 0;
        refractcolor = vec(1, 1, 1);
    }

    void cleanup()
    {
        linked = false;
    }

    bool isdynamic() const;
};

struct DecalSlot : Slot, VSlot
{
    float depth, fade;

    DecalSlot(int index = -1) : Slot(index), VSlot(this), depth(1), fade(0.5f) {}

    int type() const
    {
        return SlotType_Decal;
    }

    const char *name() const;
    const char *texturedir() const
    {
        return "media/decal";
    }

    VSlot &emptyvslot()
    {
        return *this;
    }

    int cancombine(int type) const;
    bool shouldpremul(int type) const;

    void reset()
    {
        Slot::reset();
        VSlot::reset();
        depth = 1;
        fade = 0.5f;
    }

    void cleanup()
    {
        Slot::cleanup();
        VSlot::cleanup();
    }
};

struct vslotmap
{
    int index;
    VSlot *vslot;

    vslotmap() {}
    vslotmap(int index, VSlot *vslot) : index(index), vslot(vslot) {}
};

extern vector<Slot *> slots;
extern vector<VSlot *> vslots;
extern std::vector<int *> editingvslots;

struct vslotref
{
    vslotref(int &index) { editingvslots.push_back(&index); }
    ~vslotref() { editingvslots.pop_back(); }
};

extern const char *getshaderparamname(const char *name, bool insert = true);
extern void setldrnotexture();

extern VSlot *findvslot(Slot &slot, const VSlot &src, const VSlot &delta);
extern VSlot *editvslot(const VSlot &src, const VSlot &delta);
extern Slot &lookupslot(int slot, bool load = true);
extern VSlot &lookupvslot(int slot, bool load = true);
extern bool unpackvslot(ucharbuf &buf, VSlot &dst, bool delta);
extern DecalSlot &lookupdecalslot(int slot, bool load = true);

#endif /* SLOT_H_ */