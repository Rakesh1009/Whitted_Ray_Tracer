#include <light.hpp>

using namespace rt;

light_t::light_t() { }
light_t::~light_t() { }


point_light_t::point_light_t(const Vector3d& _pos, const Vector3d& _col, const double _ka): pos(_pos), col(_col), ka(_ka) 
{ }

point_light_t::~point_light_t()
{ }

color_t point_light_t::direct(const ray_t& ray, const Vector3d& hitpt, const Vector3d& normal, const object_t* obj, const scene_t* scn) const
{	
	const material_t* mat = obj->get_material();
	Vector3d lightdir = (pos - hitpt).normalized();
	double lightdist = (pos - hitpt).norm();
	ray_t ray_to_light(hitpt, lightdir, EPSILON, lightdist);
	whitted_integrator_t::intersection_t checkintersection;
	bool isintersect = whitted_integrator_t::intersect(checkintersection, ray_to_light, scn);

	Vector3d diff_col = mat->get_diffuse(obj->get_uv_coords(hitpt));
	Vector3d ambient_comp = ka*col.cwiseProduct(diff_col);


	if(isintersect)
	{
		return color_t(ambient_comp);
	}

//diffuse component	
	Vector3d diff_comp = (std::max(0.0, lightdir.dot(normal)))*(col.cwiseProduct(diff_col));
	//attenuation
	diff_comp/=std::max(2.0,lightdist*lightdist/100);
	//specular component
	Vector3d ViewDir = -ray.direction.normalized();
	Vector3d Hvector = (ViewDir+lightdir).normalized();
	double NdotH = normal.dot(Hvector);
	double spec_intensity = std::pow(clamp(NdotH), std::pow(2, 4.0+mat->get_shininess()));
	Vector3d spec_comp = spec_intensity*col.cwiseProduct((Vector3d)mat->get_specular());
	//attenuation
	spec_comp/=std::max(2.0,2*lightdist*lightdist/100);

// col.cwiseProduct()

	return color_t(ambient_comp+diff_comp+spec_comp);
}
		

void point_light_t::print(std::ostream &stream) const
{
	Eigen::IOFormat CommaInitFmt(Eigen::StreamPrecision, Eigen::DontAlignCols, ", ", ", ", "", "", "[ ", " ]");
	
	stream<<"Light Properties: -------------------------------"<<std::endl;
	stream<<"Type: Point Light"<<std::endl;
	stream<<"Position: "<<pos.format(CommaInitFmt)<<std::endl;
	stream<<"Color: "<<col.format(CommaInitFmt)<<std::endl;
	stream<<"Ambient Coefficient: "<<ka<<std::endl<<std::endl;
}