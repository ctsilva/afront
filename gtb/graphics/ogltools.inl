GTB_BEGIN_NAMESPACE

#define GTB_OGLTOOLS_GENERATE_POINTER(type, method, glcommand) \
  template <> inline void OpenGL<type>::method(type *a) { glcommand(a); }
#define GTB_OGLTOOLS_GENERATE_2(type, method, glcommand) \
  template <> inline void OpenGL<type>::method(type a, type b) { glcommand(a, b); }
#define GTB_OGLTOOLS_GENERATE_3(type, method, glcommand) \
  template <> inline void OpenGL<type>::method(type a, type b, type c) { glcommand(a, b, c); }
#define GTB_OGLTOOLS_GENERATE_4(type, method, glcommand) \
  template <> inline void OpenGL<type>::method(type a, type b, type c, type d) { glcommand(a, b, c, d); }

// glVertex*
GTB_OGLTOOLS_GENERATE_POINTER(const GLfloat,  Vertex2v, glVertex2fv);
GTB_OGLTOOLS_GENERATE_POINTER(const GLdouble, Vertex2v, glVertex2dv);
GTB_OGLTOOLS_GENERATE_POINTER(const GLint,    Vertex2v, glVertex2iv);
GTB_OGLTOOLS_GENERATE_POINTER(const GLshort,  Vertex2v, glVertex2sv);

GTB_OGLTOOLS_GENERATE_POINTER(const GLfloat,  Vertex3v, glVertex3fv);
GTB_OGLTOOLS_GENERATE_POINTER(const GLdouble, Vertex3v, glVertex3dv);
GTB_OGLTOOLS_GENERATE_POINTER(const GLint,    Vertex3v, glVertex3iv);
GTB_OGLTOOLS_GENERATE_POINTER(const GLshort,  Vertex3v, glVertex3sv);

GTB_OGLTOOLS_GENERATE_POINTER(const GLfloat,  Vertex4v, glVertex4fv);
GTB_OGLTOOLS_GENERATE_POINTER(const GLdouble, Vertex4v, glVertex4dv);
GTB_OGLTOOLS_GENERATE_POINTER(const GLint,    Vertex4v, glVertex4iv);
GTB_OGLTOOLS_GENERATE_POINTER(const GLshort,  Vertex4v, glVertex4sv);

GTB_OGLTOOLS_GENERATE_2(GLfloat,  Vertex, glVertex2f);
GTB_OGLTOOLS_GENERATE_2(GLdouble, Vertex, glVertex2d);
GTB_OGLTOOLS_GENERATE_2(GLint,    Vertex, glVertex2i);
GTB_OGLTOOLS_GENERATE_2(GLshort,  Vertex, glVertex2s);

GTB_OGLTOOLS_GENERATE_3(GLfloat,  Vertex, glVertex3f);
GTB_OGLTOOLS_GENERATE_3(GLdouble, Vertex, glVertex3d);
GTB_OGLTOOLS_GENERATE_3(GLint,    Vertex, glVertex3i);
GTB_OGLTOOLS_GENERATE_3(GLshort,  Vertex, glVertex3s);

GTB_OGLTOOLS_GENERATE_4(GLfloat,  Vertex, glVertex4f);
GTB_OGLTOOLS_GENERATE_4(GLdouble, Vertex, glVertex4d);
GTB_OGLTOOLS_GENERATE_4(GLint,    Vertex, glVertex4i);
GTB_OGLTOOLS_GENERATE_4(GLshort,  Vertex, glVertex4s);

// glColor*
GTB_OGLTOOLS_GENERATE_POINTER(const GLbyte,   Color3v, glColor3bv);
GTB_OGLTOOLS_GENERATE_POINTER(const GLdouble, Color3v, glColor3dv);
GTB_OGLTOOLS_GENERATE_POINTER(const GLfloat,  Color3v, glColor3fv);
GTB_OGLTOOLS_GENERATE_POINTER(const GLint,    Color3v, glColor3iv);
GTB_OGLTOOLS_GENERATE_POINTER(const GLshort,  Color3v, glColor3sv);
GTB_OGLTOOLS_GENERATE_POINTER(const GLubyte,  Color3v, glColor3ubv);
GTB_OGLTOOLS_GENERATE_POINTER(const GLuint,   Color3v, glColor3uiv);
GTB_OGLTOOLS_GENERATE_POINTER(const GLushort, Color3v, glColor3usv);

GTB_OGLTOOLS_GENERATE_POINTER(const GLbyte,   Color4v, glColor4bv);
GTB_OGLTOOLS_GENERATE_POINTER(const GLdouble, Color4v, glColor4dv);
GTB_OGLTOOLS_GENERATE_POINTER(const GLfloat,  Color4v, glColor4fv);
GTB_OGLTOOLS_GENERATE_POINTER(const GLint,    Color4v, glColor4iv);
GTB_OGLTOOLS_GENERATE_POINTER(const GLshort,  Color4v, glColor4sv);
GTB_OGLTOOLS_GENERATE_POINTER(const GLubyte,  Color4v, glColor4ubv);
GTB_OGLTOOLS_GENERATE_POINTER(const GLuint,   Color4v, glColor4uiv);
GTB_OGLTOOLS_GENERATE_POINTER(const GLushort, Color4v, glColor4usv);

GTB_OGLTOOLS_GENERATE_3(GLbyte,   Color, glColor3b);
GTB_OGLTOOLS_GENERATE_3(GLdouble, Color, glColor3d);
GTB_OGLTOOLS_GENERATE_3(GLfloat,  Color, glColor3f);
GTB_OGLTOOLS_GENERATE_3(GLint,    Color, glColor3i);
GTB_OGLTOOLS_GENERATE_3(GLshort,  Color, glColor3s);
GTB_OGLTOOLS_GENERATE_3(GLubyte,  Color, glColor3ub);
GTB_OGLTOOLS_GENERATE_3(GLuint,   Color, glColor3ui);
GTB_OGLTOOLS_GENERATE_3(GLushort, Color, glColor3us);

GTB_OGLTOOLS_GENERATE_4(GLbyte,   Color, glColor4b);
GTB_OGLTOOLS_GENERATE_4(GLdouble, Color, glColor4d);
GTB_OGLTOOLS_GENERATE_4(GLfloat,  Color, glColor4f);
GTB_OGLTOOLS_GENERATE_4(GLint,    Color, glColor4i);
GTB_OGLTOOLS_GENERATE_4(GLshort,  Color, glColor4s);
GTB_OGLTOOLS_GENERATE_4(GLubyte,  Color, glColor4ub);
GTB_OGLTOOLS_GENERATE_4(GLuint,   Color, glColor4ui);
GTB_OGLTOOLS_GENERATE_4(GLushort, Color, glColor4us);

// glNormal*
GTB_OGLTOOLS_GENERATE_POINTER(const GLfloat,  Normal3v, glNormal3fv);
GTB_OGLTOOLS_GENERATE_POINTER(const GLdouble, Normal3v, glNormal3dv);
GTB_OGLTOOLS_GENERATE_POINTER(const GLint,    Normal3v, glNormal3iv);
GTB_OGLTOOLS_GENERATE_POINTER(const GLshort,  Normal3v, glNormal3sv);

GTB_OGLTOOLS_GENERATE_3(GLfloat,  Normal, glNormal3f);
GTB_OGLTOOLS_GENERATE_3(GLdouble, Normal, glNormal3d);
GTB_OGLTOOLS_GENERATE_3(GLint,    Normal, glNormal3i);
GTB_OGLTOOLS_GENERATE_3(GLshort,  Normal, glNormal3s);

#undef GTB_OGLTOOLS_GENERATE_POINTER 
#undef GTB_OGLTOOLS_GENERATE_2
#undef GTB_OGLTOOLS_GENERATE_3
#undef GTB_OGLTOOLS_GENERATE_4


GTB_END_NAMESPACE
