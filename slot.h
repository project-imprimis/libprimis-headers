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

/**
 * @brief Bitmask variables for a VSlot.changed field.
 *
 * These variables determine what about a virtual texture slot has been changed.
 * Use them by bit shifting by the value of the enum, e.g.
 * ```
 * slot.changed = 1 << VSlot_ShParam;
 * ```
 */
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
    size_t loc;
    int flags;
    float val[4];
};

struct Texture;
class Shader;
struct VSlot; //both slot and vslot depend on each other

/**
 * @brief A representation of a texture inside the engine.
 *
 * A texture slot represents the texture and shader information for a single
 * application onto an octree face. This includes a diffuse texture, as well as
 * other shader-defined textures such as normal, parallax, and specular maps if
 * they are part of the shader configuration of the slot object.
 */
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
    std::vector<Tex> sts;
    Shader *shader;
    std::vector<SlotShaderParam> params;
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

    /**
     * @brief Attempts to find a tex with index type.
     *
     * Searches for a tex with the type passed, searching only for the part of the
     * sts vector after `last`. If none is found, returns -1.
     *
     * @param type the type to seach for
     * @param last the last index to not look at
     *
     * @return the index where the type was found
     */
    int findtextype(int type, int last = -1) const;

    void load(int index, Slot::Tex &t);
    void load();

    /**
     * @brief Returns a new texture that is a thumbnail of the slot's texture.
     *
     * Creates a new texture on the heap representing a scaled-down version of
     * the slot's diffuse map.
     *
     * @return a pointer to the generated Texture containing the thumbnail
     */
    Texture *loadthumbnail();

    void reset()
    {
        smooth = -1;
        sts.clear();
        shader = nullptr;
        params.clear();
        loaded = false;
        texmask = 0;
        delete[] grass;
        grass = nullptr;
        grasstex = nullptr;
        thumbnail = nullptr;
    }

    void cleanup()
    {
        loaded = false;
        grasstex = nullptr;
        thumbnail = nullptr;
        for(uint i = 0; i < sts.size(); i++)
        {
            Tex &t = sts[i];
            t.t = nullptr;
            t.combined = -1;
        }
    }
};

/**
 * @brief A virtual texture slot.
 *
 * A virtual texture slot ("VSlot") is a derivative of a standard texture slot;
 * it contains modification information (such as rotation, translation, tinting,
 * and modifications to other shader settings) but not the fundamental attributes
 * of a texture.
 *
 * Each VSlot must point to a normal slot to have any meaning; it is from this slot
 * object where the VSlot gets the attributes of its texture files and shader
 * definition information.
 */
struct VSlot
{
    Slot *slot;
    VSlot *next;
    int index, changed;
    std::vector<SlotShaderParam> params;
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
        params.clear();
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

/**
 * @param A derivative form of a slot intended for decal objects.
 */
struct DecalSlot final : Slot, VSlot
{
    float depth, fade;

    DecalSlot(int index = -1) : Slot(index), VSlot(this), depth(1), fade(0.5f) {}

    int type() const override final
    {
        return SlotType_Decal;
    }

    const char *name() const override final;
    const char *texturedir() const override final
    {
        return "media/decal";
    }

    VSlot &emptyvslot() override final
    {
        return *this;
    }

    int cancombine(int type) const override final;
    bool shouldpremul(int type) const override final;

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

extern std::vector<Slot *> slots;
extern std::vector<VSlot *> vslots;
extern std::vector<int *> editingvslots;

extern const char *getshaderparamname(const char *name, bool insert = true);
extern void setldrnotexture();

extern VSlot *findvslot(const Slot &slot, const VSlot &src, const VSlot &delta);
extern VSlot *editvslot(const VSlot &src, const VSlot &delta);
extern Slot &lookupslot(int slot, bool load = true);
extern VSlot &lookupvslot(int slot, bool load = true);
extern bool unpackvslot(ucharbuf &buf, VSlot &dst, bool delta);
extern DecalSlot &lookupdecalslot(int slot, bool load = true);

#endif /* SLOT_H_ */
