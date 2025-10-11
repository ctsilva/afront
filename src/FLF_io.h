/**
 *
 * file:	FLF_io.h
 * description:	Feature Loop File converter provides the file
 *		write and read functions that will convert our
 *		extracted features into a format that is necessary
 *		for John and Carlos' triangulation method.
 *
 * author:	Joel Daniels and Linh Ha
 * date:	May 2006
 *
 */

#ifndef _FLF_IO_H_
#define _FLF_IO_H_

// -- INCLUDES -- //
#include "FLF_structs.h"
#include <stdio.h>
#include <fstream>
#include <vector>

/**
 * These functions write the features extracted from the point cloud
 * as feature loops or read feature loops into the standard vectors.
 * The feature loops are ordered lists of indices with associated
 * normals at each point.  These indices will access the point list
 * also given to the file.
 */
void FLF_write( const char *filename, const std::vector< InfoPoint > &ptList, const std::vector< std::vector< InfoIndex > > &featList );
void FLF_read( const char *filename, std::vector< InfoPoint > &ptList, std::vector< std::vector< InfoIndex > > &featList );

#endif
