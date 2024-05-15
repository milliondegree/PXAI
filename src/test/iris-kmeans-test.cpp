// Implementation of the KMeans Algorithm
// reference: http://mnemstudio.org/clustering-k-means-example-1.htm

#include "../KMeans.h"
#include <boost/algorithm/string.hpp>
#include <chrono>
#include <numeric>


const int number_classes = 3;
const int number_features = 4;
const int number_points = 150 * 1;
const char *iris_dataset = "./data/iris-kmeans/iris.data";
const std::string cprov_save_path = "./data/iris-kmeans/cprov/test.dot";
const std::array<std::string, number_classes> class_names =
{ "Iris-setosa", "Iris-versicolor", "Iris-virginica" };

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


void load_data(vector<Point>& points) {
  ifstream ofin(iris_dataset);
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

  vector<double> without_prov_time;
  vector<double> with_prov_time;
  vector<double> delete_time;

  int num = 10;
  int count = 0;

  for (int index = 0; index<num; index++) {
    if (index%(number_points / number_classes)==0) continue;
    vector<Point> points; 
    load_data(points);

    KMeans kmeans(K, number_points, number_features, max_iterations);
    KMeans kmeans_prov(K, number_points, number_features, max_iterations);

    auto start = std::chrono::high_resolution_clock::now();
    kmeans.run(points, index);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "Without provenance: " << duration * 1.0/1000000 << " seconds" << std::endl << std::endl;
    without_prov_time.push_back(duration * 1.0/1000000);

    points.clear();
    load_data(points);
    start = std::chrono::high_resolution_clock::now();
    vector<vector<double>*> centroids;
    vector<vector<double>*> distances;
    int iteration = kmeans_prov.runWithProv_v2(points, centroids, distances);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "With provenance: " << duration * 1.0/1000000 << " seconds" << std::endl;
    std::cout << "\n";
    with_prov_time.push_back(duration * 1.0/1000000);

    kmeans_prov.resetClusters();
    start = std::chrono::high_resolution_clock::now();
    kmeans_prov.deletePoint(points, centroids, distances, index, iteration);
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "ML deletion time: " << duration * 1.0/1000000 << " seconds" << std::endl;
    delete_time.push_back(duration * 1.0/1000000);
    std::cout << endl;

    double dist = 0;
    for (int i=0; i<number_classes; i++) {
      dist += euclideanDistance(kmeans.clusters[i].getCentralValues(), kmeans_prov.clusters[i].getCentralValues());
    }
    std::cout << "centroid distance: " << dist << std::endl << std::endl;
    
    count++;
  }

  std::cout << "without prov: " << std::accumulate(without_prov_time.begin(), without_prov_time.end(), 0.0) / count << endl;
  std::cout << "with prov: " << std::accumulate(with_prov_time.begin(), with_prov_time.end(), 0.0) / count << endl;
  std::cout << "delete prov: " << std::accumulate(delete_time.begin(), delete_time.end(), 0.0) / count << endl;

  return 0;
}