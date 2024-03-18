#include "CKMeansProvGraph.h"

using namespace ckpg;

inline vertex_t CKMeansProvGraph::addVariableVertex(const VertexType vt, const std::string& name, void* value) {
  // ASSERT_EX(!checkVertexExistByName(name), std::cout << name+" already exists" << std::endl);
  vertex_t v = boost::add_vertex(g);
  vertex_set.insert(std::make_pair(name, v));
  g[v].vt = vt;
  g[v].name = name;
  g[v].value = value;
  return v;
}

inline vertex_t CKMeansProvGraph::addVariableVertex(const VertexType vt, const std::string& name, void* value, int centroid) {
  // ASSERT_EX(!checkVertexExistByName(name), std::cout << name+" already exists" << std::endl);
  vertex_t v = boost::add_vertex(g);
  vertex_set.insert(std::make_pair(name, v));
  g[v].vt = vt;
  g[v].name = name;
  g[v].value = value;
  g[v].centroid = centroid;
  return v;
}

inline vertex_t CKMeansProvGraph::addOperatorVertex(const VertexType vt, const std::string& name) {
  // ASSERT_EX(!checkVertexExistByName(name), std::cout << name+" already exists" << std::endl);
  vertex_t v = boost::add_vertex(g);
  vertex_set.insert(std::make_pair(name, v));
  g[v].vt = vt;
  g[v].name = name;
  return v;
}

void CKMeansProvGraph::addComputingSubgraph(const std::string& output_name, void* value, VertexType vt, const std::vector<std::string>& input_names) {
  /* add subgraph of computing provenance, it is based on an asumption that every output only relies on one operator 
    output_name: name of the output variable
    value: value of output variable
    vt: vertex type of operator
    input_names: names of the input variables that are connected to the operator vertex
  */
  vertex_t v_output, v_operator;
  if (checkVertexExistByName(output_name)) {
    v_output = getVertexByName(output_name);
    v_operator = getVertexByName(vertexTypeToString(vt)+"_"+output_name);
    g[v_output].value = value;
  }
  else {
    v_output = addVariableVertex(Derived, output_name, value);
    v_operator = addOperatorVertex(vt, vertexTypeToString(vt)+"_"+output_name);
    addProvEdge(v_output, v_operator, true);
  }
  for (auto s : input_names) {
    addProvEdge(v_operator, getVertexByName(s), true);
  }
}

void CKMeansProvGraph::addComputingSubgraph(const std::string& output_name, void* value, VertexType vt, const std::vector<std::string>& input_names, int centroid) {
  /* add subgraph of computing provenance, it is based on an asumption that every output only relies on one operator 
    output_name: name of the output variable
    value: value of output variable
    vt: vertex type of operator
    input_names: names of the input variables that are connected to the operator vertex
  */
  vertex_t v_output, v_operator;
  if (checkVertexExistByName(output_name)) {
    v_output = getVertexByName(output_name);
    v_operator = getVertexByName(vertexTypeToString(vt)+"_"+output_name);
    g[v_output].value = value;
  }
  else {
    v_output = addVariableVertex(Derived, output_name, value, centroid);
    v_operator = addOperatorVertex(vt, vertexTypeToString(vt)+"_"+output_name);
    addProvEdge(v_output, v_operator, true);
  }
  for (auto s : input_names) {
    addProvEdge(v_operator, getVertexByName(s), true);
  }
}

