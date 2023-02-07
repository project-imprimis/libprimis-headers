/**
 * @file command.h
 * @brief Script binding functionality.
 *
 * This file describes the CubeScript API for binding and executing code from within
 * the engine's CubeScript context.
 */

#ifndef COMMAND_H_
#define COMMAND_H_

enum
{
    Value_Null = 0,
    Value_Integer,
    Value_Float,
    Value_String,
    Value_Any,
    Value_Code,
    Value_Macro,
    Value_Ident,
    Value_CString,
    Value_CAny,
    Value_Word,
    Value_Pop,
    Value_Cond,
};

enum
{
    Id_Var,     //1
    Id_FloatVar,
    Id_StringVar,
    Id_Command,
    Id_Alias,   //5
    Id_Local,
    Id_Do,
    Id_DoArgs,
    Id_If,
    Id_Result,  //10
    Id_Not,
    Id_And,
    Id_Or,      //13
};

enum
{
    Idf_Persist    = 1<<0,
    Idf_Override   = 1<<1,
    Idf_Hex        = 1<<2,
    Idf_ReadOnly   = 1<<3,
    Idf_Overridden = 1<<4,
    Idf_Unknown    = 1<<5,
    Idf_Arg        = 1<<6,
};

struct ident;

struct identval
{
    union
    {
        int i;      // Id_Var, ValueInteger
        float f;    // Id_FloatVar, ValueFloat
        char *s;    // Id_StringVar, ValueString
        const uint *code; // ValueCode
        ident *id;  // ValueIdent
        const char *cstr; // ValueCString
    };
};

struct tagval : identval
{
    int type;

    void setint(int val);
    void setfloat(float val);
    void setnumber(double val);
    void setstr(char *val);
    void setnull();
    void setcode(const uint *val);
    void setmacro(const uint *val);
    void setcstr(const char *val);
    void setident(ident *val);

    const char *getstr() const;
    int getint() const;
    float getfloat() const;
    double getnumber() const;
    bool getbool() const;
    void getval(tagval &r) const;

    void cleanup();
};

struct identstack
{
    identval val;
    int valtype;
    identstack *next;
};

typedef void (__cdecl *identfun)(ident *id);

/**
 * @brief An object representing all Cubescript objects.
 *
 * This object defines all possible state for any object in Cubescript. Different
 * types will have different members due to the union types, but the ident object
 * is common to them regardless. This allows storage of CS objects in a uniform
 * map consisting of one type.
 */
struct ident
{
    //this pointer will point to different types depending upon the type of variable
    union identvalptr /**< points to an ident's value */
    {
        void  *p; /**< can point to any Id_*Var */
        int   *i; /**< points to an Id_Var (int) */
        float *f; /**< points to an Id_FloatVar */
        char **s; /**< points to a pointer to a Id_StringVar (C string) */
    };

    uchar type; /**< one of Id_* in Id_ enum */
    union
    {
        uchar valtype; /**< if alias, points to Id_Alias's type */
        uchar numargs; /**< if command, number of commands the Id_Command has */
    };
    ushort flags;
    int index;
    const char *name;
    union
    {
        struct // Id_Var, Id_FloatVar, Id_StringVar
        {
            union //number var range union type
            {
                struct
                {
                    int minval, /**< if an int variable, its min allowed value*/
                        maxval; /**< if an int variable, its max allowed value*/
                };     // Id_Var
                struct
                {
                    float minvalf, /**< if a float variable, its min allowed value*/
                          maxvalf; /**< if a float variable, its max allowed value*/
                }; // Id_FloatVar
            };
            identvalptr storage;
            identval overrideval;
        };
        struct // Id_Alias
        {
            uint *code;
            identval val;
            identstack *stack;
        };
        struct // Id_Command
        {
            const char *args;
            uint argmask;
        };
    };
    identfun fun; /**< the pointer a command or variable points to (the on-change command for a var)*/

    ident() {}
    /**
     * @brief Constructor for an ident for an int variable.
     */
    ident(int t, const char *n, int m, int x, int *s, void *f = nullptr, int flags = 0)
        : type(t), flags(flags | (m > x ? Idf_ReadOnly : 0)), name(n), minval(m), maxval(x), fun((identfun)f)
    { storage.i = s; }

    /**
     * @brief Constructor for an ident for a float variable.
     */
    ident(int t, const char *n, float m, float x, float *s, void *f = nullptr, int flags = 0)
        : type(t), flags(flags | (m > x ? Idf_ReadOnly : 0)), name(n), minvalf(m), maxvalf(x), fun((identfun)f)
    { storage.f = s; }

    /**
     * @brief Constructor for an ident for a string variable.
     */
    ident(int t, const char *n, char **s, void *f = nullptr, int flags = 0)
        : type(t), flags(flags), name(n), fun((identfun)f)
    { storage.s = s; }

    // Id_Alias
    ident(int t, const char *n, char *a, int flags)
        : type(t), valtype(Value_String), flags(flags), name(n), code(nullptr), stack(nullptr)
    { val.s = a; }

    ident(int t, const char *n, int a, int flags)
        : type(t), valtype(Value_Integer), flags(flags), name(n), code(nullptr), stack(nullptr)
    { val.i = a; }

    ident(int t, const char *n, float a, int flags)
        : type(t), valtype(Value_Float), flags(flags), name(n), code(nullptr), stack(nullptr)
    { val.f = a; }

    ident(int t, const char *n, int flags)
        : type(t), valtype(Value_Null), flags(flags), name(n), code(nullptr), stack(nullptr)
    {}
    ident(int t, const char *n, const tagval &v, int flags)
        : type(t), valtype(v.type), flags(flags), name(n), code(nullptr), stack(nullptr)
    { val = v; }

    /**
     * @brief Constructor for an ident for a C++ bound command.
     */
    ident(int t, const char *n, const char *args, uint argmask, int numargs, void *f = nullptr, int flags = 0)
        : type(t), numargs(numargs), flags(flags), name(n), args(args), argmask(argmask), fun((identfun)f)
    {}

    void changed()
    {
        if(fun)
        {
            fun(this);
        }
    }

    void setval(const tagval &v)
    {
        valtype = v.type;
        val = v;
    }

    void setval(const identstack &v)
    {
        valtype = v.valtype;
        val = v.val;
    }

    void forcenull()
    {
        if(valtype==Value_String)
        {
            delete[] val.s;
        }
        valtype = Value_Null;
    }

    float getfloat() const;
    int getint() const;
    double getnumber() const;
    const char *getstr() const;
    void getval(tagval &r) const;
    void getcstr(tagval &v) const;
    void getcval(tagval &v) const;
};

/**
 * @brief Returns an integer value from a Cubescript command.
 *
 * When writing a CS command, this function returns an integer value to the inheriting
 * CS environment.
 *
 * @param v the value to return
 */
extern void intret(int v);

extern const char *floatstr(float v);

/**
 * @brief Returns a float value from a Cubescript command.
 *
 * When writing a CS command, this function returns a float value to the inheriting
 * CS environment.
 *
 * @param v the value to return
 */
extern void floatret(float v);

/**
 * @brief Returns a string value from a Cubescript command.
 *
 * When writing a CS command, this function returns a string value to the inheriting
 * CS environment.
 *
 * @param v the value to return
 */
extern void stringret(char *s);

/**
 * @brief Returns an alias' name from a Cubescript command.
 *
 * When writing a CS command, this functions a string value representing the name
 * of a Cubescript object to the inheriting CS environment.
 *
 * @param s the name of the ident to return
 */
extern void result(const char *s);

/**
 * @brief Returns the string passed as an integer.
 *
 * Parses the entire string, returning the value of the passed value as an integer.
 * The value of this value will always be greater than zero unless there is an
 * overflow beyond the size of `int`. The output may be converted by a trailing
 * radix digit as described in `strtoul`.
 *
 * @param s the string to turn into an integer
 */
inline int parseint(const char *s)
{
    return static_cast<int>(strtoul(s, nullptr, 0));
}

/**
 * @brief Registers an int variable in the Cubescript ident table.
 *
 * @param name the name of the aliased variable in Cubescript
 * @param min the minimum value the variable can be set at
 * @param cur the starting value of the variable
 * @param max the maximum value the variable can be set at
 * @param storage the pointer to the variable to be aliased to Cubescript
 * @param fun a function pointer to be called upon modification of the variable
 * @param flags the handling flags for the variable
 *
 * @return the value cur passed
 */
extern int variable(const char *name, int min, int cur, int max, int *storage, identfun fun, int flags);

/**
 * @brief Registers a float variable in the Cubescript ident table.
 *
 * Adds a float variable to the Cubescript ident table. The name of the Cubescript
 * variable does not necessarily need to correspond to the C++ variable's name.
 *
 * @param name the name of the aliased variable in Cubescript
 * @param min the minimum value the variable can be set at
 * @param cur the starting value of the variable
 * @param max the maximum value the variable can be set at
 * @param storage the pointer to the variable to be aliased to Cubescript
 * @param fun a function pointer to be called upon modification of the variable
 * @param flags the handling flags for the variable
 *
 * @return the value cur passed
 */
extern float fvariable(const char *name, float min, float cur, float max, float *storage, identfun fun, int flags);

/**
 * @brief Registers a C string variable in the Cubescript ident table.
 *
 * @param name the name of the aliased variable in Cubescript
 * @param cur the starting value of the variable
 * @param storage the pointer to the pointer to the variable to be aliased to Cubescript
 * @param fun a function pointer to be called upon modification of the variable
 * @param flags the handling flags for the variable
 *
 * @return the value cur passed
 */
extern char *svariable(const char *name, const char *cur, char **storage, identfun fun, int flags);

/**
 * @brief Sets a Cubescript integer value to the given value.
 *
 * @param name the name of the cubescript alias to change
 * @param i the value to set
 * @param dofunc whether to run the onchange function
 * @param doclamp whether to clamp the value to the specified limits
 */
extern void setvar(const char *name, int i, bool dofunc = true, bool doclamp = true);

/**
 * @brief Sets a Cubescript float value to the given value.
 *
 * @param name the name of the cubescript alias to change
 * @param i the value to set
 * @param dofunc whether to run the onchange function
 * @param doclamp whether to clamp the value to the specified limits
 */
extern void setfvar(const char *name, float f, bool dofunc = true, bool doclamp = true);

/**
 * @brief Sets a Cubescript string value to the given value.
 *
 * @param name the name of the cubescript alias to change
 * @param i the value to set
 * @param dofunc whether to run the onchange function
 */
extern void setsvar(const char *name, const char *str, bool dofunc = true);

/**
 * @brief Registers a command in the Cubescript ident table.
 *
 * The arguments of the function passed are cast away, so they are reconstructed using
 * the char * string passed to `narg`.
 *
 * @param name of the command in Cubescript
 * @param fun a function pointer to be called when the command is executed
 * @param narg string containing the arguments of the function
 * @param type the type of the command to create
 */
extern bool addcommand(const char *name, identfun fun, const char *narg = "", int type = Id_Command);

extern std::queue<ident *> triggerqueue; /**< A queue of game events for the engine to process */

/**
 * @brief Returns the pointer to the ident object with the given CS alias.
 *
 * @param the name of the cubescript object to get
 *
 * @return The address of the ident object.
 */
extern ident *getident(const char *name);
extern int execute(const uint *code);

/**
 * @brief Executes the contents of the string passed.
 *
 * Parses and executes the line of Cubescript given.
 *
 * @param the C string containing the code to execute
 *
 * @return an int from the results of the execution
 */
extern int execute(const char *p);
extern int execident(const char *name, int noid = 0, bool lookup = false);
extern bool executebool(const uint *code);

/**
 * @brief Executes the contents of the referenced file.
 *
 * @param cfgfile the relative director of the file to execute
 * @param msg whether to print to console a failure message
 *
 * @return true if the file was successfully loaded
 * @return false if the file was not found
 */
extern bool execfile(const char *cfgfile, bool msg = true);

/**
 * @brief Replaces C style excape characters with Cubescript ones
 *
 * @param s the string to convert
 *
 * @return a string containing the escaped changes
 */
extern const char *escapestring(const char *s);

/**
 * @brief Prints out the formatted variable
 *
 * The param is intended to represent the value the ident object represents.
 *
 * @param id the ident object to print out
 * @param i the value to print out the variable equalling.
 */
extern void printvar(ident *id, int i);

/**
 * @brief Modifies the value passed to fall within the boundaries passed.
 *
 * Clamps the value i to within minval and maxval. If id is a hex variable,
 * issues a warning about clamping. i is a regular free variable and does not
 * necessarily have anything to do with the ident passed.
 *
 * @param id the ident object to check the hex-ness of
 * @param i the value to clamp
 * @param minval the lowest value to clamp to
 * @param maxval the largest value to clamp to
 */
extern int clampvar(ident *id, int i, int minval, int maxval);
extern void loopiter(ident *id, identstack &stack, int i);
extern void loopend(ident *id, identstack &stack);
const char *escapeid(const char *s);
extern void writecfg(const char *savedconfig, const char *autoexec = nullptr, const char *defaultconfig = nullptr, const char *name = nullptr);

/**
 * @brief Processes the cubescript sleep queue.
 *
 * At most, one sleep command is removed from the queue per cycle. Removes
 * queued sleep commands as they expire, which stops CS commands from executing
 * for a certain period of time.
 *
 * @param millis the current timestamp
 */
extern void checksleep(int millis);
extern bool initidents();

extern int identflags;

extern void clear_command();

// nasty macros for registering script functions, abuses globals to avoid excessive infrastructure

//* note: many of the VARF comments are very repetitive, because the code itself is nearly duplicated too *//

/* how command registration works:
 *  In C++, all global variables must be initiated before main() is called
 *  Each of these macro kludges (which are placed at the global scope level) initializes some kind of global variable
 *  Because the macro kludges are the definition of global variables, at program initializiation, they must be run first
 *  The values of the variables themselves don't matter because they only exist to cheat and run before main()
 *  The macro kludges themselves register commands or values within some vector<>-s which keeps track of all cmds/vars
 */

//integer var macros
//VAR_, _VARF, VARM_ are templates for "normal" macros that do not execute an inline function
#define VAR_(name, global, min, cur, max, persist)  int global = variable(#name, min, cur, max, &global, nullptr, persist)
#define VARN(name, global, min, cur, max) VAR_(name, global, min, cur, max, 0)
#define VARNP(name, global, min, cur, max) VAR_(name, global, min, cur, max, Idf_Persist)
#define VARNR(name, global, min, cur, max) VAR_(name, global, min, cur, max, Idf_Override)
#define VAR(name, min, cur, max) VAR_(name, name, min, cur, max, 0)
#define VARP(name, min, cur, max) VAR_(name, name, min, cur, max, Idf_Persist)
#define VARR(name, min, cur, max) VAR_(name, name, min, cur, max, Idf_Override)

//variable with inline function to be executed on change (VARiable with Function = VARF)
#define VARF_(name, global, min, cur, max, body, persist) \
    int global = variable(#name, min, cur, max, &global, [] (ident *) { body; }, persist); /* assign variable, needs fxn prototype declared above */

#define VARFN(name, global, min, cur, max, body) VARF_(name, global, min, cur, max, body, 0)
#define VARF(name, min, cur, max, body) VARF_(name, name, min, cur, max, body, 0)
#define VARFP(name, min, cur, max, body) VARF_(name, name, min, cur, max, body, Idf_Persist)
#define VARFR(name, min, cur, max, body) VARF_(name, name, min, cur, max, body, Idf_Override)
#define VARFNP(name, global, min, cur, max, body) VARF_(name, global, min, cur, max, body, Idf_Persist)

//hexadecimal var macros
#define HVAR_(name, global, min, cur, max, persist)  int global = variable(#name, min, cur, max, &global, nullptr, persist | Idf_Hex)
#define HVARP(name, min, cur, max) HVAR_(name, name, min, cur, max, Idf_Persist)

//hex variable with function to be executed on change (Hexadecimal VARiable with Function = HVARF)
//the CVAR series of macros borrows the HVARF macro as it deals with hex colors
#define HVARF_(name, global, min, cur, max, body, persist) \
    int global = variable(#name, min, cur, max, &global, [] (ident *) { body; }, persist | Idf_Hex); /* assign variable, needs fxn prototype declared above */

//color var macros
#define CVAR_(name, cur, init, body, persist) bvec name = bvec::hexcolor(cur); HVARF_(name, _##name, 0, cur, 0xFFFFFF, { init; name = bvec::hexcolor(_##name); body; }, persist)
#define CVARP(name, cur) CVAR_(name, cur, , , Idf_Persist)
#define CVARR(name, cur) CVAR_(name, cur, , , Idf_Override)
#define CVARFP(name, cur, body) CVAR_(name, cur, , body, Idf_Persist)
#define CVAR0_(name, cur, body, persist) CVAR_(name, cur, { if(!_##name) _##name = cur; }, body, persist)
#define CVAR0R(name, cur) CVAR0_(name, cur, , Idf_Override)
#define CVAR1_(name, cur, body, persist) CVAR_(name, cur, { if(_##name <= 255) _##name |= (_##name<<8) | (_##name<<16); }, body, persist)
#define CVAR1R(name, cur) CVAR1_(name, cur, , Idf_Override)
#define CVAR1FR(name, cur, body) CVAR1_(name, cur, body, Idf_Override)

//float var macros
#define FVAR_(name, global, min, cur, max, persist) float global = fvariable(#name, min, cur, max, &global, nullptr, persist)
#define FVARNP(name, global, min, cur, max) FVAR_(name, global, min, cur, max, Idf_Persist)
#define FVAR(name, min, cur, max) FVAR_(name, name, min, cur, max, 0)
#define FVARP(name, min, cur, max) FVAR_(name, name, min, cur, max, Idf_Persist)
#define FVARR(name, min, cur, max) FVAR_(name, name, min, cur, max, Idf_Override)

//float variable with function to be executed on change (Float VARiable with Function = FVARF)
#define FVARF_(name, global, min, cur, max, body, persist) \
    float global = fvariable(#name, min, cur, max, &global, [] (ident *) { body; }, persist); /* assign variable, needs fxn prototype declared above */ \

#define FVARF(name, min, cur, max, body) FVARF_(name, name, min, cur, max, body, 0)
#define FVARFP(name, min, cur, max, body) FVARF_(name, name, min, cur, max, body, Idf_Persist)
#define FVARFR(name, min, cur, max, body) FVARF_(name, name, min, cur, max, body, Idf_Override)

//string var macros
#define SVAR_(name, global, cur, persist) char *global = svariable(#name, cur, &global, nullptr, persist)
#define SVAR(name, cur) SVAR_(name, name, cur, 0)
#define SVARP(name, cur) SVAR_(name, name, cur, Idf_Persist)
#define SVARR(name, cur) SVAR_(name, name, cur, Idf_Override)

//string variable with function to be executed on change (Float VARiable with Function = FVARF)
#define SVARF_(name, global, cur, body, persist) \
    char *global = svariable(#name, cur, &global, [] (ident *) { body; }, persist); /* assign variable, needs fxn prototype declared above */

#define SVARF(name, cur, body) SVARF_(name, name, cur, body, 0)
#define SVARFR(name, cur, body) SVARF_(name, name, cur, body, Idf_Override)

#endif /* COMMAND_H_ */
