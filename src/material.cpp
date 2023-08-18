#include <material.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


using namespace rt;

material_t::material_t(std::string _name)
{ name = _name; }

material_t::~material_t()
{ }

std::string material_t::get_name(void) const
{ return name; }

simplemat_t::simplemat_t(std::string _name, color_t _kd, color_t _ks, color_t _kr, color_t _kt, double _eta, double _n, bool _is_r, bool _is_t):
	material_t(_name),kd(_kd),ks(_ks),kr(_kr),kt(_kt),eta(_eta),n(_n),is_reflect(_is_r),is_transmit(_is_t)
	{ }

simplemat_t::~simplemat_t()
{ }

color_t simplemat_t::get_diffuse(const Vector2d coord) const { return kd; }
color_t simplemat_t::get_specular(void) const { return ks; }
color_t simplemat_t::get_reflect(void) const { return kr; }
color_t simplemat_t::get_transmit(void) const { return kt; }

double simplemat_t::get_eta(void) const { return eta; }
double simplemat_t::get_shininess(void) const {return n; }

bool simplemat_t::get_is_reflect(void) const {return is_reflect;}
bool simplemat_t::get_is_transmit(void) const { return is_transmit;}

void simplemat_t::print(std::ostream &stream) const
{
	Eigen::IOFormat CommaInitFmt(Eigen::StreamPrecision, Eigen::DontAlignCols, ", ", ", ", "", "", "[ ", " ]");
	
	stream<<"Material Properties: -------------------------------"<<std::endl;
	stream<<"Name: "<<this->get_name()<<std::endl;
	stream<<"kd: "<<kd.format(CommaInitFmt)<<std::endl;
	stream<<"ks: "<<ks.format(CommaInitFmt)<<std::endl;
	stream<<"kr: "<<kr.format(CommaInitFmt)<<std::endl;
	stream<<"kt: "<<kt.format(CommaInitFmt)<<std::endl;
	stream<<"eta: "<<eta<<std::endl;
	stream<<"Shininess: "<<n<<std::endl;
	stream<<"Is Reflecting: "<<is_reflect<<std::endl;
	stream<<"Is Transmiting: "<<is_transmit<<std::endl<<std::endl;
}

texturemat_t::texturemat_t(std::string _name, std::string _img_path, color_t _ks, color_t _kr, color_t _kt, double _eta, double _n, bool _is_r, bool _is_t):
	simplemat_t(_name, color_t(1,0,1), _ks, _kr, _kt, _eta, _n, _is_r, _is_t)
{
	img_data = stbi_load(_img_path.c_str(), &width, &height, &nrChannels, 0);
	// std::cout<<"image loaded!!!!!!!"<<img_data<<std::endl;
}

texturemat_t::~texturemat_t()
{
	stbi_image_free(img_data);
}

color_t texturemat_t::get_diffuse(const Vector2d coord) const
{
	if(img_data==NULL)
		return kd;
	int u = std::floor(clamp(coord.x())*width); 
	int v = std::floor(clamp(coord.y())*height);
	unsigned char *color = &img_data[(u*width+v)*nrChannels];
	return color_t(color[0]/255.0, color[1]/255.0, color[2]/255.0);
}