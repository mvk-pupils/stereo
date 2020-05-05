
// This is an program used for embedding multiple shader
// files into string literals in a c++ header file.

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
using namespace std;

// add_shader
// Copies the given text file in a readable line by line
// format into the output stream.
void add_shader(ofstream& output, string shader_path, string name){
  cout << "Reading file " << shader_path << "\n";
  ifstream shader(shader_path);
  if (shader.is_open()){
    cout << "\tCreating " << name << " literal...\n";
    output << "const char* shader_" << name << " = R\"(\n";
    string line;
    while (getline(shader, line)) output << line << '\n';
    output << ")\";\n";
    shader.close();
    cout << "\tDone\n";
  }
  else cout << name << "could not open\n";
}

// main
// first argument - output file
// further arguments - file location followed by literal name
//
// The output file will be located in the output directory.
// Input files must have be given a literal name in the argument
// directly after.
int main(int argc, char** argv) {
  if (argc < 2) return 1;
  string output_dir = (string)argv[1];
  cout << "Creating file " << output_dir << "\n";
  ofstream shader_header (output_dir);
  if (shader_header.is_open()){
    cout << "Header opened...\n";
    for (int i = 2; argv[i] != nullptr; i += 2){
      if (argc < i+2 ) return 1;
      string input_source = argv[i], input_name = argv[i+1];
      add_shader(shader_header, input_source, input_name);
    }
    shader_header.close();
    cout << "Header done\n";
  }
  else cout << "Could not create header\n";
  return 0;
}