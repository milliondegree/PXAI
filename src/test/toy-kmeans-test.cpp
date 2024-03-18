// Implementation of the KMeans Algorithm
// reference: http://mnemstudio.org/clustering-k-means-example-1.htm

#include "../KMeans.h"
#include <boost/algorithm/string.hpp>
#include <chrono>


const int number_classes = 3;
const int number_features = 2;
const int number_points = 6;
const char *toy_kmeans_dataset = "./data/toy-kmeans/toy-kmeans.data";
const std::string cprov_save_path = "./data/toy-kmeans/cprov/test.dot";
const std::array<std::string, number_classes> class_names =
{ "Class-1", "Class-2", "Class-3" };


void load_data(vector<Point>& points) {
  ifstream ofin(toy_kmeans_dataset);
  string line;

  int n = 0;
  while (getline(ofin, line)) {
    if (!line.empty()) {
      line.erase(0, line.find_first_not_of(" \t\r\n"));
      line.erase(line.find_last_not_of(" \t\r\n")+1);
      vector<string> split;
      boost::split(split, line, boost::is_any_of(","));
      vector<double> values;
      for (int i=0; i<number_features; i++) {
        values.push_back(stod(split[i]));
      }
      string class_name = split[number_features];
      Point p(n, values, class_name);
      points.push_back(p);
      n++;
    }
  }
}


int main(int argc, char *argv[])
{
  srand (time(NULL));

  int K, max_iterations;

  K = number_classes;
  max_iterations = 100;

  vector<Point> points; 
  load_data(points);
 
  KMeans kmeans(K, number_points, number_features, max_iterations);
  KMeans kmeans_prov(K, number_points, number_features, max_iterations);

  auto start = std::chrono::high_resolution_clock::now();
  kmeans.run(points, 5);
  auto end = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
  std::cout << "Without provenance: " << duration * 1.0/1000000 << " seconds" << std::endl << std::endl;

  points.clear();
  load_data(points);
  start = std::chrono::high_resolution_clock::now();
  vector<vector<double>*> centroids;
  vector<vector<double>*> distances;
  int iteration = kmeans_prov.runWithProv_v2(points, centroids, distances);
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
  std::cout << "With provenance: " << duration * 1.0/1000000 << " seconds" << std::endl;

  kmeans_prov.provG.setSavePath(cprov_save_path);
  kmeans_prov.provG.saveGraph();

  start = std::chrono::high_resolution_clock::now();
  kmeans_prov.deletePoint(points, centroids, distances, 5, iteration);
  end = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
  std::cout << "ML deletion time: " << duration * 1.0/1000000 << " seconds" << std::endl;

  return 0;
}