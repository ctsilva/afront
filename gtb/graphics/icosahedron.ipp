GTB_BEGIN_NAMESPACE


inline Box3 icosahedron::bounding_box()
{
	return Box3(-1.0, -1.0, -1.0, 1.0, 1.0, 1.0);
}


inline Point3 icosahedron::centroid()
{
	return Point3::ZERO;
}


GTB_END_NAMESPACE
