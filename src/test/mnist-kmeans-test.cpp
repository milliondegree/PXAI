// Implementation of the KMeans Algorithm
// reference: http://mnemstudio.org/clustering-k-means-example-1.htm

#include "../KMeans.h"
#include <boost/algorithm/string.hpp>
#include <chrono>
#include <numeric>
#include <random>


const int number_classes = 100;
const int number_features = 784;
const int number_points = 2000;
const char *mnist_dataset = "./data/mnist/mnist_train.csv";



double euclideanDistance(const std::vector<double>* vec1, const std::vector<double>* vec2) {
  if (vec1->size() != vec2->size()) {
    throw std::invalid_argument("Vectors must be of the same size.");
  }

  double sum_of_squares = 0.0;
  for (size_t i = 0; i < vec1->size(); ++i) {
    sum_of_squares += std::pow((*vec1)[i] - (*vec2)[i], 2);
  }

  return std::sqrt(sum_of_squares);
}


// void load_data(vector<Point>& points, int pos, int point_num) {
//   ifstream ofin(mnist_dataset);
//   string line;

//   int n = 0;
//   while (getline(ofin, line)) {
//     if (!line.empty()) {
//       if (n<pos) {
//         n++;
//         continue;
//       }
//       line.erase(0, line.find_first_not_of(" \t\r\n"));
//       line.erase(line.find_last_not_of(" \t\r\n")+1);
//       vector<string> split;
//       boost::split(split, line, boost::is_any_of(","));
//       vector<double> values;
//       for (int i=1; i<=number_features; i++) {
//         values.push_back(stod(split[i]));
//       }
//       string class_name = split[0];
//       Point p(n, values, class_name); 
//       points.push_back(p);
//       n++;
//       if (n==point_num+pos) break;
//     }
//   }
//   // cout << n << endl;
// }

void load_data(vector<Point>& points) {
  ifstream ofin(mnist_dataset);
  string line;

  int n = 0;
  while (getline(ofin, line)) {
    if (!line.empty()) {
      line.erase(0, line.find_first_not_of(" \t\r\n"));
      line.erase(line.find_last_not_of(" \t\r\n")+1);
      vector<string> split;
      boost::split(split, line, boost::is_any_of(","));
      vector<double> values;
      for (int i=1; i<=number_features; i++) {
        values.push_back(stod(split[i]));
      }
      string class_name = split[0];
      Point p(n, values, class_name); 
      points.push_back(p);
      n++;
      if (n==60000) break;
    }
  }
}

void sample_data(vector<Point>& data, vector<Point>& sampled_data, int sample_size) {
  std::random_device rd;
  std::mt19937 gen(rd());

  // Consider the entire dataset scenario.
  if (sample_size >= data.size()) {
    sampled_data = data;
    return;
  }

  // Copy the original dataset to a temporary variable for shuffling.
  sampled_data = data;

  // Use a variant of the Fisher-Yates shuffle to randomly shuffle the vector.
  for (int i = 0; i < sample_size; ++i) {
    std::uniform_int_distribution<> distrib(i, sampled_data.size() - 1);
    int random_index = distrib(gen);
    std::swap(sampled_data[i], sampled_data[random_index]);
  }

  // Use erase-remove idiom to shrink the vector without default initialization
  sampled_data.erase(sampled_data.begin() + sample_size, sampled_data.end());
}

int main(int argc, char *argv[])
{
  int K, max_iterations;

  K = number_classes;
  max_iterations = 50;

  vector<double> without_prov_time;
  vector<double> with_prov_time;
  vector<double> delete_time;
  vector<double> dist_list;
  vector<size_t> prov_size_list;

  int num = 100;
  int count = 0;

  auto start = std::chrono::high_resolution_clock::now();
  vector<Point> points; 
  load_data(points);
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
  std::cout << "Loading data: " << duration * 1.0/1000000 << " seconds" << std::endl;

  for (int index = 0; index<num; index++) {
    vector<Point> sampled_points;
    sample_data(points, sampled_points, number_points);
    vector<Point> copied_sampled_points = vector<Point> (sampled_points);

    KMeans kmeans(K, number_points, number_features, max_iterations);
    KMeans kmeans_prov(K, number_points, number_features, max_iterations);

    start = std::chrono::high_resolution_clock::now();
    kmeans.run(sampled_points, 0);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    // std::cout << "Without provenance: " << duration * 1.0/1000000 << " seconds" << std::endl;
    without_prov_time.push_back(duration * 1.0/1000000);

    vector<vector<double>*> centroids;
    vector<vector<double>*> distances;
    start = std::chrono::high_resolution_clock::now();
    int iteration = kmeans_prov.runWithProv_v2(copied_sampled_points, centroids, distances);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    // std::cout << "With provenance: " << duration * 1.0/1000000 << " seconds" << std::endl;
    // std::cout << "\n";
    with_prov_time.push_back(duration * 1.0/1000000); 

    prov_size_list.push_back(kmeans_prov.provG.getProvSize()+centroids.size()*(*centroids[0]).size()*sizeof(double)+distances.size()*(*distances[0]).size()*sizeof(double));

    kmeans_prov.resetClusters();
    start = std::chrono::high_resolution_clock::now();
    kmeans_prov.deletePoint(copied_sampled_points, centroids, distances, 0, iteration);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    // std::cout << "ML deletion time: " << duration * 1.0/1000000 << " seconds" << std::endl;
    delete_time.push_back(duration * 1.0/1000000);
    // std::cout << endl;

    double dist = 0;
    for (int i=0; i<number_classes; i++) {
      dist += euclideanDistance(kmeans.clusters[i].getCentralValues(), kmeans_prov.clusters[i].getCentralValues());
    }
    // std::cout << "centroid distance: " << dist << std::endl << std::endl;
    dist_list.push_back(dist);
    
    count++;
  }

  std::cout << "# of clusters: " << number_classes << std::endl;
  std::cout << "# of points: " << number_points << std::endl; 
  std::cout << "dist: " << std::accumulate(dist_list.begin(), dist_list.end(), 0.0) / count << endl;
  std::cout << "without prov: " << std::accumulate(without_prov_time.begin(), without_prov_time.end(), 0.0) / count << endl;
  std::cout << "with prov: " << std::accumulate(with_prov_time.begin(), with_prov_time.end(), 0.0) / count << endl;
  std::cout << "delete prov: " << std::accumulate(delete_time.begin(), delete_time.end(), 0.0) / count << endl;
  std::cout << "storage: " << std::accumulate(prov_size_list.begin(), prov_size_list.end(), 0.0) / (1024*1024*count) << endl;
  std::cout << std::endl;

  return 0;
}