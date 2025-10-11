
#ifndef _TRIANGULATE_ISO
#define	_TRIANGULATE_ISO



template <typename T>
    class TrivariateSpline {
    public:

    TrivariateSpline() { }

	
    void SetCoefsCatmullRom(real_type tau=0.5) {
	CoefsFromMatrix(gtb::tMatrix4<double>(0, 1, 0, 0,
					      -tau, 0, tau, 0,
					      2*tau, tau-3, 3-2*tau, -tau,
					      -tau, 2-tau, tau-2, tau));
    }

    void SetCoefsBSpline() {
	CoefsFromMatrix(gtb::tMatrix4<double>( 1/6.0,  4/6.0,  1/6.0, 0/6.0,
					       -3/6.0,  0/6.0,  3/6.0, 0/6.0,
					       3/6.0, -6/6.0,  3/6.0, 0/6.0,
					       -1/6.0,  3/6.0, -3/6.0, 1/6.0));
    }


    T Eval(T xaspect, T yaspect, T zaspect, const T p[4][4][4], T x[3]) const {

	T xscale = 1.0 / xaspect;
	T yscale = 1.0 / yaspect;
	T zscale = 1.0 / zaspect;

	T xs[3] = { x[0]*xscale, x[1]*yscale, x[2]*zscale };

	T xpows[4] = { 1, xs[0], xs[0]*xs[0], xs[0]*xs[0]*xs[0] };
	T ypows[4] = { 1, xs[1], xs[1]*xs[1], xs[1]*xs[1]*xs[1] };
	T zpows[4] = { 1, xs[2], xs[2]*xs[2], xs[2]*xs[2]*xs[2] };

	T ret=0;
	for (unsigned i=0; i<coefs.size(); i++) {

	    const TrivariateCoefficients &tc = coefs[i];
	    T c=0;
	    for (unsigned j=0; j<tc.coefs.size(); j++) {
		c += (&p[0][0][0])[tc.coefs[j].first] * tc.coefs[j].second;
	    }

	    ret += c * xpows[tc.xpow] * ypows[tc.ypow] * zpows[tc.zpow];

	}

	return ret;
    }


    void Gradient(T xaspect, T yaspect, T zaspect, const T p[4][4][4], const T x[3], T gradient[3]) const {

	T xscale = 1.0 / xaspect;
	T yscale = 1.0 / yaspect;
	T zscale = 1.0 / zaspect;

	T xs[3] = { x[0]*xscale, x[1]*yscale, x[2]*zscale };

	T xpows[4] = { 1, xs[0], xs[0]*xs[0], xs[0]*xs[0]*xs[0] };
	T ypows[4] = { 1, xs[1], xs[1]*xs[1], xs[1]*xs[1]*xs[1] };
	T zpows[4] = { 1, xs[2], xs[2]*xs[2], xs[2]*xs[2]*xs[2] };

	gradient[0]=gradient[1]=gradient[2]=0;

	for (unsigned i=0; i<coefs.size(); i++) {

	    const TrivariateCoefficients &tc = coefs[i];
	    T c=0;
	    for (unsigned j=0; j<tc.coefs.size(); j++) {
		c += (&p[0][0][0])[tc.coefs[j].first] * tc.coefs[j].second;
	    }

	    if (tc.xpow>0)
		gradient[0] += c * xscale * tc.xpow * xpows[tc.xpow-1] * ypows[tc.ypow] * zpows[tc.zpow];

	    if (tc.ypow>0)
		gradient[1] += c * yscale * tc.ypow * xpows[tc.xpow] * ypows[tc.ypow-1] * zpows[tc.zpow];

	    if (tc.zpow>0)
		gradient[2] += c * zscale * tc.zpow * xpows[tc.xpow] * ypows[tc.ypow] * zpows[tc.zpow-1];

	}
    }


    void Hessian(T xaspect, T yaspect, T zaspect, const T p[4][4][4], const T x[3], gtb::tmat3<T> &hessian) const {

	T xscale = 1.0 / xaspect;
	T yscale = 1.0 / yaspect;
	T zscale = 1.0 / zaspect;

	T xs[3] = { x[0]*xscale, x[1]*yscale, x[2]*zscale };

	T xpows[4] = { 1, xs[0], xs[0]*xs[0], xs[0]*xs[0]*xs[0] };
	T ypows[4] = { 1, xs[1], xs[1]*xs[1], xs[1]*xs[1]*xs[1] };
	T zpows[4] = { 1, xs[2], xs[2]*xs[2], xs[2]*xs[2]*xs[2] };

	for (int i=0; i<3; i++) {
	    for (int j=0; j<3; j++) {
		hessian[i][j] = 0;
	    }
	}

	for (unsigned i=0; i<coefs.size(); i++) {

	    const TrivariateCoefficients &tc = coefs[i];
	    T c=0;
	    for (unsigned j=0; j<tc.coefs.size(); j++) {
		c += (&p[0][0][0])[tc.coefs[j].first] * tc.coefs[j].second;
	    }

	    if (tc.xpow>1)
		hessian[0][0] += c * xscale * xscale * tc.xpow * (tc.xpow-1) * xpows[tc.xpow-2] * ypows[tc.ypow] * zpows[tc.zpow];
	    if (tc.ypow>1)
		hessian[1][1] += c * yscale * yscale * tc.ypow * (tc.ypow-1) * xpows[tc.xpow] * ypows[tc.ypow-2] * zpows[tc.zpow];
	    if (tc.zpow>1)
		hessian[2][2] += c * zscale * zscale * tc.zpow * (tc.zpow-1) * xpows[tc.xpow] * ypows[tc.ypow] * zpows[tc.zpow-2];

	    if (tc.xpow>0 && tc.ypow>0) {
		hessian[1][0] += c * xscale * yscale * tc.xpow * tc.ypow * xpows[tc.xpow-1] * ypows[tc.ypow-1] * zpows[tc.zpow];
		hessian[0][1] += c * xscale * yscale * tc.xpow * tc.ypow * xpows[tc.xpow-1] * ypows[tc.ypow-1] * zpows[tc.zpow];
	    }
	    if (tc.xpow>0 && tc.zpow>0) {
		hessian[2][0] += c * xscale * zscale * tc.xpow * tc.zpow * xpows[tc.xpow-1] * ypows[tc.ypow] * zpows[tc.zpow-1];
		hessian[0][2] += c * xscale * zscale * tc.xpow * tc.zpow * xpows[tc.xpow-1] * ypows[tc.ypow] * zpows[tc.zpow-1];
	    }
	    if (tc.ypow>0 && tc.zpow>0) {
		hessian[2][1] += c * yscale * zscale * tc.ypow * tc.zpow * xpows[tc.xpow] * ypows[tc.ypow-1] * zpows[tc.zpow-1];
		hessian[1][2] += c * yscale * zscale * tc.ypow * tc.zpow * xpows[tc.xpow] * ypows[tc.ypow-1] * zpows[tc.zpow-1];
	    }
	}
    }


    void CoefsFromMatrix(const gtb::tMatrix4<T> &m) {

	int indexing[4][4][4];	// for computing the index of a sample

	vector<coefterm> afterZremoval[4][4];

	for (int x=0; x<4; x++) {
	    for (int y=0; y<4; y++) {

		for (int mr=0; mr<4; mr++) {
		    for (int mc=0; mc<4; mc++) {
			afterZremoval[x][y].push_back(coefterm(m[mr][mc],
							       &indexing[x][y][mc]-&indexing[0][0][0],
							       0, 0, mr));
		    }
		}
	    }
	}

	vector<coefterm> afterYremoval[4];
	for (int x=0; x<4; x++) {
	    for (int mr=0; mr<4; mr++) {
		for (int mc=0; mc<4; mc++) {
		    Append(afterYremoval[x],  MultByScalarPower(afterZremoval[x][mc], m[mr][mc], 0, mr, 0));
		}
	    }
	}


	vector<coefterm> afterAll;
	for (int mr=0; mr<4; mr++) {
	    for (int mc=0; mc<4; mc++) {
		Append(afterAll,  MultByScalarPower(afterYremoval[mc], m[mr][mc], mr, 0, 0));
	    }
	}



	coefs.clear();
	for (int xpow=0; xpow<4; xpow++) {
	    for (int ypow=0; ypow<4; ypow++) {
		for (int zpow=0; zpow<4; zpow++) {

		    //					if (xpow+ypow+zpow > 3)
		    //						continue;


		    coefs.push_back(TrivariateCoefficients());
		    coefs.back().xpow = xpow;
		    coefs.back().ypow = ypow;
		    coefs.back().zpow = zpow;

		    for (unsigned i=0; i<afterAll.size(); i++) {
			if (afterAll[i].xpow==xpow &&
			    afterAll[i].ypow==ypow &&
			    afterAll[i].zpow==zpow &&
			    afterAll[i].coef!=0) {
			    coefs.back().coefs.push_back(std::pair<int,T>(afterAll[i].sample, afterAll[i].coef));
			}
		    }
		}
	    }
	}
    }
    private:

    class coefterm {
	public:
	coefterm(T c, int s, int _xpow, int _ypow, int _zpow) {
	    coef=c;
	    sample=s;
	    xpow = _xpow;
	    ypow = _ypow;
	    zpow = _zpow;
	}
	T coef;
	int sample;
	int xpow;
	int ypow;
	int zpow;
    };

    class TrivariateCoefficients {
	public:
	int xpow;
	int ypow;
	int zpow;
	vector< std::pair<int, T> > coefs;
    };



    vector<coefterm> MultByScalarPower(const vector<coefterm> &in, T s, int xpow, int ypow, int zpow) {
	vector<coefterm> out;
	for (unsigned i=0; i<in.size(); i++) {
	    out.push_back(in[i]);
	    out.back().coef *= s;
	    out.back().xpow += xpow;
	    out.back().ypow += ypow;
	    out.back().zpow += zpow;
	}
	return out;
    }


    void Append(vector<coefterm> &v, const vector<coefterm> &a) {
	for (unsigned i=0; i<a.size(); i++) {
	    v.push_back(a[i]);
	}
    }


    vector<TrivariateCoefficients> coefs;
};





// a function sampled on a regular grid
class RegularVolume : public gtb::tModel<real_type> {

    public:
    RegularVolume();
    ~RegularVolume();

    bool Read(const char *fname);
    bool Write(const char *fname) const;

    bool ReadNRRD(const char *fname);

    template <typename SOURCETYPE>
	bool ReadSource(const char *fname, bool bigendian=false);

    template <typename SOURCETYPE>
	bool WriteSource(const char *fname) const;

    real_type& GetValue(int x, int y, int z) const { return data[xyz2index(x,y,z)]; }
    int GetDim(int i) const { return dim[i]; }
    real_type GetAspect(int i) const { return aspect[i]; }
    int GetBoundaryCells() const { return boundary_cells; }
    bool Empty() const { return (data==NULL); }

    void Smooth(int width);

    // Model stuff
    void compute_bounding_box() const;
    void compute_centroid() const;

    int xyz2index(int x, int y, int z) const;

    void Gather(const int cell[3], double nbrs[4][4][4]) const;

    bool LocalInfo(const Point3 &p, int cell[3], double xl[3]) const;

    real_type* GetBSplineValues() const;
    void SetValues(real_type *v) { memcpy(data, v, sizeof(real_type)*dim[0]*dim[1]*dim[2]); }
    real_type* CopyValues() const {
	real_type *ret = new real_type[dim[0]*dim[1]*dim[2]];
	memcpy(ret, data, sizeof(real_type)*dim[0]*dim[1]*dim[2]);	  
	return ret;
    }


    private:

    void GetBSplineValuesParallel(int nt, int id, const TrivariateSpline<double> &spline, real_type *ret) const;

    void GenSphere();

    int dim[3];
    real_type aspect[3];
    real_type *data;
    int boundary_cells;
};





class IsoSurfaceProjector : public SurfaceProjector {
    public:
    IsoSurfaceProjector(const RegularVolume &vol, real_type iso, bool bspline);
    virtual ~IsoSurfaceProjector();
    int ProjectPoint(const FrontElement &base1, const FrontElement &base2, const Point3 &fp, const Vector3 &fn, Point3 &tp, Vector3 &tn) const;
    int ProjectPoint(const Point3 &fp, Point3 &tp, Vector3 &tn) const;
    int ProjectPointInCell(const Point3 &fp, Point3 &tp, Vector3 &tn) const;

    real_type GetIsoValue() const { return isovalue; }

    private:

    real_type isovalue;
    TrivariateSpline<double> spline;
    const RegularVolume &volume;



    class RingEval {

	public:
	RingEval(const IsoSurfaceProjector &t, const Point3 &c, const Vector3 &u, const Vector3 &v) :
	hit_boundary(false), isp(t), center(c), udir(u), vdir(v) { last_cell[0] = last_cell[1] = last_cell[2] = -1; }


	real_type operator()(real_type theta) {
	    Point3 x = center + (real_type)cos(theta)*udir + (real_type)sin(theta)*vdir;
	    
	    int this_cell[3];
	    double xl[3];	// local coordinate
	    if (!isp.volume.LocalInfo(x, this_cell, xl)) {
		hit_boundary = true;
		return 1e34;
	    }
	    
	    if (this_cell[0]!=last_cell[0] || this_cell[1]!=last_cell[1] || this_cell[2]!=last_cell[2]) {
	      
		isp.volume.Gather(this_cell, nbrs);
	      
		last_cell[0]=this_cell[0];
		last_cell[1]=this_cell[1];
		last_cell[2]=this_cell[2];
	    }


	    double e = isp.spline.Eval(isp.volume.GetAspect(0),
				       isp.volume.GetAspect(1),
				       isp.volume.GetAspect(2),
				       nbrs, xl);

	    return (real_type)(e - isp.isovalue);
	}

        
	const IsoSurfaceProjector &isp;
	int last_cell[3]; 
	double nbrs[4][4][4];

	bool hit_boundary;
	Point3 center;
	Vector3 udir, vdir;
    };
};






class IsoSurfaceGuidanceField : public GuidanceField {
    public:

    IsoSurfaceGuidanceField(IsoSurfaceProjector &projector, const RegularVolume &vol, bool bspline, real_type rho, real_type min_step, real_type max_step, real_type reduction);
    ~IsoSurfaceGuidanceField();

    int ClosestPoint(const Point3 &p);
    void OrderedPointTraverseStart(const Point3 &p);
    int OrderedPointTraverseNext(real_type &squared_dist);
    void OrderedPointTraverseEnd();
    const Point3& PointLocation(int i) const;
    int NumPoints() const;

    void Extract(vector<Point3> &pts, vector<Vector3> &norms, vector<real_type> &rad);

    real_type EvalAtPoint(const Point3 &p) const;
    bool NormalAtPoint(const Point3 &p, Vector3 &n) const;
    bool CurvatureAtPoint(const Point3 &p, real_type &k1, real_type &k2) const;

    private:

    void BuildGuidanceParallel(int nt, int id, real_type isovalue, IsoSurfaceProjector &projector, const real_type *values);

    TrivariateSpline<double> spline;
    const RegularVolume &volume;

    class GetPoint {
	public:
	vector<Point3> allpoints;
	GetPoint() {}
	const Point3& operator()(unsigned idx) const {
	    return allpoints[idx];
	}
    };

    typedef gtb::KDTree<int, real_type, GetPoint> kdtree_type;
    kdtree_type *kdtree;
    GetPoint kdGetPoint;
    kdtree_type::OrderedIncrementalTraverse *kdOrderedTraverse;

    vector<Vector3> *guidanceNormals;
};




void MarchingCubes(const RegularVolume &v, TriangulatorController &tc, real_type isovalue);



#endif
