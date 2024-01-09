//============================================================================
// Name : Main.cpp
// Author : David Nogueira
//============================================================================
#include "../MLP.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <array>
#include <algorithm>
#include "../microunit.h"
// #include "../easylogging++.h"

// INITIALIZE_EASYLOGGINGPP


// Example illustrating practical use of this MLP lib.
// Disclaimer: This is NOT an example of good machine learning practices
//              regarding training/testing dataset partitioning.

const int input_size = 46;
const int number_classes = 3;
const char *credit_score_dataset = "./data/credit-score/train.csv";
const std::string credit_score_mlp_weights = "./data/credit-score/credit_score_normal_1024.mlp";
const std::string cprov_save_path = "./data/credit-score/cprov/test.dot";
const std::array<std::string, number_classes> class_names =
{ "Good", "Standard", "Poor" };


bool load_data(int *samples,
  std::vector<double>  *input,
  std::vector<double> *credit_score_class) {
  // Load the credit_score data-set. 
  FILE *in = fopen(credit_score_dataset, "r");
  if (!in) {
    std::cout << "Could not open file" << std::endl;
  }

  // Loop through the data to get a count.
  char line[2048];
  while (!feof(in) && fgets(line, 1024, in)) {
    ++(*samples);
  }
  fseek(in, 0, SEEK_SET); 

  (*samples) = 1000;
  std::cout << "Loading " << (*samples)
    << " data points from " << credit_score_dataset << "." << std::endl;
  // Allocate memory for input and output data.
  input->resize((*samples) * input_size);
  credit_score_class->resize((*samples) * number_classes);

  // Read the file into our arrays. 
  int i, j;
  int if_first_line = 0;
  for (i = 0; i < (*samples); ++i) {

    fgets(line, 2048, in);
    if (if_first_line==0) {
      i--;
      if_first_line++;
      continue;
    }

    double *p = &((*input)[0]) + i * input_size;
    double *c = &((*credit_score_class)[0]) + i * number_classes;
    for (int k = 0; k < number_classes; k++) {
      c[i] = 0.0;
    }

    char *split = strtok(line, ",");
    for (j = 0; j < input_size; ++j) {
      p[j] = atof(split);
      split = strtok(NULL, ",");
    }

    if (strlen(split) >= 1 && split[strlen(split) - 1] == '\n')
      split[strlen(split) - 1] = '\0';
    if (strlen(split) >= 2 && split[strlen(split) - 2] == '\r')
      split[strlen(split) - 2] = '\0';

    if (strcmp(split, class_names[0].c_str()) == 0) {
      c[0] = 1.0;
    }
    else if (strcmp(split, class_names[1].c_str()) == 0) {
      c[1] = 1.0;
    }
    else if (strcmp(split, class_names[2].c_str()) == 0) {
      c[2] = 1.0;
    }
    else {
      std::cout << "Unknown credit_score_class" << split << "." << std::endl;
      return false;
    }
  }

  fclose(in);
  return true;
}


int main(int argc, char *argv[]) {
  int samples = 0;
  std::vector<double> input;
  std::vector<double> credit_score_class;

  // Load the data from file.
  if (!load_data(&samples, &input, &credit_score_class)) {
    std::cout << "Error processing input file." << std::endl;
    return -1;
  }

  std::vector<TrainingSample> training_set;
  for (int j = 0; j < samples; ++j) {
    std::vector<double> training_set_input;
    std::vector<double> training_set_output;
    training_set_input.reserve(input_size);
    for (int i = 0; i < input_size; i++)
      training_set_input.push_back(*(&(input[0]) + j * input_size + i));
    training_set_output.reserve(number_classes);
    for (int i = 0; i < number_classes; i++)
      training_set_output.push_back(*(&(credit_score_class[0]) + j * 3 + i));
    training_set.emplace_back(std::move(training_set_input),
      std::move(training_set_output));
  }

  std::vector<TrainingSample> training_sample_set_with_bias(std::move(training_set));
  //set up bias
  for (auto & training_sample_with_bias : training_sample_set_with_bias) {
    training_sample_with_bias.AddBiasValue(1);
  }
  
  //Destruction/Construction of a MLP object to show off saving and loading a trained model
  

  int correct = 0;
  samples = 1;
  float total_inference = 0;
  float total_prov = 0;
  float total_prov_query = 0;
  float total_prov_inference = 0;
  float total_approx = 0;
  float total_approx_prov_inference = 0;
  for (int j = 0; j < samples; ++j) {
    std::vector<double> guess;

    MLP my_mlp(credit_score_mlp_weights);

    clock_t t1, t2;
    t1 = clock();
    my_mlp.GetOutput(training_sample_set_with_bias[j].input_vector(), &guess);
    t2 = clock();
    std::cout << "Without provenance: " << (t2-t1)*1.0/CLOCKS_PER_SEC << std::endl;
    total_inference += (t2-t1)*1.0/CLOCKS_PER_SEC;

    t1 = clock();
    my_mlp.GetOutputWithProv(training_sample_set_with_bias[j].input_vector(), &guess);
    t2 = clock();
    my_mlp.provG.setSavePath(cprov_save_path);
    // my_mlp.provG.saveGraph();
    std::cout << "With provenance: " << (t2-t1)*1.0/CLOCKS_PER_SEC << std::endl;
    total_prov += (t2-t1)*1.0/CLOCKS_PER_SEC;

    t1 = clock();
    std::string to_query = "softmax_0"; 
    cpg::CProvGraph query_output = my_mlp.provG.ProvenanceQuery(to_query);
    t2 = clock();
    std::cout << "Provenance query: " << (t2-t1)*1.0/CLOCKS_PER_SEC << std::endl;
    total_prov_query += (t2-t1)*1.0/CLOCKS_PER_SEC;

    t1 = clock();
    std::cout << query_output.computeVariable(to_query) << std::endl;
    t2 = clock();
    std::cout << "Provenance recompute time: " << (t2-t1)*1.0/CLOCKS_PER_SEC << std::endl;
    
    t1 = clock();
    std::cout << query_output.computeVariableMLP(to_query) << std::endl;
    t2 = clock();
    std::cout << "Provenance recompute time optimized: " << (t2-t1)*1.0/CLOCKS_PER_SEC << std::endl;
    total_prov_inference += (t2-t1)*1.0/CLOCKS_PER_SEC;

    // query_output.computeDerivative(to_query);
    // query_output.saveGraph();

    // std::unordered_map<std::string, float> changedEDBs;
    // changedEDBs["input_0_2"] = changedEDBs["input_0_2"];
    // t1 = clock();
    // query_output.computeVariableWithChangedEDBs(to_query, changedEDBs);
    // t2 = clock();
    // std::cout << "Recompute with changed EDBs time: " << (t2-t1)*1.0/CLOCKS_PER_SEC << std::endl;

    t1 = clock();
    cpg::CProvGraph approx_output = query_output.ApproximateSubGraphQueryPruneMLP(to_query, 0.3, 0.01);
    t2 = clock();
    std::cout << "Approx prune time: " << (t2-t1)*1.0/CLOCKS_PER_SEC << std::endl;
    total_approx += (t2-t1)*1.0/CLOCKS_PER_SEC;

    t1 = clock();
    std::cout << approx_output.computeVariableMLP(to_query) << std::endl;
    t2 = clock();
    std::cout << "Approximate provenance recompute time: " << (t2-t1)*1.0/CLOCKS_PER_SEC << std::endl;
    total_approx_prov_inference += (t2-t1)*1.0/CLOCKS_PER_SEC;

    // t1 = clock();
    // approx_output.computeVariableWithChangedEDBs(to_query, changedEDBs);
    // t2 = clock();
    // std::cout << "Recompute with changed EDBs time: " << (t2-t1)*1.0/CLOCKS_PER_SEC << std::endl;

    // approx_output.saveGraph();

  }
  std::cout << "average inference: " << total_inference/samples << "\n";
  std::cout << "average inference on prov: " << total_prov_inference/samples << "\n";
  std::cout << "average approx: " << total_approx/samples << "\n";
  std::cout << "average inference on approx: " << total_approx_prov_inference/samples << "\n";

  return 0;
}
