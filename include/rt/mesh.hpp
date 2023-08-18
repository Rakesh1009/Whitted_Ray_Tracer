/*
    This file is part of rt.

    rt is a simple ray tracer meant to be used for teaching ray tracing.

    Copyright (c) 2018 by Parag Chaudhuri

    Some parts of rt are derived from Nori by Wenzel Jacob.
    https://github.com/wjakob/nori/

    rt is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License Version 3
    as published by the Free Software Foundation.

    rt is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <material.hpp>
#include <object.hpp>
#include <ray.hpp>
#include <utils.hpp>
#include <vector>


namespace rt
{
 struct Vertex
 {
  Vector3d pos;
 };
 struct Face
 {
   // int idx1, idx2, idx3;
			Vector3d normal;
 };
 	/**
	 * \brief The sphere object class.
	 **/
	class mesh_t : public object_t
	{
	private:

  std::vector<Face> faces;
  std::vector<Vertex> verts;
		/// Mesh material
		material_t* mat;

		mutable int face_idx_of_last_hit_ray = 0;

		bool intersectTriangle(const ray_t &ray, int face_idx,  double &closest_hit) const;
	public:
		/// Constructor
		mesh_t(material_t* _mat);
		/// Constructor
		mesh_t(material_t* _mat, Vector3d pos, std::string obj_file_path);
		/// Destuctor
		virtual ~mesh_t();

		/// Returns the mandatory object name
		std::string get_name(void) const { return std::string("mesh"); }
		/**
		* Returns true if the _ray hits this object. The hit information is returned in result. 
		* This is not valid if there is no intersection and the function returns false.
		**/
		bool intersect(hit_t& result, const ray_t& _ray) const;

		/// Returns the normal to the surface at point _p.
		Eigen::Vector3d get_normal(Eigen::Vector3d& _p) const;

		virtual Vector2d get_uv_coords(const Vector3d& hitpt) const;

		/// Returns the material for the mesh.
		material_t* get_material(void) const;

		/// Prints information about the mesh. to stream.
		virtual void print(std::ostream &stream) const;
	};
}
