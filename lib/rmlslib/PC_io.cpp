/**
 *
 * file:	PC_io.cpp
 * description:	Point Cloud io operations.  read and write files
 *		that store point cloud information.
 *
 * author:	Joel Daniels and Linh Ha
 * date:	May 2006
 *
 */

// -- INCLUDES -- //
#include "PC_io.h"

/**
 * These functions write the points in a cloud.  It is just a point
 * list.
 */
void PC_write( const char *filename, const std::vector< InfoPoint > &ptList )
{
  // 0. make sure that we can open the file
  FILE *filePtr = fopen(filename,"w");
  if (!filePtr)
    {
      fprintf(stderr,"[ERROR] PC_write() couldn't open file '%s'\n",filename);
      return;
    }

  // 1. output some header information to the file
  fprintf(filePtr,"PC file\n");
  fprintf(filePtr,"# point cloud automated output\n");
  fprintf(filePtr,"# authors: Joel Daniels and Linh Ha\n");
  fprintf(filePtr,"# date: May 2006\n");
  fprintf(filePtr,"#\n");

  // 2. output the point list
  fprintf(filePtr,"# -- POINT LIST --\n");
  fprintf(filePtr,"%d # number of points\n", ptList.size());
  for(unsigned int i=0; i<ptList.size(); i++)
    {
      fprintf(filePtr,"%g %g %g %d # InfoPoint[%d]\n",ptList[i].x_,ptList[i].y_,ptList[i].z_,ptList[i].isCorner_,i);
    }
  fprintf(filePtr,"#\n");

  fclose( filePtr );
}

void PC_read( const char *filename, std::vector< InfoPoint > &ptList )
{
  ptList.clear();

  // 0. ensure that we can read in the given file
  std::ifstream in;
  in.open(filename);
  if (!in)
    {
      fprintf(stderr,"[ERROR] PC_read() couldn't open file: '%s'\n",filename);
      return;
    }

  // 1. make sure that it is really an FLF file
  char buffer[256];
  in.getline( buffer,256 );
  if ( strcmp( buffer,"PC file" ) != 0 )
    {
      fprintf(stderr,"[ERROR] PC_read() file missing PC tag: '%s'\n",filename);
      return;
    }
  fprintf(stderr," *- reading file: '%s'...",filename);

  // 2. read in the header information
  in.getline( buffer,256 );	// # feature loop file automated output
  in.getline( buffer,256 );	// # authors: ...
  in.getline( buffer,256 );	// # date: ...
  in.getline( buffer,256 );	// #

  // 3. read in the point list
  in.getline( buffer,256 );	// # -- POINT LIST --
  unsigned int size;
  in >> size;   
  fprintf(stderr,"reading %d points.",size);
  in.getline( buffer,256 );	// # number of points
  for(unsigned int i=0; i<size; i++)
    {
      // read in a new point information for the point list
      InfoPoint newPoint;
      in >> newPoint.x_ >> newPoint.y_ >> newPoint.z_ >> newPoint.isCorner_;
      ptList.push_back(newPoint);
      in.getline( buffer,256 );	// # InfoPoint[#]
    }

  fprintf(stderr,"done -*\n");
  in.close();
}
