#include <fstream>
#include <vector>
#include <string>

#include "file_writer.h"

std::string scalar_to_string(const char* name,
			     std::vector<SPH_particle> *particle_list,
			     double (*func)(SPH_particle)) {

  /**
     Return scalar variable from function func as string of named 
     VTK DataArray for particle list

     @param[in] name Name of the array.
     @param[in] particle_list The list to output.
     @param[in] func Function to access variable
  */

  std::string s;

  s +="<DataArray type=\"Float64\" Name=\"";
  s += name;
  s += "\" format=\"ascii\">\n";

  for (auto p=particle_list->begin(); p!=particle_list->end(); ++p) {
    s += " ";
    s += std::to_string(func(*p));
  }
  s += "\n";
  s += "</DataArray>\n";
       
       return s;
}

std::string vector_to_string(const char* name,
			     std::vector<SPH_particle> *particle_list,
			     double (*func)(SPH_particle, int)) {

  /** 
     Return vector variable from function func as string of named 
     VTK DataArray for particle list. (Note, vector assumed 2D).

     @param[in] name Name of the array.
     @param[in] particle_list List to output.
     @param[in] func Function to access variable elements.
  */

  std::string s;

  s +="<DataArray type=\"Float64\" Name=\"";
  s += name;
  s += "\" NumberOfComponents=\"3\" format=\"ascii\">\n";

  for (auto p=particle_list->begin(); p!=particle_list->end(); ++p) {
    for (int i=0; i<2; ++i) {
      s += " ";
      s += std::to_string(func(*p, i));
    }
    s += " 0.000000";
  }
  s += "\n";
  s += "</DataArray>\n";
       
       return s;
}

std::string range_as_string(int n, int offset) {
  /*
    Return string containing range up to n+offset, starting
    from offset.

     @param[in] n Length to output
     @param[in] offset Offset to range
  */

  std::string s;
  
  for (int i=0; i<n; ++i) {
    s += " ";
    s += std::to_string(i+offset);
  }

  s += "\n";

  return  s;
}

double get_velocity(SPH_particle p, int i) {
  /* Return ith element of velocity for particle p */
  return p.v[i];
}

double get_position(SPH_particle p, int i) {
  /* Return ith element of position for particle p */
  return p.x[i];
}

double get_pressure(SPH_particle p) {
  /* Return pressure for particle p */
  return p.P;
}


int write_file(const char *filename,
	       std::vector<SPH_particle> *particle_list) {

  /*
    
    Write VTK XMLPolyData (.vtp) file containing data in particle_list.

    @param[in] filename Filename to write to
    @param[in] particle_list Particle list to output 

   */
  
  std::fstream fs(filename, std::fstream::out);
  
  fs << "<VTKFile type=\"PolyData\">\n";
  fs << "<PolyData>\n";
  fs << "<Piece NumberOfPoints=\""<< particle_list->size() << "\" NumberOfVerts=\"" << particle_list->size() <<"\" NumberOfLines=\"0\" NumberOfStrips=\"0\" NumberOfPolys=\"0\">\n";
  fs << "<PointData>\n";
  fs << scalar_to_string("Pressure", particle_list, get_pressure);
  fs << vector_to_string("Velocity", particle_list, get_velocity);
  fs << "</PointData>\n";
  fs << "<Points>\n";
  fs << vector_to_string("Points", particle_list, get_position);
  fs << "</Points>\n";
  fs << "<Verts>\n";
  fs << "<DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">\n";
  fs << range_as_string(particle_list->size(), 0);
  fs << "</DataArray>\n";
  fs << "<DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">\n";
  fs << range_as_string(particle_list->size(), 1);
  fs << "</DataArray>\n";
  fs << "</Verts>\n";
  fs << "</Piece>\n";
  fs << "</PolyData>\n";
  fs << "</VTKFile>\n";
  fs.flush();
  fs.close();

  return 0;
}
