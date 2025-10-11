
#include "common.h"
#include "guidance.h"
#include "triangulator.h"
#include "triangulate_iso.h"
#include "parallel.h"

#ifndef WIN32
#define HAS_ZLIB
#endif

#ifdef HAS_ZLIB
#include <zlib.h>
#endif


#include <lib/mlslib/NR/nr.h>	// for zbrent?!?!?


const real_type theta_step = M_PI_2/30;
const real_type zbrent_tol = 1e-3;

real_type iso_scale = 1; //1e9;

bool endswith(const char *s, const char *end);


#define ISO_CIRCULAR_PROJECTION

extern int curvature_sub;


#define DIM 256

template <typename T>
    int round_to_minus_inf(const T &x) {
    return ((x>=0) ?
	    (int)x :
	    (int)x - 1);
}


RegularVolume::RegularVolume() {
    dim[0]=dim[1]=dim[2]=0;
    aspect[0]=aspect[1]=aspect[2]=1;
    invalidate_all();
    data = NULL;
    boundary_cells = 0;
}


RegularVolume::~RegularVolume() {
    if (data) delete [] data;
}


template <typename SOURCETYPE>
    bool RegularVolume::ReadSource(const char *fname, bool bigendian) {

    real_type min_val = 1e34;
    real_type max_val =-1e34;

    if (endswith(fname, ".gz")) {

#ifdef HAS_ZLIB
	gtb::afree<gzFile> f(gzopen(fname, "rb"), gzclose);

	if (f==0) {
	    cerr<<"couldn't open file "<<fname<<endl;
	    return false;
	}


	if (dim[2] == 0) {
	    cerr<<"dim[2] not set for gz file!"<<endl;
	    return false;
	}

	data = new real_type[dim[0]*dim[1]*dim[2]];
	for (int i=0; i<(dim[0]*dim[1]*dim[2]); i++) {
	    SOURCETYPE s;
	    gzread(f, &s, sizeof(SOURCETYPE));

	    if (bigendian) {
		char* endianswap = (char*)&s;
		for (int j=0; j<sizeof(SOURCETYPE)/2; j++) {
		    std::swap(endianswap[j], endianswap[sizeof(SOURCETYPE)-j-1]);
		}
	    }

	    data[i] = (real_type)s * iso_scale;

	    min_val = std::min(min_val,data[i]);
	    max_val = std::max(max_val,data[i]);

	}

#else

	cerr<<"gzip files not supported!"<<endl;
	return false;

#endif

    } else {

	gtb::afree<FILE*> f(fopen(fname, "rb"), fclose);

	if (f==0) {
	    cerr<<"couldn't open file "<<fname<<endl;
	    return false;
	}


	if (dim[2] == 0) {

	    fseek(f, 0, SEEK_END);
	    int size = ftell(f);

	    if (size % (sizeof(SOURCETYPE)*dim[0]*dim[1])) {
		cerr<<"volume file size strange: "<<size<<"!"<<endl;
		return false;
	    }
	  
	    dim[2] = size/(sizeof(SOURCETYPE)*dim[0]*dim[1]);
	}


	data = new real_type[dim[0]*dim[1]*dim[2]];

	fseek(f, 0, SEEK_SET);

	for (int i=0; i<(dim[0]*dim[1]*dim[2]); i++) {
	    SOURCETYPE s;
	    fread(&s, sizeof(SOURCETYPE), 1, f);

	    if (bigendian) {
		char* endianswap = (char*)&s;
		for (int j=0; j<sizeof(SOURCETYPE)/2; j++) {
		    std::swap(endianswap[j], endianswap[sizeof(SOURCETYPE)-j-1]);
		}
	    }

	    data[i] = (real_type)s * iso_scale;

	    min_val = std::min(min_val,data[i]);
	    max_val = std::max(max_val,data[i]);
	}

    }

    cerr<<"min value: "<<min_val<<endl;
    cerr<<"max value: "<<max_val<<endl;

    return true;
}


bool RegularVolume::ReadNRRD(const char *fname) {
  
    gtb::afree<FILE*> f(fopen(fname, "rb"), fclose);

    if (f==0) {
	cerr<<"couldn't open file "<<fname<<endl;
	return false;
    }


    int source_type = -1;
    dim[0] = dim[1] = dim[2] = -1;
    aspect[0] = aspect[1] = aspect[2] = 0;
    char datafile[1024]; datafile[0]=0;
    char sst[1024];

    char endians[1024];
    bool endian = false;

    float faspect[3];

    char line[1024];
    while (fgets(line, 1000, f)) {
	if (1 == sscanf(line, "type: %s", sst)) {
	    if (!stricmp(sst, "float")) source_type=0;
	    if (!stricmp(sst, "uchar")) source_type=1;
	} else if (3 == sscanf(line, "sizes: %d %d %d", &dim[0], &dim[1], &dim[2])) {
	} else if (3 == sscanf(line, "spacings: %g %g %g", &faspect[0], &faspect[1], &faspect[2])) {
	    aspect[0]=faspect[0]; aspect[1]=faspect[1]; aspect[2]=faspect[2]; 
	} else if (1 == sscanf(line, "data file: %s", datafile)) {
	} else if (1 == sscanf(line, "endian: %s", endians)) {
	    endian = (!stricmp(endians, "big"));
	}
    }

    if (source_type < 0 ||
	dim[0]<0 || dim[1]<0 || dim[2]<0 ||
	aspect[0]==0 || aspect[1]==0 || aspect[2]==0 ||
	datafile[0]==0) {
	cerr<<"error reading nrrd header"<<endl;
	return false;
    }


    char datafile2[1024];
    strcpy(datafile2, fname);
    char *slash = strrchr(datafile2, '/');

    if (!slash) {
	strcpy(datafile2, datafile);
    } else {
	strcpy(slash+1, datafile);
    }



    switch (source_type) {
    case 0:
	return ReadSource<float>(datafile2, endian);
    case 1:
	return ReadSource<unsigned char>(datafile2, endian);
    }

    return false;

}


template <typename SOURCETYPE>
    bool RegularVolume::WriteSource(const char *fname) const {

    gtb::afree<FILE*> f(fopen(fname, "wb"), fclose);
    if (f==0) {
	cerr<<"couldn't open file "<<fname<<endl;
	return false;
    }

    for (int i=0; i<(dim[0]*dim[1]*dim[2]); i++) {
	SOURCETYPE s = (SOURCETYPE)data[i];
	fwrite(&s, sizeof(SOURCETYPE), 1, f);
    }

    return true;
}



bool RegularVolume::Write(const char *fname) const {
    if (endswith(fname, ".short.vol")) {
	return WriteSource<short>(fname);
    } else if (endswith(fname, ".float.vol")) {
	return WriteSource<float>(fname);
    } else {
	return false;
    }
}


bool RegularVolume::Read(const char *fname) {

    if (!stricmp(fname, "*gensphere.vol")) {
	GenSphere();
	return true;
    }

    bool endswith(const char *s, const char *end);
    if (endswith(fname, ".short.vol")) {
	dim[0]=dim[1]=DIM;
	aspect[0]=aspect[1]=1;
	aspect[2]=2;
	return ReadSource<short>(fname);
    } else if (endswith(fname, ".byte.vol")) {
	dim[0]=dim[1]=DIM;
	aspect[0]=aspect[1]=1;
	aspect[2]=2;
	return ReadSource<unsigned char>(fname);
    } else if (endswith(fname, ".float.vol")) {
	dim[0]=dim[1]=DIM;
	aspect[0]=aspect[1]=1;
	aspect[2]=2;
	return ReadSource<float>(fname);
    } else if (endswith(fname, ".nhdr")) {
	return ReadNRRD(fname);
    } else {
	cerr<<"unknown volume format: "<<fname<<endl;
	return false;
    }
}



void RegularVolume::GenSphere() {

    real_type sx=1;
    real_type sy=1;
    real_type sz=1;

    dim[0]=dim[1]=dim[2]=64;
    aspect[0]=aspect[1]=1;
    aspect[2]=2;


    data = new real_type[dim[0]*dim[1]*dim[2]];

    for (int x=0; x<dim[0]; x++) {
	for (int y=0; y<dim[1]; y++) {
	    for (int z=0; z<dim[2]; z++) {
		//				GetValue(x,y,z) = (real_type)(short)sqrt((double)(sx*(x-dim[0]/2)*(x-dim[0]/2) + sy*(y-dim[1]/2)*(y-dim[1]/2) + sz*(z-dim[2]/2)*(z-dim[2]/2)));
		GetValue(x,y,z) = (real_type)sqrt((double)(sx*aspect[0]*aspect[0]*(x-dim[0]/2)*(x-dim[0]/2) + 
							   sy*aspect[1]*aspect[1]*(y-dim[1]/2)*(y-dim[1]/2) + 
							   sz*aspect[2]*aspect[2]*(z-dim[2]/2)*(z-dim[2]/2)));
		//				GetValue(x,y,z) *= GetValue(x,y,z);
	    }
	}
    }
}


void RegularVolume::Smooth(int width) {

    if (Empty()) return;

    real_type *ndata = new real_type[dim[0]*dim[1]*dim[2]];

    for (int x=0; x<dim[0]; x++) {
	for (int y=0; y<dim[1]; y++) {
	    for (int z=0; z<dim[2]; z++) {

		real_type sum=0;
		ndata[xyz2index(x,y,z)] = 0;

		for (int x2=0; x2<width; x2++) {
		    for (int y2=0; y2<width; y2++) {
			for (int z2=0; z2<width; z2++) {

			    int xi = x+x2 - width/2;
			    int yi = y+y2 - width/2;
			    int zi = z+z2 - width/2;

			    if (xi<0 || xi>=dim[0]) continue;
			    if (yi<0 || yi>=dim[1]) continue;
			    if (zi<0 || zi>=dim[2]) continue;

			    real_type xweight = (width/2 + 1 - fabs((real_type)(width/2) - x2)) / (width/2 + 1);
			    real_type yweight = (width/2 + 1 - fabs((real_type)(width/2) - y2)) / (width/2 + 1);
			    real_type zweight = (width/2 + 1 - fabs((real_type)(width/2) - z2)) / (width/2 + 1);

			    real_type weight = xweight*yweight*zweight;
			    sum += weight;

			    ndata[xyz2index(x,y,z)] += weight * GetValue(xi,yi,zi);

			}
		    }
		}

		ndata[xyz2index(x,y,z)] /= sum;

	    }
	}
    }


    delete [] data;
    data = ndata;

}



void RegularVolume::Gather(const int cell[3], double nbrs[4][4][4]) const {

    for (int x=0; x<4; x++) {
	int xi = cell[0]+x-1;
	if (xi<0) xi=0;
	else if (xi>=dim[0]) xi=dim[0]-1;
	for (int y=0; y<4; y++) {
	    int yi = cell[1]+y-1;
	    if (yi<0) yi=0;
	    else if (yi>=dim[1]) yi=dim[1]-1;
	    for (int z=0; z<4; z++) {
		int zi = cell[2]+z-1;
		if (zi<0) zi=0;
		else if (zi>=dim[2]) zi=dim[2]-1;
		nbrs[x][y][z] = (double)GetValue(xi, yi, zi);
	    }
	}
    }
}


bool RegularVolume::LocalInfo(const Point3 &p, int cell[3], double xl[3]) const {

    extern bool allow_outside;
  
    for (int i=0; i<3; i++) {
	cell[i] = round_to_minus_inf(p[i]/aspect[i]);
	xl[i] = p[i] - aspect[i]*cell[i];
    }

    for (int i=0; i<3; i++) {
	if (!allow_outside && 
	    (cell[i] < boundary_cells || 
	     cell[i] > dim[i]-2-boundary_cells))
	    return false;
    }

    return true;
}



int RegularVolume::xyz2index(int x, int y, int z) const {
    return (z*dim[0]*dim[1] + y*dim[0] + x);
}



void RegularVolume::compute_bounding_box() const {

    Point3 minp = Point3(aspect[0]*boundary_cells,
			 aspect[1]*boundary_cells,
			 aspect[2]*boundary_cells);
    Point3 maxp = Point3(aspect[0]*(dim[0]-1-boundary_cells),
			 aspect[1]*(dim[1]-1-boundary_cells),
			 aspect[2]*(dim[2]-1-boundary_cells));

    _bounding_box = Box3(Point3(std::min(minp[0], maxp[0]),
				std::min(minp[1], maxp[1]),
				std::min(minp[2], maxp[2])),
			 Point3(std::max(minp[0], maxp[0]),
				std::max(minp[1], maxp[1]),
				std::max(minp[2], maxp[2])));

    _is_bounding_box_valid = true;
}


void RegularVolume::compute_centroid() const {

    _centroid = bounding_box().centroid();
    _is_centroid_valid = true;
}



void RegularVolume::GetBSplineValuesParallel(int nt, int id, const TrivariateSpline<double> &spline, real_type *ret) const {

    for (int z=id; z<dim[2]; z+=nt) {
	for (int y=0; y<dim[1]; y++) {
	    for (int x=0; x<dim[0]; x++) {

		int cell[3];
		double nbrs[4][4][4];
		double xl[3];

		Point3 p((real_type)x*aspect[0], (real_type)y*aspect[1], (real_type)z*aspect[2]);

		real_type val;

		if (!LocalInfo(p, cell, xl)) {
		    val = data[xyz2index(x,y,z)];
		    cerr<<"!LocalInfo"<<endl;
		} else {
		    Gather(cell, nbrs);
		    val = spline.Eval(aspect[0], aspect[1], aspect[2], nbrs, xl);
		}

		ret[xyz2index(x,y,z)] = val;
	    }
	}
    }
}


real_type* RegularVolume::GetBSplineValues() const {

    real_type *ret = new real_type[dim[0]*dim[1]*dim[2]];

    extern bool allow_outside;
    bool oldallow=allow_outside;
    allow_outside=true;

    TrivariateSpline<double> spline;
    spline.SetCoefsBSpline();
    ParallelExecutor(idealNumThreads, makeClassFunctor(this, &RegularVolume::GetBSplineValuesParallel), spline, ret);

    allow_outside=oldallow;

    return ret;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////



IsoSurfaceProjector::IsoSurfaceProjector(const RegularVolume &vol, real_type iso, bool bspline) : volume(vol), isovalue(iso) {

    if (bspline)
	spline.SetCoefsBSpline();
    else
	spline.SetCoefsCatmullRom();
}


IsoSurfaceProjector::~IsoSurfaceProjector() {
}



int IsoSurfaceProjector::ProjectPoint(const Point3 &fp, Point3 &tp, Vector3 &tn) const {

    // newton step onto the surface
    tp = fp;
    int last_cell[3]={-1,-1,-1};
    double nbrs[4][4][4];

    int iter=0;
    while (1) {

	iter++;
	if (iter>50) {
	    cerr<<"!";
	    return PROJECT_FAILURE;
	}

	int this_cell[3];
	double xl[3];	// local coordinate
	if (!volume.LocalInfo(tp, this_cell, xl)) {
	    return PROJECT_FAILURE; //PROJECT_BOUNDARY;
	}

	if (this_cell[0]!=last_cell[0] || this_cell[1]!=last_cell[1] || this_cell[2]!=last_cell[2]) {

	    volume.Gather(this_cell, nbrs);
	    last_cell[0]=this_cell[0];
	    last_cell[1]=this_cell[1];
	    last_cell[2]=this_cell[2];
	}


	double gradient[3];
	spline.Gradient(volume.GetAspect(0), volume.GetAspect(1), volume.GetAspect(2), nbrs, xl, gradient);
	double f = spline.Eval(volume.GetAspect(0), volume.GetAspect(1), volume.GetAspect(2), nbrs, xl);

	double step = -(f-isovalue) / (gradient[0]*gradient[0] + gradient[1]*gradient[1] + gradient[2]*gradient[2]);

	tp[0] += (real_type)(step*gradient[0]);
	tp[1] += (real_type)(step*gradient[1]);
	tp[2] += (real_type)(step*gradient[2]);

	tn[0]=(real_type)gradient[0];
	tn[1]=(real_type)gradient[1];
	tn[2]=(real_type)gradient[2];


	// check for stopping
	extern real_type proj_tol;
	if (step<1e-4 && fabs(f-isovalue)<proj_tol) {
	    break;
	}

    }

    tn.normalize();
    return PROJECT_SUCCESS;
}



int IsoSurfaceProjector::ProjectPointInCell(const Point3 &fp, Point3 &tp, Vector3 &tn) const {

    // newton step onto the surface

    tp = fp;
    int last_cell[3]={-1,-1,-1};
    double nbrs[4][4][4];

    bool first = true;

    int iter=0;
    while (1) {

	iter++;
	if (iter>50) {
	    cerr<<"!";
	    return PROJECT_FAILURE;
	}

	int this_cell[3];
	double xl[3];	// local coordinate
	if (!volume.LocalInfo(tp, this_cell, xl)) return PROJECT_FAILURE; //PROJECT_BOUNDARY;

	if (this_cell[0]!=last_cell[0] || this_cell[1]!=last_cell[1] || this_cell[2]!=last_cell[2]) {

	    if (!first)
		return PROJECT_FAILURE;

	    volume.Gather(this_cell, nbrs);
	    last_cell[0]=this_cell[0];
	    last_cell[1]=this_cell[1];
	    last_cell[2]=this_cell[2];
	}


	double gradient[3];
	spline.Gradient(volume.GetAspect(0), volume.GetAspect(1), volume.GetAspect(2), nbrs, xl, gradient);
	double f = spline.Eval(volume.GetAspect(0), volume.GetAspect(1), volume.GetAspect(2), nbrs, xl);

	double step = -(f-isovalue) / (gradient[0]*gradient[0] + gradient[1]*gradient[1] + gradient[2]*gradient[2]);

	tp[0] += (real_type)(step*gradient[0]);
	tp[1] += (real_type)(step*gradient[1]);
	tp[2] += (real_type)(step*gradient[2]);

	tn[0]=(real_type)gradient[0];
	tn[1]=(real_type)gradient[1];
	tn[2]=(real_type)gradient[2];


	first = false;

	// check for stopping
	extern real_type proj_tol;
	if (step<1e-4 && fabs(f-isovalue)<proj_tol) {
	    break;
	}

    }

    if (tn.length() < 1e-5)
	return PROJECT_FAILURE;

    cerr<<"iter: "<<iter<<endl;
    cerr<<"gradmag: "<<tn.length()<<endl;
    tn.normalize();
    return PROJECT_SUCCESS;
}



int IsoSurfaceProjector::ProjectPoint(const FrontElement &base1, const FrontElement &base2, const Point3 &fp, const Vector3 &fn, Point3 &tp, Vector3 &tn) const {

#ifdef ISO_CIRCULAR_PROJECTION

    Vector3 axis = base2.position - base1.position;
    axis.normalize();
  
    Point3 center = base1.position + (axis.dot(fp - base1.position)) * axis;
    real_type rad = Point3::distance(center, fp);


    Vector3 udir = fp - center;
    udir.normalize();
    Vector3 vdir = axis.cross(udir);
    vdir.normalize();

    udir *= rad;
    vdir *= rad;

    RingEval re(*this, center, udir, vdir);


    // look for a decent bracket
    real_type theta_min, theta_max, f_theta_min, f_theta_max;
    theta_min = theta_max = 0;
    f_theta_min = f_theta_max = re(0);


    if (f_theta_max < 0) { 
		
	// turn towards positive
	do {
      
	    theta_min = theta_max;
	    f_theta_min = f_theta_max;
      
	    theta_max += theta_step;
	    f_theta_max = re(theta_max);

	    if (re.hit_boundary) {
		cerr<<"couldn't eval ring position"<<endl;
		return PROJECT_FAILURE; //BOUNDARY;
	    }
      
	    if (theta_max>M_PI_2) {
		cerr<<"couldn't bracket"<<endl;
		return PROJECT_FAILURE;
	    }
      
	} while (f_theta_max<0);
    
    } else {
    
	// turn towards negative
	do {
      
	    theta_max = theta_min;
	    f_theta_max = f_theta_min;
      
	    theta_min -= theta_step;
	    f_theta_min = re(theta_min);
      
	    if (re.hit_boundary) {
		cerr<<"couldn't eval ring position"<<endl;
		return PROJECT_FAILURE; //BOUNDARY;
	    }
      
	    if (theta_min<-M_PI_2) {
		cerr<<"couldn't bracket"<<endl;
		return PROJECT_FAILURE;
	    }
      
	} while (f_theta_min>0);
    
    }

  
    real_type zero = NR::zbrent(re, theta_min, theta_max, zbrent_tol*Point3::distance(base1.position, base2.position));
    tp = center + (real_type)cos(zero)*udir + (real_type)sin(zero)*vdir;


    int this_cell[3];
    double nbrs[4][4][4];
    double xl[3];	// local coordinate
    if (!volume.LocalInfo(tp, this_cell, xl)) return PROJECT_FAILURE; //BOUNDARY;
    volume.Gather(this_cell, nbrs);

    double gradient[3];
    spline.Gradient(volume.GetAspect(0), volume.GetAspect(1), volume.GetAspect(2), nbrs, xl, gradient);
    tn[0]=(real_type)gradient[0];
    tn[1]=(real_type)gradient[1];
    tn[2]=(real_type)gradient[2];
    tn.normalize();

    return PROJECT_SUCCESS;

#else

    return ProjectPoint(fp, tp, tn);

#endif
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void IsoSurfaceGuidanceField::BuildGuidanceParallel(int nt, int id, real_type isovalue, IsoSurfaceProjector &projector, const real_type *values) { 

    vector<Point3> mypoints;
    vector<Vector3> mynorms;
    vector<real_type> mycurvatures;
    static thlib::CSObject cs;

    mypoints.reserve(10000);
    mynorms.reserve(10000);
    mycurvatures.reserve(10000);


    for (int z=0+id; z<volume.GetDim(2)-1; z+=nt) {
	for (int y=0; y<volume.GetDim(1)-1; y++) {
	    for (int x=0; x<volume.GetDim(0)-1; x++) {
	      

		real_type max = -1e34;
		real_type min = 1e34;
	      
		extern int grid_intersect_overestimate;
		int gio = grid_intersect_overestimate;
		for (int x2=-gio; x2<2+gio; x2++) {
		    for (int y2=-gio; y2<2+gio; y2++) {
			for (int z2=-gio; z2<2+gio; z2++) {
			    int t[3] = { x+x2, y+y2, z+z2 };
			    for (int i=0; i<3; i++) {
				if (t[i]<0)
				    t[i] = 0;
				else if (t[i] >= volume.GetDim(i))
				    t[i] = volume.GetDim(i)-1;
			    }

			    real_type f = values[volume.xyz2index(t[0],t[1],t[2])];
			    max = std::max(max, f);
			    min = std::min(min, f);
			}
		    }
		}


		bool enter = (min<=isovalue && max>=isovalue);

		if (!enter) {
#if 0
		    Point3 cell_center((x+0.5)*volume.GetAspect(0),
				       (y+0.5)*volume.GetAspect(1),
				       (z+0.5)*volume.GetAspect(2));
		
		    Point3 tp;
		    Vector3 tn;
		    real_type k1,k2;
		    if (projector.ProjectPointInCell(cell_center, tp, tn) == PROJECT_SUCCESS &&
			CurvatureAtPoint(tp, k1,k2)) {
		  
			real_type kmax = k2;
			if (fabs(k1)>fabs(k2))
			    kmax = k1;
			
			mypoints.push_back(tp);
			mynorms.push_back(tn);
			mycurvatures.push_back(1/kmax);
			enter=true;
		    }
#endif


		}

		if (enter) {
		  
		    real_type spacing = 1.0 / curvature_sub;
		
		    for (int x2=0; x2<curvature_sub; x2++) {
			for (int y2=0; y2<curvature_sub; y2++) {
			    for (int z2=0; z2<curvature_sub; z2++) {
			
				Point3 sl(x+(x2+myran1f(id))*spacing,
					  y+(y2+myran1f(id))*spacing,
					  z+(z2+myran1f(id))*spacing);
				sl[0] *= volume.GetAspect(0);
				sl[1] *= volume.GetAspect(1);
				sl[2] *= volume.GetAspect(2);
			
		      
				Point3 tp;
				Vector3 tn;
				real_type k1,k2;
				if (projector.ProjectPoint(sl, tp, tn)==PROJECT_SUCCESS &&
				    CurvatureAtPoint(tp, k1, k2)) {
			
				    real_type kmax = k2;
				    if (fabs(k1)>fabs(k2))
					kmax = k1;
			  
				    mypoints.push_back(tp);
				    mynorms.push_back(tn);
				    mycurvatures.push_back(kmax);

				}			  
			    }
			}
		    }
		}
	    }
	}
    }

    cs.enter();
  
    for (unsigned i=0; i<mypoints.size(); i++)
	kdGetPoint.allpoints.push_back(mypoints[i]);
  
    for (unsigned i=0; i<mycurvatures.size(); i++)
	ideal_length.push_back(MaxCurvatureToIdeal(mycurvatures[i] / 3));

    if (guidanceNormals) {
	for (unsigned i=0; i<mypoints.size(); i++)
	    guidanceNormals->push_back(-mynorms[i]);
    }

  
    cs.leave();
}



IsoSurfaceGuidanceField::IsoSurfaceGuidanceField(IsoSurfaceProjector &projector, const RegularVolume &vol, bool bspline, real_type rho, real_type min_step, real_type max_step, real_type reduction)
    : GuidanceField(rho, min_step, max_step, reduction), volume(vol), kdGetPoint(), kdOrderedTraverse(NULL), guidanceNormals(NULL) {

    if (bspline)
	spline.SetCoefsBSpline();
    else
	spline.SetCoefsCatmullRom();


    // if want to store the guidance field points for visualization
    //	vector<Vector3> norms;
    //	guidanceNormals = &norms;



    real_type isovalue = projector.GetIsoValue();
    real_type *values = (bspline) ? volume.GetBSplineValues() : volume.CopyValues();
    ParallelExecutor(idealNumThreads, makeClassFunctor(this,&IsoSurfaceGuidanceField::BuildGuidanceParallel), isovalue, projector, values);
    delete [] values;

    // setup the kdtree
    kdtree = new kdtree_type(10, volume.bounding_box(), kdGetPoint);
    for (unsigned i=0; i<kdGetPoint.allpoints.size(); i++)
	kdtree->Insert(i);
    kdtree->MakeTree();


    if (guidanceNormals) {
	FILE *f = fopen("guidance.orig.obj", "w");
	if (f) {
	    for (unsigned i=0; i<kdGetPoint.allpoints.size(); i++) {
		fprintf(f, "v %g %g %g\n", kdGetPoint.allpoints[i][0], kdGetPoint.allpoints[i][1], kdGetPoint.allpoints[i][2]);
		fprintf(f, "vn %g %g %g\n", (*guidanceNormals)[i][0], (*guidanceNormals)[i][1], (*guidanceNormals)[i][2]);
		fprintf(f, "vc 0.97 0.97 0.97\n");
	    }
	    fclose(f);
	}
    }


    vector<int> marked; // for removal
    Trim(marked);


    vector<Point3> keepers;
    vector<real_type> keep_curv;
    for (unsigned i=0; i<marked.size(); i++) {
	if (!marked[i]) {
	    keepers.push_back(kdGetPoint.allpoints[i]);
	    keep_curv.push_back(ideal_length[i]);
	}
    }
    kdGetPoint.allpoints = keepers;
    ideal_length = keep_curv;


    if (guidanceNormals) {
	FILE *f = fopen("guidance.trimmed.obj", "w");
	if (f) {
	    for (unsigned i=0; i<guidanceNormals->size(); i++) {
		Point3 p = kdGetPoint.allpoints[i];
		p += ((real_type)0.1) * (*guidanceNormals)[i];
		if (!marked[i]) {
		    fprintf(f, "v %g %g %g\n", p[0], p[1], p[2]);
		    fprintf(f, "vn %g %g %g\n", (*guidanceNormals)[i][0], (*guidanceNormals)[i][1], (*guidanceNormals)[i][2]);
		    fprintf(f, "vc 1.0 0.3 0.1\n");
		}
	    }
	    fclose(f);
	}
    }

    guidanceNormals = NULL;



    delete kdtree;
    kdtree = new kdtree_type(10, volume.bounding_box(), kdGetPoint);
    for (unsigned i=0; i<kdGetPoint.allpoints.size(); i++)
	kdtree->Insert(i);
    kdtree->MakeTree();

    cerr<<"Guidance field with "<<kdGetPoint.allpoints.size()<<" points"<<endl;

}


IsoSurfaceGuidanceField::~IsoSurfaceGuidanceField() {
    if (kdOrderedTraverse) delete kdOrderedTraverse;
    delete kdtree;
}


int IsoSurfaceGuidanceField::ClosestPoint(const Point3 &p) {
    return kdtree->FindMin(p);
}


void IsoSurfaceGuidanceField::OrderedPointTraverseStart(const Point3 &p) {
    if (kdOrderedTraverse)
	delete kdOrderedTraverse;
    kdOrderedTraverse = new kdtree_type::OrderedIncrementalTraverse(*kdtree, p);
}

int IsoSurfaceGuidanceField::OrderedPointTraverseNext(real_type &squared_dist) {
    if (kdOrderedTraverse->empty()) return -1;
    return kdOrderedTraverse->GetNext(squared_dist);
}

void IsoSurfaceGuidanceField::OrderedPointTraverseEnd() {
    delete kdOrderedTraverse;
    kdOrderedTraverse = NULL;
}



const Point3& IsoSurfaceGuidanceField::PointLocation(int i) const {
    return kdGetPoint(i);
}


int IsoSurfaceGuidanceField::NumPoints() const {
    return (int)kdGetPoint.allpoints.size();
}


void IsoSurfaceGuidanceField::Extract(vector<Point3> &pts, vector<Vector3> &norms, vector<real_type> &rad) {
    pts.resize(kdGetPoint.allpoints.size());
    norms.resize(kdGetPoint.allpoints.size());
    rad.resize(kdGetPoint.allpoints.size());

    for (unsigned i=0; i<kdGetPoint.allpoints.size(); i++) {

	pts[i] = kdGetPoint.allpoints[i];
	if (!NormalAtPoint(pts[i], norms[i])) norms[i] = Vector3(0,0,0);
	rad[i] = ideal_length[i];

    }
}


real_type IsoSurfaceGuidanceField::EvalAtPoint(const Point3 &p) const {

    double xl[3];
    int cell[3];
    double nbrs[4][4][4];

    if (!volume.LocalInfo(p, cell, xl)) return 1e34;
    volume.Gather(cell, nbrs);
    return spline.Eval(volume.GetAspect(0), volume.GetAspect(1), volume.GetAspect(2), nbrs, xl);
}


bool IsoSurfaceGuidanceField::NormalAtPoint(const Point3 &p, Vector3 &n) const {

    double xl[3];
    int cell[3];
    double nbrs[4][4][4];

    if (!volume.LocalInfo(p, cell, xl)) return false;
    volume.Gather(cell, nbrs);

    double gradient[3];
    spline.Gradient(volume.GetAspect(0), volume.GetAspect(1), volume.GetAspect(2), nbrs, xl, gradient);

    n = Vector3(gradient[0], gradient[1], gradient[2]);
    n.normalize();

    return true;
}



bool IsoSurfaceGuidanceField::CurvatureAtPoint(const Point3 &p, real_type &k1, real_type &k2) const {

    double xl[3];
    int cell[3];
    double nbrs[4][4][4];

    if (!volume.LocalInfo(p, cell, xl)) return false;
    volume.Gather(cell, nbrs);


    typedef gtb::tmat3<double> mat3;

    mat3 H;
    spline.Hessian(volume.GetAspect(0), volume.GetAspect(1), volume.GetAspect(2), nbrs, xl, H);

    double gradient[3];
    spline.Gradient(volume.GetAspect(0), volume.GetAspect(1), volume.GetAspect(2), nbrs, xl, gradient);

    
    gtb::tVector3<double> normal(gradient[0], gradient[1], gradient[2]);
    double gradmag = normal.length();
    normal /= gradmag;

    mat3 nnt(normal[0]*normal, normal[1]*normal, normal[2]*normal);
    mat3 I(gtb::tVector3<double>(1,0,0), gtb::tVector3<double>(0,1,0), gtb::tVector3<double>(0,0,1));
    mat3 P = I - nnt;

    mat3 G = (-1/gradmag) * P*H*P;


    double T = G[0][0] + G[1][1] + G[2][2];	// trace
    double F=0;	// frobenius norm
    for (int i=0; i<3; i++) {
	for (int j=0; j<3; j++) {
	    F += G[i][j]*G[i][j];
	}
    }
    F = sqrt(F);

    double st = sqrt(std::max(0.0, 2*F*F-T*T));
    k1 = (real_type)((T+st)/2);
    k2 = (real_type)((T-st)/2);

    return true;
}



















