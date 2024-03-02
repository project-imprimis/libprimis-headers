/**
 * @file geom.h
 * @brief Physical geometry objects
 *
 * This file describes objects which represent concrete geometric constructions,
 * such as vectors that represent 2D, 3D, and 4D Cartesian coordinates, as well
 * as more complex geometric structures such as quaternions and matrices.
 */

#ifndef GEOM_H_
#define GEOM_H_

struct vec;

template<typename T>
struct vec4;

struct plane;
struct quat;
struct dualquat;
class matrix3;
struct matrix4x3;
struct matrix4;


/**
 * @brief 2 dimensional vector object
 *
 * with relevant operators
 */
struct vec2
{
    union
    {
        struct
        {
            float x, y;
        };
        float v[2];
    };

    vec2() {}
    vec2(float x, float y) : x(x), y(y) {}
    explicit vec2(const vec &v);
    explicit vec2(const vec4<float> &v);

    float &operator[](int i)       { return v[i]; }
    float  operator[](int i) const { return v[i]; }

    bool operator==(const vec2 &o) const { return x == o.x && y == o.y; }
    bool operator!=(const vec2 &o) const { return x != o.x || y != o.y; }

    bool iszero() const { return x==0 && y==0; }
    float dot(const vec2 &o) const  { return x*o.x + y*o.y; }
    float squaredlen() const { return dot(*this); }
    float magnitude() const  { return sqrtf(squaredlen()); }
    vec2 &normalize() { mul(1/magnitude()); return *this; }
    vec2 &safenormalize() { float m = magnitude(); if(m) mul(1/m); return *this; }
    float cross(const vec2 &o) const { return x*o.y - y*o.x; }
    float squaredist(const vec2 &e) const { return vec2(*this).sub(e).squaredlen(); }
    float dist(const vec2 &e) const { return sqrtf(squaredist(e)); }

    vec2 &mul(float f)       { x *= f; y *= f; return *this; }
    vec2 &mul(const vec2 &o) { x *= o.x; y *= o.y; return *this; }
    vec2 &square()           { mul(*this); return *this; }
    vec2 &div(float f)       { x /= f; y /= f; return *this; }
    vec2 &div(const vec2 &o) { x /= o.x; y /= o.y; return *this; }
    vec2 &recip()            { x = 1/x; y = 1/y; return *this; }
    vec2 &add(float f)       { x += f; y += f; return *this; }
    vec2 &add(const vec2 &o) { x += o.x; y += o.y; return *this; }
    vec2 &sub(float f)       { x -= f; y -= f; return *this; }
    vec2 &sub(const vec2 &o) { x -= o.x; y -= o.y; return *this; }
    vec2 &neg()              { x = -x; y = -y; return *this; }
    vec2 &min(const vec2 &o) { x = ::min(x, o.x); y = ::min(y, o.y); return *this; }
    vec2 &max(const vec2 &o) { x = ::max(x, o.x); y = ::max(y, o.y); return *this; }
    vec2 &min(float f)       { x = ::min(x, f); y = ::min(y, f); return *this; }
    vec2 &max(float f)       { x = ::max(x, f); y = ::max(y, f); return *this; }
    vec2 &abs() { x = fabs(x); y = fabs(y); return *this; }
    vec2 &clamp(float l, float h) { x = ::std::clamp(x, l, h); y = ::std::clamp(y, l, h); return *this; }
    vec2 &reflect(const vec2 &n) { float k = 2*dot(n); x -= k*n.x; y -= k*n.y; return *this; }
    vec2 &lerp(const vec2 &b, float t) { x += (b.x-x)*t; y += (b.y-y)*t; return *this; }
    vec2 &lerp(const vec2 &a, const vec2 &b, float t) { x = a.x + (b.x-a.x)*t; y = a.y + (b.y-a.y)*t; return *this; }
    vec2 &avg(const vec2 &b) { add(b); mul(0.5f); return *this; }

    vec2 operator+(const vec2 &v2) const
    {
        return vec2(x+v2.x, y+v2.y);
    }

    vec2 operator-(const vec2 &v2) const
    {
        return vec2(x-v2.x, y-v2.y);
    }

    vec2 operator-() const
    {
        return vec2(-x, -y);
    }

    template<typename T>
    vec2 operator*(const T &n)
    {
        return vec2(n*x, n*y);
    }

    vec2 operator*(const vec2 &v2)
    {
        return vec2(x*v2.x, y*v2.y);
    }

    template<typename T>
    vec2 operator/(const T &n)
    {
        return vec2(x/n, y/n);
    }

    vec2 operator/(const vec2 &v2)
    {
        return vec2(x/v2.x, y/v2.y);
    }

    template<class B>
    vec2 &madd(const vec2 &a, const B &b) { return add(vec2(a).mul(b)); }

    template<class B>
    vec2 &msub(const vec2 &a, const B &b) { return sub(vec2(a).mul(b)); }

    vec2 &rotate_around_z(float c, float s) { float rx = x, ry = y; x = c*rx-s*ry; y = c*ry+s*rx; return *this; }
    vec2 &rotate_around_z(float angle) { return rotate_around_z(cosf(angle), std::sin(angle)); }
    vec2 &rotate_around_z(const vec2 &sc) { return rotate_around_z(sc.x, sc.y); }
};

struct ivec;
struct svec;

/**
 * @brief three dimensional vector object
 *
 * this object finds uses in nearly every part of the engine,
 * including world geometry, mapmodels, particles, projectiles, colors, etc
 */
struct vec
{
    union
    {
        struct
        {
            float x, y, z;
        };
        struct
        {
            float r, g, b;
        };
        float v[3];
    };

    vec() {}
    explicit vec(int a) : x(a), y(a), z(a) {}
    explicit vec(float a) : x(a), y(a), z(a) {}
    vec(float a, float b, float c) : x(a), y(b), z(c) {}
    explicit vec(int v[3]) : x(v[0]), y(v[1]), z(v[2]) {}
    explicit vec(const float *v) : x(v[0]), y(v[1]), z(v[2]) {}
    explicit vec(const vec2 &v, float z = 0) : x(v.x), y(v.y), z(z) {}
    explicit vec(const vec4<float> &v);
    explicit vec(const ivec &v);
    explicit vec(const svec &v);

    vec(float yaw, float pitch) : x(-std::sin(yaw)*cosf(pitch)), y(cosf(yaw)*cosf(pitch)), z(std::sin(pitch)) {}
    vec &set(int i, float f) { v[i] = f; return *this; }

    //operator overloads
    float &operator[](int i)       { return v[i]; }
    float  operator[](int i) const { return v[i]; }
    bool operator==(const vec &o) const { return x == o.x && y == o.y && z == o.z; }
    bool operator!=(const vec &o) const { return x != o.x || y != o.y || z != o.z; }

    vec operator+(const vec &v2)
    {
        return vec(x+v2.x, y+v2.y, z+v2.z);
    }

    vec operator-(const vec &v2)
    {
        return vec(x-v2.x, y-v2.y, z-v2.z);
    }

    vec operator-()
    {
        return vec(-x, -y, -z);
    }

    template<typename T>
    vec operator*(const T &n)
    {
        return vec(n*x, n*y, n*z);
    }

    vec operator*(const vec &v2)
    {
        return vec(x*v2.x, y*v2.y, z*v2.z);
    }

    template<typename T>
    vec operator/(const T &n)
    {
        return vec(x/n, y/n, z/n);
    }

    vec operator/(const vec &v2)
    {
        return vec(x/v2.x, y/v2.y, z/v2.z);
    }


    //unary operators
    bool iszero() const { return x==0 && y==0 && z==0; }
    float squaredlen() const { return x*x + y*y + z*z; }
    vec &square()            { mul(*this); return *this; }
    vec &neg2()              { x = -x; y = -y; return *this; } //unused
    vec &neg()               { x = -x; y = -y; z = -z; return *this; } //overloaded by operator-()
    vec &abs() { x = fabs(x); y = fabs(y); z = fabs(z); return *this; }
    vec &recip()             { x = 1/x; y = 1/y; z = 1/z; return *this; } //used twice
    float magnitude2() const { return sqrtf(dot2(*this)); }
    float magnitude() const  { return sqrtf(squaredlen()); }
    vec &normalize()         { div(magnitude()); return *this; }
    vec &safenormalize()     { float m = magnitude(); if(m) div(m); return *this; }
    bool isnormalized() const { float m = squaredlen(); return (m>0.99f && m<1.01f); }

    //elementwise float operators
    vec &mul(float f)        { x *= f; y *= f; z *= f; return *this; }
    vec &mul2(float f)       { x *= f; y *= f; return *this; } //unused
    vec &div(float f)        { x /= f; y /= f; z /= f; return *this; }
    vec &div2(float f)       { x /= f; y /= f; return *this; } //unused
    vec &add(float f)        { x += f; y += f; z += f; return *this; }
    vec &add2(float f)       { x += f; y += f; return *this; } //used once
    vec &addz(float f)       { z += f; return *this; } //unused
    vec &sub(float f)        { x -= f; y -= f; z -= f; return *this; }
    vec &sub2(float f)       { x -= f; y -= f; return *this; } //unused
    vec &subz(float f)       { z -= f; return *this; } //unused
    vec &min(float f)        { x = ::min(x, f); y = ::min(y, f); z = ::min(z, f); return *this; }
    vec &max(float f)        { x = ::max(x, f); y = ::max(y, f); z = ::max(z, f); return *this; }
    vec &clamp(float l, float h) { x = ::std::clamp(x, l, h); y = ::std::clamp(y, l, h); z = ::std::clamp(z, l, h); return *this; }

    //elementwise vector operators
    vec &mul(const vec &o)   { x *= o.x; y *= o.y; z *= o.z; return *this; }
    vec &div(const vec &o)   { x /= o.x; y /= o.y; z /= o.z; return *this; }
    vec &add(const vec &o)   { x += o.x; y += o.y; z += o.z; return *this; }
    vec &sub(const vec &o)   { x -= o.x; y -= o.y; z -= o.z; return *this; }
    vec &min(const vec &o)   { x = ::min(x, o.x); y = ::min(y, o.y); z = ::min(z, o.z); return *this; }
    vec &max(const vec &o)   { x = ::max(x, o.x); y = ::max(y, o.y); z = ::max(z, o.z); return *this; }

    //dot products
    float dot2(const vec2 &o) const { return x*o.x + y*o.y; }
    float dot2(const vec &o) const { return x*o.x + y*o.y; }
    float dot(const vec &o) const { return x*o.x + y*o.y + z*o.z; }
    float squaredot(const vec &o) const { float k = dot(o); return k*k; } //unused
    float absdot(const vec &o) const { return fabs(x*o.x) + fabs(y*o.y) + fabs(z*o.z); } //used once
    float zdot(const vec &o) const { return z*o.z; } //unused

    //distances
    float squaredist(const vec &e) const { return vec(*this).sub(e).squaredlen(); }
    float dist(const vec &e) const { return sqrtf(squaredist(e)); }
    float dist(const vec &e, vec &t) const { t = *this; t.sub(e); return t.magnitude(); }
    float dist2(const vec &o) const { float dx = x-o.x, dy = y-o.y; return sqrtf(dx*dx + dy*dy); }

    //cross products
    template<class T>
    bool reject(const T &o, float r) const { return x>o.x+r || x<o.x-r || y>o.y+r || y<o.y-r; }

    template<class A, class B>
    vec &cross(const A &a, const B &b) { x = a.y*b.z-a.z*b.y; y = a.z*b.x-a.x*b.z; z = a.x*b.y-a.y*b.x; return *this; }

    vec &cross(const vec &o, const vec &a, const vec &b) { return cross(vec(a).sub(o), vec(b).sub(o)); }

    /**
     * @brief scalar triple product A*(BxC)
     */
    float scalartriple(const vec &a, const vec &b) const { return x*(a.y*b.z-a.z*b.y) + y*(a.z*b.x-a.x*b.z) + z*(a.x*b.y-a.y*b.x); }
    float zscalartriple(const vec &a, const vec &b) const { return z*(a.x*b.y-a.y*b.x); } //unused

    //transformations
    vec &reflectz(float rz) { z = 2*rz - z; return *this; }
    vec &reflect(const vec &n) { float k = 2*dot(n); x -= k*n.x; y -= k*n.y; z -= k*n.z; return *this; }
    vec &project(const vec &n) { float k = dot(n); x -= k*n.x; y -= k*n.y; z -= k*n.z; return *this; }
    vec &projectxydir(const vec &n) { if(n.z) z = -(x*n.x/n.z + y*n.y/n.z); return *this; } //used once
    vec &projectxy(const vec &n)
    {
        float m = squaredlen(), k = dot(n);
        projectxydir(n);
        rescale(sqrtf(::max(m - k*k, 0.0f)));
        return *this;
    } //used once
    vec &projectxy(const vec &n, float threshold)
    {
        float m = squaredlen(), k = ::min(dot(n), threshold);
        projectxydir(n);
        rescale(sqrtf(::max(m - k*k, 0.0f)));
        return *this;
    } //used once
    vec &lerp(const vec &b, float t) { x += (b.x-x)*t; y += (b.y-y)*t; z += (b.z-z)*t; return *this; }
    vec &lerp(const vec &a, const vec &b, float t) { x = a.x + (b.x-a.x)*t; y = a.y + (b.y-a.y)*t; z = a.z + (b.z-a.z)*t; return *this; }
    vec &avg(const vec &b) { add(b); mul(0.5f); return *this; }

    template<class B>
    vec &madd(const vec &a, const B &b) { return add(vec(a).mul(b)); }

    template<class B>
    vec &msub(const vec &a, const B &b) { return sub(vec(a).mul(b)); }

    vec &rescale(float k)
    {
        float mag = magnitude();
        if(mag > 1e-6f) mul(k / mag);
        return *this;
    }

    vec &rotate_around_z(float c, float s) { float rx = x, ry = y; x = c*rx-s*ry; y = c*ry+s*rx; return *this; }
    vec &rotate_around_x(float c, float s) { float ry = y, rz = z; y = c*ry-s*rz; z = c*rz+s*ry; return *this; }
    vec &rotate_around_y(float c, float s) { float rx = x, rz = z; x = c*rx+s*rz; z = c*rz-s*rx; return *this; }

    vec &rotate_around_z(float angle) { return rotate_around_z(cosf(angle), std::sin(angle)); }
    vec &rotate_around_x(float angle) { return rotate_around_x(cosf(angle), std::sin(angle)); }
    vec &rotate_around_y(float angle) { return rotate_around_y(cosf(angle), std::sin(angle)); }

    vec &rotate_around_z(const vec2 &sc) { return rotate_around_z(sc.x, sc.y); }
    vec &rotate_around_x(const vec2 &sc) { return rotate_around_x(sc.x, sc.y); }
    vec &rotate_around_y(const vec2 &sc) { return rotate_around_y(sc.x, sc.y); }

    vec &rotate(float c, float s, const vec &d)
    {
        *this = vec(x*(d.x*d.x*(1-c)+c) + y*(d.x*d.y*(1-c)-d.z*s) + z*(d.x*d.z*(1-c)+d.y*s),
                    x*(d.y*d.x*(1-c)+d.z*s) + y*(d.y*d.y*(1-c)+c) + z*(d.y*d.z*(1-c)-d.x*s),
                    x*(d.x*d.z*(1-c)-d.y*s) + y*(d.y*d.z*(1-c)+d.x*s) + z*(d.z*d.z*(1-c)+c));
        return *this;
    }
    vec &rotate(float angle, const vec &d) { return rotate(cosf(angle), std::sin(angle), d); }
    vec &rotate(const vec2 &sc, const vec &d) { return rotate(sc.x, sc.y, d); }

    void orthogonal(const vec &d)
    {
        *this = fabs(d.x) > fabs(d.z) ? vec(-d.y, d.x, 0) : vec(0, -d.z, d.y);
    }

    void orthonormalize(vec &s, vec &t) const
    {
        s.project(*this);
        t.project(*this).project(s);
    } //unused

    template<class T>
    bool insidebb(const T &bbmin, const T &bbmax) const
    {
        return x >= bbmin.x && x <= bbmax.x && y >= bbmin.y && y <= bbmax.y && z >= bbmin.z && z <= bbmax.z;
    }

    template<class T, class U>
    bool insidebb(const T &bbmin, const T &bbmax, U margin) const
    {
        return x >= bbmin.x-margin && x <= bbmax.x+margin && y >= bbmin.y-margin && y <= bbmax.y+margin && z >= bbmin.z-margin && z <= bbmax.z+margin;
    }

    template<class T, class U>
    bool insidebb(const T &o, U size) const
    {
        return x >= o.x && x <= o.x + size && y >= o.y && y <= o.y + size && z >= o.z && z <= o.z + size;
    }

    template<class T, class U>
    bool insidebb(const T &o, U size, U margin) const
    {
        size += margin;
        return x >= o.x-margin && x <= o.x + size && y >= o.y-margin && y <= o.y + size && z >= o.z-margin && z <= o.z + size;
    }

    template<class T>
    float dist_to_bb(const T &min, const T &max) const
    {
        float sqrdist = 0;
        for(int i = 0; i < 3; ++i)
        {
            if (v[i] < min[i])
            {
                float delta = v[i]-min[i];
                sqrdist += delta*delta;
            }
            else if(v[i] > max[i])
            {
                float delta = max[i]-v[i];
                sqrdist += delta*delta;
            }
        }
        return sqrtf(sqrdist);
    }

    template<class T, class S>
    float dist_to_bb(const T &o, S size) const
    {
        return dist_to_bb(o, T(o).add(size));
    }

    template<class T>
    float project_bb(const T &min, const T &max) const
    {
        return x*(x < 0 ? max.x : min.x) + y*(y < 0 ? max.y : min.y) + z*(z < 0 ? max.z : min.z);
    }

    static vec hexcolor(int color)
    {
        return vec(((color>>16)&0xFF)*(1.0f/255.0f), ((color>>8)&0xFF)*(1.0f/255.0f), (color&0xFF)*(1.0f/255.0f));
    }

    int tohexcolor() const
    {
        return (static_cast<int>(::std::clamp(r, 0.0f, 1.0f)*255)<<16) |
               (static_cast<int>(::std::clamp(g, 0.0f, 1.0f)*255)<<8)  |
                static_cast<int>(::std::clamp(b, 0.0f, 1.0f)*255);
    }
};

inline vec2::vec2(const vec &v) : x(v.x), y(v.y) {}

template<>
struct std::hash<vec>
{
    size_t operator()(const vec& k) const
    {
        union { uint i; float f; } x, y, z;
        x.f = k.x;
        y.f = k.y;
        z.f = k.z;
        uint v = x.i^y.i^z.i;
        return v + (v>>12);
    }
};

/**
 * @brief color vector3 (r,g,b)
 */
struct bvec
{
    union
    {
        struct
        {
            uchar x, y, z;
        };
        struct
        {
            uchar r, g, b;
        };
        uchar v[3];
    };

    bvec() {}
    bvec(uchar x, uchar y, uchar z) : x(x), y(y), z(z) {}
    explicit bvec(const vec &v) : x(static_cast<uchar>((v.x+1)*(255.0f/2.0f))), y(static_cast<uchar>((v.y+1)*(255.0f/2.0f))), z(static_cast<uchar>((v.z+1)*(255.0f/2.0f))) {}
    explicit bvec(const vec4<uchar> &v);

    uchar &operator[](int i)       { return v[i]; }
    uchar  operator[](int i) const { return v[i]; }

    bool operator==(const bvec &v) const { return x==v.x && y==v.y && z==v.z; }
    bool operator!=(const bvec &v) const { return x!=v.x || y!=v.y || z!=v.z; }

    bool iszero() const { return x==0 && y==0 && z==0; }

    vec tonormal() const { return vec(x*(2.0f/255.0f)-1.0f, y*(2.0f/255.0f)-1.0f, z*(2.0f/255.0f)-1.0f); }

    bvec &normalize()
    {
        vec n(x-127.5f, y-127.5f, z-127.5f);
        float mag = 127.5f/n.magnitude();
        x = static_cast<uchar>(n.x*mag+127.5f);
        y = static_cast<uchar>(n.y*mag+127.5f);
        z = static_cast<uchar>(n.z*mag+127.5f);
        return *this;
    }

    void lerp(const bvec &a, const bvec &b, float t)
    {
        x = static_cast<uchar>(a.x + (b.x-a.x)*t);
        y = static_cast<uchar>(a.y + (b.y-a.y)*t);
        z = static_cast<uchar>(a.z + (b.z-a.z)*t);
    }

    void lerp(const bvec &a, const bvec &b, int ka, int kb, int d)
    {
        x = static_cast<uchar>((a.x*ka + b.x*kb)/d);
        y = static_cast<uchar>((a.y*ka + b.y*kb)/d);
        z = static_cast<uchar>((a.z*ka + b.z*kb)/d);
    }

    void flip()
    {
        x ^= 0x80;
        y ^= 0x80;
        z ^= 0x80;
    }

    void scale(int k, int d)
    {
        x = static_cast<uchar>((x*k)/d);
        y = static_cast<uchar>((y*k)/d);
        z = static_cast<uchar>((z*k)/d);
    }

    bvec &shl(int n)
    {
        x <<= n;
        y <<= n;
        z <<= n;
        return *this;
    }
    bvec &shr(int n)
    {
        x >>= n;
        y >>= n;
        z >>= n;
        return *this;
    }

    static bvec fromcolor(const vec &v)
    {
        return bvec(static_cast<uchar>(v.x*255.0f), static_cast<uchar>(v.y*255.0f), static_cast<uchar>(v.z*255.0f));
    }

    vec tocolor() const
    {
        return vec(x*(1.0f/255.0f), y*(1.0f/255.0f), z*(1.0f/255.0f));
    }

    static bvec from565(ushort c)
    {
        return bvec((((c>>11)&0x1F)*527 + 15) >> 6, (((c>>5)&0x3F)*259 + 35) >> 6, ((c&0x1F)*527 + 15) >> 6);
    }

    static bvec hexcolor(int color)
    {
        return bvec((color>>16)&0xFF, (color>>8)&0xFF, color&0xFF);
    }

    int tohexcolor() const
    {
        return (static_cast<int>(r)<<16)|(static_cast<int>(g)<<8)|static_cast<int>(b);
    }
};

/**
 * @brief A four dimensional Cartesian-space vector template.
 *
 * This object defines a location in four dimensional Cartesian space, in whatever
 * arithmetic type it is specialized to be. All four values are of the type T
 * specialized, and all operators (unless explicitly specified, require types
 * trivially convertable to T (or preferably of type T).
 */
template<typename T>
struct vec4
{
    union
    {
        struct { T x, y, z, w; }; /** geometric space representation */
        struct { T r, g, b, a; }; /** color space representation (red, green, blue, alpha)*/
        T v[4]; /** four-entry array representation*/
        uint mask; /** used for uchar (color) specific operations*/
    };

    vec4() {}
    explicit vec4(const vec &p, T w = 0) : x(p.x), y(p.y), z(p.z), w(w) {}
    explicit vec4(const vec2 &p, T z = 0, T w = 0) : x(p.x), y(p.y), z(z), w(w) {}
    vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
    vec4(bvec v, uchar c)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        w = c;
    }
    vec4(bvec v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        w = 0;
    }

    explicit vec4(const T *v) : x(v[0]), y(v[1]), z(v[2]), w(v[3]) {}

    template<class U>
    operator vec4<U>()
    {
        return vec4<U>(static_cast<U>(this->x),
                       static_cast<U>(this->y),
                       static_cast<U>(this->z),
                       static_cast<U>(this->w));
    }

    T &operator[](int i)       { return v[i]; }
    T  operator[](int i) const { return v[i]; }

    bool operator==(const vec4 &o) const { return x == o.x && y == o.y && z == o.z && w == o.w; }
    bool operator!=(const vec4 &o) const { return x != o.x || y != o.y || z != o.z || w != o.w; }

    /**
     * @brief Returns the 3 dimensional dot product between two 4-vecs.
     *
     * Returns the scalar (dot) product of two 4D vecs, `this` and the passed
     * one, as if they are 3D vecs, by ignoring the fourth term in each vector.
     *
     * @param o the vec4 to multiply by
     *
     * @return the dot product of the first three elements of each vector
     */
    T dot3(const vec4 &o) const { return x*o.x + y*o.y + z*o.z; }

    /**
     * @brief Returns the 3 dimensional dot product between `this` and a 3D vec.
     *
     * Returns the scalar (dot) product of a 3D and 4D vec, `this` and the passed
     * 3D vec, by ignoring the fourth term in the 4D vec.
     *
     * @param o the vec3 to multiply by
     *
     * @return the dot product of the first three elements of each vector
     */
    T dot3(const vec &o) const { return x*o.x + y*o.y + z*o.z; }

    /**
     * @brief Returns the scalar product with another vec4.
     *
     * Returns the scalar (dot) product of two 4D vecs, `this` and the passed
     * vec4 `o`.
     *
     * @param o the vector to multiply with
     *
     * @return the dot product of the two vectors
     */
    T dot(const vec4 &o) const { return dot3(o) + w*o.w; }

    /**
     * @brief Returns the dot product of `this` and a vec3, assuming o.w = 1.
     *
     * Calculates the dot product with a vec3, with `o.w` implied to equal 1.
     *
     * @param o the vec3 to multiply by
     *
     * @return the dot product of the two vectors
     */
    T dot(const vec &o) const  { return x*o.x + y*o.y + z*o.z + w; }

    /**
     * @brief Returns the square of the magnitude of the vector.
     *
     * Calculates the dot product of the vector with itself, yielding the
     * square of the magnitude of the vec4.
     *
     * @return the magnitude of `this` squared
     */
    T squaredlen() const { return dot(*this); }

    /**
     * @brief Returns the magnitude of the vector.
     *
     * Calculates the magnitude (length) of the vector, by taking the square
     * root of the dot product of the vector with itself.
     *
     * @return the maginitude of `this`
     */
    T magnitude() const  { return sqrtf(squaredlen()); }

    /**
     * @brief Returns the magnitude of the vector, ignoring the w dimension.
     *
     * Calculates the #D magnitude (length) of the vector, by taking the square
     * root of the 3D dot product of the vector with itself.
     *
     * @return the maginitude of `this`
     */
    T magnitude3() const { return sqrtf(dot3(*this)); }

    /**
     * @brief Scales the vector to have a magnitude of 1
     *
     * Will cause a divide-by-zero if the vector is (0,0,0,0).
     *
     * @return a reference to `this` vector
     */
    vec4 &normalize() { mul(1/magnitude()); return *this; }

    /**
     * @brief Scales the vector to have a magnitude of 1
     *
     * Will return (0,0,0,0) if the vector passed (0,0,0,0).
     *
     * @return a reference to `this` vector
     */
    vec4 &safenormalize() { T m = magnitude(); if(m) mul(1/m); return *this; }

    void lerp(const vec4<uchar> &a, const vec4<uchar> &b, float t)
    {
        x = static_cast<uchar>(a.x + (b.x-a.x)*t);
        y = static_cast<uchar>(a.y + (b.y-a.y)*t);
        z = static_cast<uchar>(a.z + (b.z-a.z)*t);
        w = a.w;
    }

    void lerp(const vec4<uchar> &a, const vec4<uchar> &b, int ka, int kb, int d)
    {
        x = static_cast<uchar>((a.x*ka + b.x*kb)/d);
        y = static_cast<uchar>((a.y*ka + b.y*kb)/d);
        z = static_cast<uchar>((a.z*ka + b.z*kb)/d);
        w = a.w;
    }

    void lerp(const vec4<uchar> &a, const vec4<uchar> &b, const vec4<uchar> &c, float ta, float tb, float tc)
    {
        x = static_cast<uchar>(a.x*ta + b.x*tb + c.x*tc);
        y = static_cast<uchar>(a.y*ta + b.y*tb + c.y*tc);
        z = static_cast<uchar>(a.z*ta + b.z*tb + c.z*tc);
        w = static_cast<uchar>(a.w*ta + b.w*tb + c.w*tc);
    }

    /**
     * @brief Flips a vec4<uchar> by using the mask type pun.
     *
     * Not for use with non-char vec4<> objects.
     */
    void flip() { mask ^= 0x80808080; }

    vec4 &lerp(const vec4 &b, T t)
    {
        x += (b.x-x)*t;
        y += (b.y-y)*t;
        z += (b.z-z)*t;
        w += (b.w-w)*t;
        return *this;
    }
    vec4 &lerp(const vec4 &a, const vec4 &b, T t)
    {
        x = a.x+(b.x-a.x)*t;
        y = a.y+(b.y-a.y)*t;
        z = a.z+(b.z-a.z)*t;
        w = a.w+(b.w-a.w)*t;
        return *this;
    }

    /**
     * @brief Calculates the elementwise arithmetic mean.
     *
     * Sets `this` to the elementwise arithmetic mean of `this` object and the
     * passed object. The original vector is not preserved, but the passed one
     * is.
     *
     * @param b the vec4 to average with
     *
     * @return a reference to `this` object following the operation
     */
    vec4 &avg(const vec4 &b) { add(b); mul(0.5f); return *this; }

    template<class B>
    vec4 &madd(const vec4 &a, const B &b) { return add(vec4(a).mul(b)); }

    template<class B>
    vec4 &msub(const vec4 &a, const B &b) { return sub(vec4(a).mul(b)); }

    /**
     * @brief Calculates the elementwise product.
     *
     * Calculates the elementwise product of `f` with the first three entries
     * in `this`. This means that the original vector is not preserved.
     *
     * @param f the value to multiply by
     *
     * @return a reference to `this` object following the operation
     */
    vec4 &mul3(T f)      { x *= f; y *= f; z *= f; return *this; }

    /**
     * @brief Calculates the elementwise product.
     *
     * Calculates the elementwise product of `f` with all four entries
     * in `this`. This means that the original vector is not preserved.
     *
     * @param f the value to multiply by
     *
     * @return a reference to `this` object following the operation
     */
    vec4 &mul(T f)       { mul3(f); w *= f; return *this; }

    /**
     * @brief Calculates the elementwise product.
     *
     * Calculates the elementwise product of the four parameters in `this` with
     * the four parameters in `o`. This measn that the original vector is not
     * preserved.
     *
     * @param o the vec4 to multiply by
     *
     * @return a reference to `this` object following the operation
     */
    vec4 &mul(const vec4 &o) { x *= o.x; y *= o.y; z *= o.z; w *= o.w; return *this; }

    /**
     * @brief Calculates the elementwise product.
     *
     * Calculates the elementwise product of the first three parameters in `this`
     * with the three parameters in `o`. This means that the original vector is
     * not preserved.
     *
     * @param o the vec3 to multiply by
     *
     * @return a reference to `this` object following the operation
     */
    vec4 &mul(const vec &o)  { x *= o.x; y *= o.y; z *= o.z; return *this; }

    /**
     * @brief Calculates the elementwise square.
     *
     * Calculates the elementwise product of `this` with itself. This means that
     * the original vector is not preserved.
     *
     * @return a reference to `this` object following the operation
     */
    vec4 &square()           { mul(*this); return *this; }

    /**
     * @brief Calculates the elementwise quotient.
     *
     * Calculates the quotient of the first three values in `this` with the value
     * passed to `f`. This means that the original vector is not preserved.
     *
     * @param f the value to divide by
     *
     * @return a reference to `this` object following the operation
     *
     */
    vec4 &div3(T f)      { x /= f; y /= f; z /= f; return *this; }

    /**
     * @brief Calculates the elementwise quotient.
     *
     * Calculates the quotient of the four values in `this` with the value passed
     * to `f`. This means that the original vector is not preserved.
     *
     * @param f the value to divide by
     *
     * @return a reference to `this` object following the operation
     */
    vec4 &div(T f)       { div3(f); w /= f; return *this; }

    /**
     * @brief Calculates the elementwise quotient.
     *
     * Calculates the quotient of the four values in `this` with the four values in
     * `o`, element-by-element. This means that the original vector is not preserved.
     *
     * @param o the vector to divide by
     *
     * @return a reference to `this` object following the operation
     */
    vec4 &div(const vec4 &o) { x /= o.x; y /= o.y; z /= o.z; w /= o.w; return *this; }

    /**
     * @brief Calculates the elementwise quotient.
     *
     * Calculates the quotient of the three values in `this` with the four values in
     * `o`, element-by-element. This means that the original vector is not preserved.
     *
     * @param o the vector to divide by
     *
     * @return a reference to `this` object following the operation
     */
    vec4 &div(const vec &o)  { x /= o.x; y /= o.y; z /= o.z; return *this; }

    /**
     * @brief Calculates the elementwise reciprocal.
     *
     * Calculates the value 1/x for each of the four values in the vector, and assigns
     * them to `this`. This means that the original vector is not preserved
     *
     * @return a reference to `this` object following the operation
     */
    vec4 &recip()            { x = 1/x; y = 1/y; z = 1/z; w = 1/w; return *this; }

    /**
     * @brief Calculates the elementwise sum.
     *
     * Calculates the sum of the four elements in `this` with the four values in
     * `o`, element-by-element. This means that the original vector is not preserved.
     *
     * @param o the vector to add with
     *
     * @return a reference to `this` object following the operation
     */
    vec4 &add(const vec4 &o) { x += o.x; y += o.y; z += o.z; w += o.w; return *this; }

    /**
     * @brief Calculates the elementwise sum.
     *
     * Calculates the sum of the first three elements in `this` with the first
     * three values in `o`, element-by-element. This means that the original
     * vector is not preserved.
     *
     * @param o the vector to add with
     *
     * @return a reference to `this` object following the operation
     */
    vec4 &add(const vec &o)  { x += o.x; y += o.y; z += o.z; return *this; }

    /**
     * @brief Calculates the elementwise sum.
     *
     * Calculates the sum of the first three elements in `this` with the value
     * passed to `f`. This means that the original vector is not preserved.
     *
     * @param f the value to add with
     *
     * @return a reference to `this` object following the operation
     */
    vec4 &add3(T f)      { x += f; y += f; z += f; return *this; }

    /**
     * @brief Calculates the elementwise sum.
     *
     * Calculates the sum of the four elements in `this` with the four elements
     * comprising `f`. This means that the original vector is not preserved.
     *
     * @param f the value to add with
     *
     * @return a reference to `this` object following the operation
     */
    vec4 &add(T f)       { add3(f); w += f; return *this; }

    /**
     * @brief Adds to the fourth value of the vector (w/a).
     *
     * Calculates the sum of the passed value and the fourth element of the vec4,
     * modifying the original vector to equal this sum.
     *
     * @param f the value to add with
     *
     * @return a reference to `this` object following the operation
     */
    vec4 &addw(T f)      { w += f; return *this; }

    /**
     * @brief Subtracts from `this` the vec4 passed.
     *
     * Calculates the difference between the four elements in `this` and the
     * four elements of `o`, modifying the original vector to equal this difference.
     *
     * @param o the vec4 to subtract
     *
     * @return a reference to `this` object following the operation
     */
    vec4 &sub(const vec4 &o) { x -= o.x; y -= o.y; z -= o.z; w -= o.w; return *this; }

    /**
     * @brief Subtracts from `this` the vec passed.
     *
     * Calculates the difference between the four elements in `this` and the three
     * elements of `o`, modifying the original vector to equal this difference.
     *
     * @param o the vec to subtract
     *
     * @return a reference to `this` object following the operation
     */
    vec4 &sub(const vec &o)  { x -= o.x; y -= o.y; z -= o.z; return *this; }

    /**
     * @brief Subtracts from the first three entries `this` the svalue passed.
     *
     * Calculates the difference between the first three elements in `this` and
     * the value passed, modifying the original vector to equal this difference.
     *
     * @param f the value to subtract
     *
     * @return a return to `this` object following the operation
     */
    vec4 &sub3(T f)      { x -= f; y -= f; z -= f; return *this; }
    vec4 &sub(T f)       { sub3(f); w -= f; return *this; }
    vec4 &subw(T f)      { w -= f; return *this; }
    vec4 &neg3()             { x = -x; y = -y; z = -z; return *this; }
    vec4 &neg()              { neg3(); w = -w; return *this; }
    vec4 &clamp(T l, T h) { x = ::std::clamp(x, l, h); y = ::std::clamp(y, l, h); z = ::std::clamp(z, l, h); w = ::std::clamp(w, l, h); return *this; }


    vec4 operator+(const vec4 &v2) const
    {
        return vec4(x+v2.x, y+v2.y, z+v2.z, w+v2.w);
    }

    vec4 operator-(const vec4 &v2) const
    {
        return vec4(x-v2.x, y-v2.y, z-v2.z, w+v2.w);
    }

    vec4 operator-() const
    {
        return vec4(-x, -y, -z, -w);
    }

    template<typename U>
    vec4 operator*(const U &n) const
    {
        return vec4(n*x, n*y, n*z, n*w);
    }

    vec4 operator*(const vec4 &v2) const
    {
        return vec4(x*v2.x, y*v2.y, z*v2.z, w*v2.w);
    }

    template<typename U>
    vec4 operator/(const U &n) const
    {
        return vec4(x/n, y/n, z/n, w/n);
    }

    vec4 operator/(const vec4 &v2) const
    {
        return vec4(x/v2.x, y/v2.y, z/v2.z, w/v2.w);
    }


    template<class A, class B>
    vec4 &cross(const A &a, const B &b) { x = a.y*b.z-a.z*b.y; y = a.z*b.x-a.x*b.z; z = a.x*b.y-a.y*b.x; return *this; }

    vec4 &cross(const vec &o, const vec &a, const vec &b) { return cross(vec(a).sub(o), vec(b).sub(o)); }

    void setxyz(const vec &v) { x = v.x; y = v.y; z = v.z; }

    vec4 &rotate_around_z(T c, T s) { T rx = x, ry = y; x = c*rx-s*ry; y = c*ry+s*rx; return *this; }
    vec4 &rotate_around_x(T c, T s) { T ry = y, rz = z; y = c*ry-s*rz; z = c*rz+s*ry; return *this; }
    vec4 &rotate_around_y(T c, T s) { T rx = x, rz = z; x = c*rx-s*rz; z = c*rz+s*rx; return *this; }

    vec4 &rotate_around_z(T angle) { return rotate_around_z(cosf(angle), std::sin(angle)); }
    vec4 &rotate_around_x(T angle) { return rotate_around_x(cosf(angle), std::sin(angle)); }
    vec4 &rotate_around_y(T angle) { return rotate_around_y(cosf(angle), std::sin(angle)); }

    vec4 &rotate_around_z(const vec2 &sc) { return rotate_around_z(sc.x, sc.y); }
    vec4 &rotate_around_x(const vec2 &sc) { return rotate_around_x(sc.x, sc.y); }
    vec4 &rotate_around_y(const vec2 &sc) { return rotate_around_y(sc.x, sc.y); }

    vec tonormal() const { return vec(x*(2.0f/255.0f)-1.0f, y*(2.0f/255.0f)-1.0f, z*(2.0f/255.0f)-1.0f); }

};
inline vec2::vec2(const vec4<float> &v) : x(v.x), y(v.y) {}
inline vec::vec(const vec4<float> &v) : x(v.x), y(v.y), z(v.z) {}

/**
 * @brief matrix3: 3x3 matrix
 * comprised of three vec3 vectors
 * arranged as follows:
 * [ a1 b1 c1
 *   a2 b2 c2
 *   a3 b3 c3 ]
 *
 * used largely in models/anims/ragdolls but also in stuff like bih
 */
class matrix3
{
    public:
        /**
         * @brief the three vectors making up the rows of the matrix
         *
         * a is the top vector
         * b is the middle vector
         * c is the bottom vector
         */
        vec a, b, c;

        /**
         * @brief Creates an empty matrix.
         */
        matrix3();

        /**
         * @brief Creates a new matrix with the given vectors.
         *
         * @param a the vector to assign to the top row
         * @param b the vector to assign to the middle row
         * @param c the vector to assign to the bottom row
         */
        matrix3(const vec &a, const vec &b, const vec &c);

        /**
         * @brief Creates a new matrix as a rotation matrix.
         *
         * @param angle
         * @param axis
         */
        explicit matrix3(float angle, const vec &axis);

        /**
         * @brief Creates a 3d rotation matrix given a quaternion object.
         *
         * Note that quaternions are an engine-specific construct and not part of the
         * game API beyond the prototype defined in this header.
         *
         * @param q the quaternion to use
         */
        explicit matrix3(const quat &q);

        /**
         * @brief Creates a 3d matrix given a 4x3 matrix.
         *
         *
         * @param m the matrix to use
         */
        explicit matrix3(const matrix4x3 &m);

        /**
         * @brief Creates a 3x3 matrix given a 4x4 matrix.
         *
         * This truncates the 4th member in both dimensions of the matrix.
         *
         * @param m the matrix to use
         */
        explicit matrix3(const matrix4 &m);

        /**
         * @brief Calculates the product of three matrices.
         *
         * @param m the first matrix to multiply by
         * @param n the second matrix to multiply by
         */
        void mul(const matrix3 &m, const matrix3 &n);

        /**
         * @brief Multiplies the matrix by another
         *
         * This operation changes the value of the matrix.
         *
         * @param n
         */
        void mul(const matrix3 &n);

        /**
         * @brief Calculates the multiplication-transpose with another matrix.
         *
         * @param n the matrix to use
         */
        void multranspose(const matrix3 &n);

        /**
         * @brief Calculates the transpose-multiplication with another two matrices.
         *
         * @param m the first matrix to use
         * @param n the second matrix to use
         */
        void transposemul(const matrix3 &m, const matrix3 &n);

        /**
         * @brief Calculates the transpose-multiplication with another matrix.
         *
         * @param n the matrix to multiply by
         */
        void transposemul(const matrix3 &n);

        /**
         * @brief Transposes the matrix.
         *
         * ```
         * a b c       a d g
         * d e f   ->  b e h
         * g h i       c f i
         *```
         */
        void transpose();

        /**
         * @brief Inverts the matrix using another matrix for the scale factor.
         *
         * This operation can be undone by calling invert() again using the same
         * argument.
         *
         */
        void invert(const matrix3 &o);

        /**
         * @brief Inverts the matrix using itself for the scale factor.
         *
         * This operation can be undone by calling invert() again.
         *
         */
        void invert();

        /**
         * @brief Normalizes each of the three rows to a magnitude of 1.
         *
         * Calls normalize() for each of the three vec objects making up the matrix.
         */
        void normalize();

        /**
         * @brief Multiplies each element of the matrix by the scale factor given.
         *
         * @param k the scale factor to multiply by
         */
        void scale(float k);

        /**
         * @brief Rotates the matrix around the given axis by the given angle.
         *
         * @param angle the angle to rotate by (radians)
         * @param axis the axis to rotate about
         */
        void rotate(float angle, const vec &axis);

        /**
         * @brief Rotates the matrix around the given axis by the given angle
         *
         * @param ck the cosine term
         * @param sk the sine term
         * @param axis the axis to rotate about
         */
        void rotate(float ck, float sk, const vec &axis);

        /**
         * @brief Sets the matrix to a 2D rotation matrix.
         *
         * The output matrix looks like this:
         *
         * ```
         *  cosf(angle) sinf(angle) 0
         * -sinf(angle) cosf(angle) 0
         *  0           0           1
         * ```
         *
         * @param angle the angle to rotate by (radians)
         */
        void setyaw(float angle);

        /**
         * @brief Returns the trace of the matrix.
         *
         * @return a float representing the trace of the matrix
         */
        float trace() const;

        /**
         * @brief Calculates ragdoll angle axis compliance.
         *
         * Only used for some ragdoll calculations.
         *
         * @param tr the trace of the matrix
         * @param angle the angle to test
         * @param axis the axis to use
         * @param threshold the threshhold for a pass
         *
         * @return whether the angle is within the allowable range
         */
        bool calcangleaxis(float tr, float &angle, vec &axis, float threshold = 1e-16f) const;

        /**
         * @brief Calculates ragdoll angle axis compliance.
         *
         * Ony used for some ragdoll calculations.
         *
         * @param angle the angle to test
         * @param axis the axis to use
         * @param threshold the threshhold for a pass
         *
         * @return whether the angle is within the allowable range
         */
        bool calcangleaxis(float &angle, vec &axis, float threshold = 1e-16f) const;

        /**
         * @brief Sets the matrix to the transform of the matrix.
         *
         * @param o the 3d vector to transform by
         *
         * @return whether the angle is within the allowable range
         */
        vec transform(const vec &o) const;

        /**
         * @brief Sets the matrix to the transpose-transform.
         *
         * @param o the 3d vector to transform by
         *
         * @return the result of the transform
         */
        vec transposedtransform(const vec &o) const;

        /**
         * @brief Sets the matrix to the absolute value of the transform
         *
         * @param o the 3d vector to transform by
         *
         * @return the dot products of the resulting matrix
         */
        vec abstransform(const vec &o) const;

        /**
         * @brief Sets the matrix to the absolute value of the transpose-transform.
         *
         * @param o the 3d vector to transform by
         *
         * @return the result of the transform
         */
        vec abstransposedtransform(const vec &o) const;

        /**
         * @brief Sets the matrix to be the identity matrix.
         *
         * ```
         * 1 0 0
         * 0 1 0
         * 0 0 1
         * ```
         */
        void identity();

        /**
         * @brief Rotates the matrix values around the X axis.
         *
         * @param angle the angle by which to rotate about
         */
        void rotate_around_x(float angle);

        /**
         * @brief Rotates the matrix values around the X axis.
         *
         * @param sc a vector containing the cosine + sine terms
         */
        void rotate_around_x(const vec2 &sc);

        /**
         * @brief Rotates the matrix values around the Y axis.
         *
         * @param angle the angle by which to rotate about
         */
        void rotate_around_y(float angle);

        /**
         * @brief Rotates the matrix values around the Y axis.
         *
         * @param sc a vector containing the cosine + sine terms
         */
        void rotate_around_y(const vec2 &sc);

        /**
         * @brief Rotates the matrix values around the Z axis.
         *
         * @param angle by which to rotate
         */
        void rotate_around_z(float angle);

        /**
         * @brief Rotates the matrix values around the Z axis.
         *
         * @param sc a vector containing the cosine + sine terms
         */
        void rotate_around_z(const vec2 &sc);

        /**
         * @brief Returns the transform of the matrix.
         *
         * @param o the 2d vector to transform by
         *
         * @return the transform of the matrix
         */
        vec transform(const vec2 &o);

        /**
         * @brief Returns the transposed transform of the matrix.
         *
         * @param o the vector to transform with
         *
         * @return the transposed transform of the matrix
         */
        vec transposedtransform(const vec2 &o) const;

        /**
         * @brief Returns the first (top) row of the matrix.
         *
         * @return a 3d vector containing the first row of the matrix
         */
        vec rowx() const;

        /**
         * @brief Returns the second (middle) row of the matrix.
         *
         * @return a 3d vector containing the second row of the matrix
         */
        vec rowy() const;

        /**
         * @brief Returns the third (bottom) row of the matrix.
         *
         * @return a 3d fector containing the thrid row of the matrix
         */
        vec rowz() const;
    private:

        /**
         * @brief Calculates the multiplication-transpose of the three matrices
         *
         * @param m the first matrix to use
         * @param n the second matrix to use
         */
        void multranspose(const matrix3 &m, const matrix3 &n);

        /**
         * @brief Sets the matrix to a 2D rotation matrix.
         *
         * The output matrix looks like this:
         * ```
         *  ck sk 0
         * -sk ck 0
         *  0  0  1
         * ```
         *
         * @param ck the cosine term
         * @param sk the sine term
         */
        void setyaw(float ck, float sk);

        /**
         * @brief Rotates the matrix values around the X axis
         *
         * @param ck the cosine term
         * @param sk the sine term
         */
        void rotate_around_x(float ck, float sk);

        /**
         * @brief Rotates the matrix values around the Y axis.
         *
         * @param ck the cosine term
         * @param sk the sine term
         */
        void rotate_around_y(float ck, float sk);

        /**
         * @brief Rotates the matrix values around the Z axis.
         *
         * @param ck the cosine term
         * @param sk the sine term
         */
        void rotate_around_z(float ck, float sk);

        /**
         * @brief Sets the matrix to the transpose of the one given.
         *
         * @param m the matrix to use to set
         */
        void transpose(const matrix3 &m);
};

/**
 * @brief floating point 4x3 matrix object
 * defined as four column vectors, a-d
 * takes the form as follows:
 * [ a1 b1 c1 d1   (x)
 *   a2 b2 c2 d2   (y)
 *   a3 b3 c3 d3 ] (z)
 *
 * used in animation along with dualquats
 */
struct matrix4x3
{
    vec a, b, c, d;

    /**
     * @brief Creates an empty matrix4x3 object.
     *
     * Creates a matrix4x3 object, where all values within the matrix are set to
     * zero.
     */
    matrix4x3();

    /**
     * @brief Creates a matrix4x3 from four three-dimensional vec objects.
     *
     * The values of the passed vecs are copied to the new matrix4x3 object.
     *
     * @param a the vec to assign to `matrix4x3::a`
     * @param b the vec to assign to `matrix4x3::b`
     * @param c the vec to assign to `matrix4x3::c`
     * @param d the vec to assign to `matrix4x3::d`
     */
    matrix4x3(const vec &a, const vec &b, const vec &c, const vec &d);

    /**
     * @brief Creates a matrix4x3 from a rotation matrix and a translation vector.
     *
     * The rotation matrix is assigned to members `a` `b` `c` and the translation `d`.
     * No transformations are made to the values of the passed parameters.
     *
     * @param rot the rotation matrix to assign to `a` `b` `c`
     * @param trans the translation to assign to `d`
     */
    matrix4x3(const matrix3 &rot, const vec &trans);

    /**
     * @brief Creates a matrix4x3 that represents a dual quaternion transformation.
     *
     * @param dq the dual quaternion to transform into a matrix4x3
     */
    matrix4x3(const dualquat &dq);

    /**
     * @brief Creates a matrix4x3 by truncating `w` from a matrix4.
     *
     * The four vecs that make up the matrix4 are copied, omitting the `w` parameter.
     *
     * @param m the matrix4 to truncate into a matrix4x3
     */
    explicit matrix4x3(const matrix4 &m);

    /**
     * @brief Multiplies all values inside the matrix by a scalar constant.
     *
     * @param k the scale factor to multiply by
     */
    void mul(float k);

    /**
     * @brief Sets the diagonals a.x, b.y, c.z to the given values.
     *
     * Does not modify any of the values except for a.x, b.y, c.z.
     *
     * @param x the first value to set in the diagonal
     * @param y the second value to set in the diagonal
     * @param z the third value to set in the diagonal
     */
    void setscale(float x, float y, float z);

    /**
     * @brief Sets the diagonals a.x, b.y, c.z to the given vector's x, y, z values.
     *
     * Does not modify any of the values except for a.x, b.y, c.z.
     *
     * @param v the vector to assign from
     */
    void setscale(const vec &v);

    /**
     * @brief Sets the diagonals a.x, b.y, c.z identically to a given value.
     *
     * @parm n the value to set to
     */
    void setscale(float n);

    /**
     * @brief Scales the first three vectors by x, y, and z respectively.
     *
     * Does not modify the fourth matrix element vector`d`.
     *
     * @param x the scale to multiply the first vector by
     * @param y the scale to multiply the second vector by
     * @param z the scale to multiply the third vector by
     */
    void scale(float x, float y, float z);

    /**
     * @brief Multiplies the first three vectors by v.x, v.y, and v.z respectively.
     *
     * Does not modify the fourth matrix element vector`d`.
     *
     * @param v the vector to multiply by
     */
    void scale(const vec &v);

    /**
     * @brief Scales the first three vector elements in the matrix by the specified amount.
     *
     * Does not modify the fourth matrix element vector`d`.
     *
     * @param n the scale factor to multiply by
     */
    void scale(float n);

    /**
     * @brief Copies the `p` vector into the fourth column vector (`d`).
     *
     * @param p the vector to copy to the `d` vector.
     */
    void settranslation(const vec &p);

    /**
     * @brief Copies x, y, z into the fourth column vector (`d`).
     *
     * @param x the first value to copy into d.x
     * @param y the second value to copy into d.y
     * @param z the third value to copy into d.z
     */
    void settranslation(float x, float y, float z);
    void translate(const vec &p);
    void translate(float x, float y, float z);
    void translate(const vec &p, float scale);
    void accumulate(const matrix4x3 &m, float k);

    /**
     * @brief Normalizes the first three column vectors.
     *
     * Sets the three first vectors to have a magnitude of 1. That is,
     * sqrt(x^2 + y^2 + z^2) = 1.
     *
     * Does not check for a divide-by-zero condition.
     */
    void normalize();
    void lerp(const matrix4x3 &to, float t);
    void lerp(const matrix4x3 &from, const matrix4x3 &to, float t);

    /**
     * @brief Sets this matrix to the identity matrix.
     *
     * ```
     *   a b c d
     * x 1 0 0 0
     * y 0 1 0 0
     * z 0 0 1 0
     * ```
     */
    void identity();
    void mul(const matrix4x3 &m, const matrix4x3 &n);
    void mul(const matrix4x3 &n);

    void mul(const matrix3 &m, const matrix4x3 &n);

    void mul(const matrix3 &rot, const vec &trans, const matrix4x3 &n);

    void transpose();
    void transpose(const matrix4x3 &o);

    void transposemul(const matrix4x3 &m, const matrix4x3 &n);

    void multranspose(const matrix4x3 &m, const matrix4x3 &n);

    void invert(const matrix4x3 &o);
    void invert();
    void rotate(float angle, const vec &d);

    void rotate(float ck, float sk, const vec &axis);

    void rotate_around_x(float ck, float sk);
    void rotate_around_x(float angle);

    void rotate_around_x(const vec2 &sc);

    void rotate_around_y(float ck, float sk);
    void rotate_around_y(float angle);
    void rotate_around_y(const vec2 &sc);

    void rotate_around_z(float ck, float sk);
    void rotate_around_z(float angle);
    void rotate_around_z(const vec2 &sc);

    vec transposedtransform(const vec &o) const;
    vec transformnormal(const vec &o) const;
    vec transposedtransformnormal(const vec &o) const;
    vec transform(const vec &o) const;
    vec transform(const vec2 &o) const;

    /**
     * @brief Returns the x values of the four vectors as a four-vector.
     *
     * @return a vector containing four x values
     */
    vec4<float> rowx() const;

    /**
     * @brief Returns the y values of the four vectors as a four-vector.
     *
     * @return a vector containing four y values
     */
    vec4<float> rowy() const;

    /**
     * @brief Returns the z values of the four vectors as a four-vector.
     *
     * @return a vector containing four z values
     */
    vec4<float> rowz() const;
};

/*

The engine uses 3 different linear coordinate systems
which are oriented around each of the axis dimensions.

So any point within the game can be defined by four coordinates: (d, x, y, z)

d is the reference axis dimension
x is the coordinate of the ROW dimension
y is the coordinate of the COL dimension
z is the coordinate of the reference dimension (DEPTH)

typically, if d is not used, then it is implicitly the Z dimension.
ie: d=z => x=x, y=y, z=z

*/

// DIM: X=0 Y=1 Z=2.
const int R[3]  = {1, 2, 0}; /**< row */
const int C[3]  = {2, 0, 1}; /**< col */
const int D[3]  = {0, 1, 2}; /**< depth */

struct ivec2;

//integer vector3
struct ivec
{
    union
    {
        struct
        {
            int x, y, z;
        };
        struct
        {
            int r, g, b;
        };
        int v[3];
    };

    ivec() {}
    explicit ivec(const vec &v) : x(static_cast<int>(v.x)), y(static_cast<int>(v.y)), z(static_cast<int>(v.z)) {}
    ivec(int a, int b, int c) : x(a), y(b), z(c) {}
    ivec(int d, int row, int col, int depth)
    {
        v[R[d]] = row;
        v[C[d]] = col;
        v[D[d]] = depth;
    }
    ivec(int i, const ivec &co, int size) : x(co.x+((i&1)>>0)*size), y(co.y+((i&2)>>1)*size), z(co.z +((i&4)>>2)*size) {}
    explicit ivec(const ivec2 &v, int z = 0);
    explicit ivec(const svec &v);

    int &operator[](int i)       { return v[i]; }
    int  operator[](int i) const { return v[i]; }

    //int idx(int i) { return v[i]; }
    bool operator==(const ivec &v) const { return x==v.x && y==v.y && z==v.z; }
    bool operator!=(const ivec &v) const { return x!=v.x || y!=v.y || z!=v.z; }
    ivec operator+(const ivec &v)  const { return ivec(x+v.x, y+v.y, z+v.z); }
    bool iszero() const { return x==0 && y==0 && z==0; }
    ivec &shl(int n) { x<<= n; y<<= n; z<<= n; return *this; }
    ivec &shr(int n) { x>>= n; y>>= n; z>>= n; return *this; }
    ivec &mul(int n) { x *= n; y *= n; z *= n; return *this; }
    ivec &div(int n) { x /= n; y /= n; z /= n; return *this; }
    ivec &add(int n) { x += n; y += n; z += n; return *this; }
    ivec &sub(int n) { x -= n; y -= n; z -= n; return *this; }
    ivec &mul(const ivec &v) { x *= v.x; y *= v.y; z *= v.z; return *this; }
    ivec &div(const ivec &v) { x /= v.x; y /= v.y; z /= v.z; return *this; }
    ivec &add(const ivec &v) { x += v.x; y += v.y; z += v.z; return *this; }
    ivec &sub(const ivec &v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    ivec &mask(int n) { x &= n; y &= n; z &= n; return *this; }
    ivec &neg() { x = -x; y = -y; z = -z; return *this; }
    ivec &min(const ivec &o) { x = ::min(x, o.x); y = ::min(y, o.y); z = ::min(z, o.z); return *this; }
    ivec &max(const ivec &o) { x = ::max(x, o.x); y = ::max(y, o.y); z = ::max(z, o.z); return *this; }
    ivec &min(int n) { x = ::min(x, n); y = ::min(y, n); z = ::min(z, n); return *this; }
    ivec &max(int n) { x = ::max(x, n); y = ::max(y, n); z = ::max(z, n); return *this; }
    ivec &abs() { x = ::abs(x); y = ::abs(y); z = ::abs(z); return *this; }
    ivec &clamp(int l, int h) { x = ::std::clamp(x, l, h); y = ::std::clamp(y, l, h); z = ::std::clamp(z, l, h); return *this; }
    ivec &cross(const ivec &a, const ivec &b) { x = a.y*b.z-a.z*b.y; y = a.z*b.x-a.x*b.z; z = a.x*b.y-a.y*b.x; return *this; }
    int dot(const ivec &o) const { return x*o.x + y*o.y + z*o.z; }
    float dist(const plane &p) const;

    static inline ivec floor(const vec &o) { return ivec(static_cast<int>(::floor(o.x)), static_cast<int>(::floor(o.y)), static_cast<int>(::floor(o.z))); }
    static inline ivec ceil(const vec &o) { return ivec(static_cast<int>(::ceil(o.x)), static_cast<int>(::ceil(o.y)), static_cast<int>(::ceil(o.z))); }
};

inline vec::vec(const ivec &v) : x(v.x), y(v.y), z(v.z) {}

template<>
struct std::hash<ivec>
{
    size_t operator()(const ivec &k) const
    {
        return k.x^k.y^k.z;
    }
};

/**
 * @brief integer vector2
 */
struct ivec2
{
    union
    {
        struct { int x, y; };
        int v[2];
    };

    ivec2() {}
    ivec2(int x, int y) : x(x), y(y) {}
    explicit ivec2(const vec2 &v) : x(static_cast<int>(v.x)), y(static_cast<int>(v.y)) {}
    explicit ivec2(const ivec &v) : x(v.x), y(v.y) {}

    int &operator[](int i)       { return v[i]; }
    int  operator[](int i) const { return v[i]; }

    bool operator==(const ivec2 &o) const { return x == o.x && y == o.y; }
    bool operator!=(const ivec2 &o) const { return x != o.x || y != o.y; }

    bool iszero() const { return x==0 && y==0; }
    ivec2 &shl(int n) { x<<= n; y<<= n; return *this; }
    ivec2 &shr(int n) { x>>= n; y>>= n; return *this; }
    ivec2 &mul(int n) { x *= n; y *= n; return *this; }
    ivec2 &div(int n) { x /= n; y /= n; return *this; }
    ivec2 &add(int n) { x += n; y += n; return *this; }
    ivec2 &sub(int n) { x -= n; y -= n; return *this; }
    ivec2 &mul(const ivec2 &v) { x *= v.x; y *= v.y; return *this; }
    ivec2 &div(const ivec2 &v) { x /= v.x; y /= v.y; return *this; }
    ivec2 &add(const ivec2 &v) { x += v.x; y += v.y; return *this; }
    ivec2 &sub(const ivec2 &v) { x -= v.x; y -= v.y; return *this; }
    ivec2 &mask(int n) { x &= n; y &= n; return *this; }
    ivec2 &neg() { x = -x; y = -y; return *this; }
    ivec2 &min(const ivec2 &o) { x = ::min(x, o.x); y = ::min(y, o.y); return *this; }
    ivec2 &max(const ivec2 &o) { x = ::max(x, o.x); y = ::max(y, o.y); return *this; }
    ivec2 &min(int n) { x = ::min(x, n); y = ::min(y, n); return *this; }
    ivec2 &max(int n) { x = ::max(x, n); y = ::max(y, n); return *this; }
    ivec2 &abs() { x = ::abs(x); y = ::abs(y); return *this; }
    int dot(const ivec2 &o) const { return x*o.x + y*o.y; }
    int cross(const ivec2 &o) const { return x*o.y - y*o.x; }
};

inline ivec::ivec(const ivec2 &v, int z) : x(v.x), y(v.y), z(z) {}

inline bvec::bvec(const vec4<uchar> &v) : x(v.x), y(v.y), z(v.z) {}

/**
 * @brief short integer three-vector object
 */
struct svec
{
    union
    {
        struct { short x, y, z; };
        short v[3];
    };

    svec() {}
    svec(short x, short y, short z) : x(x), y(y), z(z) {}
    explicit svec(const ivec &v) : x(v.x), y(v.y), z(v.z) {}

    short &operator[](int i) { return v[i]; }
    short operator[](int i) const { return v[i]; }
};

inline vec::vec(const svec &v) : x(v.x), y(v.y), z(v.z) {}
inline ivec::ivec(const svec &v) : x(v.x), y(v.y), z(v.z) {}

/**
 * @brief floating point 4x4 array object
 */
struct matrix4
{
    vec4<float> a, b, c, d;

    template<class T, class U>
    T transformnormal(const U &in) const
    {
        T v;
        transformnormal(in, v);
        return v;
    }

    template<class T, class U>
    T transform(const U &in) const
    {
        T v;
        transform(in, v);
        return v;
    }

    template<class T>
    vec perspectivetransform(const T &in) const
    {
        vec4<float> v;
        transform(in, v);
        return vec(v).div(v.w);
    }

    template<class T>
    void mult(const matrix4 &x, const matrix4 &y)
    {
        a = T(x.a).mul(y.a.x).madd(x.b, y.a.y).madd(x.c, y.a.z).madd(x.d, y.a.w);
        b = T(x.a).mul(y.b.x).madd(x.b, y.b.y).madd(x.c, y.b.z).madd(x.d, y.b.w);
        c = T(x.a).mul(y.c.x).madd(x.b, y.c.y).madd(x.c, y.c.z).madd(x.d, y.c.w);
        d = T(x.a).mul(y.d.x).madd(x.b, y.d.y).madd(x.c, y.d.z).madd(x.d, y.d.w);
    }

    matrix4();
    matrix4(const float *m);
    matrix4(const vec &a, const vec &b, const vec &c = vec(0, 0, 1));
    matrix4(const vec4<float> &a, const vec4<float> &b, const vec4<float> &c, const vec4<float> &d = vec4<float>(0, 0, 0, 1));
    matrix4(const matrix4x3 &m);
    matrix4(const matrix3 &rot, const vec &trans);
    void mul(const matrix4 &x, const matrix3 &y);
    void mul(const matrix3 &y);
    void mul(const matrix4 &x, const matrix4 &y);
    void mul(const matrix4 &y);
    void muld(const matrix4 &x, const matrix4 &y);
    void muld(const matrix4 &y);
    void rotate_around_x(float ck, float sk);
    void rotate_around_x(float angle);
    void rotate_around_x(const vec2 &sc);
    void rotate_around_y(float ck, float sk);
    void rotate_around_y(float angle);
    void rotate_around_y(const vec2 &sc);

    void rotate_around_z(float ck, float sk);
    void rotate_around_z(float angle);
    void rotate_around_z(const vec2 &sc);

    void rotate(float ck, float sk, const vec &axis);
    void rotate(float angle, const vec &dir);
    void rotate(const vec2 &sc, const vec &dir);

    /**
     * @brief Sets the matrix to I, the identity matrix
     *
     * Sets the matrix to I, with the form as follows:
     * ```
     *   a b c d
     * x 1 0 0 0
     * y 0 1 0 0
     * z 0 0 1 0
     * w 0 0 0 1
     * ```
     */
    void identity();

    void settranslation(const vec &v);
    void settranslation(float x, float y, float z);
    void translate(const vec &p);
    void translate(float x, float y, float z);
    void translate(const vec &p, float scale);
    void setscale(float x, float y, float z);
    void setscale(const vec &v);
    void setscale(float n);
    void scale(float x, float y, float z);
    void scale(const vec &v);
    void scale(float n);
    void scalexy(float x, float y);

    void scalez(float k);

    void reflectz(float z);

    void jitter(float x, float y);

    /**
     * @brief Transposes the matrix along the diagonal.
     *
     * Transforms the matrix like so (letters are just arbitrary value placeholders)
     * ```
     * a b c d     a e i m
     * e f g h  -> b f j n
     * i j k l     c g k o
     * m n o p     d h l p
     * ```
     */
    void transpose();

    /**
     * @brief Copies the transpose of the given matrix4 to `this`
     *
     * Copies the transform of the matrix passed like so (letters are just arbitrary
     * value placeholders) to `this` object.
     * ```
     * a b c d     a e i m
     * e f g h  -> b f j n
     * i j k l     c g k o
     * m n o p     d h l p
     * ```
     */
    void transpose(const matrix4 &m);
    void frustum(float left, float right, float bottom, float top, float znear, float zfar);
    void perspective(float fovy, float aspect, float znear, float zfar);

    void ortho(float left, float right, float bottom, float top, float znear, float zfar);

    void transform(const vec &in, vec &out) const;
    void transform(const vec4<float> &in, vec &out) const;

    void transform(const vec &in, vec4<float> &out) const;
    void transform(const vec4<float> &in, vec4<float> &out) const;
    void transformnormal(const vec &in, vec &out) const;
    void transformnormal(const vec &in, vec4<float> &out) const;

    void transposedtransform(const vec &in, vec &out) const;
    void transposedtransformnormal(const vec &in, vec &out) const;
    void transposedtransform(const plane &in, plane &out) const;

    vec gettranslation() const;

    /**
     * @brief Returns the first row of the matrix.
     *
     * Returns the first value of each column vector that makes up the matrix.
     *
     * @return a vec4 of the first row of the vector
     */
    vec4<float> rowx() const;

    /**
     * @brief Returns the second row of the matrix.
     *
     * Returns the second value of each column vector that makes up the matrix.
     *
     * @return a vec4 of the second row of the vector
     */
    vec4<float> rowy() const;

    /**
     * @brief Returns the third row of the matrix.
     *
     * Returns the third value of each column vector that makes up the matrix.
     *
     * @return a vec4 of the third row of the vector
     */
    vec4<float> rowz() const;

    /**
     * @brief Returns the fourth row of the matrix.
     *
     * Returns the fourth value of each column vector that makes up the matrix.
     *
     * @return a vec4 of the last row of the vector
     */
    vec4<float> roww() const;

    /**
     * @brief Sets this matrix to the inverse of the provided matrix.
     *
     * Sets the matrix values to the inverse of the provided matrix A*A^-1 = I
     * returns false if singular (or nearly singular to within tolerance of mindet)
     * or true if matrix was inverted successfully
     *
     * @param m a matrix4 object to be inverted and assigned to the object
     * @param mindet the determinant value at which matrices are considered singular
     *
     * @return false if the matrix is singular, true otherwise
     */
    bool invert(const matrix4 &m, double mindet = 1.0e-12);

    /**
     * @brief Returns the inverse of the matrix.
     *
     * Returns a zero matrix if the matrix is singular.
     *
     * @param mindet the determinant value at which matrices are considered singular
     *
     * @return a matrix4 containing the matrix inverse
     */
    matrix4 inverse(double mindet = 1.0e-12) const;

    vec2 lineardepthscale() const;
};

inline matrix3::matrix3(const matrix4 &m)
    : a(m.a), b(m.b), c(m.c)
{}

inline matrix4x3::matrix4x3(const matrix4 &m)
    : a(m.a), b(m.b), c(m.c), d(m.d)
{}

inline matrix3::matrix3(const matrix4x3 &m) : a(m.a), b(m.b), c(m.c) {}

//generic two dimensional vector
template<class T>
struct GenericVec2
{
    T x,y;

    GenericVec2() {}
    GenericVec2(T x, T y) : x(x), y(y) {}
    GenericVec2(const vec2 &v) : x(v.x), y(v.y) {}

    bool operator==(const GenericVec2 &h) const { return x == h.x && y == h.y; }
    bool operator!=(const GenericVec2 &h) const { return x != h.x || y != h.y; }
};

//generic three dmensional vector
template<class T>
struct GenericVec3
{
    T x,y,z;

    GenericVec3() {}
    GenericVec3(T x, T y, T z) : x(x), y(y), z(z) {}
    GenericVec3(const vec &v) : x(v.x), y(v.y), z(v.z) {}

    GenericVec3<T> operator+(const GenericVec3<T> &h) const { return GenericVec3<T>(x+h.x, y+h.y,z+h.z); }
    GenericVec3<T> operator-(const GenericVec3<T> &h) const { return GenericVec3<T>(x-h.x, y-h.y, z-h.z); }

    //comparison
    bool operator==(const GenericVec3<T> &h) const { return x == h.x && y == h.y && z == h.z; }
    bool operator!=(const GenericVec3<T> &h) const { return x != h.x || y != h.y || z != h.z; }
    bool operator>(const GenericVec3<T> &h) const { return x > h.x && y > h.y && z > h.z; }
    bool operator<(const GenericVec3<T> &h) const { return x < h.x && y < h.y && z < h.z; }
    bool operator>=(const GenericVec3<T> &h) const { return x >= h.x && y >= h.y && z >= h.z; }
    bool operator<=(const GenericVec3<T> &h) const { return x <= h.x && y <= h.y && z <= h.z; }
};

extern bool raysphereintersect(const vec &center, float radius, const vec &o, const vec &ray, float &dist);
extern bool rayboxintersect(const vec &b, const vec &s, const vec &o, const vec &ray, float &dist, int &orient);
extern bool linecylinderintersect(const vec &from, const vec &to, const vec &start, const vec &end, float radius, float &dist);
extern int polyclip(const vec *in, int numin, const vec &dir, float below, float above, vec *out);

extern const vec2 sincos360[]; /**< a 721 element table of cosines, sines given integral values */

/**
 * @brief Returns the angle passed to it, clamped to 0...360
 *
 * Normalizes negative/large angles passed to it to the range [0,360).
 *
 * @param angle the angle to parse
 *
 * @return the angle clamped to 0...360
 */
inline int mod360(int angle)
{
    if(angle < 0)
    {
        angle = 360 + (angle <= -360 ? angle%360 : angle);
    }
    else if(angle >= 360)
    {
        angle %= 360;
    }
    return angle;
}

/**
 * @brief Returns a vec2 containing (cos, sine) for a given integral angle.
 *
 * @param angle the angle to get the sine/cos value of
 *
 * @return a vec2 containing the cosine and sine of the given angle
 */
inline const vec2 &sincosmod360(int angle) { return sincos360[mod360(angle)]; }

/**
 * @brief Returns the cosine for an angle (in degrees)
 *
 * Must be within the bounds of 0...720. Uses a lookup table for integral values in degrees.
 *
 * @param angle the angle to get the cosine of (in degrees)
 *
 * @return the cosine of that angle
 */
inline float cos360(int angle) { return sincos360[angle].x; }

/**
 * @brief Returns the sine for an angle (in degrees)
 *
 * Must be within the bounds of 0...720. Uses a lookup table for integral values in degrees.
 *
 * @param angle the angle to get the sine of (in degrees)
 *
 * @return the sine of that angle
 */
inline float sin360(int angle) { return sincos360[angle].y; }

/**
 * @brief Returns the tangent for an angle (in degrees)
 *
 * Must be within the bounds of 0...720. Uses a lookup table for integral values in degrees.
 *
 * @param angle the angle to get the tangent of (in degrees)
 *
 * @return the tangent of that angle
 */
inline float tan360(int angle) { const vec2 &sc = sincos360[angle]; return sc.y/sc.x; }

/**
 * @brief Returns the cotangent for an angle (in degrees)
 *
 * Must be within the bounds of 0...720. Uses a lookup table for integral values in degrees.
 *
 * @param angle the angle to get the cotangent of (in degrees)
 *
 * @return the cotangent of that angle
 */
inline float cotan360(int angle) { const vec2 &sc = sincos360[angle]; return sc.x/sc.y; }

#endif /* GEOM_H_ */
