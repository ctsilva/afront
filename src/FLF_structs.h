/**
 *
 * file:	FLF_structs.h
 * description:	Saves some structures used in the output and creation
 *              of the feature loop files.
 *
 * author:	Joel Daniels and Linh Ha
 * date:	May 2006
 *
 */

#ifndef _FLF_STRUCTS_H_
#define _FLF_STRUCTS_H_

typedef struct {
  float x_,y_,z_;
  bool isCorner_;
} InfoPoint;

typedef struct {
  unsigned int index_;
  float x_,y_,z_;
} InfoIndex;

#endif
