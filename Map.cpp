#include <iostream>
#include <utility>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>
#include <regex>
#include <cmath>
#define RADS_TO_MILES 3956
#define RADS_TO_KMS 6371
using namespace std;


class Location{
public:

    double longitude;
    double latitude;
    string name;
    string amenity;
    double distanceToUser;

    Location(double longit, double lat, string nam, string amen)
    {
        longitude = longit;
        latitude = lat;
        name = nam;
        amenity = amen;
        distanceToUser = 0;
    }

    double getLongitude() const
    {
        return longitude;
    }

    double getLatitude() const
    {
        return latitude;
    }

    string getName() const
    {
        return name;
    }

    string getAmenity() const
    {
        return amenity;
    }

    double getDistanceToUser() const
    {
        return distanceToUser;
    }

    void setDistanceToUser(double distance)
    {
        this->distanceToUser = distance;
    }
};

class Map{

public:
    string cityName;
    vector<Location> theLocations; //Could do this instead...
    vector<double> distancesToPerson; //User will input their location and then we will calculate the distances to each location//The two vectors will correspond to the same locations. Must be added to at the same time

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


//Each location has an associated name, longitude and latitude
vector<double> CalculateDistances(vector<Location>& cityLocations, double userLongitude, double userLatitude)
{
    //Reference : https://www.geeksforgeeks.org/program-distance-two-points-earth/

    //We want the return to be in miles, not KM....
    //convert the user's position into radians
    double lat1 = (((M_PI) / 180) * userLatitude);
    double long1 = (((M_PI) / 180) * userLongitude);

    vector<double> distances;
    for(int i = 0; i < cityLocations.size(); i++)
    {
        //convert the location in position i into radians
        double lat2 = (((M_PI) / 180) * cityLocations.at(i).getLatitude());
        double long2 =  (((M_PI) / 180) * cityLocations.at(i).getLongitude());
        //find the difference in lat and long between the two locations
        double lat = lat2 - lat1;
        double longi = long2 - long1;
        //Haversine formula, convert to miles
        distances.push_back(RADS_TO_MILES * (2 * (asin(sqrt(pow(sin(lat / 2), 2) + cos(lat1) * cos(lat2) * pow(sin(longi / 2), 2))))));
        cityLocations.at(i).setDistanceToUser(distances.at(i));
    }
    return distances;//currently the function both modifies cityLocations and returns a new vector.  To disable the latter, delete this line.
}

//Might want a helper function for this actually



void Heapify(vector<Location>& distances, int n, int i) //Reference : https://www.geeksforgeeks.org/heap-sort/
{
    int smallest = i; //initalize smallest as root
    int left = (2 * i) + 1; // left child of root
    int right = (2 * i) + 2; //right child of root

    //if left child is smaller
    if(left < n && distances.at(left).getDistanceToUser() < distances.at(smallest).getDistanceToUser())
    {
        smallest = left;
    }
    //if right child is smaller
    if(right < n && distances.at(right).getDistanceToUser() < distances.at(smallest).getDistanceToUser())
    {
        smallest = right;
    }
    //if smallest is not root
    if(smallest != i)
    {
        swap(distances.at(i), distances.at(smallest));
        Heapify(distances, n, smallest);
    }
}

void HeapSort(vector<Location>& distances, int n) //N is the number of elements in distances
{
    //build heap
    for(int i = (n / 2) - 1; i >= 0; i--)
    {
        Heapify(distances, n, i);
    }
    //extract elements from heap
    for(int i = n - 1; i > 0; i--)
    {
        //move root to end
        swap(distances.at(0), distances.at(i));
        //heapify reduced heap
        Heapify(distances, i, 0);
    }
}

int partition(vector<Location>& distances, int low, int high)
{
    double pivot = distances.at(high).getDistanceToUser();
    int i = (low - 1);

    for(int j = low; j <= high - 1; j++)
    {
        if (distances.at(j).getDistanceToUser() < pivot)
        {
            i++; // increment index of smaller element
            swap(distances.at(i), distances.at(j));
        }
    }
    swap(distances.at(i + 1), distances.at(high));
    return (i + 1);
}

void QuickSort(vector<Location>& distances, int low, int high) //Reference : https://www.geeksforgeeks.org/quick-sort/
{
    if (low < high)
    {
        int pi = partition(distances, low, high);
        QuickSort(distances, low, pi - 1);
        QuickSort(distances, pi + 1, high);
    }
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



void parseTextFile(string fileName, vector<Location>& theCity)
{

    ifstream inFile;

    inFile.open(fileName);

    if(inFile.is_open())
    {
        string inputText;

        while(getline(inFile, inputText)){

            if(inputText.substr(0,5) == "POINT")
            {
                string points = inputText.substr(inputText.find('(') + 1, (inputText.find(')') - inputText.find('(')-1));
                istringstream temp(points);

                string tempPoints;
                vector<string> longAndLat;

                while(getline(temp,tempPoints, ' '))
                {
                    longAndLat.push_back(tempPoints);
                }

                double longitude = stod(longAndLat[0]);
                double latitude = stod(longAndLat[1]);

                if(inputText.find("amenity") == string::npos && inputText.find("name") == string::npos && inputText.find("highway") != string::npos)
                {
                    string tempTypeHighway = inputText.substr(inputText.find("highway") + 8 );
                    istringstream theHighway(tempTypeHighway);
                    string highway;
                    getline(theHighway, highway, ',');
                    Location tempLocation(longitude, latitude, "NULL", highway);
                    theCity.push_back(tempLocation);
                }


                if(inputText.find("amenity") != string::npos && inputText.find("name") == string::npos)
                {
                    string tempAmenity = inputText.substr(inputText.find("amenity") + 8);
                    istringstream theAmenity(tempAmenity);
                    string typeOfAmenity;
                    getline(theAmenity, typeOfAmenity, ',');
                    Location tempLocation(longitude, latitude, "NULL", typeOfAmenity);
                    theCity.push_back(tempLocation);
                }


                if(inputText.find("amenity") != string::npos && inputText.find("name") != string::npos)
                {
                    //string typeOfAmenity = inputText.substr(inputText.find("amenity") + 8 , inputText.find(',') - inputText.find("amenity") - 8 );

                    string tempAmenity = inputText.substr(inputText.find("amenity") + 8);
                    string tempName = inputText.substr(inputText.find("name") + 5);

                    istringstream theAmenity(tempAmenity);
                    string typeOfAmenity;
                    getline(theAmenity, typeOfAmenity, ',');

                    istringstream theName(tempName);
                    string name;
                    getline(theName, name, ',');

                    string removeString = "%20%";
                    string replaceString = " ";

                    size_t position = name.find(removeString);


                    while(position != string::npos)
                    {
                        name.replace(position, removeString.size(), replaceString);
                        position = name.find(removeString, position + replaceString.size());
                    }

                    Location tempLocation(longitude, latitude, name, typeOfAmenity);
                    theCity.push_back(tempLocation);
                }
            }
        }
        inFile.close();
    }
}


int main()
{

    Map GainesvilleMap("Gainesville");
    Map OrlandoMap("Orlando");


    parseTextFile("Gainesville.text", GainesvilleMap.theLocations);
    parseTextFile("Orlando.text", OrlandoMap.theLocations);

    double testUserLongitude = -82.349150;
    double testUserLatitude = 29.655120;


    int n = (int) (GainesvilleMap.theLocations.size());

    CalculateDistances(GainesvilleMap.theLocations, testUserLongitude, testUserLatitude);

    QuickSort(GainesvilleMap.theLocations, 0, n-1);

    HeapSort(GainesvilleMap.theLocations, n);

    QuickSort(GainesvilleMap.theLocations, GainesvilleMap.theLocations[0].getDistanceToUser(), GainesvilleMap.theLocations[n-1].getDistanceToUser());







    //Just call your calculate distances methods and heapsort and quicksort.

}



