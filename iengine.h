/**
 * @file iengine.h
 * @brief The interface the game uses to access the engine.
 *
 * The file contains functions which comprise the API for writing games. This
 * header file is not called by the game internally but should be included in
 * the game code to expose functions to the game.
 */

#ifndef IENGINE_H_
#define IENGINE_H_

extern uint totalsecs;
extern int gamespeed, paused;
extern vector<int> entgroup;

extern int worldscale, worldsize;


extern bool settexture(const char *name, int clamp = 0);
extern int xtraverts, xtravertsva;
extern SDL_Window *screen;

//forward object declarations (used in some functions below)
struct DecalSlot;
struct VSlot;
struct Texture;
struct editinfo;
class model;
struct undolist;
struct undoblock;
struct vslotmap;
struct prefab;

// ragdoll

 /**
  * @brief Update a ragdoll's position and physics.
  *
  * If the dynent pointed to does not have a valid ragdoll object as a member,
  * nothing happens.
  * @param d a dynent pointer.
  */
extern void moveragdoll(dynent *d);

/**
 * @brief Deletes the ragdoll associated with the passed dynent.
 *
 * The function does nothinf if the dynent does not point to a ragdoll object.
 * Also, the function does nothing if the dynent is a ragdoll that is not
 * present.
 * @param d a dynent pointer.
 */
extern void cleanragdoll(dynent *d);

// crypto

/**
 * @brief Creates a private-public key pair from a given seed value.
 *
 * The function takes a reference to the desired private and public keys, and
 * then modifies the contents of both strings using the seed. The modified
 * strings are the matching private-public key pair.
 * @param seed The seed value for generating random private-public key pairs.
 * @param privstr The private string to modify.
 * @param pubstr The public string to modify.
 */
extern void genprivkey(const char *seed, vector<char> &privstr, vector<char> &pubstr);

/**
 * @brief Verify a public key against a private key.
 *
 * Verify that the given public key and the given private key make a matching pair.
 * @param privstr The private key that was generated as part of a private-public key pair.
 * @param pubstr The public key that was generated as part of a private-public key pair.
 * @return true If the strings match and make a private-public key pair.
 * @return false If the strings do not match and do not make a private-public key pair.
 */
extern bool calcpubkey(const char *privstr, vector<char> &pubstr);
extern bool hashstring(const char *str, char *result, int maxlen);
extern void answerchallenge(const char *privstr, const char *challenge, vector<char> &answerstr);

/*==============================================================================*\
 * Interface Functions & Values                                                 *
 *                                                                              *
 * Input, Scripting, Sound, UI                                                  *
 *                                                                              *
 * command.cpp                                                                  *
 * console.cpp                                                                  *
 * control.cpp                                                                  *
 * input.cpp                                                                    *
 * menus.cpp                                                                    *
 * sound.cpp                                                                    *
 * ui.cpp                                                                       *
\*==============================================================================*/

// command

/* see command.h */

// console

extern void conoutf(const char *s, ...) PRINTFARGS(1, 2);
extern void conoutf(int type, const char *s, ...) PRINTFARGS(2, 3);
extern void clear_console();

// control
extern int curtime;                     // current frame time
extern int lastmillis;                  // last time
extern int elapsedtime;                 // elapsed frame time
extern int totalmillis;                 // total elapsed time
extern FILE *logfile;

FILE *getlogfile();
void logoutf(const char *fmt, ...);

extern dynent *player;
extern bool inbetweenframes,
            renderedframe;

extern bool initsdl();

/**
 * @brief Immediately shuts down the game and prints error message(s)
 *
 * Shuts down SDL and closes the game immediately. Up to two error messages are
 * printed in a SDL message box after exiting the game.
 *
 * @param s a series of C strings corresponding to error messages
 */
extern void fatal(const char *s, ...) PRINTFARGS(1, 2);
extern int getclockmillis();
extern int initing;
extern int scr_w, scr_h;
extern int desktopw, desktoph;

/**
 * @brief Returns the engine library's build date.
 *
 * Returns the value of __DATE__ macro at the time the engine was built; this
 * value is in the format MMM DD YYYY (e.g. Jan 01 1970). The purpose of the
 * function is to quickly disambiguate how old a (development) build of the engine
 * is.
 *
 * @return a std::string corresponding to the engine's build date
 */
extern std::string enginebuilddate();

/**
 * @brief Returns the engine library's name string
 *
 * Returns the version string for the engine library, which is a compiled-in
 * constant inside the engine library. The purpose of the function is to quickly
 * disambiguate different versions of the library.
 *
 * @return a std::string corresponding to the engine's name string
 */
extern std::string enginestr();

// input

extern bool grabinput, minimized;

extern bool interceptkey(int sym);
extern void inputgrab(bool on);
extern void checkinput();
extern void ignoremousemotion();
extern void keyrepeat(bool on, int mask = ~0);


// menus

extern void menuprocess();
extern int mainmenu;

// sound

extern int playsound(int n, const vec *loc = nullptr, extentity *ent = nullptr, int flags = 0, int loops = 0, int fade = 0, int chanid = -1, int radius = 0, int expire = -1);
extern int playsoundname(const char *s, const vec *loc = nullptr, int vol = 0, int flags = 0, int loops = 0, int fade = 0, int chanid = -1, int radius = 0, int expire = -1);
extern void preloadsound(int n);
extern void preloadmapsound(int n);
extern void preloadmapsounds();
extern bool stopsound(int n, int chanid, int fade = 0);
extern void stopsounds();
extern void initsound();
extern void updatesounds();
extern void clear_sound();


// UI

namespace UI
{
    /**
     * @brief Queries UI windows' cursor input ability.
     *
     * Returns whether the UI system is accepting cursor input; if any windows in
     * the UI system accept cursor input, returns True.
     *
     * @return true if any UI window is accepting cursor input
     * @return false if no windows exist that accept cursor input
     */
    bool hascursor();

    /**
     * @brief Queries current UI cursor location.
     *
     * Returns to the parameters passed by reference the location of the UI cursor.
     * This location ranges from 0 to 1.f in x and y, with 0,0 being the bottom
     * left part of the screen, and 0.5 0.5 being the middle of the screen.
     * The x and y parameters do not scale by aspect ratio, and 1,1 is always the
     * top right corner of the screen
     *
     * @param x variable to assign x-coordinate of the cursor's location to
     * @param y variable to assign y-coordinate of the cursor's location to
     */
    void getcursorpos(float &x, float &y);

    /**
     * @brief Sets the cursor to the middle of the screen.
     *
     * Sets the location of the UI cursor to the middle of the screen; that is,
     * the cursor's x and y values are both set to 0.5.
     */
    void resetcursor();

    /**
     * @brief Moves the UI cursor by the amount perscribed.
     *
     * Moves the UI cursor by the amount described (positive or negative), scaled
     * by the UI sensitivity variable. Values are capped in both dimensions between
     * 0 and 1, thereby preventing the cursor from completely leaving the screen.
     *
     * @param dx amount to move the cursor in the x direction
     * @param dy amount to move the cursor in the y direction
     */
    bool movecursor(int dx, int dy);
    bool keypress(int code, bool isdown);
    bool textinput(const char *str, int len);
    float abovehud();

    void setup();
    void update();
    void render();
    void cleanup();

    bool showui(const char *name);
    bool hideui(const char *name);
    bool toggleui(const char *name);
    void holdui(const char *name, bool on);
    bool uivisible(const char *name);
}


extern void addchange(const char *desc, int type);
/*==============================================================================*\
 * Render Functions & Values                                                    *
 *                                                                              *
 * World, Model, Material, Screenspace FX Rendering                             *
 *                                                                              *
 * aa.cpp                                                                       *
 * grass.cpp                                                                    *
 * normal.cpp                                                                   *
 * octarender.cpp                                                               *
 * radiancehints.cpp                                                            *
 * rendergl.cpp                                                                 *
 * renderlights.cpp                                                             *
 * rendermodel.cpp                                                              *
 * renderparticles.cpp                                                          *
 * rendersky.cpp                                                                *
 * rendertext.cpp                                                               *
 * renderva.cpp                                                                 *
 * renderwindow.cpp                                                             *
 * shader.cpp                                                                   *
 * stain.cpp                                                                    *
 * texture.cpp                                                                  *
 * water.cpp                                                                    *
\*==============================================================================*/

// octarender

extern void allchanged(bool load = false);

// rendergl

extern physent *camera1; /**< Camera location for the renderer to render at */
extern vec worldpos, camdir, camright, camup;
extern float curfov, fovy, aspect;
extern bool detachedcamera; /**< read only variable corresponding to camera at ent location (1p) or away from it (3p) */

extern void disablezoom();

extern vec calcavatarpos(const vec &pos, float dist);
extern vec calcmodelpreviewpos(const vec &radius, float &yaw);

extern void damageblend(int n);
extern void damagecompass(int n, const vec &loc);

extern vec minimapcenter, minimapradius, minimapscale;
extern void bindminimap();

extern matrix4 hudmatrix;
extern void resethudmatrix();
extern void pushhudmatrix();
extern void flushhudmatrix(bool flushparams = true);
extern void pophudmatrix(bool flush = true, bool flushparams = true);
extern void pushhudscale(float sx, float sy = 0);
extern void resethudshader();
extern void recomputecamera();
extern void initgbuffer();
extern void computezoom();
extern void enablepolygonoffset(GLenum type);
extern void disablepolygonoffset(GLenum type);

/**
 * @brief Returns 2 Euler angles for a given vector.
 *
 * Given a vector v, yaw and pitch are assigned to the orientation of the vector
 * in 3D space. Roll is not assigned, as it is ambiguous for a vector.
 *
 * @param v     vector to be used to calc yaw & pitch
 * @param yaw   output parameter v's yaw is assigned to
 * @param pitch output parameter v's pitch is assigned to
 */
extern void vectoryawpitch(const vec &v, float &yaw, float &pitch);

/**
 * @brief Ensures computer environment can run the engine.
 *
 * This function checks for the OpenGL contexts, GLSL version information, and
 * other graphics capabilities required to run the engine. This function will
 * call fatal() and exit the game if any of its conditions are not met.
 */
extern void gl_checkextensions();
extern void gl_init();
extern void gl_resize();
extern void gl_setupframe(bool force = false);
extern void gl_drawframe(int crosshairindex, void (*gamefxn)(), void (*hudfxn)(), void (*editfxn)(), void (*hud2d)());
extern void drawminimap(int yaw, int pitch, vec loc);

// renderlights

extern void clearshadowcache();

extern int spotlights; /**< Toggles spotlights, behaves like bool; 0 for no spotlights and 1 for spotlights */
extern int volumetriclights;
extern int nospeclights;

// rendermodel
extern int numanims;
extern std::vector<std::string> animnames;

extern void rendermodel(const char *mdl, int anim, const vec &o, float yaw = 0, float pitch = 0, float roll = 0, int cull = Model_CullVFC | Model_CullDist | Model_CullOccluded, dynent *d = nullptr, modelattach *a = nullptr, int basetime = 0, int basetime2 = 0, float size = 1, const vec4 &color = vec4(1, 1, 1, 1));
extern int intersectmodel(const char *mdl, int anim, const vec &pos, float yaw, float pitch, float roll, const vec &o, const vec &ray, float &dist, int mode = 0, dynent *d = nullptr, modelattach *a = nullptr, int basetime = 0, int basetime2 = 0, float size = 1);
extern void abovemodel(vec &o, const char *mdl);
extern void renderclient(dynent *d, const char *mdlname, modelattach *attachments, int hold, int attack, int attackdelay, int lastaction, int lastpain, float scale = 1, bool ragdoll = false, float trans = 1);
extern void interpolateorientation(dynent *d, float &interpyaw, float &interppitch);
extern void setbbfrommodel(dynent *d, const char *mdl);

/**
 * @brief Returns the mapmodel name at the given index.
 *
 * If the passed index corresponds to a valid mapmodel, the function returns a C
 * string with its name; if not, the function returns a nullptr.
 *
 * @return a C string with the name of the model
 */
extern const char *mapmodelname(int i);

/**
 * @brief Adds a model to the cache of models to load.
 *
 * When flushpreloadedmodels() is called, the entries added to the preloadedmodels
 * vector by this function are loaded. Silently fails and does nothing if the model
 * name passed is not valid or already exists in the vector.
 */
extern void preloadmodel(const char *name);

/**
 * @brief Loads the mapmodel assigned to the specified index.
 *
 * Loads the mapmodel assigned to the specified model index into a model object.
 * If a model object already exists, a new model is not created and a pointer to
 * that model is returned instead; otherwise, returns a pointer to the new model
 * object.
 * If the index is out of bounds, nullptr is returned.
 *
 * @return a pointer to the model object
 */
extern model *loadmapmodel(int n);
extern model *loadmodel(const char *name, int i = -1, bool msg = false);
extern void flushpreloadedmodels(bool msg = true);
extern void preloadusedmapmodels(bool msg = false, bool bih = false);
extern void clear_models();

// renderparticles

extern std::vector<std::string> entnames;

/**
 * @brief Returns the entity name of of a given entity type index.
 *
 * For the given entity type index (light, model, etc.) returns the
 * human readable name of the entity type.
 * Returns an empty string if index out of bounds.
 *
 * @return a C string corresponding to the ent type's name
 */
extern const char * getentname(int i);
extern void regular_particle_splash(int type, int num, int fade, const vec &p, int color = 0xFFFFFF, float size = 1.0f, int radius = 150, int gravity = 2, int delay = 0);
extern void regular_particle_flame(int type, const vec &p, float radius, float height, int color, int density = 3, float scale = 2.0f, float speed = 200.0f, float fade = 600.0f, int gravity = -15);
extern void particle_splash(int type, int num, int fade, const vec &p, int color = 0xFFFFFF, float size = 1.0f, int radius = 150, int gravity = 2);
extern void particle_trail(int type, int fade, const vec &from, const vec &to, int color = 0xFFFFFF, float size = 1.0f, int gravity = 20);
extern void particle_text(const vec &s, const char *t, int type, int fade = 2000, int color = 0xFFFFFF, float size = 2.0f, int gravity = 0);
extern void particle_textcopy(const vec &s, const char *t, int type, int fade = 2000, int color = 0xFFFFFF, float size = 2.0f, int gravity = 0);
extern void particle_icon(const vec &s, int ix, int iy, int type, int fade = 2000, int color = 0xFFFFFF, float size = 2.0f, int gravity = 0);
extern void particle_meter(const vec &s, float val, int type, int fade = 1, int color = 0xFFFFFF, int color2 = 0xFFFFF, float size = 2.0f);
extern void particle_flare(const vec &p, const vec &dest, int fade, int type, int color = 0xFFFFFF, float size = 0.28f, physent *owner = nullptr);
extern void particle_fireball(const vec &dest, float max, int type, int fade = -1, int color = 0xFFFFFF, float size = 4.0f);
extern void removetrackedparticles(physent *owner = nullptr);

/**
 * @brief Loads the shaders/renderer objects to render particles.
 *
 * Loads a series of particle related shader (particle, particlenotexture,
 * particlesoft, particletext) as well as initializing particlerenderer objects
 * as necessary to begin particle rendering.
 */
extern void initparticles();
extern void updateparticles();

// rendertext

extern void draw_text(const char *str, float left, float top, int r = 255, int g = 255, int b = 255, int a = 255, int cursor = -1, int maxwidth = -1);
extern void draw_textf(const char *fstr, float left, float top, ...) PRINTFARGS(1, 4);
extern void text_boundsf(const char *str, float &width, float &height, int maxwidth = -1);

extern bool setfont(const char *name);

// renderwindow

extern void swapbuffers(bool overlay = true);
extern int fullscreen; /**< Toggles fullscreen, behaves like bool; 0 for windowed and 1 for fullscreen */
extern void setupscreen();
extern void restoregamma();
extern void restorevsync();
extern void resetfpshistory();
extern void limitfps(int &millis, int curmillis);

/**
 * @brief Delays rendering of a frame.
 *
 * Uses SDL_Delay to delay a frame, given the time the last frame was
 * rendered and the current time. The amount of delay is calculated from the
 * global maxfps and menufps variables.
 *
 * @param millis the time (in ms) since program started
 * @param curmillis the last registered frame time
 */
extern void updatefpshistory(int millis);
extern void cleargamma();

extern void renderbackground(const char *caption = nullptr, Texture *mapshot = nullptr, const char *mapname = nullptr, const char *mapinfo = nullptr, bool force = false);
extern void renderprogress(float bar, const char *text, bool background = false);

// shader

extern void loadshaders();

// stain
extern void initstains();
extern void addstain(int type, const vec &center, const vec &surface, float radius, const bvec &color = bvec(0xFF, 0xFF, 0xFF), int info = 0);

inline void addstain(int type, const vec &center, const vec &surface, float radius, int color, int info = 0)
{
    addstain(type, center, surface, radius, bvec::hexcolor(color), info);
}

// texture

extern Texture *notexture;

extern Texture *textureload(const char *name, int clamp = 0, bool mipit = true, bool msg = true);
extern void packvslot(vector<uchar> &buf, int index);
extern void packvslot(vector<uchar> &buf, const VSlot *vs);

/*==============================================================================*\
 * World Functions & Values                                                     *
 *                                                                              *
 * World Loading, Physics, Collision, Entity Handling                           *
 *                                                                              *
 * bih.cpp                                                                      *
 * dynlight.cpp                                                                 *
 * light.cpp                                                                    *
 * material.cpp                                                                 *
 * octa.cpp                                                                     *
 * octaedit.cpp                                                                 *
 * physics.cpp                                                                  *
 * raycube.cpp                                                                  *
 * world.cpp                                                                    *
 * worldio.cpp                                                                  *
\*==============================================================================*/

// dynlight

extern void adddynlight(const vec &o, float radius, const vec &color, int fade = 0, int peak = 0, int flags = 0, float initradius = 0, const vec &initcolor = vec(0, 0, 0), physent *owner = nullptr, const vec &dir = vec(0, 0, 0), int spot = 0);
extern void removetrackeddynlights(physent *owner = nullptr);

// light

extern void clearlightcache(int id = -1);

/**
 * @brief Calculates normals and re-calculates geometry.
 *
 * Re-mips the cube structure of the level to remove unnecessary nodes, then
 * calculates normal maps for corners.
 */
extern void calclight();

// material
extern int showmat;

/**
 * @brief Returns the bitmask for the given material name.
 *
 * Given a material name (water, clip, noclip, etc.), returns the bitmask value
 * the material is associated with in the octree world.
 *
 * @param name a C string with the name of the material to look up
 *
 * @return an integer corresponding to the bitmask value of the material
 */
extern int findmaterial(const char *name);

// octa
extern ivec lu;
extern int lusize;

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
extern int lookupmaterial(const vec &o);
extern void freeocta(cube *c);
extern void getcubevector(cube &c, int d, int x, int y, int z, ivec &p);
extern void remip();
extern void optiface(uchar *p, cube &c);
extern bool isvalidcube(const cube &c);
extern cube &lookupcube(const ivec &to, int tsize = 0, ivec &ro = lu, int &rsize = lusize);

// octaedit

namespace hmap
{
    extern bool isheightmap(int o, int d, bool empty, cube *c);
    extern void run(int dir, int mode);
}

extern bool allowediting;
extern bool multiplayer;
extern editinfo *localedit;
extern selinfo sel;
extern std::vector<ushort> texmru;
extern selinfo repsel;
extern int reptex;
extern int lasttex;
extern int lasttexmillis;
extern int curtexindex;
extern int moving, orient, horient;
extern bool editmode;
extern int entmoving;
extern int entediting;
extern selinfo sel, lastsel, savedsel;
extern vector<vslotmap> unpackingvslots;
extern std::vector<vslotmap> remappedvslots;
extern undolist undos, redos;
extern int nompedit;
extern int hmapedit;
extern bool havesel;
extern std::vector<editinfo *> editinfos;
extern int texpaneltimer; /**< sets the length before the texture scroll panel turns off */
extern hashnameset<prefab> prefabs;
extern ivec cor, cur, lastcur, lastcor;
extern int gridsize;
extern int passthroughsel;
extern int outline;
extern int dragging;
extern int selectcorners;
extern bool boxoutline;

extern void boxs3D(const vec &o, vec s, int g);
extern void boxs(int orient, vec o, const vec &s);
extern void boxsgrid(int orient, vec o, vec s, int g);
extern bool editmoveplane(const vec &o, const vec &ray, int d, float off, vec &handle, vec &dest, bool first);
extern void countselchild(cube *c, const ivec &cor, int size);
extern void normalizelookupcube(const ivec &o);
extern void updateselection();

extern int shouldpacktex(int index);
extern bool packeditinfo(editinfo *e, int &inlen, uchar *&outbuf, int &outlen);
extern bool unpackeditinfo(editinfo *&e, const uchar *inbuf, int inlen, int outlen);
extern void freeeditinfo(editinfo *&e);
extern bool packundo(int op, int &inlen, uchar *&outbuf, int &outlen);
extern bool unpackundo(const uchar *inbuf, int inlen, int outlen);
extern bool noedit(bool view = false, bool msg = true);
extern void commitchanges(bool force = false);

/**
 * @brief Determines whether selection contains location at specified location.
 *
 * Returns whether the location of the vector passed falls within the selection
 * information object's volume. If the passed vector is exactly along the boundary,
 * the function still returns true.
 *
 * @param sel selinfo object corresponding to a world volume
 * @param origin vector corresponding to a world location
 *
 * @return true if the vector is inside the selinfo object
 * @return false if the vector is outside the selinfo object
 */
extern bool pointinsel(const selinfo &sel, const vec &o);

/**
 * @brief Deselects edit selection.
 *
 * This function deselects the cubes and entities currently selected.
 */
extern void cancelsel();
extern void addundo(undoblock *u);
extern cube &blockcube(int x, int y, int z, const block3 &b, int rgrid);
extern void changed(const ivec &bbmin, const ivec &bbmax, bool commit);
extern void changed(const block3 &sel, bool commit = true);
extern void discardchildren(cube &c, bool fixtex = false, int depth = 0);
extern void makeundo(selinfo &s);
extern void makeundo();
extern void reorient();
extern int countblock(block3 *b);
extern bool hmapsel;
extern void forcenextundo();
extern void freeundo(undoblock *u);
extern void pasteblock(block3 &b, selinfo &sel, bool local);
extern void pasteundo(undoblock *u);
extern undoblock *newundocube(const selinfo &s);
extern void remapvslots(cube &c, bool delta, const VSlot &ds, int orient, bool &findrep, VSlot *&findedit);
extern void setmat(cube &c, ushort mat, ushort matmask, ushort filtermat, ushort filtermask, int filtergeom);
extern void edittexcube(cube &c, int tex, int orient, bool &findrep);
extern bool haveselent();
extern void pastecube(const cube &src, cube &dst);
extern void pasteundoblock(block3 *b, uchar *g);
extern bool uncompresseditinfo(const uchar *inbuf, int inlen, uchar *&outbuf, int &outlen);
extern void unpackundocube(ucharbuf buf, uchar *outbuf);

/**
 * @brief Prints a warning message to the console.
 *
 * This function prints a message warning the user that the operation is not
 * possible in multiplayer. It does nothing else.
 */
extern void multiplayerwarn();

// raycube

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
extern float raycube   (const vec &o, const vec &ray, float radius = 0, int mode = Ray_ClipMat, int size = 0, extentity *t = 0);

/**
 * @brief Returns the distance before a ray hits a cube.
 *
 * This function behaves the same as raycube() execept also returning the location
 * where the hit happened.
 *
 * @param o      the starting location of the ray to be drawn
 * @param ray    normalized direction vector for the ray to point
 * @param hit    output parameter for the vector location the hit happened at
 * @param radius region around ray that counts as a hit
 * @param mode   flags which determine what counts as a hit
 * @param size   size of cube which registers a hit
 * @param t      entity to check against
 */
extern float raycubepos(const vec &o, const vec &ray, vec &hit, float radius = 0, int mode = Ray_ClipMat, int size = 0);
extern float rayfloor  (const vec &o, vec &floor, int mode = 0, float radius = 0);
extern bool  raycubelos(const vec &o, const vec &dest, vec &hitpos);
extern float rayent(const vec &o, const vec &ray, float radius, int mode, int size, int &orient, int &ent);

/**
 * @brief Queries whether the given vector lies inside the octree world.
 *
 * Returns truth value for whether the vector given has coordinates which lie
 * within the space occupied by the worldroot cube. If the dimension passed is
 * exactly equal to a boundary, the function returns true (this is considered as
 * being inside the world).
 *
 * @return true if the vector is inside the world boundaries
 * @return false if the vector is outside the world
 *
 */
extern bool insideworld(const vec &o);

// physics

extern int floatspeed;
extern const float slopez, wallz, floorz, stairheight;
extern vec collidewall;
extern int collideinside;
extern physent *collideplayer;
extern const float gravity;
extern int numdynents;
extern vector<dynent *> dynents;

extern dynent *iterdynents(int i);
extern void crouchplayer(physent *pl, int moveres, bool local);
extern bool collide(physent *d, const vec &dir = vec(0, 0, 0), float cutoff = 0.0f, bool playercol = true, bool insideplayercol = false);
extern bool bounce(physent *d, float secs, float elasticity, float waterfric, float grav);
extern void avoidcollision(physent *d, const vec &dir, physent *obstacle, float space);
extern bool movecamera(physent *pl, const vec &dir, float dist, float stepdist);
extern void dropenttofloor(entity *e);
extern bool droptofloor(vec &o, float radius, float height);
extern void rotatebb(vec &center, vec &radius, int yaw, int pitch, int roll = 0);

extern void vecfromyawpitch(float yaw, float pitch, int move, int strafe, vec &m);
extern void updatephysstate(physent *d);
extern void cleardynentcache();
extern void updatedynentcache(physent *d);
extern bool entinmap(dynent *d, bool avoidplayers = false);
extern void findplayerspawn(dynent *d, int forceent = -1, int tag = 0);

extern void modifyvelocity(physent *pl, bool local, bool water, bool floating, int curtime);
extern void recalcdir(physent *d, const vec &oldvel, vec &dir);
extern void slideagainst(physent *d, vec &dir, const vec &obstacle, bool foundfloor, bool slidecollide);
extern void freeblock(block3 *b, bool alloced = true);
extern block3 *blockcopy(const block3 &s, int rgrid);

/**
 * @brief Returns the material bitmask for the given cube.
 *
 * Returns the bitmask for the cube passed, which corresponds to the sum of all
 * materials occupying the cube. Every sum is unique due to the basis selection,
 * and therefore any combination of materials can be represented in the output.
 * Returns 0 (air) if there is no valid material that describes the entire cube.
 *
 * @return An unsigned short integer corresponding to the material bitmask
 */
extern ushort getmaterial(cube &c);

// world

extern int octaentsize;

/**
 * @brief Grows the map to an additional gridsize.
 *
 * Expands the map by placing the old map in the corner nearest the origin and
 * adding 7 old-map sized cubes to create a new largest cube.
 *
 * This moves the worldroot cube to the new parent cube of the old map.
 */
extern bool emptymap(int factor, bool force, bool usecfg = true);
extern bool enlargemap(bool force);
extern vec getselpos();

/**
 * @brief Returns the size of the world.
 *
 * This function returns the size of the currently loaded world in terms of gridpower
 * 0 cubes (the smallest possible size). A size 10 map will be 2^10 = 1024 cubes
 * in size.
 *
 * @return the size of the world, in size 0 cubes
 */
extern int getworldsize();

/**
 * @brief Cancels selection of selected entities.
 *
 * Clears the selection of any entities in edit mode. Has no effect if no entities
 * are selected.
 */
extern void entcancel();

/**
 * @brief Attempts to attach the given entity to another entity.
 *
 * Attempts to attach the entity passed to another entity that it can be attached
 * to, as long as it is within the attach radius.
 *
 * Fails silently if the given entity has no close by entities to link to or the
 * entity type is ineligible to be linked
 *
 * @param e the extentity to attempt to link
 */
extern void attachentity(extentity &e);

/**
 * @brief Attempts to attach all entities in selection.
 *
 * For all eligible entities in the current selection, the engine attempts to link
 * it with its nearest entity it can link with; silently fails and does nothing if
 * no entities in selection are eligible or selected.
 */
extern void attachentities();
extern void removeentityedit(int id);
extern void addentityedit(int id);

/**
 * @brief Attempts to dettach the given entity.
 *
 * Attempts to detach the entity from any entity it may be attached to. Fails
 * silently if the entity is not attached.
 *
 * @param e the extentity to attempt to de-link
 */
extern void detachentity(extentity &e);
extern void entselectionbox(const entity &e, vec &eo, vec &es);
extern void mmboundbox(const entity &e, model *m, vec &center, vec &radius);
extern float getdecalslotdepth(DecalSlot &s);
extern void entitiesinoctanodes();


namespace entities
{
    extern vector<extentity *> ents;

    extern extentity *newentity();
    extern void deleteentity(extentity *e);
}

// worldio

extern string clientmap;

extern bool load_world(const char *mname, const char *gameident, const char *gameinfo = nullptr, const char *cname = nullptr);
extern bool save_world(const char *mname, const char *gameident);

/**
 * @brief Changes the passed string to conform to valid map names.
 *
 * This function modifies the input parameter, passed by pointer, to comply with
 * the requirements for Cube map names. This ensures it can be rendered with the
 * engine's text library.
 *
 * @param name The name of the string to modify to compliance
 */
extern void fixmapname(char *name);

/**
 * @brief Returns the CRC code for the map currently loaded.
 *
 * Returns the cyclic redundancy checksum for the file currently loaded. This
 * value is unique for every revision of a map binary, and is useful to make
 * sure multiple clients have the same binary.
 *
 * @return the cyclic redundancy code of the map file currently loaded
 */
extern uint getmapcrc();
extern void clearmapcrc();
extern bool loadents(const char *fname, const char *gameident, vector<entity> &ents, uint *crc = nullptr);

#endif /* IENGINE_H_ */
