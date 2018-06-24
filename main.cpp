#include "main.hpp"



void generate_random_points(vector<Point2f> * point_list){

}

void load_data_set(vector<Point2f> * point_list, vector<int> * labels){
    ifstream sample_file("/home/mc/Desktop/container/1/currenttime-11-32-36--224.png.data.txt");
    string buffer;

    while (sample_file >> buffer){
        if (buffer == "points") break;
    }

    int x, y;
    while (sample_file >> x >> y){
        point_list->push_back(Point2f(x,y));
        labels->push_back(0);
    }
}

Mat display_points_with_label(vector<Point2f> * point_list, vector<int> * labels){
    int max_width, max_height;
    max_width = max_height = 0;
    for (const auto & item: *point_list){
        if (item.x > max_width){
            max_width = (int)item.x;
        }
        if (item.y > max_height){
            max_height = (int)item.y;
        }
    }
    cout << max_height << "x" << max_width << endl;

    Mat image(max_height, max_width, CV_8UC3);
    image = Scalar::all(0);

    set<int> label_set;
    for (const auto & item: *labels){
        label_set.insert(item);
    }

    map<int,Scalar> random_color_list;
    for (const auto & item: label_set){
        random_color_list[item] = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
    }

    for (auto i = 0; i < point_list->size(); ++i){
        circle(image, point_list->at(i), 5, random_color_list[labels->at(i)], 2);
    }

    return image;
}


vector<int> square_query_range(vector<Point2f> * sorted_point_list, Point2f point, int range){
    // binary search point
    unsigned long left, mid, right;
    left = 0;
    right = sorted_point_list->size() -1;
    mid = (left+right)/2;
    while (left < mid){
        if (sorted_point_list->at(mid).x < point.x){
            left = mid;
        }else if (sorted_point_list->at(mid).x > point.x){
            right = mid;
        }else{
            break;
        }
        mid = (left + right)/2;
    }

    // search left and right to see if the point falls in range
    //vector<Point2f> result;
    vector<int> result;
    auto i = mid;
    while (sorted_point_list->at(i).x < point.x + range){
        if ((sorted_point_list->at(i).y < point.y + range) && (sorted_point_list->at(i).y > point.y - range)){
            //result.push_back(sorted_point_list->at(i));
            result.push_back(i);
        }
        i++;
    }
    i = mid;
    while (sorted_point_list->at(i).x > point.x - range){
        if ((sorted_point_list->at(i).y < point.y + range) && (sorted_point_list->at(i).y > point.y - range)){
            //result.push_back(sorted_point_list->at(i));
            result.push_back(i);
        }
        i--;
    }

    // return all the points
    return result;
}


int main() {
    vector<Point2f> points;
    vector<int> labels;

    load_data_set(&points, &labels);
    sort(points.begin(), points.end(), [](Point2f a, Point2f b)->bool{
        return a.x < b.x;
    });

    for (const auto & item: points){
        cout << "(" << item.x << "," << item.y << ")" << endl;
    }

    unsigned long target = 120;
    auto neighbour_points = square_query_range(&points, points.at(target), 50);
    cout << "neighbour points" << neighbour_points.size() << endl;
    for (const auto & item: neighbour_points){
        labels.at(item) = 23333;
    }
    labels.at(target) = 888;

    auto display_image = display_points_with_label(&points, &labels);

    // Configuration
    namedWindow("test", WINDOW_FREERATIO);

    // Display
    imshow("test", display_image);


    waitKey(0);

    return 0;
}