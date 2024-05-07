#ifndef _CKMEANSPROVGRAPH_H
#define _CKMEANSPROVGRAPH_H

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/graphviz.hpp>
#include <string>
#include <queue>
#include <unordered_set>
#include <unordered_map>


namespace ckpg {

enum VertexType {
                Input, Derived, Parameter,  // variables
                Dist, NearestCentroid, VectorMean
                };

inline std::ostream& operator<<(std::ostream& out, const VertexType value){
  const char* s = 0;
  #define PROCESS_VAL(p) case(p): s = #p; break;
    switch(value) {
      PROCESS_VAL(Input)
      PROCESS_VAL(Derived)     
      PROCESS_VAL(Parameter)
      PROCESS_VAL(Dist)
      PROCESS_VAL(NearestCentroid)
      PROCESS_VAL(VectorMean)
    }
  #undef PROCESS_VAL
    return out << s;
}

inline std::string vertexTypeToString(VertexType vt) {
  std::string s;
  switch(vt) {
    case(Input): s = "input"; break;
    case(Derived): s = "derived"; break;
    case(Parameter): s = "param"; break;
    case(Dist): s = "dist"; break;
    case(NearestCentroid): s = "nearestcentroid"; break;
    case(VectorMean): s = "vectormean"; break;
  }
  return s;
}

struct ProvVertex {
  VertexType vt;
  std::string name;
  void* value;
  int centroid;
};

struct ProvEdge{
};

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, ProvVertex, ProvEdge> Graph;
typedef boost::graph_traits<Graph>::vertex_descriptor vertex_t;
typedef boost::graph_traits<Graph>::edge_descriptor edge_t;
typedef boost::graph_traits<Graph>::vertex_iterator vertex_iter;
typedef boost::graph_traits<Graph>::edge_iterator edge_iter;
typedef boost::graph_traits<Graph>::in_edge_iterator in_edge_iter;
typedef boost::graph_traits<Graph>::out_edge_iterator out_edge_iter;
typedef boost::graph_traits<Graph>::adjacency_iterator adjacency_iter;

struct my_node_writer {
  my_node_writer(Graph& g_) : g (g_) {};
  template <class Vertex>
  void operator()(std::ostream& out, Vertex v) {
    if (g[v].vt==Input) {
      // out << " [label=\"" << g[v].name << ": " << g[v].value << ", d: " << g[v].derivative << "\", shape=oval, color=dodgerblue]";
      out << " [label=\"" << g[v].name << ": " << g[v].value << "\", shape=oval, color=dodgerblue]";
    }
    else if (g[v].vt==Derived) 
      // out << " [label=\"" << g[v].name << ": " << g[v].value << ", d: " << g[v].derivative << "\", shape=oval, color=limegreen]";
      out << " [label=\"" << g[v].name << ": " << g[v].value << "\", shape=oval, color=limegreen]";
    else if (g[v].vt==Parameter) 
      // out << " [label=\"" << g[v].name << ": " << g[v].value << ", d: " << g[v].derivative << "\", shape=oval, color=darkviolet]";
      out << " [label=\"" << g[v].name << ": " << g[v].value << "\", shape=oval, color=darkviolet]";
    else 
      out << " [label=\"" << g[v].name << "\", shape=box, color=tomato]";
  };
  Graph g;
};

struct my_edge_writer {
  my_edge_writer(Graph& g_) : g (g_) {};
  template <class Edge>
  void operator()(std::ostream& out, Edge e) {
    out << " [color=purple]" << std::endl;
    out << " [label=\"";
    vertex_t v_target = boost::target(e, g);
    out << "\"]";
  };
  Graph g;
};

struct my_graph_writer {
  void operator()(std::ostream& out) const {
    out << "graph [bgcolor=lightgrey]";
    out << "node [shape=circle color=blue]";
    out << "edge [color=red]";
  }
};



class CKMeansProvGraph {
public:
  /* initializer*/
  CKMeansProvGraph() {
  }

  CKMeansProvGraph(const std::string& sp): save_path(sp) {}

  /* read functions */
  inline vertex_t getVertexByName(const std::string& name) {
    return vertex_set[name];
  }

  inline std::string edgeToString(const edge_t e) {
    return std::to_string(e.m_source)+"_"+std::to_string(e.m_target);
  }

  inline vertex_t checkVertexExistByName(const std::string& name) {
    return vertex_set.find(name)!=vertex_set.end();
  }

  // inline float getVertexDerivativeByName(const std::string& name) {
  //   return g[getVertexByName(name)].derivative;
  // }

  inline void* getVertexValueByName(const std::string& name) {
    return g[getVertexByName(name)].value;
  }

  int getNumberOfEdgesOfInterest();


  /* write functions */
  inline vertex_t addVariableVertex(const VertexType vt, const std::string& name, void* value);

  inline vertex_t addVariableVertex(const VertexType vt, const std::string& name, void* value, int centroid);

  inline vertex_t addOperatorVertex(const VertexType vt, const std::string& name);

  void addComputingSubgraph(const std::string& output_name, void* value, VertexType vt, const std::vector<std::string>& input_names);

  void addComputingSubgraph(const std::string& output_name, void* value, VertexType vt, const std::vector<std::string>& input_names, int centroid);

  inline void addProvEdge(vertex_t v1, vertex_t v2, bool reverse) {
    if (!reverse) boost::add_edge(v1, v2, g);
    else boost::add_edge(v2, v1, g);
  }

  inline void addProvEdge(vertex_t v1, vertex_t v2) {
    boost::add_edge(v1, v2, g);
  }

  // inline void addProvEdge(vertex_t v1, vertex_t v2, float contribution) {
  //   edge_t e;
  //   bool b;
  //   boost::tie(e, b) = boost::add_edge(v1, v2, g);
  //   g[e].contribution = contribution;
  // }

  // inline void addProvEdge(vertex_t v1, vertex_t v2, float contribution, float derivative) {
  //   edge_t e;
  //   bool b;
  //   boost::tie(e, b) = boost::add_edge(v1, v2, g);
  //   g[e].contribution = contribution;
  //   g[e].derivative = derivative;
  // }

  inline void setVertexValue(vertex_t v, void* value) {
    g[v].value = value;
  }


  /* provenance query functions */
public:
  CKMeansProvGraph ProvenanceQuery(const std::string& name);

  void DFSProvQuery(vertex_t s, CKMeansProvGraph& subProvG);


 /* print and save functions */
public:
  inline void printVertex(vertex_t v) {
    std::cout << g[v].vt << " vertex ";
    std::cout << g[v].name << ", value=" << g[v].value << std::endl;
    std::cout << std::endl;
  }

  inline void printGraph() {
    write_graphviz (std::cout, g, my_node_writer(g));
  }

  inline void setSavePath(const std::string& sp) {
    save_path = sp;
  }

  inline void saveGraph() {
    std::ofstream fout(save_path);
    write_graphviz (fout, g, my_node_writer(g), my_edge_writer(g));
    fout.close();
  }

  ~CKMeansProvGraph() {};

  size_t getProvSize() {
    size_t vertex_count = num_vertices(g);
    size_t edge_count = num_edges(g);
    
    size_t base_size = sizeof(g);
    size_t vertex_size = sizeof(vertex_t) * vertex_count;
    size_t edge_size = sizeof(edge_t) * edge_count;

    return base_size+vertex_size+edge_size;
  }

 /* class members */
public:
  Graph g;
  
private:
  
  std::string save_path;
  std::unordered_map<std::string, vertex_t> vertex_set;
};



};

#endif