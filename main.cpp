#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <chrono> // For measuring time
#include <omp.h>

using namespace std;
using namespace std::chrono;

// iPair is integer pairs
#define iPair pair<int, int>

// Stores the result (points of convex hull)
set<iPair> hull;

// Returns the side of point p with respect to line
// joining points p1 and p2.
int findSide(iPair p1, iPair p2, iPair p)
{
    int val = (p.second - p1.second) * (p2.first - p1.first) -
              (p2.second - p1.second) * (p.first - p1.first);

    if (val > 0)
        return 1;
    if (val < 0)
        return -1;
    return 0;
}

// returns a value proportional to the distance
// between the point p and the line joining the
// points p1 and p2
int lineDist(iPair p1, iPair p2, iPair p)
{
    return abs((p.second - p1.second) * (p2.first - p1.first) -
               (p2.second - p1.second) * (p.first - p1.first));
}

// End points of line L are p1 and p2. side can have value
// 1 or -1 specifying each of the parts made by the line L
void quickHull(vector<iPair>& a, iPair p1, iPair p2, int side)
{
    int ind = -1;
    int max_dist = 0;

    // finding the point with maximum distance
    // from L and also on the specified side of L.
    for (int i = 0; i < a.size(); i++)
    {
        int temp = lineDist(p1, p2, a[i]);
        if (findSide(p1, p2, a[i]) == side && temp > max_dist)
        {
            ind = i;
            max_dist = temp;
        }
    }

    // If no point is found, add the end points
    // of L to the convex hull.
    if (ind == -1)
    {
        #pragma omp critical
        {
            hull.insert(p1);
            hull.insert(p2);
        }
        return;
    }

    // Recur for the two parts divided by a[ind]
    quickHull(a, a[ind], p1, -findSide(a[ind], p1, p2));
    quickHull(a, a[ind], p2, -findSide(a[ind], p2, p1));
}

vector<iPair> printHull(vector<iPair>& a)
{
    vector<iPair> convexHull;
    // a[i].second -> y-coordinate of the ith point
    if (a.size() < 3)
    {
        cout << "Convex hull not possible\n";
        return convexHull;
    }

    // Finding the point with minimum and
    // maximum x-coordinate
    int min_x = 0, max_x = 0;
    for (int i = 1; i < a.size(); i++)
    {
        if (a[i].first < a[min_x].first)
            min_x = i;
        if (a[i].first > a[max_x].first)
            max_x = i;
    }

    // Recursively find convex hull points on
    // one side of line joining a[min_x] and
    // a[max_x]
    quickHull(a, a[min_x], a[max_x], 1);

    // Recursively find convex hull points on
    // other side of line joining a[min_x] and
    // a[max_x]
    quickHull(a, a[min_x], a[max_x], -1);

    // Copy the points from set to vector
    for (const auto& point : hull)
    {
        convexHull.push_back(point);
    }

    // Clear the global hull set for next iterations
    hull.clear();

    return convexHull;
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <number_of_threads>" << endl;
        return 1;
    }

    int threads = atoi(argv[1]);
    omp_set_num_threads(threads);

    vector<iPair> points;
    ifstream infile("output6.txt");

    if (!infile)
    {
        cerr << "Error: Unable to open input file!" << endl;
        return 1;
    }

    int x, y;
    while (infile >> x >> y)
    {
        points.push_back(make_pair(x, y));
    }

    auto start_time = high_resolution_clock::now(); // Start measuring time

    // Calculate the convex hull points
    vector<iPair> convexHullPoints = printHull(points);

    cout << "The points in Convex Hull 3 are:\n";
    for (const auto& point : convexHullPoints)
    {
        cout << "(" << point.first << ", "
             << point.second << ") ";
    }
    cout << endl;

    auto end_time = high_resolution_clock::now(); // End measuring time
    auto duration = duration_cast<milliseconds>(end_time - start_time);
    cout << "Total time taken: " << duration.count() << " milliseconds" << endl;

    return 0;
}
