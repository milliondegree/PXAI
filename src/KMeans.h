// Implementation of the KMeans Algorithm
// reference: http://mnemstudio.org/clustering-k-means-example-1.htm

#include <iostream>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>

#include "CKMeansProvGraph.h"
#include "Utils.h"

using namespace std;

class Point
{
private:
  int id_point, id_cluster;
  vector<double> values;
  int total_values;
  string name;

public:
  Point(int id_point, vector<double>& values, string name = "")
  {
    this->id_point = id_point;
    total_values = values.size();

    for(int i = 0; i < total_values; i++)
      this->values.push_back(values[i]);

    this->name = name;
    id_cluster = -1;
  }

  int getID()
  {
    return id_point;
  }

  void setCluster(int id_cluster)
  {
    this->id_cluster = id_cluster;
  }

  int getCluster()
  {
    return id_cluster;
  }

  double getValue(int index)
  {
    return values[index];
  }

  vector<double> getValues() 
  {
    return values;
  }

  int getTotalValues()
  {
    return total_values;
  }

  void addValue(double value)
  {
    values.push_back(value);
  }

  string getName()
  {
    return name;
  }
};

class Cluster
{
private:
  int id_cluster;
  vector<double> central_values;
  vector<Point> points;

public:
  Cluster(int id_cluster, Point point)
  {
    this->id_cluster = id_cluster;

    int total_values = point.getTotalValues();

    for(int i = 0; i < total_values; i++)
      central_values.push_back(point.getValue(i));

    points.push_back(point);
  }

  Cluster(int id_cluster, int number_of_values, vector<double>* centroid)
  {
    this->id_cluster = id_cluster;

    int total_values = number_of_values;

    central_values = *centroid;
  }

  void addPoint(Point point)
  {
    points.push_back(point);
  }

  bool removePoint(int id_point)
  {
    int total_points = points.size();

    for(int i = 0; i < total_points; i++)
    {
      if(points[i].getID() == id_point)
      {
        points.erase(points.begin() + i);
        return true;
      }
    }
    return false;
  }

  double getCentralValue(int index)
  {
    return central_values[index];
  }

  vector<double>* getCentralValues() 
  {
    return &central_values;
  }

  void setCentralValue(int index, double value)
  {
    central_values[index] = value;
  }

  Point getPoint(int index)
  {
    return points[index];
  }

  int getTotalPoints()
  {
    return points.size();
  }

  int getID()
  {
    return id_cluster;
  }
};

class KMeans
{
private:
  int K; // number of clusters
  int total_values, total_points, max_iterations;

public:
  vector<Cluster> clusters;
  // std::vector<std::vector<double>*> centroids; // Store pointers to centroid data

public:
  // return ID of nearest center (uses euclidean distance)
  int getIDNearestCenter(Point point)
  {
    double sum = 0.0, min_dist;
    int id_cluster_center = 0;

    for(int i = 0; i < total_values; i++)
    {
      sum += pow(clusters[0].getCentralValue(i) -
             point.getValue(i), 2.0);
    }

    min_dist = sqrt(sum);

    for(int i = 1; i < K; i++)
    {
      double dist;
      sum = 0.0;

      for(int j = 0; j < total_values; j++)
      {
        sum += pow(clusters[i].getCentralValue(j) -
               point.getValue(j), 2.0);
      }

      dist = sqrt(sum);

      if(dist < min_dist)
      {
        min_dist = dist;
        id_cluster_center = i;
      }
    }

    return id_cluster_center;
  }

  void resetClusters() {
    this->clusters.clear();
  }

public:
  KMeans(int K, int total_points, int total_values, int max_iterations)
  {
    this->K = K;
    this->total_points = total_points;
    this->total_values = total_values;
    this->max_iterations = max_iterations;
  }

  void run(vector<Point> & points);

  void run(vector<Point> & points, int index);

  ckpg::CKMeansProvGraph provG;

  void runWithProv(vector<Point> & points);

  int runWithProv_v2(vector<Point> & points, vector<vector<double>*>& centroids, vector<vector<double>*>& distances);

    /* ML deletion */
public:
  void deletePoint(vector<Point> & points, vector<vector<double>*>& centroids, vector<vector<double>*>& distances, int index, int iteration);

  void printCentroid(vector<double>* centroid) {
    for (auto d : *centroid) {
      cout << d << ' ';
    }
  }

};
