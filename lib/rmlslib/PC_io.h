/**
 *
 * file:	PC_io.h
 * description:	Point Cloud io operations.  read and write files
 *		that store point cloud information.
 *
 * author:	Joel Daniels and Linh Ha
 * date:	May 2006
 *
 */

#ifndef _PC_IO_H_
#define _PC_IO_H_

// -- INCLUDES -- //
#include "FLF_structs.h"
#include <stdio.h>
#include <fstream>
#include <vector>

/**
 * These functions write the points in a cloud.  It is just a point
 * list.
 */
void PC_write( const char *filename, const std::vector< InfoPoint > &ptList );
void PC_read( const char *filename, std::vector< InfoPoint > &ptList );

#endif
