/**
 * @file ents.h
 * @brief Static map entities and dynamic entities.
 *
 * The file defines static map entities of type "entity" and dynamic entities of
 * type "dynent" like players or monsters. The gamecode extends these types to
 * add game specific functionality. Note that entities with the prefix "EngineEnt_"
 * are the only static entity types dictated by the engine. The rest are gamecode
 * dependent.
 */

#ifndef ENTS_H_
#define ENTS_H_

//constants
constexpr int   maxents      = 10000;
constexpr int   crouchtime   = 200;
constexpr float crouchheight = 0.75f;
constexpr int   maxanimparts = 3;
//forward declarations for some internal engine parts of objects
struct occludequery;
class ragdolldata;
class model;

enum
{
    Anim_Mapmodel = 0,
    Anim_GameSpecific
};

enum
{
    Anim_All        = 0x1FF,
    Anim_Index      = 0x1FF,
    Anim_Loop       = (1 << 9),
    Anim_Clamp      = (1 << 10),
    Anim_Reverse    = (1 << 11),
    Anim_Start      = (Anim_Loop | Anim_Clamp),
    Anim_End        = (Anim_Loop | Anim_Clamp | Anim_Reverse),
    Anim_Dir        = 0xE00,
    Anim_Secondary  = 12,
    Anim_Reuse      = 0xFFFFFF,
    Anim_NoSkin     = (1 << 24),
    Anim_SetTime    = (1 << 25),
    Anim_FullBright = (1 << 26),
    Anim_NoRender   = (1 << 27),
    Anim_Ragdoll    = (1 << 28),
    Anim_SetSpeed   = (1 << 29),
    Anim_NoPitch    = (1 << 30),
    Anim_Flags      = 0xFF000000,
};


enum
{
    ClientState_Alive = 0,
    ClientState_Dead,
    ClientState_Spawning,
    ClientState_Lagged,
    ClientState_Editing,
    ClientState_Spectator,
};

enum
{
    EngineEnt_Empty=0,
    EngineEnt_Light,
    EngineEnt_Mapmodel,
    EngineEnt_Playerstart,
    EngineEnt_Particles,
    EngineEnt_Sound,
    EngineEnt_Spotlight,
    EngineEnt_Decal,
    EngineEnt_GameSpecific,
};

enum
{
    PhysEnt_Player = 0,
    PhysEnt_Camera,
    PhysEnt_Bounce,
};

enum
{
    PhysEntState_Float = 0,
    PhysEntState_Fall,
    PhysEntState_Slide,
    PhysEntState_Slope,
    PhysEntState_Floor,
    PhysEntState_StepUp,
    PhysEntState_StepDown,
    PhysEntState_Bounce,
};

/**
 * @brief Entity data capable of being saved to disk.
 */
struct entity                                   /**< persistent map entity */
{
    vec o;                                      /**< position */
    short attr1, attr2, attr3, attr4, attr5;    /**< attributes */
    uchar type;                                 /**< type is one of the above */
    uchar reserved;
};

/**
 * @brief Extended entity data not of the subset which is saved to disk.
 */
struct extentity : entity
{
    int flags;
    extentity *attached;

    extentity();

    bool spawned() const;
    void setspawned(bool val);
    void setspawned();
    void clearspawned();
};

/**
 * @brief A base object representing a class
 *
 * Intended to be specialized for game specific functionality.
 */
struct physent
{
    vec o, vel, falling;                        /**< origin, velocity */
    vec deltapos, newpos;                       /**< movement interpolation */
    float yaw, pitch, roll;
    float maxspeed;                             /**< cubes per second, 50 for player */
    int timeinair;
    float radius, eyeheight, maxheight, aboveeye; /**< bounding box size */
    float xradius, yradius, zmargin;
    vec floor;                                  /**< the normal of floor the dynent is on */

    int inwater;
    bool jumping;

    // The velocity type: `move` is movement along camera axis while `strafe` is
    // side-to-side movement perpendicular to camera axis.
    char move, strafe, crouching;

    uchar physstate;                            /**< one of PHYS_* above */
    uchar state, editstate;                     /**< one of CS_* above */
    uchar type;                                 /**< one of ENT_* above */
    uchar collidetype;                          /**< one of COLLIDE_* above */

    bool blocked;                               /**< used by physics to signal ai */

    physent();

    void resetinterp();
    void reset();
    vec feetpos(float offset = 0) const;
    vec headpos(float offset = 0) const;
    bool crouched() const;
};

/**
 * @brief An representation of a model attachment (e.g. weapon to player).
 */
struct modelattach final
{
    const char *tag, *name;
    int anim, basetime;
    vec *pos;
    model *m;

    modelattach();
    modelattach(const char *tag, const char *name, int anim = -1, int basetime = 0);
    modelattach(const char *tag, vec *pos);
};

/**
 * @brief description of a character's animation
 */
struct animinfo final
{
    int anim, frame, range, basetime;
    float speed;
    uint varseed;

    animinfo();
    bool operator==(const animinfo &o) const;
    bool operator!=(const animinfo &o) const;
};

/**
 * @brief used for animation blending of animated characters
 */
struct animinterpinfo final
{
    animinfo prev, cur;
    int lastswitch;
    void *lastmodel;

    animinterpinfo();
    void reset();
};

/**
 * An extension of dynent with mobile/animated model handling
 */
struct dynent : physent
{
    bool k_left,    /**< whether keyboard left movement is enabled */
         k_right,   /**< whether keyboard right movement is enabled */
         k_up,      /**< whether keyboard up movement is enabled */
         k_down;    /**< whether keyboard down movement is enabled */

    animinterpinfo animinterp[maxanimparts];
    ragdolldata *ragdoll;
    occludequery *query;
    int lastrendered;

    dynent();
    ~dynent();

    void stopmoving();
    void reset();
    vec abovehead();
};

#endif /* ENTS_H_ */
