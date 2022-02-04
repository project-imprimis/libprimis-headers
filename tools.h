/**
 * @file tools.h
 * @brief Generic and useful functions, constants, macros, etc., for any C++ program.
 */

#ifndef TOOLS_H_
#define TOOLS_H_

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef signed long long int llong;
typedef unsigned long long int ullong;

#if defined(__GNUC__) || (defined(_MSC_VER) && _MSC_VER >= 1400)
    #define RESTRICT __restrict
#else
    #define RESTRICT
#endif

#ifdef swap
    #undef swap
#endif
template<class T>
inline void swap(T &a, T &b)
{
    T t = a;
    a = b;
    b = t;
}
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

#define DELETEP(p) if(p) { delete   p; p = 0; }
#define DELETEA(p) if(p) { delete[] p; p = 0; }

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

#define RAD static_cast<float>(M_PI / 180.0f) //M_PI is a double

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
    memcpy(d, s, slen);
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

template<size_t N>
inline bool matchstring(const char *s, size_t len, const char (&d)[N])
{
    return len == N-1 && !memcmp(s, d, N-1);
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
    memcpy(d, s, l+1);
    return d;
}

inline char *newconcatstring(const char *s, const char *t)
{
    size_t slen = strlen(s),
           tlen = strlen(t);
    char *r = newstring(slen + tlen);
    memcpy(r, s, slen);
    memcpy(&r[slen], t, tlen);
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
    databuf(T *buf, U maxlen) : buf(buf), len(0), maxlen((int)maxlen), flags(0) {}

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

    databuf subbuf(int sz)
    {
        sz = std::clamp(sz, 0, maxlen-len);
        len += sz;
        return databuf(&buf[len-sz], sz);
    }

    T *pad(int numvals)
    {
        T *vals = &buf[len];
        len += min(numvals, maxlen-len);
        return vals;
    }

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
        memcpy(&buf[len], (const void *)vals, numvals*sizeof(T));
        len += numvals;
    }

    int get(T *vals, int numvals)
    {
        if(maxlen - len < numvals)
        {
            numvals = maxlen - len;
            flags |= OVERREAD;
        }
        memcpy(vals, (void *)&buf[len], numvals*sizeof(T));
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

    T *getbuf() const { return buf; }
    bool empty() const { return len==0; }
    int length() const { return len; }
    int remaining() const { return maxlen-len; }
    bool overread() const { return (flags&OVERREAD)!=0; }
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

template<class T>
inline float heapscore(const T &n) { return n; }

struct sortless
{
    template<class T>
    bool operator()(const T &x, const T &y) const { return x < y; }

    bool operator()(char *x, char *y) const { return strcmp(x, y) < 0; }

    bool operator()(const char *x, const char *y) const { return strcmp(x, y) < 0; }
};

struct sortnameless
{
    template<class T>
    bool operator()(const T &x, const T &y) const { return sortless()(x.name, y.name); }

    template<class T>
    bool operator()(T *x, T *y) const { return sortless()(x->name, y->name); }

    template<class T>
    bool operator()(const T *x, const T *y) const { return sortless()(x->name, y->name); }
};

template<class T, class F>
inline void insertionsort(T *start, T *end, F fun)
{
    for(T *i = start+1; i < end; i++)
    {
        if(fun(*i, i[-1]))
        {
            T tmp = *i;
            *i = i[-1];
            T *j = i-1;
            for(; j > start && fun(tmp, j[-1]); --j)
            {
                *j = j[-1];
            }
            *j = tmp;
        }
    }

}

template<class T, class F>
inline void insertionsort(T *buf, int n, F fun)
{
    insertionsort(buf, buf+n, fun);
}

template<class T>
inline void insertionsort(T *buf, int n)
{
    insertionsort(buf, buf+n, sortless());
}

template<class T, class F>
inline void quicksort(T *start, T *end, F fun)
{
    while(end-start > 10)
    {
        T *mid = &start[(end-start)/2], *i = start+1,
          *j = end-2,
          pivot;
        if(fun(*start, *mid)) /* start < mid */
        {
            if(fun(end[-1], *start))
            {
                pivot = *start;
                *start = end[-1];
                end[-1] = *mid;
            } /* end < start < mid */
            else if(fun(end[-1], *mid))
            {
                pivot = end[-1];
                end[-1] = *mid;
            } /* start <= end < mid */
            else
            {
                pivot = *mid;
            } /* start < mid <= end */
        }
        else if(fun(*start, end[-1]))
        {
            pivot = *start;
            *start = *mid;
        } /*mid <= start < end */
        else if(fun(*mid, end[-1]))
        {
            pivot = end[-1];
            end[-1] = *start;
            *start = *mid;
        } /* mid < end <= start */
        else
        {
            pivot = *mid;
            swap(*start, end[-1]);
        }  /* end <= mid <= start */
        *mid = end[-2];
        do
        {
            while(fun(*i, pivot))
            {
                if(++i >= j)
                {
                    goto partitioned;
                }
            }
            while(fun(pivot, *--j))
            {
                if(i >= j)
                {
                    goto partitioned;
                }
            }
            swap(*i, *j);
        } while(++i < j);
    partitioned:
        end[-2] = *i;
        *i = pivot;

        if(i-start < end-(i+1))
        {
            quicksort(start, i, fun);
            start = i+1;
        }
        else
        {
            quicksort(i+1, end, fun);
            end = i;
        }
    }

    insertionsort(start, end, fun);
}

template<class T, class F>
inline void quicksort(T *buf, int n, F fun)
{
    quicksort(buf, buf+n, fun);
}

template<class T>
inline void quicksort(T *buf, int n)
{
    quicksort(buf, buf+n, sortless());
}

inline uint hthash(const char *key)
{
    uint h = 5381;
    for(int i = 0, k; (k = key[i]); i++)
    {
        h = ((h<<5)+h)^k;    // bernstein k=33 xor
    }
    return h;
}

inline bool htcmp(const char *x, const char *y)
{
    return !strcmp(x, y);
}

struct stringslice
{
    const char *str;
    int len;
    stringslice() {}
    stringslice(const char *str, int len) : str(str), len(len) {}
    stringslice(const char *str, const char *end) : str(str), len(static_cast<int>(end-str)) {}

    const char *end() const { return &str[len]; }
};

inline char *newstring(const stringslice &s) { return newstring(s.str, s.len); }
inline const char *stringptr(const char *s) { return s; }
inline const char *stringptr(const stringslice &s) { return s.str; }
inline int stringlen(const char *s) { return static_cast<int>(strlen(s)); }
inline int stringlen(const stringslice &s) { return s.len; }

inline char *copystring(char *d, const stringslice &s, size_t len)
{
    size_t slen = min(size_t(s.len), len-1);
    memcpy(d, s.str, slen);
    d[slen] = 0;
    return d;
}

template<size_t N>
inline char *copystring(char (&d)[N], const stringslice &s) { return copystring(d, s, N); }

inline uint memhash(const void *ptr, int len)
{
    const uchar *data = (const uchar *)ptr;
    uint h = 5381;
    for(int i = 0; i < static_cast<int>(len); ++i)
    {
        h = ((h<<5)+h)^data[i];
    }
    return h;
}

inline uint hthash(const stringslice &s) { return memhash(s.str, s.len); }

inline bool htcmp(const stringslice &x, const char *y)
{
    return x.len == (int)strlen(y) && !memcmp(x.str, y, x.len);
}

inline uint hthash(int key)
{
    return key;
}

inline bool htcmp(int x, int y)
{
    return x==y;
}

inline uint hthash(GLuint key)
{
    return key;
}

inline bool htcmp(GLuint x, GLuint y)
{
    return x==y;
}

/**
 * @brief A container template for storing variable lists of items.
 *
 * This template implements a generic container class, consisting of a databuf
 * object holding the data which changes size as the container has elements added
 * to it. This container is similar in principle to std::vector<> but has a few
 * notable differences in semantics.
 *
 * The vector container does not support iterators, and therefore std library functions
 * depending on them (such as sort()) will not work. Instead, the builtin functions
 * for some such features are included in the class.
 */
template <class T>
struct vector
{
    static const int MINSIZE = 8; /**< The minimum number of elements a vector can be created with. */

    T *buf; /**< The array of data values making up the vector's data.*/
    int alen, /**< The allocated length of the vector, including unused space in the buffer. */
        ulen; /**< The used length of the vector, only counting the part of the buffer which has a vector index */

    /**
     * @brief Creates a new vector object.
     *
     * Creates a new vector object of length 0 and an allocated length of 0; the
     * buffer storing the data is not initialized.
     */
    vector() : buf(nullptr), alen(0), ulen(0)
    {
    }

    /**
     * @brief Assigns the vector to another vector by reference.
     *
     * Points `this` vector to another vector passed by reference.
     * The initialized parameters are replaced by the pointer assignment.
     *
     * @param v The vector for this vector to point to.
     */
    vector(const vector &v) : buf(nullptr), alen(0), ulen(0)
    {
        *this = v;
    }

    /**
     * @brief Destroys the vector object.
     *
     * Deletes the vector and frees the space on the heap allocated to store the
     * vector's data.
     */
    ~vector() { shrink(0); if(buf) delete[] (uchar *)buf; }

    /**
     * @brief Sets this vector equal to the passed one.
     *
     * Returns this vector, after copying the values from the passed vector to
     * it. The allocated length of the two vectors are not necessarily equal after
     * copying; however the assigned values will be equal.
     *
     * @param v the vector to copy to this vector
     *
     * @return a vector of type T with the values of v
     */
    vector<T> &operator=(const vector<T> &v)
    {
        shrink(0);
        if(v.length() > alen)
        {
            growbuf(v.length());
        }
        for(int i = 0; i < v.length(); i++)
        {
            add(v[i]);
        }
        return *this;
    }

    /**
     * @brief Adds a reference to the back of the array, with assigned value.
     *
     * Adds a reference to a new element located at the back of the vector.
     * Dynamically constructs a new object on the heap (using the value passed
     * to construct it) and returns the reference to the newly created element.
     *
     * This function's closest equivalents in std::vector<> are push_back() and
     * emplace_back(). However, these functions do not return a reference to the
     * newly created value. Manually allocating the pointer on the heap and pushing
     * it into an array of pointers or references, or using iterators, can work
     * around this difference in architecture.
     *
     * @param x the value to assign to the newly created value
     *
     * @return a reference to the newly created object
     */
    T &add(const T &x)
    {
        if(ulen==alen)
        {
            growbuf(ulen+1);
        }
        new (&buf[ulen]) T(x);
        return buf[ulen++];
    }

    /**
     * @brief Adds a reference to the back of the array.
     *
     * Adds a reference to a new element located at the back of the vector.
     * Dynamically allocates an object of type T on the heap (which will be
     * constructed if necessary) and returns the reference to the newly created
     * element.
     *
     * This function has no equivalent in the std::vector<> methods, since iterators
     * are used to represent addresses of values inside standard template library
     * containers. The closest equivalent is manually allocating the pointer on
     * the heap yourself, then pushing it back to a vector containing pointers or
     * references using push_back().
     *
     * @return a reference to the newly created object
     */
    T &add()
    {
        if(ulen==alen)
        {
            growbuf(ulen+1);
        }
        new (&buf[ulen]) T;
        return buf[ulen++];
    }

    /**
     * @brief Duplicates the last element in the array.
     *
     * Copies and assigns to a new element at the end of the vector the value that
     * the previous last member of the vector has. Expands the allocated length
     * of the vector as necessary if there is no room left.
     *
     * @return the value of the entry which was duplicated.
     */
    T &dup()
    {
        if(ulen==alen)
        {
            growbuf(ulen+1);
        }
        new (&buf[ulen]) T(buf[ulen-1]);
        return buf[ulen++];
    }

    /**
     * @brief Moves this vector into the passed vector and vise versa.
     *
     * Moves the values of *this into the vector pointed to by v, and moves the
     * values inside v into *this.
     *
     * @param v the vector to swap with
     */
    void move(vector<T> &v)
    {
        if(!ulen)
        {
            swap(buf, v.buf);
            swap(ulen, v.ulen);
            swap(alen, v.alen);
        }
        else
        {
            growbuf(ulen+v.ulen);
            if(v.ulen)
            {
                memcpy(&buf[ulen], (void  *)v.buf, v.ulen*sizeof(T));
            }
            ulen += v.ulen;
            v.ulen = 0;
        }
    }

    /**
     * @brief Returns whether the index passed is valid for this vector.
     *
     * Compares the size of the vector to the size_t given
     *
     * The std::vector equivalent is to compare the desired value to the size()
     * of the vector.
     *
     * @param i the index to validate
     *
     * @return true if the index is in range
     * @return false if the index is out of range
     */
    bool inrange(size_t i) const { return i<size_t(ulen); }

    /**
     * @brief Returns whether the index passed is valid for this vector.
     *
     * Compares the size of the vector to the integer given.
     *
     * The std::vector equivalent is to compare the desired value to the size()
     * of the vector.
     *
     * @param i the index to validate
     *
     * @return true if the index is in range
     * @return false if the index is out of range
     */
    bool inrange(int i) const { return i>=0 && i<ulen; }

    /**
     * @brief Removes and returns the last value.
     *
     * Shortens the end of the array by one and returns the value previously
     * stored at that location.
     *
     * @return the element that was popped off the end
     */
    T &pop() { return buf[--ulen]; }

    /**
     * @brief Returns the last value in the array.
     *
     * The std::vector<> equivalent of this method is back().
     *
     * @return the last element in the array
     */
    T &last() { return buf[ulen-1]; }

    /**
     * @brief Removes the last element from the vector.
     *
     * Deletes the last element of the vector, de-allocating it from the heap.
     *
     * The std::vector<> equivalent of this method is pop_back().
     *
     * @param
     */
    void drop() { ulen--; buf[ulen].~T(); }

    /**
     * @brief Queries whether the vector has no elements.
     *
     * Even if the vector has a large allocated memory size, this function returns
     * true unless there are accessible elements.
     *
     * The std::vector<> equivalent of this method is also called empty().
     *
     * @return true if the vector has no elements
     * @return false if the vector has elements
     */
    bool empty() const { return ulen==0; }

    /**
     * @brief Returns the number of objects the vector can hold.
     *
     * Returns the total allocated size of the vector, in terms of the size of
     * the object it is storing. This is equal to the maximum number of elements
     * the vector can hold without having to expand its allocated space and copy
     * its contents to a new array.
     *
     * The std::vector<> equivalent of this method is also called capacity().
     * @return the number of objects the vector can hold
     */
    int capacity() const { return alen; }

    /**
     * @brief Returns the size of the array.
     *
     * Returns the number of entries in the array.
     * The std::vector<> equivalent of this method is called size()
     *
     * @return the length of the array
     */
    int length() const { return ulen; }

    /**
     * @brief Returns the element at index i.
     *
     * Gets the ith element in the vector. Does not protect against the boundaries
     * of the vector, either below or above.
     *
     * @param i the index to get the element for
     *
     * @return the element at index i
     */
    T &operator[](int i) { return buf[i]; }

    /**
     * @brief Returns a const reference to the element at index i.
     *
     * Gets the ith element in the vector. Does not protect against the boundaries
     * of the vector, either below or above.
     *
     * @param i the index to get the element for
     *
     * @return a const reference to the element at index i
     */
    const T &operator[](int i) const { return buf[i]; }

    /**
     * @brief Transfers ownership of array to return value.
     *
     * The vector's return array is de-assigned from the vector object and passed
     * to the output.
     *
     * **This function will cause a memory leak if the return value is not assigned
     * to a variable. You must then call delete[] on this returned pointer to avoid
     * the memory leak.**
     */
    T *disown() { T *r = buf; buf = nullptr; alen = ulen = 0; return r; }

    /**
     * @brief Removes elements until i are left.
     *
     * The vector's values beyond i are all dropped. leaving a vector of size i
     * as the remainder.
     *
     * @param i the last index in the array to keep
     */
    void shrink(int i)
    {
        if(!std::is_class<T>::value)
        {
            ulen = i;
        }
        else
        {
            while(ulen>i)
            {
                drop();
            }
        }
    }

    /**
     * @brief Sets the used size of the array to i
     *
     * Sets the used size of the array to i, even if it is negative or larger than
     * the allocated length.
     */
    void setsize(int i) { ulen = i; }

    /**
     * @brief Deletes contents beyond the passed argument.
     *
     * The elements of the vector with an index above n are deleted. This method
     * is intended for vectors with non-array types; use deletearrays() instead
     * for vectors of arrays.
     *
     * @param n the last parameter to keep in the array
     */
    void deletecontents(int n = 0)
    {
        while(ulen > n)
        {
            delete pop();
        }
    }

    /**
     * @brief Deletes array contents beyond the passed argument.
     *
     * The elements of the vector with an index above n are deleted. This method
     * is intended for vectors with array types; use deletcontents() instead
     * for vectors of non-arrays.
     *
     * @param n the last parameter to keep in the array
     */
    void deletearrays(int n = 0)
    {
        while(ulen > n)
        {
            delete[] pop();
        }
    }

    /**
     * @brief Returns a pointer to the vector's internal array.
     *
     * Returns the address of the vector's internal array. If the vector's size
     * changes, this pointer will become invalidated.
     *
     * @return A pointer to the vector's internal array.
     */
    T *getbuf() { return buf; }

    /**
     * @brief Returns a pointer to const for the vector's internal array.
     *
     * Returns the address of the vector's internal array. If the vector's size
     * changes, this pointer will become invalidated. This vector points to const,
     * so it cannot change the values it sees.
     *
     * @return A pointer to const for the vector's internal array.
     */
    const T *getbuf() const { return buf; }

    /**
     * @brief returns whether the pointer is inside the vector's internal array
     *
     * Returns whether the pointer passed has an address that falls within
     * the used addresses of the vector. It will return false if the address is
     * only in the allocated buffer, or is not inside the address range at all.
     *
     * @param e the pointer to determine the status of
     *
     * @return true if the address is inside the vector's array
     * @return false if the address is outside the vector's array
     */
    bool inbuf(const T *e) const { return e >= buf && e < &buf[ulen]; }

    /**
     * @brief Sorts using the passed function between passed indices.
     *
     * Calls quicksort on `this` array, using the comparison function passed as
     * its first argument. If n = -1, the last element to be sorted is the last
     * element of the array. Other wise, the array is sorted between `i` and `n`.
     *
     * @param fun the function to sort elements with
     * @param i the first element to sort from
     * @param n the last element to sort until
     */
    template<class F>
    void sort(F fun, int i = 0, int n = -1)
    {
        quicksort(&buf[i], n < 0 ? ulen-i : n, fun);
    }

    /**
     * @brief Sorts the values of the array in ascending order.
     *
     * Runs the quicksort algorithm to sort the values in the vector in ascending
     * order. The std::vector<> equivalent of this operation is
     * std::sort(vector.begin(), vector.end()).
     *
     * This operation has a time complexity of n*log(n) on average and n^2 in the
     * worst-case scenario.
     */
    void sort() { sort(sortless()); }

    /**
     * @brief Sorts the values of the array in ascending order.
     *
     * Runs the quicksort algorithm to sort the values in the vector in ascending
     * order. This function calls sortnameless() which calls sortless(), so the
     * behavior is the same as sort()
     * The std::vector<> equivalent of this operation is
     * std::sort(vector.begin(), vector.end()).
     *
     * This operation has a time complexity of n*log(n) on average and n^2 in the
     * worst-case scenario.
     */
    void sortname() { sort(sortnameless()); }

    /**
     * @brief Expands the array to the requested size.
     *
     * If successful, the pointer to the new data array will be changed, invalidating
     * any existing pointers. Fails silently if the requested size is smaller than the
     * current allocated size.
     *
     * @param sz the new requested size of the array.
     */
    void growbuf(int sz)
    {
        int olen = alen;
        if(alen <= 0)
        {
            alen = max(MINSIZE, sz);
        }
        else
        {
            while(alen < sz)
            {
                alen += alen/2;
            }
        }
        if(alen <= olen)
        {
            return;
        }
        uchar *newbuf = new uchar[alen*sizeof(T)];
        if(olen > 0)
        {
            if(ulen > 0)
            {
                memcpy(newbuf, (void *)buf, ulen*sizeof(T));
            }
            delete[] (uchar *)buf;
        }
        buf = (T *)newbuf;
    }

    /**
     * @brief Expands the array by the amount requested.
     *
     * Expands the data array by the amount requested. Silently fails to expand
     * the array if it is already long enough. If this operation succeeds, the
     * pointers to the old array will become invalidated.
     *
     * @return the buffer starting at the old used length for sz entries
     */
    databuf<T> reserve(int sz)
    {
        if(alen-ulen < sz)
        {
            growbuf(ulen+sz);
        }
        return databuf<T>(&buf[ulen], sz);
    }

    /**
     * @brief Expands the array's used length by the amount given.
     *
     * @param sz the amount to increase the size of the array by
     */
    void advance(int sz)
    {
        ulen += sz;
    }

    /**
     * @brief Expands the array given the size of a buffer.
     *
     * @param p the buffer to get the length to expand by
     */
    void addbuf(const databuf<T> &p)
    {
        advance(p.length());
    }

    /**
     * @brief Adds n empty entries to the array.
     *
     * If this operation extends the vector beyond its original allocated length,
     * old array pointers will become invalidated.
     *
     * @param n the number of empty entries to add.
     */
    T *pad(int n)
    {
        T *buf = reserve(n).buf;
        advance(n);
        return buf;
    }

    /**
     * @brief Adds an element to the vector.
     *
     * This function is a wrapper around add() and has no other behavior.
     * The std::vector<> equivalent is either push_back() or emplace_back().
     *
     * @param v the element to add to the vector
     *
     */
    void put(const T &v) { add(v); }

    /**
     * @brief Adds n-1 empty entries after adding v.
     *
     * @param v the value to assign to the
     * @param n the number of total entries to add
     */
    void put(const T *v, int n)
    {
        databuf<T> buf = reserve(n);
        buf.put(v, n);
        addbuf(buf);
    }

    /**
     * @brief Removes n elements starting at index i.
     *
     * Removes elements (one-indexed) starting with the element after i and continuing
     * for n entries. Shifts entries beyond those removed by copying to lower array
     * indices.
     *
     * @param i the index after which elements are removed
     * @param n the number of elements to remove
     */
    void remove(int i, int n)
    {
        for(int p = i+n; p<ulen; p++)
        {
            buf[p-n] = buf[p];
        }
        ulen -= n;
    }

    /**
     * @brief Removes an element from the vector.
     *
     * Removes the ith element of the vector. If i is greater than the size of the
     * vector, the array is accessed out-of-bounds. The elements beyond the index
     * selected are all moved forward one space, and the element removed is returned
     * by value.
     *
     * The equivalent std::vector<> expression is to get the element's value with
     * operator[] or at() and then to remove the element with erase().
     *
     * @param i the index of the element to remove
     *
     * @return the value of the element removed
     */
    T remove(int i)
    {
        T e = buf[i];
        for(int p = i+1; p<ulen; p++)
        {
            buf[p-1] = buf[p];
        }
        ulen--;
        return e;
    }

    /**
     * @brief Removes the element at index i, non-order preserving.
     *
     * Removes the ith element of the vector, copying the last element into its
     * location. This changes the resulting order of the vector, while avoiding
     * a more costly shift of vector entries from i upwards.
     *
     * @param i the index of the value to remove
     *
     * @return the value of the element removed
     */
    T removeunordered(int i)
    {
        T e = buf[i];
        ulen--;
        if(ulen>0)
        {
            buf[i] = buf[ulen];
        }
        return e;
    }

    /**
     * @brief Finds the argument inside this array.
     *
     * Returns the index of the vector where the argument is first found.
     * If the object is not found, -1 is returned for the index.
     *
     * @param o the object to find in the array
     *
     * @return the index of the object found int the array
     */
    template<class U>
    int find(const U &o)
    {
        for(int i = 0; i < ulen; ++i)
        {
            if(buf[i]==o)
            {
                return i;
            }
        }
        return -1;
    }

    /**
     * @brief Adds an element iff it is not in the list.
     *
     * This method searches for an element with the same value, and adds a new
     * element containing the value passed iff there is no identical element already
     * present. Fails silently if the element already exists in the vector.
     *
     * @param o The object to attempt to add.
     */
    void addunique(const T &o)
    {
        if(find(o) < 0)
        {
            add(o);
        }
    }

    /**
     * @brief Removes element by searching.
     *
     * Attempts to remove an element that compares equal to the argument passed.
     * Only removes the first element found, if multiple identical entries to
     * the argument are found.
     *
     * @param o The object to search for to delete.
     */
    void removeobj(const T &o)
    {
        for(int i = 0; i < static_cast<int>(ulen); ++i)
        {
            if(buf[i] == o)
            {
                int dst = i;
                for(int j = i+1; j < ulen; j++)
                {
                    if(!(buf[j] == o))
                    {
                        buf[dst++] = buf[j];
                    }
                }
                setsize(dst);
                break;
            }
        }
    }

    /**
     * @brief Removes element by searching, non order-preserving.
     *
     * Attempts to remove an element that compares equal to the argument passed,
     * replacing it with the last element. If multiple identical entries to the
     * argument are found, only the first element is removed.
     *
     * This function acts similarly to removeobj() but does not preserve the order
     * of the resulting vector; it is faster however by not having to shift elements
     * i and upward to preserve the order.
     *
     * @param o THe object to search for to delete
     */
    void replacewithlast(const T &o)
    {
        if(!ulen) //do not operate on empty vector
        {
            return;
        }
        for(int i = 0; i < static_cast<int>(ulen-1); ++i) //loop through
        {
            if(buf[i]==o) //compare to passed item
            {
                buf[i] = buf[ulen-1]; //move last element
                break;
            }
            ulen--; //move index down one
        }
    }

    /**
     * @brief Inserts an element at the specified index.
     *
     * Adds an element to the vector at the specified location. The std::vector<>
     * equivalent is also called insert() and has essentially the same semantics,
     * except that std::vector<> returns an iterator and this function returns a
     * copy of the data.
     *
     * @param i index to insert at
     * @param e element to insert
     *
     * @return the value that was inserted into the array
     */
    T &insert(int i, const T &e)
    {
        add(T()); //add to end of vector
        for(int p = ulen-1; p>i; p--) //from end to index, shift elements forwards one
        {
            buf[p] = buf[p-1];
        }
        buf[i] = e; //insert at ith location in buffer
        return buf[i]; //return the value assigned
    }

    /**
     * @brief Inserts n elements at the specified index.
     *
     * Adds n elements to the vector at the specified index. Equivalent to calling
     * insert(int, const T) multiple times with i, i+1, i+2... i+n, except slightly
     * more efficient. The same value e is assigned to all n entries created.
     *
     * @param i index to insert at
     * @param e element to insert
     * @param n number of elements to add
     *
     */
    T *insert(int i, const T *e, int n)
    {
        if(alen-ulen < n) //expand buffer if needed
        {
            growbuf(ulen+n);
        }
        for(int j = 0; j < n; ++j) //add n elements to end
        {
            add(T());
        }
        for(int p = ulen-1; p>=i+n; p--) //for descending elements after i+n (new elements) move upwards by n
        {
            buf[p] = buf[p-n];
        }
        for(int j = 0; j < n; ++j) //set value of n elements to e
        {
            buf[i+j] = e[j];
        }
        return &buf[i]; //return first newly allocated
    }

    /**
     * @brief Reverses the order of the vector.
     *
     * Flips the first n/2 (rounded down) elements with the back n/2 elements,
     * changing the vector's array so that it is in reverse.
     */
    void reverse()
    {
        for(int i = 0; i < static_cast<int>(ulen/2); ++i)
        {
            swap(buf[i], buf[ulen-1-i]);
        }
    }

    /**
     * @brief Returns the binary heap parent of the index
     *
     * For this result to make sense, the vector must be put in heap mode using
     * buildheap().
     *
     * @param i the index to query
     */
    static int heapparent(int i) { return (i - 1) >> 1; }

    /**
     * @brief returns the first binary heap child of the index
     *
     * For this result to make sense, the vector must be put in heap mode using
     * buildheap().
     *
     * The second heap child is located at the return value plus one.
     *
     * @param i the index to query
     */
    static int heapchild(int i) { return (i << 1) + 1; }

    /**
     * @brief Puts the vector in heap mode.
     */
    void buildheap()
    {
        for(int i = ulen/2; i >= 0; i--) downheap(i);
    }

    /**
     * @brief Compares up the vector heap.
     *
     * Used when adding a member to the heap. Ensures that upstream members all
     * satisfy the binary heap condition and moves members accordingly.
     *
     * @param i the index to evaluate
     *
     * @return the index representing the same data as the input index
     */
    int upheap(int i)
    {
        float score = heapscore(buf[i]);
        while(i > 0)
        {
            int pi = heapparent(i);
            if(score >= heapscore(buf[pi]))
            {
                break;
            }
            swap(buf[i], buf[pi]);
            i = pi;
        }
        return i;
    }

    /**
     * @brief Adds a number to the heap.
     *
     * Adds a new member at the end of the heap, and then compares it with its
     * parents to place it in the correct part of the heap.
     *
     * @param x the object to add to the heap
     */
    T &addheap(const T &x)
    {
        add(x);
        return buf[upheap(ulen-1)];
    }

    /**
     * @brief Checks down the heap for valid children.
     *
     * Moves elements around the heap beneath the given index.
     *
     * @param i index to evaluate
     *
     * @return the index representing the same data as the input index
     */
    int downheap(int i)
    {
        float score = heapscore(buf[i]);
        for(;;)
        {
            int ci = heapchild(i);
            if(ci >= ulen)
            {
                break;
            }
            float cscore = heapscore(buf[ci]);
            if(score > cscore)
            {
               if(ci+1 < ulen && heapscore(buf[ci+1]) < cscore)
               {
                   swap(buf[ci+1], buf[i]); i = ci+1;
               }
               else
               {
                   swap(buf[ci], buf[i]); i = ci;
               }
            }
            else if(ci+1 < ulen && heapscore(buf[ci+1]) < score)
            {
                swap(buf[ci+1], buf[i]); i = ci+1;
            }
            else break;
        }
        return i;
    }

    /**
     * @brief Removes the first element from the heap.
     *
     * Removes the element at the top of the heap. Recalculates the heap structure
     * after removing the top element.
     *
     * @return the value that was removed
     */
    T removeheap()
    {
        T e = removeunordered(0);
        if(ulen)
        {
            downheap(0);
        }
        return e;
    }

    /**
     * @brief finds the key from a hashtable in the vector.
     *
     * the implementation takes advantage of the numerous overloads
     * of the htcmp() function. Note that this means the generic parameter
     * can be one of the following:
     * - const char *
     * - const stringslice
     * - int
     * - GLuint
     *
     * @returns the index of the element if it exists; -1 otherwise
     */
    template<class K>
    int htfind(const K &key)
    {
        for(int i = 0; i < static_cast<int>(ulen); ++i)
        {
            if(htcmp(key, buf[i]))
            {
                return i;
            }
        }
        return -1;
    }

    #define UNIQUE(overwrite, cleanup) \
        for(int i = 1; i < ulen; i++) \
            if(htcmp(buf[i-1], buf[i])) \
            { \
                int n = i; \
                while(++i < ulen) \
                { \
                    if(!htcmp(buf[n-1], buf[i])) \
                    { \
                        overwrite; \
                        n++; \
                    } \
                } \
                cleanup; \
                break; \
            }

    /**
     * @brief removes every duplicate **stack allocated value** from the vector.
     *
     * Contents must be initally sorted.
     * Duplicated items get deleted via a call to setsize().
     *
     * **It may leak memory if used with heap allocated and array items.**
     * **see uniquedeletecontents() and uniquedeletearrays() for that case**
     *
    */
    void unique() // contents must be initially sorted
    {
        UNIQUE(buf[n] = buf[i], setsize(n));
    }

    /**
     * @brief removes every duplicate **heap-allocated value** from the vector.
     *
     * Duplicated items get deleted via deletecontents().
     * for the equivalent function for stack values see unique().
     * for the equivalent function for array values see uniquedeletearrays().
    */
    void uniquedeletecontents()
    {
        UNIQUE(swap(buf[n], buf[i]), deletecontents(n));
    }

    /**
     * @brief removes every duplicate **array value** from the vector.
     *
     * Duplicated items get deleted via deletearrays().
     * for the equivalent function for stack values see unique().
     * for the equivalent function for heap-allocated values see uniquedeletecontents().
     */
    void uniquedeletearrays()
    {
        UNIQUE(swap(buf[n], buf[i]), deletearrays(n));
    }
    #undef UNIQUE
};

template<class H, class E, class K, class T>
struct hashbase
{
    typedef E elemtype;
    typedef K keytype;
    typedef T datatype;

    enum { CHUNKSIZE = 64 };

    struct chain { E elem; chain *next; };
    struct chainchunk { chain chains[CHUNKSIZE]; chainchunk *next; };

    int size;
    int numelems;
    chain **chains;

    chainchunk *chunks;
    chain *unused;

    enum { DEFAULTSIZE = 1<<10 };

    hashbase(int size = DEFAULTSIZE)
      : size(size)
    {
        numelems = 0;
        chunks = nullptr;
        unused = nullptr;
        chains = new chain *[size];
        memset(chains, 0, size*sizeof(chain *));
    }

    ~hashbase()
    {
        DELETEA(chains);
        deletechunks();
    }

    chain *insert(uint h)
    {
        if(!unused)
        {
            chainchunk *chunk = new chainchunk;
            chunk->next = chunks;
            chunks = chunk;
            for(int i = 0; i < static_cast<int>(CHUNKSIZE-1); ++i)
            {
                chunk->chains[i].next = &chunk->chains[i+1];
            }
            chunk->chains[CHUNKSIZE-1].next = unused;
            unused = chunk->chains;
        }
        chain *c = unused;
        unused = unused->next;
        c->next = chains[h];
        chains[h] = c;
        numelems++;
        return c;
    }

    template<class U>
    T &insert(uint h, const U &key)
    {
        chain *c = insert(h);
        H::setkey(c->elem, key);
        return H::getdata(c->elem);
    }

    #define HTFIND(success, fail) \
        uint h = hthash(key)&(this->size-1); \
        for(chain *c = this->chains[h]; c; c = c->next) \
        { \
            if(htcmp(key, H::getkey(c->elem))) \
            { \
                return success H::getdata(c->elem); \
            } \
        } \
        return (fail);

    template<class U>
    T *access(const U &key)
    {
        HTFIND(&, nullptr);
    }

    template<class U, class V>
    T &access(const U &key, const V &elem)
    {
        HTFIND( , insert(h, key) = elem);
    }

    template<class U>
    T &operator[](const U &key)
    {
        HTFIND( , insert(h, key));
    }

    template<class U>
    T &find(const U &key, T &notfound)
    {
        HTFIND( , notfound);
    }

    template<class U>
    const T &find(const U &key, const T &notfound)
    {
        HTFIND( , notfound);
    }

    template<class U>
    bool remove(const U &key)
    {
        uint h = hthash(key)&(size-1);
        for(chain **p = &chains[h], *c = chains[h]; c; p = &c->next, c = c->next)
        {
            if(htcmp(key, H::getkey(c->elem)))
            {
                *p = c->next;
                c->elem.~E();
                new (&c->elem) E;
                c->next = unused;
                unused = c;
                numelems--;
                return true;
            }
        }
        return false;
    }

    void recycle()
    {
        if(!numelems)
        {
            return;
        }
        for(int i = 0; i < static_cast<int>(size); ++i)
        {
            chain *c = chains[i];
            if(!c)
            {
                continue;
            }
            for(;;)
            {
                htrecycle(c->elem);
                if(!c->next)
                {
                    break;
                }
                c = c->next;
            }
            c->next = unused;
            unused = chains[i];
            chains[i] = nullptr;
        }
        numelems = 0;
    }

    void deletechunks()
    {
        for(chainchunk *nextchunk; chunks; chunks = nextchunk)
        {
            nextchunk = chunks->next;
            delete chunks;
        }
    }

    void clear()
    {
        if(!numelems)
        {
            return;
        }
        memset(chains, 0, size*sizeof(chain *));
        numelems = 0;
        unused = nullptr;
        deletechunks();
    }

    static inline chain *enumnext(void *i) { return ((chain *)i)->next; }
    static inline K &enumkey(void *i) { return H::getkey(((chain *)i)->elem); }
    static inline T &enumdata(void *i) { return H::getdata(((chain *)i)->elem); }
};

template<class T>
inline void htrecycle(const T &) {}

template<class T>
struct hashset : hashbase<hashset<T>, T, T, T>
{
    typedef hashbase<hashset<T>, T, T, T> basetype;

    hashset(int size = basetype::DEFAULTSIZE) : basetype(size) {}

    static inline const T &getkey(const T &elem) { return elem; }
    static inline T &getdata(T &elem) { return elem; }
    template<class K>
    static inline void setkey(T &, const K &) {}

    template<class V>
    T &add(const V &elem)
    {
        return basetype::access(elem, elem);
    }
};

template<class T>
struct hashnameset : hashbase<hashnameset<T>, T, const char *, T>
{
    typedef hashbase<hashnameset<T>, T, const char *, T> basetype;

    hashnameset(int size = basetype::DEFAULTSIZE) : basetype(size) {}

    template<class U>
    static inline const char *getkey(const U &elem) { return elem.name; }

    template<class U>
    static inline const char *getkey(U *elem) { return elem->name; }

    static inline T &getdata(T &elem) { return elem; }

    template<class K>
    static inline void setkey(T &, const K &) {}

    template<class V>
    T &add(const V &elem)
    {
        return basetype::access(getkey(elem), elem);
    }
};

template<class K, class T>
struct hashtableentry
{
    K key;
    T data;
};

template<class K, class T>
inline void htrecycle(hashtableentry<K, T> &entry)
{
    htrecycle(entry.key);
    htrecycle(entry.data);
}

template<class K, class T>
struct hashtable : hashbase<hashtable<K, T>, hashtableentry<K, T>, K, T>
{
    typedef hashbase<hashtable<K, T>, hashtableentry<K, T>, K, T> basetype;
    typedef typename basetype::elemtype elemtype;

    hashtable(int size = basetype::DEFAULTSIZE) : basetype(size) {}

    static inline K &getkey(elemtype &elem) { return elem.key; }
    static inline T &getdata(elemtype &elem) { return elem.data; }

    template<class U>
    static inline void setkey(elemtype &elem, const U &key) { elem.key = key; }
};

//ht stands for a hash table
#define ENUMERATE_KT(ht,k,e,t,f,b) \
    for(int i = 0; i < static_cast<int>((ht).size); ++i) \
    { \
        for(void *ec = (ht).chains[i]; ec;) \
        { \
            k &e = (ht).enumkey(ec); \
            t &f = (ht).enumdata(ec); \
            ec = (ht).enumnext(ec); \
            b; \
        } \
    }
#define ENUMERATE(ht,t,e,b) \
    for(int i = 0; i < static_cast<int>((ht).size); ++i) \
    { \
        for(void *ec = (ht).chains[i]; ec;) \
        { \
            t &e = (ht).enumdata(ec); \
            ec = (ht).enumnext(ec); \
            b; \
        } \
    }

template <class T, int SIZE>
struct queue
{
    int head, tail, len;
    T data[SIZE];

    queue() { clear(); }

    void clear() { head = tail = len = 0; }

    int length() const { return len; }
    bool empty() const { return !len; }
    bool full() const { return len == SIZE; }

    bool inrange(size_t i) const { return i<size_t(len); }
    bool inrange(int i) const { return i>=0 && i<len; }

    T &added() { return data[tail > 0 ? tail-1 : SIZE-1]; }
    T &added(int offset) { return data[tail-offset > 0 ? tail-offset-1 : tail-offset-1 + SIZE]; }
    T &adding() { return data[tail]; }
    T &adding(int offset) { return data[tail+offset >= SIZE ? tail+offset - SIZE : tail+offset]; }
    T &add()
    {
        T &t = data[tail];
        tail++;
        if(tail >= SIZE) tail -= SIZE;
        if(len < SIZE) len++;
        return t;
    }
    T &add(const T &e) { return add() = e; }

    T &pop()
    {
        tail--;
        if(tail < 0)
        {
            tail += SIZE;
        }
        len--;
        return data[tail];
    }

    T &removing() { return data[head]; }
    T &removing(int offset) { return data[head+offset >= SIZE ? head+offset - SIZE : head+offset]; }
    T &remove()
    {
        T &t = data[head];
        head++;
        if(head >= SIZE)
        {
            head -= SIZE;
        }
        len--;
        return t;
    }

    T &operator[](int offset) { return removing(offset); }
    const T &operator[](int offset) const { return removing(offset); }
};

template <class T, int SIZE>
struct reversequeue : queue<T, SIZE>
{
    T &operator[](int offset) { return queue<T, SIZE>::added(offset); }
    const T &operator[](int offset) const { return queue<T, SIZE>::added(offset); }
};

#ifdef SDL_BYTEORDER
    #define endianswap16 SDL_Swap16
    #define endianswap32 SDL_Swap32
    #define endianswap64 SDL_Swap64
#else
inline ushort endianswap16(ushort n) { return (n<<8) | (n>>8); }
inline uint endianswap32(uint n) { return (n<<24) | (n>>24) | ((n>>8)&0xFF00) | ((n<<8)&0xFF0000); }
inline ullong endianswap64(ullong n) { return endianswap32(static_cast<uint>(n >> 32)) | ((ullong)endianswap32(static_cast<uint>(n)) << 32); }
#endif

template<class T>
inline T endianswap(T n) { union { T t; uint i; } conv; conv.t = n; conv.i = SDL_Swap32(conv.i); return conv.t; }

template<>
inline uint endianswap<uint>(uint n) { return endianswap32(n); }

template<>
inline int endianswap<int>(int n) { return endianswap32(n); }

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
    //see file/gz/utf8/zipstream children for more interesting forms
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

template<class T>
struct streambuf
{
    stream *s;

    streambuf(stream *s) : s(s) {}

    T get() { return s->get<T>(); }
    size_t get(T *vals, size_t numvals) { return s->get(vals, numvals); }
    void put(const T &val) { s->put(&val, 1); }
    void put(const T *vals, size_t numvals) { s->put(vals, numvals); }
    size_t length() { return s->size(); }
};

enum
{
    CubeType_Print   = 1 << 0,
    CubeType_Space   = 1 << 1,
    CubeType_Digit   = 1 << 2,
    CubeType_Alpha   = 1 << 3,
    CubeType_Lower   = 1 << 4,
    CubeType_Upper   = 1 << 5,
    CubeType_Unicode = 1 << 6
};
extern const uchar cubectype[256];
inline int iscubeprint(uchar c) { return cubectype[c] & CubeType_Print; }
inline int iscubespace(uchar c) { return cubectype[c] & CubeType_Space; }
inline int iscubealpha(uchar c) { return cubectype[c] & CubeType_Alpha; }
inline int iscubealnum(uchar c) { return cubectype[c]&(CubeType_Alpha | CubeType_Digit); }
inline int iscubelower(uchar c) { return cubectype[c] & CubeType_Lower; }
inline int iscubeupper(uchar c) { return cubectype[c] & CubeType_Upper; }
inline int iscubepunct(uchar c) { return cubectype[c] == CubeType_Print; }
inline int cube2uni(uchar c)
{
    extern const int cube2unichars[256];
    return cube2unichars[c];
}
inline uchar uni2cube(int c)
{
    extern const int uni2cubeoffsets[8];
    extern const uchar uni2cubechars[];
    return static_cast<uint>(c) <= 0x7FF ? uni2cubechars[uni2cubeoffsets[c>>8] + (c&0xFF)] : 0;
}
inline uchar cubelower(uchar c)
{
    extern const uchar cubelowerchars[256];
    return cubelowerchars[c];
}
inline uchar cubeupper(uchar c)
{
    extern const uchar cubeupperchars[256];
    return cubeupperchars[c];
}
extern size_t decodeutf8(uchar *dst, size_t dstlen, const uchar *src, size_t srclen, size_t *carry = nullptr);
extern size_t encodeutf8(uchar *dstbuf, size_t dstlen, const uchar *srcbuf, size_t srclen, size_t *carry = nullptr);

extern string homedir;

extern char *makerelpath(const char *dir, const char *file, const char *prefix = nullptr, const char *cmd = nullptr);
extern char *path(char *s);
extern char *copypath(const char *s);
extern const char *parentdir(const char *directory);
extern bool fileexists(const char *path, const char *mode);
extern bool createdir(const char *path);
extern size_t fixpackagedir(char *dir);
extern const char *sethomedir(const char *dir);
extern const char *addpackagedir(const char *dir);
extern const char *findfile(const char *filename, const char *mode);
extern bool findzipfile(const char *filename);
extern stream *openrawfile(const char *filename, const char *mode);
extern stream *openzipfile(const char *filename, const char *mode);
extern stream *openfile(const char *filename, const char *mode);
extern stream *opentempfile(const char *filename, const char *mode);
extern stream *opengzfile(const char *filename, const char *mode, stream *file = nullptr, int level = Z_BEST_COMPRESSION);
extern stream *openutf8file(const char *filename, const char *mode, stream *file = nullptr);
extern char *loadfile(const char *fn, size_t *size, bool utf8 = true);
extern bool listdir(const char *dir, bool rel, const char *ext, vector<char *> &files);
extern int listfiles(const char *dir, const char *ext, vector<char *> &files);
extern int listzipfiles(const char *dir, const char *ext, vector<char *> &files);


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
    p.put((uchar *)&f, sizeof(float));
}

extern void putint(ucharbuf &p, int n);
extern void putint(vector<uchar> &p, int n);
extern int getint(ucharbuf &p);
extern void putuint(ucharbuf &p, int n);
extern void putuint(vector<uchar> &p, int n);
extern int getuint(ucharbuf &p);
extern void putfloat(ucharbuf &p, float f);
extern void putfloat(vector<uchar> &p, float f);
extern float getfloat(ucharbuf &p);
extern void sendstring(const char *t, ucharbuf &p);
extern void sendstring(const char *t, vector<uchar> &p);
extern void getstring(char *t, ucharbuf &p, size_t len);

template<size_t N>
inline void getstring(char (&t)[N], ucharbuf &p) { getstring(t, p, N); }

extern void filtertext(char *dst, const char *src, bool whitespace, bool forcespace, size_t len);

template<size_t N>
inline void filtertext(char (&dst)[N], const char *src, bool whitespace = true, bool forcespace = false) { filtertext(dst, src, whitespace, forcespace, N-1); }

#endif /* TOOLS_H_ */
