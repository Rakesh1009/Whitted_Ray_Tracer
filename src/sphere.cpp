#include <sphere.hpp>

using namespace rt;

sphere_t::sphere_t(material_t* _mat):center(0.0,0.0,0.0),radius(1.0),mat(_mat) { }
sphere_t::sphere_t(material_t* _mat, Eigen::Vector3d _c, double _r): center(_c), radius(_r), mat(_mat) { }

sphere_t::~sphere_t() { }

bool sphere_t::intersect(hit_t& result, const ray_t& _ray) const
{
	Vector3d r2c = center - _ray.origin;
	const double b = r2c.dot(_ray.direction);
	double d = b*b - r2c.dot(r2c) + radius*radius;

	if (d < 0)
		return false;
	
	d = sqrt(d);
	double t1 = b - d;
	double t2 = b + d;
	if (t1 <= EPSILON && t2 <= EPSILON) {
					return false;
	}
	double t = t1 <= EPSILON ? t2 : t1;
	result = hit_t(this, t);

	return t >= _ray.mint && t <= _ray.maxt;
}

Eigen::Vector3d sphere_t::get_normal(Eigen::Vector3d& _p) const
{
	Eigen::Vector3d normal = _p - center;
	normal.normalize();

	return normal;
}

material_t* sphere_t::get_material(void) const
{
	return mat;
}

Vector2d sphere_t::get_uv_coords(const Vector3d& hitpt) const
{
  double x = hitpt.x()-center.x();
  double y = hitpt.y()-center.y();
  double z = hitpt.z()-center.z();
  double absX = fabs(x);
  double absY = fabs(y);
  double absZ = fabs(z);
  
  int isXPositive = x > 0 ? 1 : 0;
  int isYPositive = y > 0 ? 1 : 0;
  int isZPositive = z > 0 ? 1 : 0;
  
  double maxAxis=1.0, uc=0.0, vc=0.0;
  // POSITIVE X
  if (isXPositive && absX >= absY && absX >= absZ) {
    maxAxis = absX;
    uc = -z;
    vc = y;
  }
  // NEGATIVE X
  if (!isXPositive && absX >= absY && absX >= absZ) {
    maxAxis = absX;
    uc = z;
    vc = y;
  }
  // POSITIVE Y
  if (isYPositive && absY >= absX && absY >= absZ) {
    maxAxis = absY;
    uc = x;
    vc = -z;
  }
  // NEGATIVE Y
  if (!isYPositive && absY >= absX && absY >= absZ) {
    maxAxis = absY;
    uc = x;
    vc = z;
  }
  // POSITIVE Z
  if (isZPositive && absZ >= absX && absZ >= absY) {
    maxAxis = absZ;
    uc = x;
    vc = y;
  }
  // NEGATIVE Z
  if (!isZPositive && absZ >= absX && absZ >= absY) {
    maxAxis = absZ;
    uc = -x;
    vc = y;
  }
  // Convert range from -1 to 1 to 0 to 1
		double u = 0.5f*(uc/maxAxis+1.0f);
		double v = 0.5f*(vc/maxAxis+1.0f);
  return Vector2d(std::fmod(radius*u/10,1), std::fmod(radius*v/10,1));
}


void sphere_t::print(std::ostream &stream) const
{
	Eigen::IOFormat CommaInitFmt(Eigen::StreamPrecision, Eigen::DontAlignCols, ", ", ", ", "", "", "[ ", " ]");
	
	stream<<"Object Properties: -------------------------------"<<std::endl;
	stream<<"Type: Sphere"<<std::endl;
	stream<<"Center: "<<center.format(CommaInitFmt)<<std::endl;
	stream<<"Radius: "<<radius<<std::endl<<std::endl;
}