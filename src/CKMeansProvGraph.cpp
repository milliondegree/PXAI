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


CKMeansProvGraph CKMeansProvGraph::ProvenanceQuery(const std::string& name) {
  // ASSERT_EX(checkVertexExistByName(name), std::cout << name+" does not exist" << std::endl);
  vertex_t v = getVertexByName(name);
  
  // initialize CProvGraph of the queried vertex
  std::string new_save_path = save_path.substr(0, save_path.find_last_of("."));
  // std::cout << save_path << ' ' << new_save_path << std::endl;
  new_save_path += "-"+name+".dot";
  CKMeansProvGraph subProvG(new_save_path);

  // insert the source vertex to subProvG
  // ASSERT_EX(g[v].vt==Derived, std::cout << g[v].name << " is a " << vertexTypeToString(g[v].vt) << " vertex" << std::endl);
  subProvG.addVariableVertex(g[v].vt, g[v].name, g[v].value);

  // std::cout << "start provenance query" << std::endl;
  DFSProvQuery(v, subProvG);
  return subProvG;
}

void CKMeansProvGraph::DFSProvQuery(vertex_t s, CKMeansProvGraph& subProvG) {
  vertex_t parent = subProvG.getVertexByName(g[s].name);
  // start iterating adjacent vertices
  boost::graph_traits<Graph>::adjacency_iterator ai_v, ai_v_end;
  boost::tie(ai_v, ai_v_end) = boost::adjacent_vertices(s, g);
  for (; ai_v!=ai_v_end; ai_v++) {
    vertex_t v_operator = *ai_v;
    vertex_t a_operator = subProvG.addOperatorVertex(g[v_operator].vt, g[v_operator].name);
    subProvG.addProvEdge(parent, a_operator);
    boost::graph_traits<Graph>::adjacency_iterator ai, ai_end;
    for (boost::tie(ai, ai_end)=boost::adjacent_vertices(v_operator, g); ai!=ai_end; ai++) {
      vertex_t v = *ai;
      vertex_t child;
      if (subProvG.checkVertexExistByName(g[v].name)) {
        child = subProvG.getVertexByName(g[v].name);
        subProvG.addProvEdge(a_operator, child);
      }
      else {
        child = subProvG.addVariableVertex(g[v].vt, g[v].name, g[v].value);
        subProvG.addProvEdge(a_operator, child);
        DFSProvQuery(v, subProvG);
      }
    }
  }
  return;
}

