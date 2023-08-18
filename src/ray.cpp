#include <ray.hpp>

using namespace rt;

ray_t::ray_t():origin(Vector3d(0.0,0.0,0.0)), direction(Vector3d(0.0,0.0,1.0)), mint(EPSILON), maxt(std::numeric_limits<double>::infinity())
{ }

ray_t::ray_t(const Vector3d& _o, const Vector3d& _d):origin(_o), direction(_d), mint(EPSILON), maxt(std::numeric_limits<double>::infinity())
{ }

ray_t::ray_t(const Vector3d& _o, const Vector3d& _d, const double _mint, const double _maxt):origin(_o), direction(_d), mint(_mint), maxt(_maxt)
{ }

ray_t::ray_t(const ray_t &_ray): origin(_ray.origin), direction(_ray.direction), 
       mint(_ray.mint), maxt(_ray.maxt) 
{ }

ray_t::ray_t(const ray_t &_ray, double _mint, double _maxt): origin(_ray.origin), direction(_ray.direction), 
     	mint(_mint), maxt(_maxt) 
{ }

ray_t::~ray_t()
{ }

const Vector3d ray_t::operator()(const double _t) {return (origin + _t*direction);}

ray_t ray_t::reflected(const Vector3d& hitpt, const Vector3d& normal) const
{
 double mag_along_normal = normal.dot(direction);
 Vector3d res_dir;
 res_dir = direction-2*mag_along_normal*normal;
 res_dir.normalize();
 return ray_t(hitpt, res_dir);
}

ray_t ray_t::transmitted(const Vector3d& hitpt, const Vector3d& normal, double ref_idx) const
{
 Vector3d I = direction.normalized(); 
 Vector3d N = normal.normalized(); 
 double N_dot_I = N.dot(I); 
 Vector3d transmit_dir;
 //ray travelling into object
 if(N_dot_I<0)
 {
  double eta_r = 1/ref_idx;
  double coeff = 1-pow(eta_r,2)*(1-pow(N_dot_I,2));
  if(coeff<0)
   return reflected(hitpt, normal);
  transmit_dir = eta_r*I + (-eta_r*N_dot_I - sqrt(coeff))*N;
 }
 //ray travelling out of object
 else
 {
  double eta_r = ref_idx;
  double coeff = 1-pow(eta_r,2)*(1-pow(N_dot_I,2));
  if(coeff<0)
   return reflected(hitpt, normal);
  transmit_dir = eta_r*I - (eta_r*N_dot_I - sqrt(coeff))*N;
 }
 transmit_dir.normalize();
 return ray_t(hitpt, transmit_dir);
}
