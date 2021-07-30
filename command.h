// script binding functionality

enum
{
    Max_Args = 25,
    Max_Results = 7,
    Max_CommandArgs = 12
};

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
    Code_Start = 0,          //0
    Code_Offset,
    Code_Null,
    Code_True,
    Code_False,
    Code_Not,               //5
    Code_Pop,
    Code_Enter,
    Code_EnterResult,
    Code_Exit,
    Code_ResultArg,       //10
    Code_Val,
    Code_ValI,
    Code_Dup,
    Code_Macro,
    Code_Bool,            //15 (unused)
    Code_Block,
    Code_Empty,
    Code_Compile,
    Code_Cond,
    Code_Force,           //20
    Code_Result,
    Code_Ident,
    Code_IdentU,
    Code_IdentArg,
    Code_Com,             //25
    Code_ComD,
    Code_ComC,
    Code_ComV,
    Code_ConC,
    Code_ConCW,           //30
    Code_ConCM,
    Code_Down, // (unused)
    Code_StrVar,
    Code_StrVarM,
    Code_StrVar1,         //35
    Code_IntVar,
    Code_IntVar1,
    Code_IntVar2,
    Code_IntVar3,
    Code_FloatVar,        //40
    Code_FloatVar1,
    Code_Lookup,
    Code_LookupU,
    Code_LookupArg,
    Code_LookupM,         //45
    Code_LookupMU,
    Code_LookupMArg,
    Code_Alias,
    Code_AliasU,
    Code_AliasArg,        //50
    Code_Call,
    Code_CallU,
    Code_CallArg,
    Code_Print,
    Code_Local,           //55
    Code_Do,
    Code_DoArgs,
    Code_Jump,
    Code_JumpTrue,
    Code_JumpFalse,
    Code_JumpResultTrue,  //60
    Code_JumpResultFalse,

    Code_OpMask = 0x3F,
    Code_Ret = 6,
    Code_RetMask = 0xC0,

    /* return type flags */
    Ret_Null    = Value_Null<<Code_Ret,
    Ret_String  = Value_String<<Code_Ret,
    Ret_Integer = Value_Integer<<Code_Ret,
    Ret_Float   = Value_Float<<Code_Ret,
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

extern const char *sourcefile,
                  *sourcestr;

extern vector<char> strbuf[4];
extern int stridx;

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

extern vector<ident *> identmap;

struct NullVal : tagval
{
    NullVal() { setnull(); }
};

extern NullVal nullval;

struct identstack
{
    identval val;
    int valtype;
    identstack *next;
};

//this pointer will point to different types depending upon the type of variable
union identvalptr
{
    void  *p; // ID_*VAR
    int   *i; // Id_Var
    float *f; // Id_FloatVar
    char **s; // Id_StringVar
};

typedef void (__cdecl *identfun)(ident *id);

struct ident
{
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

struct IdentLink
{
    ident *id;
    IdentLink *next;
    int usedargs;
    identstack *argstack;
};

extern IdentLink noalias;
extern IdentLink *aliasstack;

extern void addident(ident *id);

extern tagval *commandret;
extern const char *intstr(int v);
extern void intret(int v);
extern const char *floatstr(float v);
extern void floatret(float v);
extern const char *numberstr(double v);
extern void numberret(double v);
extern void stringret(char *s);
extern void result(tagval &v);
extern void result(const char *s);

extern void poparg(ident &id);
extern void pusharg(ident &id, const tagval &v, identstack &stack);
extern bool getbool(const tagval &v);
extern void cleancode(ident &id);
extern char *conc(tagval *v, int n, bool space);
extern char *conc(tagval *v, int n, bool space, const char *prefix);
extern void freearg(tagval &v);
extern int unescapestring(char *dst, const char *src, const char *end);
extern const char *parsestring(const char *p);
extern void setarg(ident &id, tagval &v);
extern void setalias(ident &id, tagval &v);
extern void undoarg(ident &id, identstack &stack);
extern void redoarg(ident &id, const identstack &stack);
extern const char *parseword(const char *p);

inline int parseint(const char *s)
{
    return static_cast<int>(strtoul(s, nullptr, 0));
}
const int undoflag = 1<<Max_Args;

#define UNDOARGS \
    identstack argstack[Max_Args]; \
    IdentLink *prevstack = aliasstack; \
    IdentLink aliaslink; \
    for(int undos = 0; prevstack != &noalias; prevstack = prevstack->next) \
    { \
        if(prevstack->usedargs & undoflag) \
        { \
            ++undos; \
        } \
        else if(undos > 0) \
        { \
            --undos; \
        } \
        else \
        { \
            prevstack = prevstack->next; \
            for(int argmask = aliasstack->usedargs & ~undoflag, i = 0; argmask; argmask >>= 1, i++) \
            { \
                if(argmask&1) \
                { \
                    undoarg(*identmap[i], argstack[i]); \
                } \
            } \
            aliaslink.id = aliasstack->id; \
            aliaslink.next = aliasstack; \
            aliaslink.usedargs = undoflag | prevstack->usedargs; \
            aliaslink.argstack = prevstack->argstack; \
            aliasstack = &aliaslink; \
            break; \
        } \
    } \


#define REDOARGS \
    if(aliasstack == &aliaslink) \
    { \
        prevstack->usedargs |= aliaslink.usedargs & ~undoflag; \
        aliasstack = aliaslink.next; \
        for(int argmask = aliasstack->usedargs & ~undoflag, i = 0; argmask; argmask >>= 1, i++) \
        { \
            if(argmask&1) \
            { \
                redoarg(*identmap[i], argstack[i]); \
            } \
        } \
    }

#define PARSEFLOAT(name, type) \
    inline type parse##name(const char *s) \
    { \
        /* not all platforms (windows) can parse hexadecimal integers via strtod */ \
        char *end; \
        double val = strtod(s, &end); \
        return val || end==s || (*end!='x' && *end!='X') ? type(val) : type(parseint(s)); \
    }
PARSEFLOAT(float, float)
PARSEFLOAT(number, double)

inline void intformat(char *buf, int v, int len = 20) { nformatstring(buf, len, "%d", v); }
inline void floatformat(char *buf, float v, int len = 20) { nformatstring(buf, len, v==static_cast<int>(v) ? "%.1f" : "%.7g", v); }
inline void numberformat(char *buf, double v, int len = 20)
{
    int i = static_cast<int>(v);
    if(v == i)
    {
        nformatstring(buf, len, "%d", i);
    }
    else
    {
        nformatstring(buf, len, "%.7g", v);
    }
}

inline const char *getstr(const identval &v, int type)
{
    switch(type)
    {
        case Value_String:
        case Value_Macro:
        case Value_CString:
        {
            return v.s;
        }
        case Value_Integer:
        {
            return intstr(v.i);
        }
        case Value_Float:
        {
            return floatstr(v.f);
        }
        default:
        {
            return "";
        }
    }
}
inline const char *tagval::getstr() const
{
    return ::getstr(*this, type);
}

inline const char *ident::getstr() const
{
    return ::getstr(val, valtype);
}

//defines three getter functions, kind of like a bastard template (though we want the fxns to have different names)
// very fun!
#define GETNUMBER(name, ret) \
    inline ret get##name(const identval &v, int type) \
    { \
        switch(type) \
        { \
            case Value_Float: \
            { \
                return ret(v.f); \
            } \
            case Value_Integer: \
            { \
                return ret(v.i); \
            } \
            case Value_String: \
            case Value_Macro: \
            case Value_CString: \
            { \
                return parse##name(v.s); \
            } \
            default: \
            { \
                return ret(0); \
            } \
        } \
    } \
    inline ret tagval::get##name() const \
    { \
        return ::get##name(*this, type); \
    } \
    inline ret ident::get##name() const \
    { \
        return ::get##name(val, valtype); \
    }

GETNUMBER(int, int)
GETNUMBER(float, float)
GETNUMBER(number, double)

inline void getval(const identval &v, int type, tagval &r)
{
    switch(type)
    {
        case Value_String:
        case Value_Macro:
        case Value_CString:
        {
            r.setstr(newstring(v.s));
            break;
        }
        case Value_Integer:
        {
            r.setint(v.i);
            break;
        }
        case Value_Float:
        {
            r.setfloat(v.f);
            break;
        }
        default:
        {
            r.setnull();
            break;
        }
    }
}

inline void tagval::getval(tagval &r) const
{
    ::getval(*this, type, r);
}

inline void ident::getval(tagval &r) const
{
    ::getval(val, valtype, r);
}

inline void ident::getcstr(tagval &v) const
{
    switch(valtype)
    {
        case Value_Macro:
        {
            v.setmacro(val.code);
            break;
        }
        case Value_String:
        case Value_CString:
        {
            v.setcstr(val.s);
            break;
        }
        case Value_Integer:
        {
            v.setstr(newstring(intstr(val.i)));
            break;
        }
        case Value_Float:
        {
            v.setstr(newstring(floatstr(val.f)));
            break;
        }
        default:
        {
            v.setcstr("");
            break;
        }
    }
}

inline void ident::getcval(tagval &v) const
{
    switch(valtype)
    {
        case Value_Macro:
        {
            v.setmacro(val.code);
            break;
        }
        case Value_String:
        case Value_CString:
        {
            v.setcstr(val.s);
            break;
        }
        case Value_Integer:
        {
            v.setint(val.i);
            break;
        }
        case Value_Float:
        {
            v.setfloat(val.f);
            break;
        }
        default:
        {
            v.setnull();
            break;
        }
    }
}

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
    void var_##name(ident *); /* prototype the function to be executed */ \
    int global = variable(#name, min, cur, max, &global, var_##name, persist); /* assign variable, needs fxn prototype declared above */ \
    void var_##name(ident *) /* function to be executed */ \
    { \
        body; \
    }

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
    void var_##name(ident *id); /* prototype the function to be executed */ \
    int global = variable(#name, min, cur, max, &global, var_##name, persist | Idf_Hex); /* assign variable, needs fxn prototype declared above */ \
    void var_##name(ident *) /* function to be executed */ \
    { \
        body; \
    }

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
    void var_##name(ident *id); /* prototype the function to be executed */ \
    float global = fvariable(#name, min, cur, max, &global, var_##name, persist); /* assign variable, needs fxn prototype declared above */ \
    void var_##name(ident *) /* function to be executed */ \
    { \
        body; \
    }

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
    void var_##name(ident *id); /* prototype the function to be executed */ \
    char *global = svariable(#name, cur, &global, var_##name, persist); /* assign variable, needs fxn prototype declared above */ \
    void var_##name(ident *) /* function to be executed */ \
    { \
        body; \
    }

#define SVARF(name, cur, body) SVARF_(name, name, cur, body, 0)
#define SVARFR(name, cur, body) SVARF_(name, name, cur, body, Idf_Override)

// anonymous inline commands, uses nasty template trick with line numbers to keep names unique
#define ICOMMANDNAME(name) _icmd_##name
#define ICOMMANDSNAME _icmds_

#define ICOMMANDKNS(name, type, cmdname, nargs, proto, b) \
    template<int N> struct cmdname; /* prototype the cmdname object */ \
    template<> struct cmdname<__LINE__> /* create a generic template for the above cmdname obj */ \
    { \
        static bool init; \
        static void run proto; \
    }; \
    /* register command "prototype" so script parser knows how many args (and of what type) to parse */ \
    bool cmdname<__LINE__>::init = addcommand(name, (identfun)cmdname<__LINE__>::run, nargs, type); \
    void cmdname<__LINE__>::run proto /* contents `b` (body) to be executed when called by script engine */ \
    { \
        b; /* b */ \
    }

#define ICOMMANDKN(name, type, cmdname, nargs, proto, b) ICOMMANDKNS(#name, type, cmdname, nargs, proto, b)
#define ICOMMANDK(name, type, nargs, proto, b) ICOMMANDKN(name, type, ICOMMANDNAME(name), nargs, proto, b)
#define ICOMMANDNS(name, cmdname, nargs, proto, b) ICOMMANDKNS(name, Id_Command, cmdname, nargs, proto, b)
#define ICOMMANDN(name, cmdname, nargs, proto, b) ICOMMANDNS(#name, cmdname, nargs, proto, b)
#define ICOMMAND(name, nargs, proto, b) ICOMMANDN(name, ICOMMANDNAME(name), nargs, proto, b)
#define ICOMMANDS(name, nargs, proto, b) ICOMMANDNS(name, ICOMMANDSNAME, nargs, proto, b)
