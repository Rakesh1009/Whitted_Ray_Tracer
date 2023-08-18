#include <mesh.hpp>
#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
// Optional. define TINYOBJLOADER_USE_MAPBOX_EARCUT gives robust trinagulation. Requires C++11
//#define TINYOBJLOADER_USE_MAPBOX_EARCUT
#include "tiny_obj_loader.h"


using namespace rt;

mesh_t::mesh_t(material_t* _mat):mat(_mat) { }
mesh_t::mesh_t(material_t* _mat, Vector3d pos, std::string obj_file_path):mat(_mat) 
{
 tinyobj::ObjReader reader;
 if (!reader.ParseFromFile(obj_file_path))
 {
  if (!reader.Error().empty())
  {
   std::cerr << "TinyObjReader: " << reader.Error();
  }
  exit(1);
 }

 if (!reader.Warning().empty())
 {
  std::cout << "TinyObjReader: " << reader.Warning();
 }

 auto& attrib = reader.GetAttrib();
 auto& shapes = reader.GetShapes();
 // Loop over shapes
 for (size_t s = 0; s < shapes.size(); s++)
 {
  // Loop over faces(polygon)
  size_t index_offset = 0;
  for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
  {
   Face face;
   size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
   if(fv!=3)
   {
    std::cerr<<"nontriangulated face"<<std::endl;
    exit(3);
   }
   // Loop over vertices in the face.
   for (size_t v = 0; v < 3; v++)
   {
    Vertex vert;
    tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
    
    vert.pos.x() = pos.x() + attrib.vertices[3*size_t(idx.vertex_index)+0];
    vert.pos.y() = pos.y() + attrib.vertices[3*size_t(idx.vertex_index)+1];
    vert.pos.z() = pos.z() + attrib.vertices[3*size_t(idx.vertex_index)+2];
    verts.push_back(vert);
   }
   Vector3d& v1 = verts[index_offset].pos;
   Vector3d& v2 = verts[index_offset+1].pos;
   Vector3d& v3 = verts[index_offset+2].pos;
   face.normal = ((v2-v1).cross(v3-v2)).normalized();
   index_offset += 3;
   faces.push_back(face);
   // per-face material
   shapes[s].mesh.material_ids[f];
  }
 }
}

mesh_t::~mesh_t() { }

bool mesh_t::intersectTriangle(const ray_t &_ray, int face_idx, double &closest_hit) const
{

  Vector3d normal = faces[face_idx].normal;
  const Vector3d& v1 = verts[face_idx*3].pos;
  const Vector3d& v2 = verts[face_idx*3+1].pos;
  const Vector3d& v3 = verts[face_idx*3+2].pos;
  // Compute the denominator of the dot product between the ray direction and the triangle normal
  double denominator = normal.dot(_ray.direction);

  // Check if the ray is parallel to the triangle
  if (denominator == 0) {
    return false;
  }

  // Compute the distance from the ray origin to the triangle plane
  double t = normal.dot(v1 - _ray.origin) / denominator;

  // Check if the triangle is behind the ray
  if (t < 0) {
    return false;
  }
  if(t>closest_hit)
   return false;
  


  // Compute the intersection point
  Vector3d intersection = _ray.origin + t * _ray.direction;

  //check if intersection lies in triangle
  Vector3d bary0 = v3-v1;
  Vector3d bary1 = v2-v1;
  Vector3d bary2 = intersection-v1;
  double dot00 = bary0.dot(bary0);
  double dot01 = bary0.dot(bary1);
  double dot02 = bary0.dot(bary2);
  double dot11 = bary1.dot(bary1);
  double dot12 = bary1.dot(bary2);

  double Deno = dot00*dot11-dot01*dot01;

  double u = (dot11*dot02-dot01*dot12)/Deno;
  if(u<0||u>1)
   return false;
  float v = (dot00 * dot12 - dot01 * dot02)/Deno;
  if (v < 0 || v > 1)
   return false;


  if(u + v <= 1)
  {
   closest_hit = t;
   return true;
  }
  return false;
}

bool mesh_t::intersect(hit_t& result, const ray_t& _ray) const
{
 double closest_distance = std::numeric_limits<double>::infinity();

 for (size_t f_idx=0; f_idx<faces.size();f_idx++)
 {
  if (intersectTriangle(_ray, f_idx, closest_distance))
  {
   face_idx_of_last_hit_ray = f_idx;
  }
 }
 if(closest_distance==std::numeric_limits<double>::infinity())
  return false;

 if(closest_distance<=_ray.maxt && closest_distance>=_ray.mint)
 {
  result = hit_t(this, closest_distance);
  return true;
 }
 return false;
}

Eigen::Vector3d mesh_t::get_normal(Eigen::Vector3d& _p) const
{

	return faces[face_idx_of_last_hit_ray].normal;
}

material_t* mesh_t::get_material(void) const
{
	return mat;
}

Vector2d mesh_t::get_uv_coords(const Vector3d& hitpt) const
{
 return Vector2d(0,0);
}


void mesh_t::print(std::ostream &stream) const
{
	Eigen::IOFormat CommaInitFmt(Eigen::StreamPrecision, Eigen::DontAlignCols, ", ", ", ", "", "", "[ ", " ]");
	
	stream<<"Object Properties: -------------------------------"<<std::endl;
	stream<<"Type: Mesh"<<std::endl;
}


