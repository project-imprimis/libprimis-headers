/**
 * @file command.h
 * @brief Script binding functionality.
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

    void setint(int val)
    {
        type = Value_Integer;
        i = val;
    }

    void setfloat(float val)
    {
        type = Value_Float;
        f = val;
    }

    void setnumber(double val)
    {
        i = static_cast<int>(val);
        if(val == i)
        {
            type = Value_Integer;
        }
        else
        {
            type = Value_Float;
            f = val;
        }
    }

    void setstr(char *val)
    {
        type = Value_String;
        s = val;
    }

    void setnull()
    {
        type = Value_Null;
        i = 0;
    }

    void setcode(const uint *val)
    {
        type = Value_Code;
        code = val;
    }

    void setmacro(const uint *val)
    {
        type = Value_Macro;
        code = val;
    }

    void setcstr(const char *val)
    {
        type = Value_CString;
        cstr = val;
    }

    void setident(ident *val)
    {
        type = Value_Ident;
        id = val;
    }

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

struct ident
{
    //this pointer will point to different types depending upon the type of variable
    union identvalptr
    {
        void  *p; // ID_*VAR
        int   *i; // Id_Var
        float *f; // Id_FloatVar
        char **s; // Id_StringVar
    };

    uchar type; // one of ID_* above
    union
    {
        uchar valtype; // Id_Alias
        uchar numargs; // Id_Command
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
                    int minval,
                        maxval;
                };     // Id_Var
                struct
                {
                    float minvalf,
                          maxvalf;
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
    identfun fun; // Id_Var, Id_FloatVar, Id_StringVar, Id_Command

    ident() {}
    // Id_Var
    ident(int t, const char *n, int m, int x, int *s, void *f = nullptr, int flags = 0)
        : type(t), flags(flags | (m > x ? Idf_ReadOnly : 0)), name(n), minval(m), maxval(x), fun((identfun)f)
    { storage.i = s; }

    // Id_FloatVar
    ident(int t, const char *n, float m, float x, float *s, void *f = nullptr, int flags = 0)
        : type(t), flags(flags | (m > x ? Idf_ReadOnly : 0)), name(n), minvalf(m), maxvalf(x), fun((identfun)f)
    { storage.f = s; }

    // Id_StringVar
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

    // Id_Command
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

extern const char *intstr(int v);
extern void intret(int v);
extern const char *floatstr(float v);
extern void floatret(float v);
extern const char *numberstr(double v);
extern void numberret(double v);
extern void stringret(char *s);
extern void result(tagval &v);
extern void result(const char *s);

inline int parseint(const char *s)
{
    return static_cast<int>(strtoul(s, nullptr, 0));
}

extern int variable(const char *name, int min, int cur, int max, int *storage, identfun fun, int flags);
extern float fvariable(const char *name, float min, float cur, float max, float *storage, identfun fun, int flags);
extern char *svariable(const char *name, const char *cur, char **storage, identfun fun, int flags);
extern void setvar(const char *name, int i, bool dofunc = true, bool doclamp = true);
extern void setfvar(const char *name, float f, bool dofunc = true, bool doclamp = true);
extern void setsvar(const char *name, const char *str, bool dofunc = true);
extern bool addcommand(const char *name, identfun fun, const char *narg, int type = Id_Command);

extern std::queue<ident *> triggerqueue;
extern bool identexists(const char *name);
extern ident *getident(const char *name);
extern ident *newident(const char *name, int flags = 0);
extern ident *readident(const char *name);
extern ident *writeident(const char *name, int flags = 0);
extern uint *compilecode(const char *p);
extern void freecode(uint *p);
extern int execute(const uint *code);
extern int execute(const char *p);
extern int execute(ident *id, tagval *args, int numargs, bool lookup = false);
extern int execident(const char *name, int noid = 0, bool lookup = false);
extern bool executebool(const uint *code);
extern bool executebool(const char *p);
extern bool executebool(ident *id, tagval *args, int numargs, bool lookup = false);
extern bool execidentbool(const char *name, bool noid = false, bool lookup = false);
extern bool execfile(const char *cfgfile, bool msg = true);
extern void alias(const char *name, const char *action);
extern const char *escapestring(const char *s);
extern void explodelist(const char *s, vector<char *> &elems, int limit = -1);
extern void printvar(ident *id, int i);
extern int clampvar(ident *id, int i, int minval, int maxval);
extern void loopiter(ident *id, identstack &stack, int i);
extern void loopend(ident *id, identstack &stack);
const char *escapeid(const char *s);
extern void writecfg(const char *savedconfig, const char *autoexec = nullptr, const char *defaultconfig = nullptr, const char *name = nullptr);
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

//command macros
#define KEYWORD(name, type) static bool dummy_##type = addcommand(#name, (identfun)nullptr, nullptr, type)
#define COMMANDKN(name, type, fun, nargs) static bool dummy_##fun = addcommand(#name, (identfun)fun, nargs, type)
#define COMMANDK(name, type, nargs) COMMANDKN(name, type, name, nargs)
#define COMMANDN(name, fun, nargs) COMMANDKN(name, Id_Command, fun, nargs)
#define COMMAND(name, nargs) COMMANDN(name, name, nargs)

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
