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
    Id_Var,     //0
    Id_FloatVar,
    Id_StringVar,
    Id_Command,
    Id_Alias,
    Id_Local,   //5
    Id_Do,
    Id_DoArgs,
    Id_If,
    Id_Result,
    Id_Not,     //10
    Id_And,
    Id_Or,      //12
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
                    int min, /**< if an int variable, its min allowed value*/
                        max; /**< if an int variable, its max allowed value*/
                } i;     // Id_Var
                struct
                {
                    float min, /**< if a float variable, its min allowed value*/
                          max; /**< if a float variable, its max allowed value*/
                } f; // Id_FloatVar
            };
            identvalptr storage;
            identval overrideval;
        } val;
        struct // Id_Alias
        {
            uint *code;
            identval val;
            identstack *stack;
        } alias;
        struct // Id_Command
        {
            const char *args;
            uint argmask;
        } cmd;
    };
    identfun fun; /**< the pointer a command or variable points to (the on-change command for a var)*/

    ident() {}
    /**
     * @brief Constructor for an ident for an int variable.
     */
    ident(int t, const char *n, int m, int x, int *s, void *f = nullptr, int flags = 0)
        : type(t), flags(flags | (m > x ? Idf_ReadOnly : 0)), name(n), fun((identfun)f)
    {
        val.storage.i = s;
        val.i.min = m;
        val.i.max = x;
    }

    /**
     * @brief Constructor for an ident for a float variable.
     */
    ident(int t, const char *n, float m, float x, float *s, void *f = nullptr, int flags = 0)
        : type(t), flags(flags | (m > x ? Idf_ReadOnly : 0)), name(n), fun((identfun)f)
    {
        val.storage.f = s;
        val.f.min = m;
        val.f.max = x;
    }

    /**
     * @brief Constructor for an ident for a string variable.
     */
    ident(int t, const char *n, char **s, void *f = nullptr, int flags = 0)
        : type(t), flags(flags), name(n), fun((identfun)f)
    {
        val.storage.s = s;
    }

    // Id_Alias
    ident(int t, const char *n, char *a, int flags)
        : type(t), valtype(Value_String), flags(flags), name(n)
    {
        alias.code = nullptr;
        alias.stack = nullptr;
        alias.val.s = a;
    }

    ident(int t, const char *n, int a, int flags)
        : type(t), valtype(Value_Integer), flags(flags), name(n)
    {
        alias.code = nullptr;
        alias.stack = nullptr;
        alias.val.i = a;
    }

    ident(int t, const char *n, float a, int flags)
        : type(t), valtype(Value_Float), flags(flags), name(n)
    {
        alias.code = nullptr;
        alias.stack = nullptr;
        alias.val.f = a;
    }

    ident(int t, const char *n, int flags)
        : type(t), valtype(Value_Null), flags(flags), name(n)
    {
        alias.code = nullptr;
        alias.stack = nullptr;
    }

    ident(int t, const char *n, const tagval &v, int flags)
        : type(t), valtype(v.type), flags(flags), name(n)
    {
        alias.code = nullptr;
        alias.stack = nullptr;
        alias.val = v;
    }

    /**
     * @brief Constructor for an ident for a C++ bound command.
     */
    ident(int t, const char *n, const char *args, uint argmask, int numargs, void *f = nullptr, int flags = 0)
        : type(t), numargs(numargs), flags(flags), name(n), fun((identfun)f)
    {
        cmd.args = args;
        cmd.argmask = argmask;
    }

    /**
     * @brief Calls a change effect for this ident, if one exists.
     *
     * If there is no function pointed to by `fun` (it is null), then nothing
     * will occur.
     */
    void changed()
    {
        if(fun)
        {
            fun(this);
        }
    }

    /**
     * @brief Sets the value and type of value of this ident given a tagval
     *
     * Sets this ident's value type and value to the corresponding values from
     * the passed tagval object.
     *
     * @param v the tagval to set values from
     */
    void setval(const tagval &v)
    {
        valtype = v.type;
        alias.val = v;
    }

    /**
     * @brief Sets the value and type of value of this ident given an identstack
     *
     * Sets this ident's value type and value to the corresponding values from
     * the passed identstack object.
     *
     * @param v the identstack to set values from
     */
    void setval(const identstack &v)
    {
        valtype = v.valtype;
        alias.val = v.val;
    }

    /**
     * @brief Sets the value type of this ident to null.
     *
     * If a string is being stored inside this ident, it is freed.
     */
    void forcenull()
    {
        if(valtype==Value_String)
        {
            delete[] alias.val.s;
        }
        valtype = Value_Null;
    }

    /**
     * @brief Returns the saved value of the ident as a float.
     *
     * Returns a float even if the ident is of another type, e.g. integer
     *
     * @return the float value of the ident
     */
    float getfloat() const;

    /**
     * @brief Returns the saved value of the ident as an integer.
     *
     * Returns an int even if the ident is of another type, e.g. float
     */
    int getint() const;

    /**
     * @brief Returns the saved value of the ident as a double.
     *
     * Returns a double even if the ident is of another type, e.g. integer
     *
     * @return the double value of the ident
     */
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
    return static_cast<int>(std::strtoul(s, nullptr, 0));
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
 * @param p the C string containing the code to execute
 *
 * @return an int from the results of the execution
 */
extern int execute(const char *p);

/**
 * @brief Executes the contents of an ident, searched by name.
 *
 * Attempts to find in the ident table an ident with the given name, and
 * if found executes it.
 *
 * @param name the name of the ident to look for
 * @param noid the value to return if no ident is found
 * @param lookup if a command, and parameters are of format 'N', sets to -1
 */
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
 * The resulting string is also wrapped in quotes ("").
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
extern void printvar(const ident *id, int i);

/**
 * @brief Modifies the value passed to fall within the boundaries passed.
 *
 * Clamps the value i to within minval and maxval. If hex is passed, warns in hex
 * mode, otherwise decimal. If the values are not within bounds, issued aformentioned
 * warning, refering to the passed name in the console message.
 *
 * @param hex toggles whether to display hex warning instead of decimal
 * @param name the name
 * @param i the value to clamp
 * @param minval the lowest value to clamp to
 * @param maxval the largest value to clamp to
 *
 * @return the clamped value
 */
extern int clampvar(bool hex, std::string name, int i, int minval, int maxval);
extern void loopiter(ident *id, identstack &stack, int i);
extern void loopend(ident *id, identstack &stack);

/**
 * @brief Escapes a string unless it is null.
 *
 * If an empty string is passed, escapestring() is not called. Otherwise, the same
 * behavior as in escapestring() is executed.
 *
 * @param s the string to convert
 *
 * @return a string containing the potentially applied escape changes
 */
const char *escapeid(const char *s);

/**
 * @brief Writes out the state of the CubeScript idents to a file.
 *
 * @param savedconfig the path to write to if name is nullptr
 * @param autoexec the path for the autoexec file for user modification, for display in top of file comment
 * @param defaultconfig the path for the master copy of the configuration file, for display in top of file comment
 * @param name the path to write to, if nullptr savedconfig is used instead
 */
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

/**
 * @brief Initializes the CubeScript preset argument idents.
 *
 * Intitializes the argument parameters (arg1, arg2, ..., arg<Max_Args>) as CS
 * ident objects. Required to use of $argN variables.
 * Also initializes the dummy ident, `//dummy`.
 *
 * @return always returns true
 */
extern bool initidents();

extern int identflags; /**< The flags to automatically set on ident objects */

/**
 * @brief Clears all aliases from the ident map.
 *
 * All aliases, aka objects created from within CubeScript, have their contents
 * (the names and code values associated with them) freed. Does not remove the
 * ident object itself from the global ident map.
 */
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

/*
 * Parameter Flags (exhaustive list, not exhaustive descriptions)
 * i an integer parameter
 * b a boolean parameter
 * f a float parameter (sets to 0 if over parameter limit)
 * F a float parameter (sets to value of previous parameter if over parameter limit)
 * s a string parameter (sets to new allocated string "" if over parameter limit)
 * S a string paremter (sets current string to "" if over parameter limit)
 * t sets to null if over parameter limit
 * T same as "t"
 * e a code block parameter
 * E a code block paremter (null)
 * r an ident parameter
 * $ an ident parameter
 * N a number parameter
 * D a bind parameter (e.g. player movement), adds a release action for the command
 * C an ident parameter, ComC
 * V a series of variadic parameter(s) (open ended series of values)
 * 1 repeat one arg
 * 2 repeat two args
 * 3 repeat three args
 * 4 repeat four args
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
