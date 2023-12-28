//============================================================================
// Name : Node.h
// Author : David Nogueira
//============================================================================
#ifndef NODE_H
#define NODE_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cassert> // for assert()
#include <exception>
#include "Utils.h"
#include "CProvGraph.h"

#define CONSTANT_WEIGHT_INITIALIZATION 0

class Node {
public:
  Node() {
    m_num_inputs = 0;
    m_bias = 0.0;
    m_weights.clear();
  };
  Node(int num_inputs,
       bool use_constant_weight_init = true,
       double constant_weight_init = 0.5) {
    m_num_inputs = num_inputs;
    m_bias = 0.0;
    m_weights.clear();
    //initialize weight vector
    WeightInitialization(m_num_inputs,
                         use_constant_weight_init,
                         constant_weight_init);
  };

  ~Node() {
    m_num_inputs = 0;
    m_bias = 0.0;
    m_weights.clear();
  };

  void WeightInitialization(int num_inputs,
                            bool use_constant_weight_init = true,
                            double constant_weight_init = 0.5) {
    m_num_inputs = num_inputs;
    //initialize weight vector
    if (use_constant_weight_init) {
      m_weights.resize(m_num_inputs, constant_weight_init);
    } else {
      m_weights.resize(m_num_inputs);
      std::generate_n(m_weights.begin(),
                      m_num_inputs,
                      utils::gen_rand());
    }
  }

  int GetInputSize() const {
    return m_num_inputs;
  }

  void SetInputSize(int num_inputs) {
    m_num_inputs = num_inputs;
  }

  double GetBias() const {
    return m_bias;
  }
  void SetBias(double bias) {
    m_bias = bias;
  }

  std::vector<double> & GetWeights() {
    return m_weights;
  }

  const std::vector<double> & GetWeights() const {
    return m_weights;
  }

  void SetWeights( std::vector<double> & weights ){
      // check size of the weights vector
      if( weights.size() == m_num_inputs )
          m_weights = weights;
      else
          throw new std::logic_error("Incorrect weight size in SetWeights call");
  }

  size_t GetWeightsVectorSize() const {
    return m_weights.size();
  }

  void GetInputInnerProdWithWeights(const std::vector<double> &input,
                                    double * output) const {
    assert(input.size() == m_weights.size());
    double inner_prod = std::inner_product(begin(input),
                                           end(input),
                                           begin(m_weights),
                                           0.0);
    *output = inner_prod;
  }

  void GetOutputAfterActivationFunction(const std::vector<double> &input,
                                        std::function<double(double)> activation_function,
                                        double * output) const {
    double inner_prod = 0.0;
    GetInputInnerProdWithWeights(input, &inner_prod);
    *output = activation_function(inner_prod);
  }

  void GetOutputAfterActivationFunctionWithProv(const std::vector<double>& input,
                                                std::function<double(double)> activation_function,
                                                double * output,
                                                std::vector<std::string>& input_names,
                                                int layer_num,
                                                int node_num,
                                                const std::string& m_activation_function_str,
                                                cpg::CProvGraph& provG) {
    double inner_prod = 0.0;
    GetInputInnerProdWithWeights(input, &inner_prod);

    // build provenance for inner product
    // std::vector<std::string> sum_input_names;
    // for (int j=0; j<m_weights.size(); j++) {
    //   std::vector<std::string> input_names_tmp;
    //   std::string weight_name = "weight_"+std::to_string(layer_num)+"_"+std::to_string(node_num)+"_"+std::to_string(j);
    //   provG.addVariableVertex(cpg::Parameter, weight_name, m_weights[j]);
    //   input_names_tmp.push_back(weight_name);
    //   input_names_tmp.push_back(input_names[j]);
    //   std::string tmp_name = "input_"+std::to_string(layer_num+1)+"_"+std::to_string(node_num)+"_"+std::to_string(j);
    //   provG.addComputingSubgraph(tmp_name, input[j]*m_weights[j], cpg::Mul, input_names_tmp);
    //   sum_input_names.push_back(tmp_name);
    // }
    // std::unordered_map<std::string, float> weights;
    // for (int j=0; j<m_weights.size(); j++) {
    //   weights[input_names[j]] = m_weights[j];
    // }
    // std::string sum_output_name;
    // if (m_activation_function_str=="linear") {
    //   *output = activation_function(inner_prod);
    //   sum_output_name = "input_"+std::to_string(layer_num+1)+"_"+std::to_string(node_num);
    //   provG.addComputingSubgraph(sum_output_name, *output, cpg::InnerProduct, input_names, weights);
    //   return;
    // }
    // else sum_output_name = "input_"+std::to_string(layer_num+1)+"_"+std::to_string(node_num)+"_no_act";
    // provG.addComputingSubgraph(sum_output_name, inner_prod, cpg::InnerProduct, input_names, weights);

    *output = activation_function(inner_prod);

    // build provenance for activation function
    // std::string act_output_name = "input_"+std::to_string(layer_num+1)+"_"+std::to_string(node_num);
    // std::vector<std::string> act_input_names;
    // act_input_names.push_back(sum_output_name);
    // if (m_activation_function_str=="sigmoid") {
    //   provG.addComputingSubgraph(act_output_name, *output, cpg::Sigmoid, act_input_names);
    // }

    std::string output_name = "input_"+std::to_string(layer_num+1);
    if (node_num==0) {
      provG.auxilary_data.push_back(std::vector<double> (0));
    }
    int pos = provG.auxilary_data.size()-1;
    provG.auxilary_data[pos].push_back(*output);
    
    std::unordered_map<std::string, std::string> params;
    params["act"] = m_activation_function_str;
    params["node_num"] = std::to_string(node_num);
    provG.addComputingSubgraph(output_name, float(pos), cpg::InnerProductAct, input_names, params, m_weights, node_num);
  }

  void GetBooleanOutput(const std::vector<double> &input,
                        std::function<double(double)> activation_function,
                        bool * bool_output,
                        double threshold = 0.5) const {
    double value;
    GetOutputAfterActivationFunction(input, activation_function, &value);
    *bool_output = (value > threshold) ? true : false;
  };

  void UpdateWeights(const std::vector<double> &x,
                     double error,
                     double learning_rate) {
    assert(x.size() == m_weights.size());
    for (size_t i = 0; i < m_weights.size(); i++)
      m_weights[i] += x[i] * learning_rate *  error;
  };

  void UpdateWeight(int weight_id,
                    double increment,
                    double learning_rate) {
    m_weights[weight_id] += learning_rate*increment;
  }

  void SaveNode(FILE * file) const {
    fwrite(&m_num_inputs, sizeof(m_num_inputs), 1, file);
    fwrite(&m_bias, sizeof(m_bias), 1, file);
    fwrite(&m_weights[0], sizeof(m_weights[0]), m_weights.size(), file);
  };
  void LoadNode(FILE * file) {
    m_weights.clear();

    fread(&m_num_inputs, sizeof(m_num_inputs), 1, file);
    fread(&m_bias, sizeof(m_bias), 1, file);
    m_weights.resize(m_num_inputs);
    fread(&m_weights[0], sizeof(m_weights[0]), m_weights.size(), file);
  };

protected:
  size_t m_num_inputs{ 0 };
  double m_bias{ 0.0 };
  std::vector<double> m_weights;
};

#endif //NODE_H
