#include "CProvGraph.h"
#include "Utils.h"

using namespace cpg;

inline vertex_t CProvGraph::addVariableVertex(const VertexType vt, const std::string& name, float value) {
  ASSERT_EX(!checkVertexExistByName(name), std::cout << name+" already exists" << std::endl);
  vertex_t v = boost::add_vertex(g);
  vertex_set.insert(std::make_pair(name, v));
  g[v].vt = vt;
  g[v].name = name;
  g[v].value = value;
  return v;
}

inline vertex_t CProvGraph::addOperatorVertex(const VertexType vt, const std::string& name) {
  ASSERT_EX(!checkVertexExistByName(name), std::cout << name+" already exists" << std::endl);
  vertex_t v = boost::add_vertex(g);
  vertex_set.insert(std::make_pair(name, v));
  g[v].vt = vt;
  g[v].name = name;
  return v;
}

inline vertex_t CProvGraph::addOperatorVertex(const VertexType vt, const std::string& name, const std::unordered_map<std::string, std::string>& params) {
  ASSERT_EX(!checkVertexExistByName(name), std::cout << name+" already exists" << std::endl);
  vertex_t v = boost::add_vertex(g);
  vertex_set.insert(std::make_pair(name, v));
  g[v].vt = vt;
  g[v].name = name;
  g[v].params.insert(params.begin(), params.end());
  return v;
}

inline vertex_t CProvGraph::addOperatorVertex(const VertexType vt, const std::string& name, std::vector<double>* weights) {
  ASSERT_EX(!checkVertexExistByName(name), std::cout << name+" already exists" << std::endl);
  vertex_t v = boost::add_vertex(g);
  vertex_set.insert(std::make_pair(name, v));
  g[v].vt = vt;
  g[v].name = name;
  // for (int i=0; i<weights.size(); i++) g[v].weights.push_back(weights[i]);
  g[v].weights = weights;
  return v;
}

inline vertex_t CProvGraph::addOperatorVertex(const VertexType vt, const std::string& name, const std::unordered_map<std::string, std::string>& params, std::vector<double>* weights) {
  ASSERT_EX(!checkVertexExistByName(name), std::cout << name+" already exists" << std::endl);
  vertex_t v = boost::add_vertex(g);
  vertex_set.insert(std::make_pair(name, v));
  g[v].vt = vt;
  g[v].name = name;
  g[v].params.insert(params.begin(), params.end());
  // for (int i=0; i<weights.size(); i++) g[v].weights.push_back(weights[i]);
  g[v].weights = weights;
  return v;
}

void CProvGraph::addComputingSubgraph(const std::string& output_name, const float value, VertexType vt, const std::vector<std::string>& input_names) {
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
    addProvEdge(v_output, v_operator);
  }
  for (auto s : input_names) {
    addProvEdge(v_operator, getVertexByName(s));
  }
}

void CProvGraph::addComputingSubgraph(const std::string& output_name, const float value, VertexType vt, const std::vector<std::string>& input_names, const std::unordered_map<std::string, std::string>& params) {
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
    v_operator = addOperatorVertex(vt, vertexTypeToString(vt)+"_"+output_name, params);
    addProvEdge(v_output, v_operator);
  }
  for (auto s : input_names) {
    addProvEdge(v_operator, getVertexByName(s));
  }
}

void CProvGraph::addComputingSubgraph(const std::string& output_name, const float value, VertexType vt, const std::vector<std::string>& input_names, std::vector<double>* weights) {
  /* add subgraph of computing provenance, it is based on an asumption that every output only relies on one operator 
    output_name: name of the output variable
    value: value of output variable
    vt: vertex type of operator
    input_names: names of the input variables that are connected to the operator vertex
  */
  vertex_t v_output, v_operator;
  if (checkVertexExistByName(output_name)) {
    v_output = getVertexByName(output_name);
    v_operator = addOperatorVertex(vt, vertexTypeToString(vt)+"_"+output_name, weights);
    addProvEdge(v_output, v_operator);
  }
  else {
    v_output = addVariableVertex(Derived, output_name, value);
    v_operator = addOperatorVertex(vt, vertexTypeToString(vt)+"_"+output_name, weights);
    addProvEdge(v_output, v_operator);
  }
  for (auto s : input_names) {
    addProvEdge(v_operator, getVertexByName(s));
  }
}

void CProvGraph::addComputingSubgraph(const std::string& output_name, const float value, VertexType vt, const std::vector<std::string>& input_names, const std::unordered_map<std::string, std::string>& params, std::vector<double>* weights, int number_of_op) {
  /* add subgraph of computing provenance, it is based on an asumption that every output only relies on one operator 
    output_name: name of the output variable
    value: value of output variable
    vt: vertex type of operator
    input_names: names of the input variables that are connected to the operator vertex
  */
  vertex_t v_output, v_operator;
  if (checkVertexExistByName(output_name)) {
    v_output = getVertexByName(output_name);
    v_operator = addOperatorVertex(vt, vertexTypeToString(vt)+"_"+output_name+"_"+std::to_string(number_of_op), params, weights);
    addProvEdge(v_output, v_operator);
  }
  else {
    v_output = addVariableVertex(Derived, output_name, value);
    v_operator = addOperatorVertex(vt, vertexTypeToString(vt)+"_"+output_name, params, weights);
    addProvEdge(v_output, v_operator);
  }
  for (auto s : input_names) {
    addProvEdge(v_operator, getVertexByName(s));
  }
}

CProvGraph CProvGraph::ProvenanceQuery(const std::string& name) {
  ASSERT_EX(checkVertexExistByName(name), std::cout << name+" does not exist" << std::endl);
  vertex_t v = getVertexByName(name);
  
  // initialize CProvGraph of the queried vertex
  std::string new_save_path = save_path.substr(0, save_path.find_last_of("."));
  // std::cout << save_path << ' ' << new_save_path << std::endl;
  new_save_path += "-"+name+".dot";
  CProvGraph subProvG(new_save_path);
  subProvG.auxilary_data = this->auxilary_data;

  // insert the source vertex to subProvG
  ASSERT_EX(g[v].vt==Derived, std::cout << g[v].name << " is a " << vertexTypeToString(g[v].vt) << " vertex" << std::endl);
  subProvG.addVariableVertex(g[v].vt, g[v].name, g[v].value);

  // std::cout << "start provenance query" << std::endl;
  DFSProvQuery(v, subProvG);
  return subProvG;
}

void CProvGraph::DFSProvQuery(vertex_t s, CProvGraph& subProvG) {
  vertex_t parent = subProvG.getVertexByName(g[s].name);
  if (g[s].vt==Input || g[s].vt==Parameter) {
    subProvG.updateVertexEDBs(parent);
    return;
  }
  // start iterating adjacent vertices
  adjacency_tier ai_v, ai_v_end;
  boost::tie(ai_v, ai_v_end) = boost::adjacent_vertices(s, g);
  for (; ai_v!=ai_v_end; ai_v++) {
    vertex_t v_operator = *ai_v;
    vertex_t a_operator = subProvG.addOperatorVertex(g[v_operator].vt, g[v_operator].name, g[v_operator].params, g[v_operator].weights);
    subProvG.addProvEdge(parent, a_operator);
    adjacency_tier ai, ai_end;
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
    subProvG.updateVertexEDBs(parent);
  }
  return;
}

void CProvGraph::computeContribution(const std::string& name) {
  ASSERT_EX(checkVertexExistByName(name), std::cout << name+" does not exist" << std::endl);
  vertex_t v = getVertexByName(name);
  float previous_value = g[v].value;
  edge_iter ei_begin, ei_end, ei;
  boost::tie(ei_begin, ei_end) = boost::edges(g);
  std::vector<edge_t> edge_list;
  for (edge_iter ei=ei_begin; ei!=ei_end; ei++) {
    vertex_t v_source = boost::source(*ei, g);
    if (g[v_source].vt!=Derived && g[v_source].vt!=Softmax) edge_list.push_back(*ei);
  }
  int count = 1;
  for (edge_t e : edge_list) {
    vertex_t v_source, v_target;
    v_source = boost::source(e, g);
    v_target = boost::target(e, g);
    boost::remove_edge(v_source, v_target, g);
    float new_value = computeVariable(name);
    addProvEdge(v_source, v_target, previous_value-new_value, g[e].derivative);
    std::cout << "\r"+std::to_string(count)+"/"+std::to_string(edge_list.size()) << "     ";
    count += 1;
  }
  computeVariable(name);
  std::cout << "\n";
}

void CProvGraph::computeContribution_v2(const std::string& name) {
  ASSERT_EX(checkVertexExistByName(name), std::cout << name+" does not exist" << std::endl);
  vertex_t v = getVertexByName(name);
  
  g[v].contribution = 1;
  
  std::unordered_set<std::string> visited;
  DFSComputeContribution(v, visited);
}

void CProvGraph::DFSComputeContribution(vertex_t s, std::unordered_set<std::string>& visited) {
  if (g[s].vt==Input || g[s].vt==Parameter) return;
  adjacency_tier ai_v, ai_v_end;
  boost::tie(ai_v, ai_v_end) = boost::adjacent_vertices(s, g);
  int cnt = 0;
  for (; ai_v!=ai_v_end; ai_v++) {
    vertex_t v_operator = *ai_v;
    switch (g[v_operator].vt) {
      case Softmax: {
        adjacency_tier ai, ai_end;
        boost::tie(ai, ai_end)=boost::adjacent_vertices(v_operator, g);
        vertex_t v = *ai;
        edge_t e = boost::edge(v_operator, v, g).first;
        g[e].contribution = g[s].contribution * g[s].value;
        visited.insert(edgeToString(e));

        float denominator = 0;
        float numerator = 0; 
        for (int i=0; i<auxilary_data[g[v].value].size(); i++) {
          float value = std::exp(auxilary_data[g[v].value][i]);
          denominator += value;
          if (i==std::stoi(g[v_operator].params["numerator_pos"]))
            numerator = value;
        }

        auxilary_data.push_back(std::vector<double> (auxilary_data[int(g[v].value)].size(), 1.0));
        int pos = auxilary_data.size()-1;
        for (int i=0; i<auxilary_data[pos].size(); i++) {
          if (i==std::stoi(g[v_operator].params["numerator_pos"]))
            auxilary_data[pos][i] = g[s].value - 0/(denominator-numerator);
          else
            auxilary_data[pos][i] = g[s].value - numerator/(denominator-std::exp(auxilary_data[g[v].value][i]));
        }
        g[v].contribution = pos;

        DFSComputeContribution(v, visited);
        break;
      }
      case InnerProductAct: {
        adjacency_tier ai, ai_end;
        boost::tie(ai, ai_end)=boost::adjacent_vertices(v_operator, g);
        vertex_t v = *ai;
        edge_t e = boost::edge(v_operator, v, g).first;

        float source_con = auxilary_data[g[s].contribution][std::stoi(g[v_operator].params["node_num"])];
        g[e].contribution = source_con;
        visited.insert(edgeToString(e));

        bool all_visited = true;
        in_edge_iter ei, ei_end;
        for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) {
          if (visited.find(edgeToString(*ei))==visited.end()) {
            all_visited = false;
            break;
          }
        }
        if (all_visited & g[v].name!="input_0") {
          auxilary_data.push_back(std::vector<double> (auxilary_data[int(g[v].value)].size(), 0.0));
          int pos = auxilary_data.size()-1;
          float sum = 0;
          for (int i=0; i<auxilary_data[pos].size(); i++) {
            float previous_input = auxilary_data[g[v].value][i];
            adjacency_tier ai_vv, ai_vv_end;
            boost::tie(ai_vv, ai_vv_end) = boost::adjacent_vertices(s, g);
            auxilary_data[g[v].value][i] = 0;
            int cnt = 0;
            for (; ai_vv!=ai_vv_end; ai_vv++) {
              // float new_output = std::inner_product(begin(auxilary_data[g[v].value]), end(auxilary_data[g[v].value]), begin(*g[* ai_vv].weights), 0.0);
              // if (g[* ai_vv].params["act"]=="sigmoid") 
              //   new_output = utils::sigmoid(new_output);
              // float previous_output = auxilary_data[g[s].value][cnt];
              // auxilary_data[pos][i] += (previous_output - new_output) / previous_output * auxilary_data[g[s].contribution][cnt];

              float previous_output = auxilary_data[g[s].value][cnt];
              float new_output;
              if (g[* ai_vv].params["act"]=="sigmoid") {
                float inverse_previous_output = utils::inverse_sigmoid(previous_output);
                new_output = utils::sigmoid(inverse_previous_output-previous_input*(*g[* ai_vv].weights)[i]);
              }
              else{
                new_output = previous_output-previous_input*(*g[* ai_vv].weights)[i];
              }
              auxilary_data[pos][i] += (previous_output - new_output) / previous_output * auxilary_data[g[s].contribution][cnt];
              cnt += 1;
            }
            sum += auxilary_data[pos][i];
            // std::cout << auxilary_data[pos][i] << " ";
            auxilary_data[g[v].value][i] = previous_input;
          }

          for (int i=0; i<auxilary_data[pos].size(); i++) {
            auxilary_data[pos][i] /= sum;
          }

          g[v].contribution = pos;
          DFSComputeContribution(v, visited);
        }

        break;
      }
      default: std::cout << "this operator is not yet implemented\n"; exit(1);
      cnt += 1;
    }
  }
  return;
}

void CProvGraph::DFSComputeSoftmaxContribution(vertex_t s, float s_value, float c, std::unordered_set<std::string>& visited) {
  float denominator = 0;
  float numerator = 0; 
  adjacency_tier ai, ai_end;
  for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
    vertex_t v = *ai;
    if (!std::isnan(g[v].value)) {
      denominator += std::exp(g[v].value);
      if (g[v].name==g[s].params["numerator_name"])
        numerator = std::exp(g[v].value);
    }
  }
  for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
    vertex_t v = *ai;
    edge_t e = boost::edge(s, v, g).first;
    if (g[v].name==g[s].params["numerator_name"]) {
      g[e].contribution = c * (s_value - 1/(denominator-numerator));
    }
    else {
      g[e].contribution = c * (s_value - numerator/(denominator-g[v].value));
    }
    visited.insert(edgeToString(e));
    bool all_visited = true;
    in_edge_iter ei, ei_end;
    for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) {
      if (visited.find(edgeToString(*ei))==visited.end()) {
        all_visited = false;
        break;
      }
    }
    if (all_visited) {
      g[v].contribution = 0.0;
      for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) 
        g[v].contribution += g[*ei].contribution;
      DFSComputeContribution(v, visited);
    }
  }
}

// void CProvGraph::DFSComputeInnerProductContribution(vertex_t s, float s_value, float c, std::unordered_set<std::string>& visited) {
//   adjacency_tier ai, ai_end;
//   for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
//     vertex_t v = *ai;
//     edge_t e = boost::edge(s, v, g).first;
//     g[e].contribution = c;
//     visited.insert(edgeToString(e));
//     bool all_visited = true;
//     in_edge_iter ei, ei_end;
//     for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) {
//       if (visited.find(edgeToString(*ei))==visited.end()) {
//         all_visited = false;
//         break;
//       }
//     }
//     if (all_visited) {
//       g[v].contribution = 0.0;
//       for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) 
//         g[v].contribution += g[*ei].contribution;
//       DFSComputeContribution(v, visited);
//     }
//   }
// }

void CProvGraph::DFSComputeSigmoidContribution(vertex_t s, float s_value, float c, std::unordered_set<std::string>& visited) {
  adjacency_tier ai, ai_end;
  for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
    vertex_t v = *ai;
    edge_t e = boost::edge(s, v, g).first;
    g[e].contribution = c;
    visited.insert(edgeToString(e));
    bool all_visited = true;
    in_edge_iter ei, ei_end;
    for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) {
      if (visited.find(edgeToString(*ei))==visited.end()) {
        all_visited = false;
        break;
      }
    }
    if (all_visited) {
      g[v].contribution = 0.0;
      for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) 
        g[v].contribution += g[*ei].contribution;
      DFSComputeContribution(v, visited);
    }
  }
}

void CProvGraph::DFSComputeInnerProductActContribution(vertex_t s, float s_value, float c, std::unordered_set<std::string>& visited) {
  adjacency_tier ai, ai_end;
  for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
    vertex_t v = *ai;
    edge_t e = boost::edge(s, v, g).first;
    g[e].contribution = c;
    visited.insert(edgeToString(e));
    bool all_visited = true;
    in_edge_iter ei, ei_end;
    for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) {
      if (visited.find(edgeToString(*ei))==visited.end()) {
        all_visited = false;
        break;
      }
    }
    if (all_visited) {
      g[v].contribution = 0.0;
      for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) 
        g[v].contribution += g[*ei].contribution;
      DFSComputeContribution(v, visited);
    }
  }
}

CProvGraph CProvGraph::ApproximateSubGraphQuery(std::string& name, float epsilon, float lambda) {
  ASSERT_EX(checkVertexExistByName(name), std::cout << name+" does not exist" << std::endl);
  vertex_t v = getVertexByName(name);
  ASSERT_EX(g[v].vt==Derived, std::cout << name << " is a " << vertexTypeToString(g[v].vt) << " vertex");
  float target = g[v].value;
  computeDerivative(name);
  std::unordered_map<std::string, float> target_derivatives;
  for (std::string edb : getVertexEBDsByName(name)) {
    target_derivatives[edb] = getVertexDerivativeByName(edb);
  }
  // std::cout << "target edb size: " << target_derivatives.size() << std::endl;
  
  // initialize CProvGraph of the the approximate query result
  std::string new_save_path = save_path.substr(0, save_path.find("."));
  new_save_path += "-approx.dot";

  std::priority_queue<ApproxSubGraphDiff, std::vector<ApproxSubGraphDiff>, ApproxSubGraphDiffComparison> pq;
  std::unordered_set<std::unordered_set<std::string>, EDBSetHash> visited;

  float min_diff = 1.0;
  float min_value_diff = 1.0;
  float min_derivative_diff = 1.0;
  std::unordered_set<std::string> min_set;
  std::unordered_set<std::string> includedEDBs;
  for (std::string EDB : g[v].EDBs) {
    std::unordered_set<std::string> includedEDBs;
    includedEDBs.insert(EDB);
    CProvGraph approxSubProvG(new_save_path);
    approxSubProvG.addVariableVertex(Derived, name, 0);
    DFSApproximateSubGraphQuery(v, approxSubProvG, includedEDBs);
    float value_diff = std::abs(target-approxSubProvG.getVertexValueByName(name));
    approxSubProvG.computeDerivative(name);
    // std::cout << "approx derivative finish" << std::endl;
    std::unordered_map<std::string, float> approx_derivatives;
    for (std::string edb : approxSubProvG.getVertexEBDsByName(name)) {
      approx_derivatives[edb] = approxSubProvG.getVertexDerivativeByName(edb);
    }
    // std::cout << "approx edb size: " << approx_derivatives.size() << std::endl;
    float derivative_diff = utils::computeDerivativeDiff(target_derivatives, approx_derivatives);
    ApproxSubGraphDiff asgd(includedEDBs, value_diff, derivative_diff);
    if (asgd.getDiff()<min_diff) {
      min_set = asgd.EDBs;
      min_diff = asgd.getDiff();
      min_value_diff = asgd.value_diff;
      min_derivative_diff = asgd.derivative_diff;
    }
    if (min_value_diff<epsilon && min_derivative_diff<lambda) {
      return approxSubProvG;
    }
    pq.push(asgd);
    visited.insert(includedEDBs);
  }

  int iteration = 1;
  while (!pq.empty() && visited.size()<MAX_NUM_OF_SEARCH) {
    ApproxSubGraphDiff asgd = pq.top();
    pq.pop();
    if (asgd.getDiff()<min_diff) {
      min_set = asgd.EDBs;
      min_diff = asgd.getDiff();
      min_value_diff = asgd.value_diff;
      min_derivative_diff = asgd.derivative_diff;
    }
    if (min_value_diff<epsilon && min_derivative_diff<lambda) {
      break;
    }
    // search neighbors of asgd.EDBs
    std::unordered_set<std::string> includedEDBs = asgd.EDBs;
    for (std::string EDB : g[v].EDBs) {
      if (includedEDBs.find(EDB)==includedEDBs.end()) {
        includedEDBs.insert(EDB);
        if (visited.find(includedEDBs)!=visited.end()) continue;
        CProvGraph approxSubProvG(new_save_path);
        approxSubProvG.addVariableVertex(Derived, name, 0);
        DFSApproximateSubGraphQuery(v, approxSubProvG, includedEDBs);
        float value_diff = std::abs(target-approxSubProvG.getVertexValueByName(name));
        approxSubProvG.computeDerivative(name);
        std::unordered_map<std::string, float> approx_derivatives;
        for (std::string edb : approxSubProvG.getVertexEBDsByName(name)) {
          approx_derivatives[edb] = approxSubProvG.getVertexDerivativeByName(edb);
        }
        float derivative_diff = utils::computeDerivativeDiff(target_derivatives, approx_derivatives);
        ApproxSubGraphDiff new_asgd(includedEDBs, value_diff, derivative_diff);
        pq.push(new_asgd);
        visited.insert(includedEDBs);
        includedEDBs.erase(EDB);
      }
    }
    iteration++;
  }

  // std::cout << "iteration: " << iteration << ", min value diff: " << min_value_diff << ", min derivative diff: " << min_derivative_diff << std::endl;
  CProvGraph approxSubProvG(new_save_path);
  approxSubProvG.addVariableVertex(Derived, name, 0);
  DFSApproximateSubGraphQuery(v, approxSubProvG, min_set);
  return approxSubProvG;
}

CProvGraph CProvGraph::ApproximateSubGraphQueryPrune(std::string& name, float epsilon, float lambda) {
  computeContribution(name);
  computeDerivative(name);

  vertex_t v = getVertexByName(name);
  float target = g[v].value;
  std::unordered_map<std::string, float> target_derivatives;
  for (std::string edb : getVertexEBDsByName(name)) {
    target_derivatives[edb] = getVertexDerivativeByName(edb);
  }

  edge_iter ei_begin, ei_end, ei;
  boost::tie(ei_begin, ei_end) = boost::edges(g);
  // std::vector<edge_t> edge_list;
  edge_comparer ec(&g, true);
  std::priority_queue<edge_t, std::vector<edge_t>, edge_comparer> edge_queue(ec);
  int e_count = 0;
  for (edge_iter ei=ei_begin; ei!=ei_end; ei++) {
    vertex_t v_source = boost::source(*ei, g);
    // vertex_t v_target = boost::target(*ei, g);
    if (g[v_source].vt!=Derived) {
      // std::cout << abs(g[*ei].contribution) << ' ' << std::abs(g[*ei].derivative) << std::endl;
      g[*ei].importance = ALPHA*std::abs(g[*ei].contribution)+BETA*std::abs(g[*ei].derivative);
      edge_queue.push(*ei);
    }
  }
  
  int count = 0;
  // int step = edge_list.size()/15;
  int step = std::max(1, int(edge_queue.size()/50));
  std::cout << "number of edges: " << edge_queue.size() << ", prune step: " << step << std::endl;
  CProvGraph ret = *this;
  // for (edge_t e : edge_list) {
  while (!edge_queue.empty()) {
    edge_t e = edge_queue.top();
    edge_queue.pop();
    vertex_t v_source = boost::source(e, g);
    vertex_t v_target = boost::target(e, g);
    boost::remove_edge(v_source, v_target, g);
    count += 1;
    if (count%step==0) {
      CProvGraph approxSubProvG = ProvenanceQuery(name);
      float approxiValue = approxSubProvG.computeVariable(name);
      float value_diff = std::abs(target-approxSubProvG.getVertexValueByName(name));
      approxSubProvG.computeDerivative(name);
      std::unordered_map<std::string, float> approx_derivatives;
      for (std::string edb : approxSubProvG.getVertexEBDsByName(name)) {
        approx_derivatives[edb] = approxSubProvG.getVertexDerivativeByName(edb);
      }
      float derivative_diff = utils::computeDerivativeDiff(target_derivatives, approx_derivatives);
      // std::cout << "value diff: " << value_diff << ", derivative diff: " << derivative_diff << std::endl;
      if (value_diff>epsilon||derivative_diff>lambda) {
        std::cout << "value diff: " << value_diff << ", derivative diff: " << derivative_diff << std::endl;
        break;
      }
      ret = approxSubProvG;
    }
  }
  std::string new_save_path = save_path.substr(0, save_path.find_last_of("."));
  new_save_path += "-approx.dot";
  ret.setSavePath(new_save_path);
  std::cout << "pruned edges: " << count << std::endl;
  return ret;
}

CProvGraph CProvGraph::ApproximateSubGraphQueryPruneMLP(std::string& name, float epsilon, float lambda) {
  clock_t t1 = clock();
  computeContribution_v2(name);
  clock_t t2 = clock();
  std::cout << "contribution time: " << (t2-t1)*1.0/CLOCKS_PER_SEC << "\n";
  t1 = clock();
  computeDerivative(name);
  t2 = clock();
  std::cout << "derivative time: " << (t2-t1)*1.0/CLOCKS_PER_SEC << "\n";

  vertex_t v = getVertexByName(name);
  float target = g[v].value;
  std::vector<double> target_derivatives = auxilary_data[int(g[getVertexByName("input_0")].derivative)];
  // std::cout << "target derivative: " << utils::vector_to_string(target_derivatives) << "\n";

  edge_iter ei_begin, ei_end, ei;
  boost::tie(ei_begin, ei_end) = boost::edges(g);
  // std::vector<edge_t> edge_list;
  edge_comparer ec(&g, true);
  std::priority_queue<edge_t, std::vector<edge_t>, edge_comparer> edge_queue(ec);
  int e_count = 0;
  t1 = clock();
  for (edge_iter ei=ei_begin; ei!=ei_end; ei++) {
    vertex_t v_source = boost::source(*ei, g);
    // vertex_t v_target = boost::target(*ei, g);
    if (g[v_source].vt!=Derived && g[v_source].vt!=Softmax) {
      // std::cout << abs(g[*ei].contribution) << ' ' << std::abs(g[*ei].derivative) << std::endl;
      g[*ei].importance = ALPHA*std::abs(g[*ei].contribution)+BETA*std::abs(utils::vector_size(auxilary_data[g[*ei].derivative]));
      edge_queue.push(*ei);
    }
  }
  t2 = clock();
  std::cout << "sort time: " << (t2-t1)*1.0/CLOCKS_PER_SEC << "\n";
  
  int count = 0;
  // int step = edge_list.size()/15;
  int step = std::max(1, int(edge_queue.size()/20));
  std::cout << "number of edges: " << edge_queue.size() << ", prune step: " << step << std::endl;
  CProvGraph ret = *this;
  // for (edge_t e : edge_list) {
  while (!edge_queue.empty()) {
    edge_t e = edge_queue.top();
    edge_queue.pop();
    vertex_t v_source = boost::source(e, g);
    vertex_t v_target = boost::target(e, g);
    boost::remove_edge(v_source, v_target, g);
    count += 1;
    if (count%step==0) {
      // 
      t1 = clock();
      CProvGraph approxSubProvG = ProvenanceQuery(name);
      // approxSubProvG.computeVariable(name);
      // float value_diff = std::abs(target-approxSubProvG.getVertexValueByName(name));
      this->computeVariableMLP(name);
      float value_diff = std::abs(target-this->getVertexValueByName(name));
      approxSubProvG.computeDerivative(name);
      // approxSubProvG.computeVariable(name);
      // float value_diff_2 = std::abs(approxSubProvG.getVertexValueByName(name)-this->getVertexValueByName(name));
      // std::cout << "recompute diff: " << value_diff_2 << "\n";
      std::vector<double> approx_derivatives = approxSubProvG.auxilary_data[approxSubProvG.g[approxSubProvG.getVertexByName("input_0")].derivative];
      // std::cout << "approx derivative: " << utils::vector_to_string(approx_derivatives) << "\n";
      float derivative_diff = utils::computeDerivativeDiff(target_derivatives, approx_derivatives);
      // std::cout << "value diff: " << value_diff << ", derivative diff: " << derivative_diff << std::endl;
      t2 = clock();
      std::cout << "search time: " << (t2-t1)*1.0/CLOCKS_PER_SEC << "\n";
      if (value_diff>epsilon||derivative_diff>lambda||edge_queue.size()<=step) {
        std::cout << "value diff: " << value_diff << ", derivative diff: " << derivative_diff << std::endl;
        break;
      }
      else ret = approxSubProvG;
    }
  }
  std::string new_save_path = save_path.substr(0, save_path.find_last_of("."));
  new_save_path += "-approx.dot";
  ret.setSavePath(new_save_path);
  std::cout << "pruned edges: " << count << std::endl;
  return ret;
}

float CProvGraph::computeVariableWithChangedEDBs(const std::string& name, const std::unordered_map<std::string, float>& EDBs) {
  ASSERT_EX(checkVertexExistByName(name), std::cout << name+" does not exist" << std::endl);
  vertex_t v = getVertexByName(name);
  changedEDBs = EDBs;
  std::unordered_set<vertex_t> visited;
  return DFSComputeVariable(v, visited);
}

float CProvGraph::DFSComputeVariable(vertex_t s, std::unordered_set<vertex_t>& visited) {
  // std::cout << "entering " << g[s].name << std::endl;
  if (visited.find(s)!=visited.end()) return g[s].value;
  if (g[s].vt==Input) {
    if (changedEDBs.find(g[s].name)!=changedEDBs.end()) {
      // std::cout << "find changed edb: " << g[s].name << ", previous value: " << g[s].value << ", changed value: " << changedEDBs[g[s].name] << std::endl;
      g[s].value = changedEDBs[g[s].name];
    }
    visited.insert(s);
    return g[s].value;
  }
  else if (g[s].vt==Parameter) {
    visited.insert(s);
    return g[s].value;
  }
  else if (!hasIntersection(changedEDBs, g[s].EDBs)) {
    visited.insert(s);
    return g[s].value;
  }
  // std::cout << "first time " << g[s].name << std::endl;
  // if the current vertex is a derived veriable that depends on changed inputs
  float ret;
  adjacency_tier ai, ai_end;
  boost::tie(ai, ai_end) = boost::adjacent_vertices(s, g);
  vertex_t v_operator = *ai;
  if (boost::out_degree(v_operator, g)==0) {
    g[s].value = 0;
    return 0;
  }
  switch (g[v_operator].vt) {
    case Sum: ret = DFSComputeSum(v_operator, visited); break;
    case Mul: ret = DFSComputeMul(v_operator, visited); break;
    case Div: ret = DFSComputeDiv(v_operator, visited); break;
    case Scale: ret = DFSComputeScale(v_operator, visited); break;
    case Softmax: ret = DFSComputeSoftmax(v_operator, visited); break;
    case Sigmoid: ret = DFSComputeSigmoid(v_operator, visited); break;
    // case InnerProduct: ret = DFSComputeInnerProduct(v_operator, visited); break;
    default: std::cout << "this is not an operator vertex\n"; exit(1);
  }
  // std::cout << "finish " << g[s].name << std::endl;
  g[s].value = ret;
  visited.insert(s);
  return ret;
}

inline bool CProvGraph::hasIntersection(const std::unordered_set<std::string>& l_set, const std::unordered_set<std::string>& r_set) {
  if (l_set.size()<r_set.size()) {
    for (std::string EDB : l_set) {
      if (r_set.find(EDB)!=r_set.end()) 
        return true;
    }
  }
  else {
    for (std::string EDB : r_set) {
      if (l_set.find(EDB)!=l_set.end()) 
        return true;
    }
  }
  return false;
}

inline bool CProvGraph::hasIntersection(const std::unordered_map<std::string, float>& l_map, const std::unordered_set<std::string>& r_set) {
  if (l_map.size()<r_set.size()) {
    for (auto it=l_map.begin(); it!=l_map.end(); it++) {
      if (r_set.find(it->first)!=r_set.end()) 
        return true;
    }
  }
  else {
    for (auto it : r_set) {
      if (l_map.find(it)!=l_map.end()) 
        return true;
    }
  }
  return false;
}

inline float CProvGraph::DFSComputeSum(vertex_t s, std::unordered_set<vertex_t>& visited) {
  float ret = 0;
  adjacency_tier ai, ai_end;
  boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g);
  for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
    vertex_t v = *ai;
    float value = DFSComputeVariable(v, visited);
    if (!std::isnan(value)) ret += value;
  }
  return ret;
}

inline float CProvGraph::DFSComputeMul(vertex_t s, std::unordered_set<vertex_t>& visited) {
  float ret = 1;
  adjacency_tier ai, ai_end;
  for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
    vertex_t v = *ai;
    float value = DFSComputeVariable(v, visited);
    if (!std::isnan(value)) ret *= value;
  }
  return ret;
}

inline float CProvGraph::DFSComputeDiv(vertex_t s, std::unordered_set<vertex_t>& visited) {
  float numerator=0, denominator=0;
  adjacency_tier ai, ai_end;
  for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
    vertex_t v = *ai;
    float value = DFSComputeVariable(v, visited);
    if (!std::isnan(value)) {
      if (g[v].name==g[s].params["numerator_name"]) numerator = value;
      else denominator = value;
    }
  }
  if (denominator==0) return 0;
  return numerator/denominator;
}

inline float CProvGraph::DFSComputeScale(vertex_t s, std::unordered_set<vertex_t>& visited) {
  float denominator = 0;
  float numerator = 0; 
  adjacency_tier ai, ai_end;
  for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
    vertex_t v = *ai;
    float value = DFSComputeVariable(v, visited);
    if (!std::isnan(value)) {
      if (g[v].name==g[s].params["numerator_name"]) {
        numerator = value;
      }
      denominator += value;
    }
  }
  if (denominator==0) return 0;
  return numerator/denominator;
}

inline float CProvGraph::DFSComputeSoftmax(vertex_t s, std::unordered_set<vertex_t>& visited) {
  float denominator = 0;
  float numerator = 0; 
  adjacency_tier ai, ai_end;
  for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
    vertex_t v = *ai;
    float value = DFSComputeVariable(v, visited);
    if (!std::isnan(value)) {
      if (g[v].name==g[s].params["numerator_name"]) {
        numerator = std::exp(value);
      }
      denominator += std::exp(value);
    }
  }
  if (denominator==0) return 0;
  return numerator/denominator;
}

inline float CProvGraph::DFSComputeSigmoid(vertex_t s, std::unordered_set<vertex_t>& visited) {
  float ret = 0;
  adjacency_tier ai, ai_end;
  for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
    vertex_t v = *ai;
    float value = DFSComputeVariable(v, visited);
    if (!std::isnan(value)) ret += 1/(1+std::exp(-value));
  }
  return ret;
} 

// inline float CProvGraph::DFSComputeInnerProduct(vertex_t s, std::unordered_set<vertex_t>& visited) {
//   float ret = 0;
//   adjacency_tier ai, ai_end;
//   for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
//     vertex_t v = *ai;
//     float value = DFSComputeVariable(v, visited);
//     if (!std::isnan(value)) ret += g[s].weights[g[v].name] * value;
//   }
//   return ret;
// } 

float CProvGraph::computeVariableMLP(const std::string& name) {
  vertex_t v = getVertexByName(name);
  return DFSComputeVariableNoEDBMLP(v);
}

float CProvGraph::DFSComputeVariableNoEDBMLP(vertex_t& s) {
  if (g[s].vt==Input || g[s].vt==Parameter) {
    return g[s].value;
  }
  // if the current vertex is a derived veriable that depends on changed inputs
  float ret;
  adjacency_tier ai_v, ai_v_end, ai, ai_end;
  int cnt = 0;
  boost::tie(ai_v, ai_v_end) = boost::adjacent_vertices(s, g);
  float value;
  for (; ai_v!=ai_v_end; ai_v++) {
    vertex_t v_operator = *ai_v;
    if (boost::out_degree(v_operator, g)==0) {
      if (g[v_operator].vt==InnerProductAct) {
        // auxilary_data[g[s].value][std::stoi(g[v_operator].params["node_num"])] = 0;
        auxilary_data[g[s].value][cnt] = 0;
        cnt += 0;
        ret = g[s].value;
        continue;
      }
      else if (g[v_operator].vt==Softmax) {
        g[s].value = 0;
        return g[s].value;
      }
    }
    switch (g[v_operator].vt) {
      case Softmax: {
        float denominator = 0, numerator = 0; 
        adjacency_tier ai, ai_end;
        boost::tie(ai, ai_end)=boost::adjacent_vertices(v_operator, g);
        vertex_t v = *ai;
        value = DFSComputeVariableNoEDBMLP(v);
        for (int i=0; i<auxilary_data[int(value)].size(); i++) {
          float tmp = auxilary_data[int(value)][i];
          if (!std::isnan(tmp)) {
            if (i==std::stoi(g[v_operator].params["numerator_pos"])) {
              numerator = std::exp(tmp);
            }
            denominator += std::exp(tmp);
          }
        }
        if (denominator==0) ret = 0;
        else ret = numerator/denominator;
        break;
      }
      case InnerProductAct: {
        if (cnt==0) {
          adjacency_tier ai, ai_end;
          boost::tie(ai, ai_end)=boost::adjacent_vertices(v_operator, g);
          vertex_t v = *ai;
          value = DFSComputeVariableNoEDBMLP(v);
        }
        // float tmp = 1;
        float tmp = std::inner_product(begin(auxilary_data[value]), end(auxilary_data[value]), begin(*g[v_operator].weights), 0.0);
        if (g[v_operator].params["act"]=="sigmoid") tmp = utils::sigmoid(tmp); 
        auxilary_data[g[s].value][cnt] = tmp;
        ret = g[s].value;
        break;
      }
      default: std::cout << "this is not an operator vertex\n"; exit(1);
    }
    cnt += 1;
  }
  g[s].value = ret;
  return ret;
}


float CProvGraph::computeVariable(const std::string& name) {
  ASSERT_EX(checkVertexExistByName(name), std::cout << name+" does not exist" << std::endl);
  vertex_t v = getVertexByName(name);
  std::unordered_set<vertex_t> visited;
  return DFSComputeVariableNoEDB(v, visited);
}

float CProvGraph::DFSComputeVariableNoEDB(vertex_t s, std::unordered_set<vertex_t>& visited) {
  if (visited.find(s)!=visited.end()) return g[s].value;
  if (g[s].vt==Input || g[s].vt==Parameter) {
    visited.insert(s);
    return g[s].value;
  }
  // if the current vertex is a derived veriable that depends on changed inputs
  float ret;
  adjacency_tier ai_v, ai_v_end;
  int cnt = 0;
  boost::tie(ai_v, ai_v_end) = boost::adjacent_vertices(s, g);
  for (; ai_v!=ai_v_end; ai_v++) {
    vertex_t v_operator = *ai_v;
    if (boost::out_degree(v_operator, g)==0) {
      if (g[v_operator].vt==InnerProductAct) {
        auxilary_data[g[s].value][std::stoi(g[v_operator].params["node_num"])] = 0;
        ret = g[s].value;
        continue;
      }
      else if (g[v_operator].vt==Softmax) {
        g[s].value = 0;
        return g[s].value;
      }
      else {
        g[s].value = 0.0/0.0;
        return g[s].value;
      }
    }
    int nan_cnt = 0;
    switch (g[v_operator].vt) {
      case Sum: {
        ret = 0;
        adjacency_tier ai, ai_end;
        for (boost::tie(ai, ai_end)=boost::adjacent_vertices(v_operator, g); ai!=ai_end; ai++) {
          vertex_t v = *ai;
          float value = DFSComputeVariableNoEDB(v, visited);
          if (!std::isnan(value)) ret += DFSComputeVariableNoEDB(v, visited);
          else nan_cnt++;
        }
        break;
      }
      case Mul: {
        ret = 1;
        adjacency_tier ai, ai_end;
        for (boost::tie(ai, ai_end)=boost::adjacent_vertices(v_operator, g); ai!=ai_end; ai++) {
          vertex_t v = *ai;
          float value = DFSComputeVariableNoEDB(v, visited);
          if (!std::isnan(value)) ret *= DFSComputeVariableNoEDB(v, visited);
          else nan_cnt++;
        }
        break;
      }
      case Div: {
        float numerator = 0, denominator = 1;
        adjacency_tier ai, ai_end;
        for (boost::tie(ai, ai_end)=boost::adjacent_vertices(v_operator, g); ai!=ai_end; ai++) {
          vertex_t v = *ai;
          if (g[v].name==g[v_operator].params["numerator_name"]) {
            numerator = DFSComputeVariableNoEDB(v, visited);
          }
          else {
            denominator = DFSComputeVariableNoEDB(v, visited);
          }
        }
        if (denominator==0) ret = 0;
        else ret = numerator/denominator;
        break;
      }
      case Scale: {
        float denominator = 0, numerator = 0; 
        adjacency_tier ai, ai_end;
        for (boost::tie(ai, ai_end)=boost::adjacent_vertices(v_operator, g); ai!=ai_end; ai++) {
          vertex_t v = *ai;
          float tmp = DFSComputeVariableNoEDB(v, visited);
          if (g[v].name==g[v_operator].params["numerator_name"]) {
            numerator = tmp;
          }
          denominator += tmp;
        }
        if (denominator==0) ret = 0;
        else ret = numerator/denominator;
        break;
      }
      case Softmax: {
        float denominator = 0, numerator = 0; 
        adjacency_tier ai, ai_end;
        for (boost::tie(ai, ai_end)=boost::adjacent_vertices(v_operator, g); ai!=ai_end; ai++) {
          vertex_t v = *ai;
          float value = DFSComputeVariableNoEDB(v, visited);
          for (int i=0; i<auxilary_data[int(value)].size(); i++) {
            float tmp = auxilary_data[int(value)][i];
            if (!std::isnan(tmp)) {
              if (i==std::stoi(g[v_operator].params["numerator_pos"])) {
                numerator = std::exp(tmp);
              }
              denominator += std::exp(tmp);
            }
            else nan_cnt++;
          }
          
        }
        if (denominator==0) ret = 0;
        else ret = numerator/denominator;
        break;
      }
      case Sigmoid: {
        ret = 0;
        adjacency_tier ai, ai_end;
        for (boost::tie(ai, ai_end)=boost::adjacent_vertices(v_operator, g); ai!=ai_end; ai++) {
          vertex_t v = *ai;
          float value = DFSComputeVariableNoEDB(v, visited);
          if (!std::isnan(value)) ret += 1.0/(1+std::exp(-value));
          else nan_cnt++;
        }
        break;
      }
      // case InnerProduct: {
      //   ret = 0;
      //   adjacency_tier ai, ai_end;
      //   for (boost::tie(ai, ai_end)=boost::adjacent_vertices(v_operator, g); ai!=ai_end; ai++) {
      //     vertex_t v = *ai;
      //     float value = DFSComputeVariableNoEDB(v, visited);
      //     if (!std::isnan(value)) ret += g[v_operator].weights[g[v].name] * value;
      //     else nan_cnt++;
      //   }
      //   break;
      // }
      case InnerProductAct: {
        adjacency_tier ai, ai_end;
        boost::tie(ai, ai_end)=boost::adjacent_vertices(v_operator, g);
        vertex_t v = *ai;
        float value;
        if (cnt==0) value = DFSComputeVariableNoEDB(v, visited);
        else value = g[v].value;
        // float tmp = 1;
        float tmp = std::inner_product(begin(auxilary_data[value]), end(auxilary_data[value]), begin(*g[v_operator].weights), 0.0);
        if (g[v_operator].params["act"]=="sigmoid") tmp = utils::sigmoid(tmp); 
        auxilary_data[g[s].value][std::stoi(g[v_operator].params["node_num"])] = tmp;
        ret = g[s].value;
        break;
      }
      default: std::cout << "this is not an operator vertex\n"; exit(1);
    }
    cnt += 1;
    if (boost::out_degree(v_operator, g)==nan_cnt) ret = 0.0/0.0;
  }
  g[s].value = ret;
  visited.insert(s);
  return ret;
}



void CProvGraph::computeDerivative(const std::string& name) {
  ASSERT_EX(checkVertexExistByName(name), std::cout << name+" does not exist" << std::endl);
  vertex_t v = getVertexByName(name);

  // insert derivative of the source vertex
  auxilary_data.push_back(std::vector<double> (auxilary_data[int(g[v].value)].size(), 1.0));
  // g[v].derivative = auxilary_data.size()-1;
  g[v].derivative = 1.0;
  
  // std::cout << "start to compute derivative" << std::endl;
  std::unordered_set<std::string> visited;
  DFSComputeDerivative(v, visited);
}

void CProvGraph::DFSComputeDerivative(vertex_t s, std::unordered_set<std::string>& visited) {
  if (g[s].vt==Input || g[s].vt==Parameter) return;
  adjacency_tier ai_v, ai_v_end;
  boost::tie(ai_v, ai_v_end) = boost::adjacent_vertices(s, g);
  for (; ai_v!=ai_v_end; ai_v++) {
    vertex_t v_operator = *ai_v;
    switch (g[v_operator].vt) {
      case Sum: DFSComputeSumDerivative(v_operator, g[s].derivative, visited); break;
      case Mul: DFSComputeMulDerivative(v_operator, g[s].derivative, visited); break;
      case Div: DFSComputeDivDerivative(v_operator, g[s].derivative, visited); break;
      case Scale: DFSComputeScaleDerivative(v_operator, g[s].derivative, visited); break;
      case Softmax: DFSComputeSoftmaxDerivative(v_operator, g[s].derivative, visited); break;
      case Sigmoid: DFSComputeSigmoidDerivative(v_operator, g[s].derivative, visited); break;
      // case InnerProduct: DFSComputeInnerProductDerivative(v_operator, g[s].derivative, visited); break;
      case InnerProductAct: DFSComputeInnerProductActDerivative(v_operator, g[s].value, g[s].derivative, visited); break;
      default: std::cout << "this is not an operator vertex\n"; exit(1);
    }
  }
}

void CProvGraph::DFSComputeSumDerivative(vertex_t s, float d, std::unordered_set<std::string>& visited) {
  adjacency_tier ai, ai_end;
  for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
    vertex_t v = *ai;
    edge_t e = boost::edge(s, v, g).first;
    g[e].derivative = 1.0*d;
    visited.insert(edgeToString(e));
    bool all_visited = true;
    in_edge_iter ei, ei_end;
    for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) {
      if (visited.find(edgeToString(*ei))==visited.end()) {
        all_visited = false;
        break;
      }
    }
    if (all_visited) {
      g[v].derivative = 0.0;
      for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) 
        g[v].derivative += g[*ei].derivative;
      DFSComputeDerivative(v, visited);
    }
  }
}

void CProvGraph::DFSComputeMulDerivative(vertex_t s, float d, std::unordered_set<std::string>& visited) {
  adjacency_tier ai, ai_end;
  float value = 1.0;
  int count = 0;
  for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
    vertex_t v = *ai;
    value *= g[v].value;
    count += 1;
  }
  for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
    vertex_t v = *ai;
    edge_t e = boost::edge(s, v, g).first;
    if (count==1) g[e].derivative = d;
    else g[e].derivative = d*value/g[v].value;
    visited.insert(edgeToString(e));
    bool all_visited = true;
    in_edge_iter ei, ei_end;
    for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) {
      if (visited.find(edgeToString(*ei))==visited.end()) {
        all_visited = false;
        break;
      }
    }
    if (all_visited) {
      g[v].derivative = 0.0;
      for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) 
        g[v].derivative += g[*ei].derivative;
      DFSComputeDerivative(v, visited);
    }
  }
}

void CProvGraph::DFSComputeDivDerivative(vertex_t s, float d, std::unordered_set<std::string>& visited) {
  adjacency_tier ai, ai_end;
  float numerator = 0;
  float denominator = 1;
  for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
    vertex_t v = *ai;
    if (!std::isnan(g[v].value)) {
      if (g[v].name==g[s].params["numerator_name"]) 
        numerator = g[v].value;
      else 
        denominator = g[v].value;
    }
  }
  for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
    vertex_t v = *ai;
    edge_t e = boost::edge(s, v, g).first;
    if (g[v].name==g[s].params["numerator_name"]) 
      g[e].derivative = d*(1.0/denominator);
    else 
      g[e].derivative = d*(-1.0*numerator/std::pow(denominator, 2));
    visited.insert(edgeToString(e));
    bool all_visited = true;
    in_edge_iter ei, ei_end;
    for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) {
      if (visited.find(edgeToString(*ei))==visited.end()) {
        all_visited = false;
        break;
      }
    }
    if (all_visited) {
      g[v].derivative = 0.0;
      for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) 
        g[v].derivative += g[*ei].derivative;
      DFSComputeDerivative(v, visited);
    }
  }
}

void CProvGraph::DFSComputeScaleDerivative(vertex_t s, float d, std::unordered_set<std::string>& visited) {
  float denominator = 0;
  float numerator = 0; 
  adjacency_tier ai, ai_end;
  for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
    vertex_t v = *ai;
    if (!std::isnan(g[v].value)) {
      denominator += g[v].value;
      if (g[v].name==g[s].params["numerator_name"])
        numerator = g[v].value;
    }
  }
  for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
    vertex_t v = *ai;
    edge_t e = boost::edge(s, v, g).first;
    if (g[v].name==g[s].params["numerator_name"]) {
      g[e].derivative = d*((1.0/denominator)-(numerator/std::pow(denominator, 2)));
    }
    else {
      g[e].derivative = d*(-1.0*numerator/std::pow(denominator, 2));
    }
    visited.insert(edgeToString(e));
    bool all_visited = true;
    in_edge_iter ei, ei_end;
    for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) {
      if (visited.find(edgeToString(*ei))==visited.end()) {
        all_visited = false;
        break;
      }
    }
    if (all_visited) {
      g[v].derivative = 0.0;
      for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) 
        g[v].derivative += g[*ei].derivative;
      DFSComputeDerivative(v, visited);
    }
  }
}

void CProvGraph::DFSComputeSoftmaxDerivative(vertex_t s, float d, std::unordered_set<std::string>& visited) {
  float denominator = 0;
  float numerator = 0; 
  adjacency_tier ai, ai_end;
  for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
    vertex_t v = *ai;
    int pos = g[v].value;
    for (int i=0; i<auxilary_data[pos].size(); i++) {
      float value = auxilary_data[g[v].value][i];
      if (!std::isnan(value)) {
        denominator += std::exp(value);
        if (i==std::stoi(g[s].params["numerator_pos"]))
          numerator = std::exp(value);
      }
    }
  }
  for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
    vertex_t v = *ai;
    edge_t e = boost::edge(s, v, g).first;
    auxilary_data.push_back(std::vector<double> (0));
    int pos = auxilary_data.size()-1;
    for (int i=0; i<auxilary_data[g[v].value].size(); i++) {
      if (i==std::stoi(g[s].params["numerator_pos"])) 
        auxilary_data[pos].push_back( d*(numerator/denominator)*(1-numerator/denominator) );
      else 
        auxilary_data[pos].push_back( -d*(std::exp(g[v].value)*numerator/std::pow(denominator, 2)) );
    }
    g[e].derivative = pos;
    visited.insert(edgeToString(e));
    g[v].derivative = pos;
    // std::cout << g[v].name << ": " << g[v].derivative << " " << utils::vector_to_string(auxilary_data[g[v].derivative]) << "\n";
    // std::exit(0);
    DFSComputeDerivative(v, visited);
  }
}

void CProvGraph::DFSComputeSigmoidDerivative(vertex_t s, float d, std::unordered_set<std::string>& visited) {
  adjacency_tier ai, ai_end;
  for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
    vertex_t v = *ai;
    edge_t e = boost::edge(s, v, g).first;
    g[e].derivative = g[v].value*(1-g[v].value)*d;
    visited.insert(edgeToString(e));
    bool all_visited = true;
    in_edge_iter ei, ei_end;
    for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) {
      if (visited.find(edgeToString(*ei))==visited.end()) {
        all_visited = false;
        break;
      }
    }
    if (all_visited) {
      g[v].derivative = 0.0;
      for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) 
        g[v].derivative += g[*ei].derivative;
      DFSComputeDerivative(v, visited);
    }
  }
}

// void CProvGraph::DFSComputeInnerProductDerivative(vertex_t s, float d, std::unordered_set<std::string>& visited) {
//   adjacency_tier ai, ai_end;
//   for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
//     vertex_t v = *ai;
//     edge_t e = boost::edge(s, v, g).first;
//     g[e].derivative = g[s].weights[g[v].name] * d;
//     visited.insert(edgeToString(e));
//     bool all_visited = true;
//     in_edge_iter ei, ei_end;
//     for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) {
//       if (visited.find(edgeToString(*ei))==visited.end()) {
//         all_visited = false;
//         break;
//       }
//     }
//     if (all_visited) {
//       g[v].derivative = 0.0;
//       for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) 
//         g[v].derivative += g[*ei].derivative;
//       DFSComputeDerivative(v, visited);
//     }
//   }
// }

void CProvGraph::DFSComputeInnerProductActDerivative(vertex_t s, int value_pos, int derivative_pos, std::unordered_set<std::string>& visited) {
  adjacency_tier ai, ai_end;
  for (boost::tie(ai, ai_end)=boost::adjacent_vertices(s, g); ai!=ai_end; ai++) {
    vertex_t v = *ai;
    edge_t e = boost::edge(s, v, g).first;
    int node_num = std::stoi(g[s].params["node_num"]);
    float previous_d = auxilary_data[derivative_pos][node_num];
    float previous_value = auxilary_data[value_pos][node_num];
    auxilary_data.push_back(std::vector<double> (0));
    int pos = auxilary_data.size()-1;
    for (int i=0; i<(*g[s].weights).size(); i++) {
      float derivative;
      if (g[s].params["act"]=="sigmoid") derivative = previous_d*utils::deriv_sigmoid(previous_value)*(*g[s].weights)[i];
      else derivative = previous_d*(*g[s].weights)[i];
      auxilary_data[pos].push_back(derivative);
    }
    g[e].derivative = pos;
    visited.insert(edgeToString(e));
    bool all_visited = true;
    in_edge_iter ei, ei_end;
    for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) {
      if (visited.find(edgeToString(*ei))==visited.end()) {
        all_visited = false;
        break;
      }
    }
    if (all_visited) {
      auxilary_data.push_back(std::vector<double> (auxilary_data[g[v].value].size(), 0.0));
      int pos = auxilary_data.size()-1;
      int num_of_edges = boost::in_degree(v, g);
      for (boost::tie(ei, ei_end)=boost::in_edges(v, g); ei!=ei_end; ei++) {
        for (int i=0; i<auxilary_data[pos].size(); i++) 
          auxilary_data[pos][i] += auxilary_data[g[*ei].derivative][i] / num_of_edges;
          // auxilary_data[pos][i] += auxilary_data[g[*ei].derivative][i];
      }
      g[v].derivative = pos;
      // std::cout << g[v].name << ": " << g[v].derivative << " " << utils::vector_to_string(auxilary_data[g[v].derivative]) << "\n";
      DFSComputeDerivative(v, visited);
    }
  }
}