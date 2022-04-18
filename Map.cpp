#include <iostream>
#include <utility>
#include <vector>
#include <map>
using namespace std;


class Map{

public:
    string cityName;
    map<pair<double,double>, string> locations; //This will hold the longitude and latitude locations of the places
    vector<double> distancesToPerson; //User will input their location and then we will calculate the distances to each location
    vector<string> nameOfLocations; //The two vectors will correspond to the same locations. Must be added to at the same time


    Map(string nameOfCity)//Constructor with name passed in
    {
        this->cityName = nameOfCity;
    }

};

/*
 * This method will pass in the longitude and latitude location of the user, and then return a vector of ints
 * that has the distances from each location in cityLocations. The vector that is returned should be the
 * same size of the vector that is passed in.
 */

vector<double> CalculateDistances(map<pair<double,double>, string>& cityLocations, double longitude, double latitude)
{
    //Reference : https://www.geeksforgeeks.org/program-distance-two-points-earth/

    //We want the return to be in miles, not KM....
}

//Might want a helper function for this actually



void HeapSort(vector<double>& distances, int n) //N is the number of elements in distances
{

}

void Heapify(vector<double> distances, int n, int i) //Reference : https://www.geeksforgeeks.org/heap-sort/
{

}

void QuickSort(vector<double> distances, int low, int high) //Reference : https://www.geeksforgeeks.org/quick-sort/
{

}

//Any other methods you need for quickSort

/*
 * The 4 methods listed above are what you need to implement, the rest of the project Cainan and I will handle
 *
 * Basically what we are going to do is parse through a very large data set that has locations inside them and store
 * the latitude and longitude locations into the map, the key will be the lat and long and the value will be the
 * name of the location. They will be sorted based on different amenities like food, churches, schools, etc.
 *
 * Upon opening the application the user will input their current location with their longitude and latitude.
 * These two numbers will be passed into the first function that I listed. You will then need to take those longitudes
 * and latitude locations and calculate the distance from the user's location to each point
 * in the map that is passed in and then store those distances away inside a vector that will be returned.
 *
 *
 * Upon doing that you will then create HeapSort and QuickSort functions that will be called on the
 * Vector that has the distances away from the user inside of it.
 * This vector will not be sorted at all so we want to sort these distances ascending, with the closest
 * locations coming first and then the furthest locations coming last
 *
 * I will provide a test map and a test vector of distances so you can see if your functions are working correctly
 */


int main()
{

    Map myTestMap("Gaineville"); //Don't need this

    //Testing values
    map<pair<double,double>, string> testLocations = {
            {{-82.3347117, 29.6885229}, "Chickfila"},
            {{-82.338716, 29.6738562}, "five guys"},
            {{-82.358686, 29.6335918}, "Moes"},
            {{-82.3771434, 29.6219949}, "Wendys"},
            {{-82.3389216, 29.6372585}, "BBQ"},
            {{-82.3723534, 29.6036513}, "Subway"}
    };

    double testUserLongitude = -82.349150;
    double testUserLatitude = 29.655120;

    myTestMap.locations = testLocations; //Don't even need this honestly


    //Just call your calculate distances methods and heapsort and quicksort.

}



