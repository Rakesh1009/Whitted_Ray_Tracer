#include <iostream>

#include <integrator.hpp>

using namespace rt;

color_t whitted_integrator_t::radiance(const scene_t* _scn, ray_t& _ray, int d) const
{
	intersection_t intrsct;

	color_t d_col(0.0);
	if(d<0)
	 return d_col;
	if(intersect(intrsct, _ray, _scn))
	{
		std::list<light_t*>::const_iterator lit;
		const material_t *mat = intrsct.hit.first->get_material();
		if(!mat->get_is_reflect() && !mat->get_is_transmit())
		{
			for(lit=_scn->lits.begin(); lit!=_scn->lits.end(); lit++)
			{
				d_col += (*lit)->direct(_ray, intrsct.hitpt, intrsct.normal, intrsct.hit.first, _scn);
			}
		}
		double attenuation = std::max(1.0,(10e-6*_ray.maxt*_ray.maxt));
		if(mat->get_is_reflect())
		{
			ray_t reflected_ray = _ray.reflected(intrsct.hitpt, intrsct.normal);
			color_t reflcol = radiance(_scn, reflected_ray, d-1);

			d_col += mat->get_reflect().cwiseProduct(reflcol)/attenuation;
		}
		if(mat->get_is_transmit())
		{
			ray_t transmitted_ray = _ray.transmitted(intrsct.hitpt, intrsct.normal, mat->get_eta());
			color_t transmitcol = radiance(_scn, transmitted_ray, d-1);
			d_col += mat->get_transmit().cwiseProduct(transmitcol)/attenuation;
		}
	}
	else d_col = _scn->img->get_bgcolor();

	return d_col;
}


bool whitted_integrator_t::intersect(intersection_t& intrsctn, ray_t& _ray, const scene_t* _scn)
{
	bool found_intersection=false;
	std::vector<object_t*>::const_iterator oit;
	hit_t hit;

	for (oit=_scn->objs.begin(); oit!=_scn->objs.end(); oit++)
	{
		if ((*oit)->intersect(hit, _ray))
		{
		  _ray.maxt = hit.second;
		  intrsctn.hit = hit;
		  
		  intrsctn.hitpt = _ray.origin+_ray.maxt*_ray.direction;
		  intrsctn.normal = (*oit)->get_normal(intrsctn.hitpt);

		  found_intersection=true;
		}
	}
	return found_intersection;
}
