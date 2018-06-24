#include <queue>
#include "main.hpp"

#define CLUSTER_UNDEFINED 0
#define CLUSTER_NOISE -1
#define RANGE_QUERY_FUNCTION circle_query_range

struct Point_Unit{
    Point2f point;
    long index;
    long order;
    long label;

    Point_Unit(int x, int y, long b){
        point = Point2f(x,y);
        index = b;
        label = CLUSTER_UNDEFINED;
    }
};

///////////////////////////////////
// Utility
///////////////////////////////////

void load_data_set(vector<Point_Unit *> * points){
    ifstream sample_file("/home/mc/Desktop/container/1/currenttime-11-32-36--224.png.data.txt");
    //ifstream sample_file("/home/mc/Desktop/container/2/currenttime-11-55-24--629.png.data.txt");

    string buffer;

    while (sample_file >> buffer){
        if (buffer == "points") break;
    }

    int x, y, i = 0;
    while (sample_file >> x >> y){
        points->emplace_back(new Point_Unit(x, y, i));
        i++;
    }
}

Mat display_points_with_label(vector<Point_Unit *> * point_list){
    int max_width, max_height;
    max_width = max_height = 0;
    for (const auto & item: *point_list){
        if (item->point.x > max_width){
            max_width = (int)item->point.x;
        }
        if (item->point.y > max_height){
            max_height = (int)item->point.y;
        }
    }
    //cout << max_height << "x" << max_width << endl;

    Mat image(max_height, max_width, CV_8UC3);
    image = Scalar::all(0);

    set<int> label_set;
    for (const auto & item: *point_list){
        label_set.insert(item->label);
    }
    cout << "cluster: " << label_set.size() << endl;

    map<int,Scalar> random_color_list;
    for (const auto & item: label_set){
        random_color_list[item] = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
    }

    for (long i = 0; i < point_list->size(); ++i){
        if (point_list->at(i)->label == CLUSTER_NOISE){
            circle(image, point_list->at(i)->point, 1, random_color_list[point_list->at(i)->label], 2);
        }else{
            circle(image, point_list->at(i)->point, 5, random_color_list[point_list->at(i)->label], 2);
        }


    }

    return image;
}


///////////////////////////////////
// Helper Method
///////////////////////////////////


vector<long> square_query_range(vector<Point_Unit *> * sorted_point, long mid, int range){

    // search left and right to see if the point falls in range
    //vector<Point2f> result;
    vector<long> result;
    auto i = mid + 1;
    while (i < sorted_point->size() && sorted_point->at(i)->point.x < sorted_point->at(mid)->point.x + range){
        if ((sorted_point->at(i)->point.y < sorted_point->at(mid)->point.y + range) && (sorted_point->at(i)->point.y > sorted_point->at(mid)->point.y - range)){
            //result.push_back(sorted_point->at(i));
            result.push_back(sorted_point->at(i)->index);
        }
        i++;
    }
    i = mid - 1;
    while (i >= 0 && sorted_point->at(i)->point.x > sorted_point->at(mid)->point.x - range){
        if ((sorted_point->at(i)->point.y < sorted_point->at(mid)->point.y + range) && (sorted_point->at(i)->point.y > sorted_point->at(mid)->point.y - range)){
            //result.push_back(sorted_point->at(i));
            result.push_back(sorted_point->at(i)->index);
        }
        i--;
    }

    // return all the points
    return result;
}

queue<long> circle_query_range(vector<Point_Unit *> * sorted_point, long mid, int range) {

    int range_squared = range * range;
    queue<long> result;
    long i = mid + 1;
    while (i < sorted_point->size() && sorted_point->at(i)->point.x < sorted_point->at(mid)->point.x + range){
        auto delta_x = (sorted_point->at(i)->point.x-sorted_point->at(mid)->point.x);
        auto delta_y = (sorted_point->at(i)->point.y-sorted_point->at(mid)->point.y);
        if (delta_x * delta_x + delta_y * delta_y < range_squared){
            result.push(sorted_point->at(i)->index);
        }
        i++;
    }
    i = mid - 1;
    while (i >= 0 && sorted_point->at(i)->point.x > sorted_point->at(mid)->point.x - range){
        auto delta_x = (sorted_point->at(i)->point.x-sorted_point->at(mid)->point.x);
        auto delta_y = (sorted_point->at(i)->point.y-sorted_point->at(mid)->point.y);
        if (delta_x * delta_x + delta_y * delta_y < range_squared){
            result.push(sorted_point->at(i)->index);
        }
        i--;
    }

    // return all the points
    return result;
}

vector<long> square_query_range(vector<Point_Unit *> * sorted_point, Point2f point, int range) {
    // binary search point
    long left, mid, right;
    left = 0;
    right = sorted_point->size() -1;
    mid = (left+right)/2;
    while (left < mid){
        if (sorted_point->at(mid)->point.x < point.x){
            left = mid;
        }else if (sorted_point->at(mid)->point.x > point.x){
            right = mid;
        }else{
            break;
        }
        mid = (left + right)/2;
    }
    square_query_range(sorted_point, mid, range);
}



///////////////////////////////////
// Algorithm
///////////////////////////////////

void density_based_spatial_clustering_of_application_with_noise(vector<Point_Unit *> * unsorted_points, vector<Point_Unit *> * sorted_point, int range, int density){
    for (auto & item : *unsorted_points){
        item->label = CLUSTER_UNDEFINED;
    }
    int cluster = 0;
    for (unsigned long i = 0; i < unsorted_points->size(); ++i){
        if (unsorted_points->at(i)->label != CLUSTER_UNDEFINED) {
            continue;
        }
        auto neighbours = RANGE_QUERY_FUNCTION(sorted_point, unsorted_points->at(i)->order, range);
        if (neighbours.size() < density){
            unsorted_points->at(i)->label = CLUSTER_NOISE;
            continue;
        }
        cluster ++;
        unsorted_points->at(i)->label = cluster;
        while (!neighbours.empty()){
            long item = neighbours.front();
            neighbours.pop();

            if (unsorted_points->at(item)->label == CLUSTER_NOISE){
                unsorted_points->at(item)->label = cluster;
                continue;
            }
            if (unsorted_points->at(item)->label != CLUSTER_UNDEFINED){
                continue;
            }
            unsorted_points->at(item)->label = cluster;
            auto recursive_neighbours = RANGE_QUERY_FUNCTION(sorted_point, unsorted_points->at(item)->order, range);
            if (recursive_neighbours.size() >= density){
                while (!recursive_neighbours.empty()){
                    neighbours.push(recursive_neighbours.front());
                    recursive_neighbours.pop();
                }
            }
        }

    }

}

void sort_points(vector<Point_Unit *> * unsorted_points, vector<Point_Unit *> * sorted_points){
    // Sort Points
    copy(unsorted_points->begin(), unsorted_points->end(), back_inserter(*sorted_points));
    sort(sorted_points->begin(), sorted_points->end(), [](Point_Unit * a, Point_Unit * b)->bool{
        return a->point.x < b->point.x;
    });
    int count = 0;
    for (const auto & item: *sorted_points){
        item->order = count ++;
        //cout << "(" << item->point.x << "," << item->point.y << ") "  << item->index << " - "  << item->order << endl;
    }

}

int src_range, src_density;

vector<Point_Unit *> points, sorted_points;

Mat display_image;

void update_image(int a, void * b){
    density_based_spatial_clustering_of_application_with_noise(&points, &sorted_points, src_range, src_density);
    display_image = display_points_with_label(&points);
    imshow("test", display_image);
}

int main() {

    load_data_set(&points);
    sort_points(&points, &sorted_points);

    src_range = 120;
    src_density = 30;

    // Configuration
    namedWindow("test", WINDOW_FREERATIO);

    // Display
    for(;;) {
        createTrackbar("src_range", "test", &src_range, 300, update_image);
        createTrackbar("src_density", "test", &src_density, 300, update_image);

        update_image(0, nullptr);

        if (waitKey(0) == 'q') break;
    }

    return 0;
}