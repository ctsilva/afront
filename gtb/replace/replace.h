#ifndef GTB_REPLACE_INCLUDED
#define GTB_REPLACE_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#if !HAVE_STRICMP
extern int stricmp(const char *s1, const char *s2);
#endif

#ifdef __cplusplus
}
#endif

#endif /* GTB_REPLACE_INCLUDED */
