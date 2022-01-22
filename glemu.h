/**
 * @file glemu.h
 * @brief
 */

#ifndef GLEMU_H_
#define GLEMU_H_

namespace gle
{
    enum
    {
        Attribute_Vertex        = 0,
        Attribute_Color         = 1,
        Attribute_TexCoord0     = 2,
        Attribute_TexCoord1     = 3,
        Attribute_Normal        = 4,
        Attribute_Tangent       = 5,
        Attribute_BoneWeight    = 6,
        Attribute_BoneIndex     = 7,
        Attribute_NumAttributes = 8
    };

    inline const char * const attribnames[Attribute_NumAttributes] = { "vvertex", "vcolor", "vtexcoord0", "vtexcoord1", "vnormal", "vtangent", "vboneweight", "vboneindex" };
    extern ucharbuf attribbuf;

    extern int enabled;
    extern void forcedisable();
    inline void disable() { if(enabled) forcedisable(); }

    extern void begin(GLenum mode);
    extern void begin(GLenum mode, int numverts);
    extern void multidraw();
    extern void defattribs(const char *fmt);
    extern void defattrib(int type, int size, int format);

    extern void defvertex(int size = 3, int format = GL_FLOAT);
    extern void defcolor(int size = 3, int format = GL_FLOAT);
    extern void deftexcoord0(int size = 2, int format = GL_FLOAT);
    extern void deftexcoord1(int size = 2, int format = GL_FLOAT);
    extern void defnormal(int size = 3, int format = GL_FLOAT);
    extern void deftangent(int size = 4, int format = GL_FLOAT);
    extern void defboneweight(int size = 4, int format = GL_UNSIGNED_BYTE);
    extern void defboneindex(int size = 4, int format = GL_UNSIGNED_BYTE);

    extern void vertexf(float x);
    extern void vertexf(float x, float y);
    extern void vertexf(float x, float y, float z);
    extern void vertexf(float x, float y, float z, float w);
    extern void vertex(const vec &v);
    extern void vertex(const vec &v, float w);
    extern void vertex(const vec2<float> &v);

    extern void colorf(float x);
    extern void colorf(float x, float y);
    extern void colorf(float x, float y, float z);
    extern void colorf(float x, float y, float z, float w);

    extern void color(const vec &v);
    extern void color(const vec &v, float w);
    extern void color(const vec2<float> &v);

    extern void colorub(uchar x, uchar y, uchar z, uchar w = 255);
    extern void color(const bvec &v, uchar alpha = 255);
    extern void texcoord0f(float x);
    extern void texcoord0f(float x, float y);
    extern void texcoord0f(float x, float y, float z);
    extern void texcoord0f(float x, float y, float z, float w);

    extern void texcoord0(const vec &v);
    extern void texcoord0(const vec &v, float w);
    extern void texcoord0(const vec2<float> &v);
    extern void texcoord0(const vec4<float> &v);
    extern void texcoord1f(float x);
    extern void texcoord1f(float x, float y);
    extern void texcoord1f(float x, float y, float z);
    extern void texcoord1f(float x, float y, float z, float w);
    extern void texcoord1(const vec &v);
    extern void texcoord1(const vec &v, float w);
    extern void texcoord1(const vec2<float> &v);
    extern void normal(float x, float y, float z);
    extern void normal(const vec &v);
    extern void tangent(float x, float y, float z, float w = 1.0f);
    extern void tangent(const vec &v, float w = 1.0f);

    extern void enableattrib(int index);
    extern void disableattrib(int index);
    extern void enablevertex();
    extern void disablevertex();
    extern void vertexpointer(int stride, const void *data, GLenum type = GL_FLOAT, int size = 3, GLenum normalized = GL_FALSE);
    extern void enablecolor();
    extern void disablecolor();
    extern void colorpointer(int stride, const void *data, GLenum type = GL_UNSIGNED_BYTE, int size = 4, GLenum normalized = GL_TRUE);
    extern void enabletexcoord0();
    extern void disabletexcoord0();
    extern void texcoord0pointer(int stride, const void *data, GLenum type = GL_FLOAT, int size = 2, GLenum normalized = GL_FALSE);
    extern void enabletexcoord1();
    extern void disabletexcoord1();
    extern void texcoord1pointer(int stride, const void *data, GLenum type = GL_FLOAT, int size = 2, GLenum normalized = GL_FALSE);
    extern void enablenormal();
    extern void disablenormal();
    extern void normalpointer(int stride, const void *data, GLenum type = GL_FLOAT, int size = 3, GLenum normalized = GL_TRUE);
    extern void enabletangent();
    extern void disabletangent();
    extern void tangentpointer(int stride, const void *data, GLenum type = GL_FLOAT, int size = 4, GLenum normalized = GL_TRUE);
    extern void enableboneweight();
    extern void disableboneweight();
    extern void boneweightpointer(int stride, const void *data, GLenum type = GL_UNSIGNED_BYTE, int size = 4, GLenum normalized = GL_TRUE);
    extern void enableboneindex();
    extern void disableboneindex();
    extern void boneindexpointer(int stride, const void *data, GLenum type = GL_UNSIGNED_BYTE, int size = 4, GLenum normalized = GL_FALSE);

    extern void bindebo(GLuint ebo);
    extern void clearebo();
    extern void bindvbo(GLuint vbo);
    extern void clearvbo();

    extern void attribf(float x);
    extern void attribf(float x, float y);
    extern void attribf(float x, float y, float z);
    extern void attribf(float x, float y, float z, float w);
    extern void attribd(double x);
    extern void attribd(double x, double y);
    extern void attribd(double x, double y, double z);
    extern void attribd(double x, double y, double z, double w);
    extern void attribb(char x);
    extern void attribb(char x, char y);
    extern void attribb(char x, char y, char z);
    extern void attribb(char x, char y, char z, char w);
    extern void attribub(uchar x);
    extern void attribub(uchar x, uchar y);
    extern void attribub(uchar x, uchar y, uchar z);
    extern void attribub(uchar x, uchar y, uchar z, uchar w);
    extern void attribs(short x);
    extern void attribs(short x, short y);
    extern void attribs(short x, short y, short z);
    extern void attribs(short x, short y, short z, short w);
    extern void attribus(ushort x);
    extern void attribus(ushort x, ushort y);
    extern void attribus(ushort x, ushort y, ushort z);
    extern void attribus(ushort x, ushort y, ushort z, ushort w);
    extern void attribi(int x);
    extern void attribi(int x, int y);
    extern void attribi(int x, int y, int z);
    extern void attribi(int x, int y, int z, int w);
    extern void attribui(uint x);
    extern void attribui(uint x, uint y);
    extern void attribui(uint x, uint y, uint z);
    extern void attribui(uint x, uint y, uint z, uint w);

    extern void attrib(const vec &v);
    extern void attrib(const vec &v, float w);
    extern void attrib(const vec2<float> &v);
    extern void attrib(const ivec &v);
    extern void attrib(const ivec &v, int w);
    extern void attrib(const ivec2 &v);
    extern void attrib(const bvec &b);
    extern void attrib(const bvec &b, uchar w);

    extern int end();

    extern void enablequads();
    extern void disablequads();
    extern void drawquads(int offset, int count);

    extern void setup();
    extern void cleanup();
}

#endif /* GLEMU_H_ */
