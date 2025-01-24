/**
 * @file tools.h
 * @brief Legacy tooling inherited from the Cube engine.
 *
 * This file contains a collection of deprecated, standalone tooling that replicates
 * some standard library functionality. It is highly recommended that the std library
 * equivalents of these utilities are used in their place.
 */

#ifndef TOOLS_H_
#define TOOLS_H_

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef signed long long int llong;
typedef unsigned long long int ullong;

#if defined(__GNUC__) || defined(_MSC_VER)
    #define RESTRICT __restrict
#else
    #define RESTRICT
#endif

#ifdef max
    #undef max
#endif
#ifdef min
    #undef min
#endif
template<class T>
inline T max(T a, T b)
{
    return a > b ? a : b;
}
template<class T>
inline T max(T a, T b, T c)
{
    return max(max(a, b), c);
}
template<class T>
inline T min(T a, T b)
{
    return a < b ? a : b;
}
template<class T>
inline T min(T a, T b, T c)
{
    return min(min(a, b), c);
}

#ifdef __GNUC__
    #define BITSCAN(mask) (__builtin_ffs(mask)-1)
#else
    #ifdef WIN32
        #pragma intrinsic(_BitScanForward)
        inline int BITSCAN(uint mask)
        {
            ulong i;
            return _BitScanForward(&i, mask) ? i : -1;
        }
    #else
        inline int BITSCAN(uint mask)
        {
            if(!mask) return -1;
            int i = 1;
            if(!(mask&0xFFFF)) { i += 16; mask >>= 16; }
            if(!(mask&0xFF)) { i += 8; mask >>= 8; }
            if(!(mask&0xF)) { i += 4; mask >>= 4; }
            if(!(mask&3)) { i += 2; mask >>= 2; }
            return i - (mask&1);
        }
    #endif
#endif

inline int randomint(int x)
{
    return rand()%(x);
}
inline float randomfloat(int x)
{
    return (float((rand()*float(x))/float(RAND_MAX)));
}
//1103515245+12345 are magic constants for LCG psuedorandom generator
inline float detrnd(uint s, int x)
{
    return static_cast<int>(((s*1103515245+12345)>>16)%x);
}

constexpr double SQRT2 = 1.4142135623731;
constexpr double SQRT3 = 1.73205080756888;

#ifdef WIN32
    #ifndef M_PI
        constexpr double M_PI = 3.14159265358979323846;
    #endif
    #ifndef M_LN2
        constexpr double M_LN2 = 0.693147180559945309417;
    #endif

    #ifndef __GNUC__
        #pragma warning (3: 4189)       // local variable is initialized but not referenced
        #pragma warning (disable: 4244) // conversion from 'int' to 'float', possible loss of data
        #pragma warning (disable: 4267) // conversion from 'size_t' to 'int', possible loss of data
        #pragma warning (disable: 4355) // 'this' : used in base member initializer list
        #pragma warning (disable: 4996) // 'strncpy' was declared deprecated
    #endif

    #define strcasecmp _stricmp
    #define strncasecmp _strnicmp
    #define PATHDIV '\\'

#else
    #define __cdecl
    #define _vsnprintf vsnprintf
    #define PATHDIV '/'
#endif

#define RAD static_cast<float>(180.0f / M_PI) //M_PI is a double


#ifdef __GNUC__
    #define PRINTFARGS(fmt, args) __attribute__((format(printf, fmt, args)))
#else
    #define PRINTFARGS(fmt, args)
#endif

// easy safe strings
const int maxstrlen = 260;

typedef char string[maxstrlen];

inline void vformatstring(char *d, const char *fmt, va_list v, int len) { _vsnprintf(d, len, fmt, v); d[len-1] = 0; }

template<size_t N>
inline void vformatstring(char (&d)[N], const char *fmt, va_list v) { vformatstring(d, fmt, v, N); }

inline char *copystring(char *d, const char *s, size_t len)
{
    size_t slen = min(strlen(s), len-1);
    std::memcpy(d, s, slen);
    d[slen] = 0;
    return d;
}
template<size_t N>
inline char *copystring(char (&d)[N], const char *s) { return copystring(d, s, N); }

inline char *concatstring(char *d, const char *s, size_t len) { size_t used = strlen(d); return used < len ? copystring(d+used, s, len-used) : d; }

template<size_t N>
inline char *concatstring(char (&d)[N], const char *s) { return concatstring(d, s, N); }

inline void nformatstring(char *d, int len, const char *fmt, ...) PRINTFARGS(3, 4);
inline void nformatstring(char *d, int len, const char *fmt, ...)
{
    va_list v;
    va_start(v, fmt);
    vformatstring(d, fmt, v, len);
    va_end(v);
}

template<size_t N>
inline void formatstring(char (&d)[N], const char *fmt, ...) PRINTFARGS(2, 3);

template<size_t N>
inline void formatstring(char (&d)[N], const char *fmt, ...)
{
    va_list v;
    va_start(v, fmt);
    vformatstring(d, fmt, v, static_cast<int>(N));
    va_end(v);
}

template<size_t N>
inline void concformatstring(char (&d)[N], const char *fmt, ...) PRINTFARGS(2, 3);

template<size_t N>
inline void concformatstring(char (&d)[N], const char *fmt, ...)
{
    va_list v;
    va_start(v, fmt);
    int len = strlen(d);
    vformatstring(d + len, fmt, v, static_cast<int>(N) - len);
    va_end(v);
}

extern char *tempformatstring(const char *fmt, ...) PRINTFARGS(1, 2);

#define DEF_FORMAT_STRING(d,...) string d; formatstring(d, __VA_ARGS__)
#define DEFV_FORMAT_STRING(d,last,fmt) string d; { va_list ap; va_start(ap, last); vformatstring(d, fmt, ap); va_end(ap); }

inline bool matchstring(std::string_view s, size_t len, std::string_view d)
{
    return len == d.size() && !std::memcmp(s.data(), d.data(), d.size());
}

inline char *newstring(size_t l)
{
    return new char[l+1];
}
inline char *newstring(const char *s, size_t l)
{
    return copystring(newstring(l), s, l+1);
}
inline char *newstring(const char *s)
{
    size_t l = strlen(s);
    char *d = newstring(l);
    std::memcpy(d, s, l+1);
    return d;
}

inline char *newconcatstring(const char *s, const char *t)
{
    size_t slen = strlen(s),
           tlen = strlen(t);
    char *r = newstring(slen + tlen);
    std::memcpy(r, s, slen);
    std::memcpy(&r[slen], t, tlen);
    r[slen+tlen] = '\0';
    return r;
}

template <class T>
struct databuf
{
    enum
    {
        OVERREAD  = 1<<0,
        OVERWROTE = 1<<1
    };

    T *buf;
    int len, maxlen;
    uchar flags;

    databuf() : buf(nullptr), len(0), maxlen(0), flags(0) {}

    template<class U>
    databuf(T *buf, U maxlen) : buf(buf), len(0), maxlen(static_cast<int>(maxlen)), flags(0) {}

    void reset()
    {
        len = 0;
        flags = 0;
    }

    void reset(T *buf_, int maxlen_)
    {
        reset();
        buf = buf_;
        maxlen = maxlen_;
    }

    /**
     * @brief Adds an existing value in the underlying buffer to the databuf
     *
     * Increases the length of the databuf by one and incorporates the value at
     * the address after the end of the databuf's values.
     *
     * If the length is equal to or greater than the buffer's maximum length,
     * applies the OVERREAD flag and returns a null value.
     */
    const T &get()
    {
        static const T overreadval = 0;
        if(len<maxlen)
        {
            return buf[len++];
        }
        flags |= OVERREAD;
        return overreadval;
    }

    /**
     * @brief Returns a databuf<T> object containing the first n entries
     *
     * Returns a databuf object of type T which represents the first n elements
     * inside the databuf's array.
     *
     * If the size passed is larger than the databuf, the entire databuf object is
     * copied and returned.
     *
     * @param sz the size of the output array (aka get the first `sz` entries)
     *
     * @return the sub-buffer of `this` object
     */
    databuf subbuf(int sz)
    {
        sz = std::clamp(sz, 0, maxlen-len);
        len += sz;
        return databuf(&buf[len-sz], sz);
    }

    /**
     * @brief Adds N empty elements to the end of the array and returns a pointer
     *
     * Creates an array of size `sz` which is then pointed to and returned as
     * a naked pointer.
     *
     * @param sz the size of the buffer
     *
     * @return a pointer to the sub-array created
     */
    T *pad(int numvals)
    {
        T *vals = &buf[len];
        len += min(numvals, maxlen-len);
        return vals;
    }

    /**
     * @brief Adds an entry to the databuf if space is available
     *
     * If no space is available, applies the OVERWROTE flag to the flags field.
     *
     * @param val A const reference to the object to add
     */
    void put(const T &val)
    {
        if(len<maxlen)
        {
            buf[len++] = val;
        }
        else
        {
            flags |= OVERWROTE;
        }
    }

    void put(const T *vals, int numvals)
    {
        if(maxlen - len < numvals)
        {
            numvals = maxlen - len;
            flags |= OVERWROTE;
        }
        std::memcpy(&buf[len], static_cast<const void *>(vals), numvals*sizeof(T));
        len += numvals;
    }

    int get(T *vals, int numvals)
    {
        if(maxlen - len < numvals)
        {
            numvals = maxlen - len;
            flags |= OVERREAD;
        }
        std::memcpy(vals, static_cast<void *>(&buf[len]), numvals*sizeof(T));
        len += numvals;
        return numvals;
    }

    void offset(int n)
    {
        n = min(n, maxlen);
        buf += n;
        maxlen -= n;
        len = max(len-n, 0);
    }

    /**
     * @brief Returns a pointer to the internal data array.
     *
     * This is a naked pointer to the internal data array and can therefore be used
     * to potentially break this container, if used carelessly.
     *
     * @return A pointer the data array
     */
    T *getbuf() const { return buf; }

    /**
     * @brief Returns whether any entries have been assigned to the array.
     *
     * @return true if the array has not been assigned to
     * @return false if the array has assigned values
     */
    bool empty() const { return len==0; }

    /**
     * @brief Returns the number of allocated entries in the databuf.
     *
     * Returns the number of entries which have been added to the databuf (not the
     * overall databuf size).
     *
     * @return the number of entries allocated in the databuf.
     */
    int length() const { return len; }

    /**
     * @brief Returns the number of values remaining before the databuf overflows.
     *
     * Returns the allocatable space remaining within the databuf object, in terms
     * of the number of entires which can be `put` into it before it becomes
     * OVERWROTE.
     *
     * @return the number of available entries in the databuf.
     */
    int remaining() const { return maxlen-len; }

    /**
     * @brief Returns whether the databuf has been accessed with too large of an index.
     *
     * Returns whether the OVERREAD flag has been set. If the databuf was attempted
     * to be accessed with an index larger than the number of allocated members,
     * this value will be set.
     *
     * @return true if the array has been overread
     * @return false if all read operations have been to valid entries
     */
    bool overread() const { return (flags&OVERREAD)!=0; }

    /**
     * @brief Returns whether the databuf has had invalid writes to it
     *
     * Returns whether the OVERWROTE flag has been set. If the databuf was attempted
     * to be wrote to while the databuf is already full, this flag will be set.
     *
     * @return true if the databuf has not been overwritten
     * @return false if the databuf has had only valid writes
     */
    bool overwrote() const { return (flags&OVERWROTE)!=0; }

    bool check(int n) { return remaining() >= n; }

    void forceoverread()
    {
        len = maxlen;
        flags |= OVERREAD;
    }
};

typedef databuf<char> charbuf;
typedef databuf<uchar> ucharbuf;

inline uint memhash(const void *ptr, int len)
{
    const uchar *data = static_cast<const uchar *>(ptr);
    uint h = 5381;
    for(int i = 0; i < static_cast<int>(len); ++i)
    {
        h = ((h<<5)+h)^data[i];
    }
    return h;
}

template<class T>
inline T endianswap(T n) { union { T t; uint i; } conv; conv.t = n; conv.i = SDL_Swap32(conv.i); return conv.t; }

template<>
inline uint endianswap<uint>(uint n) { return SDL_Swap32(n); }

template<>
inline int endianswap<int>(int n) { return SDL_Swap32(n); }

/* workaround for some C platforms that have these two functions as macros - not used anywhere */
#ifdef getchar
    #undef getchar
#endif
#ifdef putchar
    #undef putchar
#endif

struct stream
{
#ifdef WIN32
    #if defined(__GNUC__) && !defined(__MINGW32__)
        typedef off64_t offset;
    #else
        typedef __int64 offset;
    #endif
    #else
        typedef off_t offset;
#endif
    //see file/gz/zipstream children for more interesting forms
    virtual ~stream() {}
    virtual void close() = 0;
    virtual bool end() = 0;
    virtual offset tell() { return -1; }
    virtual offset rawtell() { return tell(); }
    virtual bool seek(offset, int) { return false; }
    virtual offset size();
    virtual offset rawsize() { return size(); }
    virtual size_t read(void *, size_t) { return 0; }
    virtual size_t write(const void *, size_t) { return 0; }
    virtual bool flush() { return true; }
    virtual int getchar() { uchar c; return read(&c, 1) == 1 ? c : -1; }
    virtual bool putchar(int n) { uchar c = n; return write(&c, 1) == 1; }
    virtual bool getline(char *str, size_t len);
    virtual bool putstring(const char *str) { size_t len = strlen(str); return write(str, len) == len; }
    virtual bool putline(const char *str) { return putstring(str) && putchar('\n'); }
    virtual size_t printf(const char *fmt, ...) PRINTFARGS(2, 3);
    virtual uint getcrc() { return 0; }

    template<class T>
    size_t put(const T *v, size_t n) { return write(v, n*sizeof(T))/sizeof(T); }

    template<class T>
    bool put(T n) { return write(&n, sizeof(n)) == sizeof(n); }

    template<class T>
    size_t get(T *v, size_t n) { return read(v, n*sizeof(T))/sizeof(T); }

    template<class T>
    T get() { T n; return read(&n, sizeof(n)) == sizeof(n) ? n : 0; }

    template<class T>
    bool putbig(T n) { return put<T>(endianswap(n)); }

    SDL_RWops *rwops();
};

extern string homedir;

extern char *path(char *s);
extern std::string path(std::string s);
extern char *copypath(const char *s);
extern const char *sethomedir(const char *dir);
extern const char *findfile(const char *filename, const char *mode);

/**
 * @brief Initializes zip Cubescript commands.
 *
 * `addzip`
 *
 * `removezip`
 */
extern void initzipcmds();

extern stream *openrawfile(const char *filename, const char *mode);
extern stream *openzipfile(const char *filename, const char *mode);
extern stream *openfile(const char *filename, const char *mode);
extern stream *opengzfile(const char *filename, const char *mode, stream *file = nullptr, int level = Z_BEST_COMPRESSION);

template<class T>
inline void putint_(T &p, int n)
{
    if(n<128 && n>-127)
    {
        p.put(n);
    }
    else if(n<0x8000 && n>=-0x8000)
    {
        p.put(0x80);
        p.put(n);
        p.put(n>>8);
    }
    else
    {
        p.put(0x81);
        p.put(n);
        p.put(n>>8);
        p.put(n>>16);
        p.put(n>>24);
    }
}

template<class T>
inline void putuint_(T &p, int n)
{
    if(n < 0 || n >= (1<<21))
    {
        p.put(0x80 | (n & 0x7F));
        p.put(0x80 | ((n >> 7) & 0x7F));
        p.put(0x80 | ((n >> 14) & 0x7F));
        p.put(n >> 21);
    }
    else if(n < (1<<7))
    {
        p.put(n);
    }
    else if(n < (1<<14))
    {
        p.put(0x80 | (n & 0x7F));
        p.put(n >> 7);
    }
    else
    {
        p.put(0x80 | (n & 0x7F));
        p.put(0x80 | ((n >> 7) & 0x7F));
        p.put(n >> 14);
    }
}

extern void putint(std::vector<uchar> &p, int n);

template<typename T>
inline void vectorput(std::vector<uint8_t>& buf, const T& data)
{
    const uint8_t* data_ptr = reinterpret_cast<const uint8_t*>(&data);
    buf.insert(buf.end(), data_ptr, data_ptr + sizeof(T));
}

inline void vectorput(std::vector<uint8_t>& buf, const uint8_t* data, size_t size)
{
    buf.insert(buf.end(), data, data + size);
}

template<class T>
inline void sendstring_(const char *t, T &p)
{
    while(*t)
    {
        putint(p, *t++);
    }
    putint(p, 0);
}

template<class T>
inline void putfloat_(T &p, float f)
{
    p.put(reinterpret_cast<uchar *>(&f), sizeof(float));
}

extern void putint(ucharbuf &p, int n);
extern int getint(ucharbuf &p);
extern void putuint(ucharbuf &p, int n);
extern int getuint(ucharbuf &p);
extern void putfloat(ucharbuf &p, float f);
extern float getfloat(ucharbuf &p);
extern void sendstring(const char *t, ucharbuf &p);
extern void getstring(char *t, ucharbuf &p, size_t len);

template<size_t N>
inline void getstring(char (&t)[N], ucharbuf &p) { getstring(t, p, N); }

extern void filtertext(char *dst, const char *src, bool whitespace, bool forcespace, size_t len);

template<size_t N>
inline void filtertext(char (&dst)[N], const char *src, bool whitespace = true, bool forcespace = false) { filtertext(dst, src, whitespace, forcespace, N-1); }

#endif /* TOOLS_H_ */
