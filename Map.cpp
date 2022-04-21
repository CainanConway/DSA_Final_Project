#include <iostream>
#include <utility>
#include <vector>
#include <map>
#include <sstream>
#include <fstream>
#include <regex>
#include <cmath>
#include <time.h>
#include <stdio.h>
#define RADS_TO_MILES 3956
#define RADS_TO_KMS 6371
using namespace std;

// Location class based on .text file tags
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
    double pivot = distances.at(high).getDistanceToUser(); // Get end index of a user's distance
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
        // Pi is assigned to its correct location
        int pi = partition(distances, low, high);
        QuickSort(distances, low, pi - 1);
        QuickSort(distances, pi + 1, high);
    }
}



void parseTextFile(string fileName, vector<Location>& theCity)
{
    // File is read in
    ifstream inFile;

    inFile.open(fileName);

    if(inFile.is_open())
    {
        string inputText;

        while(getline(inFile, inputText)){
            // "POINT" is used to locate geographical points
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
                // Push back highway coordiantes
                if(inputText.find("amenity") == string::npos && inputText.find("name") == string::npos && inputText.find("highway") != string::npos)
                {
                    string tempTypeHighway = inputText.substr(inputText.find("highway") + 8 );
                    istringstream theHighway(tempTypeHighway);
                    string highway;
                    getline(theHighway, highway, ',');
                    Location tempLocation(longitude, latitude, "NULL", highway);
                    theCity.push_back(tempLocation);
                }

                // Push back amentity coordiantes
                if(inputText.find("amenity") != string::npos && inputText.find("name") == string::npos)
                {
                    string tempAmenity = inputText.substr(inputText.find("amenity") + 8);
                    istringstream theAmenity(tempAmenity);
                    string typeOfAmenity;
                    getline(theAmenity, typeOfAmenity, ',');
                    Location tempLocation(longitude, latitude, "NULL", typeOfAmenity);
                    theCity.push_back(tempLocation);
                }

                // Push back name of amentity
                if(inputText.find("amenity") != string::npos && inputText.find("name") != string::npos && inputText.find("fixme") == string::npos)
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

void OutputForHeap(vector<Location>& theLocations)
{
    int i = 1;
    for(int j = (int)theLocations.size()-1; j >= 0; j --)
    {
        cout << i << ". ";
        cout << "Name: " << theLocations[j].getName() << " Distance from you: " << theLocations[j].getDistanceToUser() << endl;
        i++;
    }
}

void OutputForQuick(vector<Location>& theLocations)
{
    int i = 1;
    for(const auto& spot: theLocations)
    {
        cout << i << ". ";
        cout << "Name: " << spot.getName() << " Distance from you: " << spot.getDistanceToUser() << endl;
        i++;
    }
}

int main()
{
    // A map for each text file
    Map GainesvilleMap("Gainesville"); 
    Map NewYorkMap("New York");

    // Parse given text files
    parseTextFile("NewGainesville.text", GainesvilleMap.theLocations);
    parseTextFile("New_York.text", NewYorkMap.theLocations);

    int numOfTimes = 0;

    // Menu displayed to user
    cout << "Welcome to MapForYou.io!\n" << endl;

    cout << "Please enter your longitude and latitude obtained from Google Maps" <<
    " right-click on your location to display the longitude and latitude!\n" << endl;

    system("open https://www.google.com/maps"); // User is redirected to google maps

    bool runProgram = true;

    while(runProgram)
    {
        double longitude, latitude;
        // Vectors for each possible amenitity
        vector<Location> fastFoodHeap, fastFoodQuick;                      
        vector<Location> restaurantsHeap, restaurantsQuick;                 
        vector<Location> placesOfWorshipHeap, placesOfWorshipQuick;         
        vector<Location> schoolsHeap, schoolsQuick;                         
        vector<Location> firstResponderHeap, firstResponderQuick;           
        clock_t theClock;
        double timeTaken;
        int afterOne;
        int userInput, userInput2;

        // Secondary menu shown to user
        if(numOfTimes > 0)
        {
            cout << endl;
            cout << "Would you like to re-enter your location?" << endl;
            cout << "1. Yes" << endl;
            cout << "2. No use same location." << endl;
            cout << "3. Exit Program" << endl;

            cin >> afterOne;
            if(afterOne == 1)
            {
                cout << "Enter longitude: ";
                cin >> longitude;

                cout << "Enter latitude: ";
                cin >> latitude;

                cout << "Which city would you like to know locations near you for?" << endl;

                cout <<"1. Gainesville" << endl;
                cout << "2. New York City" << endl;

                cin >> userInput;
            }
            else if(afterOne == 2)
            {
                cout << "Which city would you like to know locations near you for?" << endl;

                cout <<"1. Gainesville" << endl;
                cout << "2. New York City" << endl;

                cin >> userInput;
            }
            else if(afterOne == 3)
            {
                userInput = 3;
            }
        }

        if(numOfTimes == 0)
        {
            cout << "Enter longitude: ";
            cin >> longitude;

            cout << "Enter latitude: ";
            cin >> latitude;

            cout << "Which city would you like to know locations near you for?" << endl;

            cout <<"1. Gainesville" << endl;
            cout << "2. New York City" << endl;
            cout << "3. End Program" << endl;

            cin>>userInput;
        }

        // Inital menu shown to user
        switch (userInput) {
            case 1: // User chooses Gainesville
                numOfTimes++;
                CalculateDistances(GainesvilleMap.theLocations, longitude, latitude);
                cout << "What type of amenity are you looking for close to you?" << endl;
                cout << "1. Fast food" << endl;
                cout << "2. Restaurants" << endl;
                cout << "3. Places of Worship" << endl;
                cout << "4. Schools" << endl;
                cout << "5. First responder offices" << endl;

                cin >> userInput2;

                switch(userInput2){
                    case 1:  // Fast Food in Gainesville
                        fastFoodQuick.clear();
                        fastFoodHeap.clear();
                        for(const auto& location : GainesvilleMap.theLocations)
                        {
                           if(location.getAmenity() == "fast_food" && location.getName() != "NULL")
                           {
                               fastFoodHeap.push_back(location);
                               fastFoodQuick.push_back(location);
                           }
                        }
                       cout << "Sorting with Heap sort..." << endl;


                       theClock = clock();
                       HeapSort(fastFoodHeap, (int)fastFoodHeap.size());
                       theClock = clock() - theClock;
                       timeTaken = ((double)theClock) / CLOCKS_PER_SEC;

                       OutputForHeap(fastFoodHeap);

                       cout << "Heap sort took: " << timeTaken << "seconds!" << endl; // Show total time taken for Heap sort
                       cout << "--------------------\n" << endl;

                       cout << "Sorting with Quick sort..." << endl;

                       theClock = clock();
                       QuickSort(fastFoodQuick, 0, (int) (fastFoodQuick.size() - 1));
                       theClock = clock() - theClock;
                       timeTaken = ((double)theClock) / CLOCKS_PER_SEC;

                       OutputForQuick(fastFoodQuick);
                       cout << "Quick sort took: " << timeTaken << "seconds!" << endl; // Show total time taken for Quick sort

                       break;

                    case 2: // Resturants in Gainesville
                        restaurantsHeap.clear();
                        restaurantsQuick.clear();
                        for(const auto& location : GainesvilleMap.theLocations)
                        {
                            if(location.getAmenity() == "restaurant" && location.getName() != "NULL")
                            {
                                restaurantsHeap.push_back(location);
                                restaurantsQuick.push_back(location);
                            }
                        }

                        cout << "Sorting with Heap sort..." << endl;


                        theClock = clock();
                        HeapSort(restaurantsHeap, (int)restaurantsHeap.size());
                        theClock = clock() - theClock;
                        timeTaken = ((double)theClock) / CLOCKS_PER_SEC;

                        OutputForHeap(restaurantsHeap);
                        cout << "Heap sort took: " << timeTaken << "seconds!" << endl; // Show total time taken for Heap sort
                        cout << "--------------------\n" << endl;

                        cout << "Sorting with Quick sort..." << endl;

                        theClock = clock();
                        QuickSort(restaurantsQuick, 0, (int) (restaurantsQuick.size() - 1));
                        theClock = clock() - theClock;
                        timeTaken = ((double)theClock) / CLOCKS_PER_SEC;

                        OutputForQuick(restaurantsQuick);
                        cout << "Quick sort took: " << timeTaken << "seconds!" << endl; // Show total time taken for Quick sort

                        break;

                    case 3: // Places of worship in Gainesville
                        placesOfWorshipQuick.clear();
                        placesOfWorshipHeap.clear();
                        for(const auto& location : GainesvilleMap.theLocations)
                        {
                            if(location.getAmenity() == "place_of_worship" && location.getName() != "NULL")
                            {
                                placesOfWorshipHeap.push_back(location);
                                placesOfWorshipQuick.push_back(location);
                            }
                        }

                        cout << "Sorting with Heap sort..." << endl;


                        theClock = clock();
                        HeapSort(placesOfWorshipHeap, (int)placesOfWorshipHeap.size());
                        theClock = clock() - theClock;
                        timeTaken = ((double)theClock) / CLOCKS_PER_SEC;

                        OutputForHeap(placesOfWorshipHeap);
                        cout << "Heap sort took: " << timeTaken << "seconds!" << endl; // Show total time taken for Heap sort
                        cout << "--------------------\n" << endl;

                        cout << "Sorting with Quick sort..." << endl;

                        theClock = clock();
                        QuickSort(placesOfWorshipQuick, 0, (int) (placesOfWorshipQuick.size() - 1));
                        theClock = clock() - theClock;
                        timeTaken = ((double)theClock) / CLOCKS_PER_SEC;

                        OutputForQuick(placesOfWorshipQuick);
                        cout << "Quick sort took: " << timeTaken << "seconds!" << endl; // Show total time taken for Quick sort
                        break;

                    case 4: // Schools in Gainesville
                        schoolsQuick.clear();
                        schoolsHeap.clear();
                        for(const auto& location : GainesvilleMap.theLocations)
                        {
                            if(location.getAmenity() == "school" && location.getName() != "NULL")
                            {
                                schoolsHeap.push_back(location);
                                schoolsQuick.push_back(location);
                            }
                        }

                        cout << "Sorting with Heap sort..." << endl;


                        theClock = clock();
                        HeapSort(schoolsHeap, (int)schoolsHeap.size());
                        theClock = clock() - theClock;
                        timeTaken = ((double)theClock) / CLOCKS_PER_SEC;

                        OutputForHeap(schoolsHeap);
                        cout << "Heap sort took: " << timeTaken << "seconds!" << endl; // Show total time taken for Heap sort
                        cout << "--------------------\n" << endl;

                        cout << "Sorting with Quick sort..." << endl;

                        theClock = clock();
                        QuickSort(schoolsQuick, 0, (int) (schoolsQuick.size() - 1));
                        theClock = clock() - theClock;
                        timeTaken = ((double)theClock) / CLOCKS_PER_SEC;

                        OutputForQuick(schoolsQuick);
                        cout << "Quick sort took: " << timeTaken << "seconds!" << endl; // Show total time taken for Quick sort
                        break;

                    case 5: // First responders in Gainesville
                        firstResponderQuick.clear();
                        firstResponderHeap.clear();
                        for(const auto& location : GainesvilleMap.theLocations)
                        {
                            if(location.getAmenity() == "fire_station" || location.getAmenity() == "police" && location.getName() != "NULL")
                            {
                                firstResponderHeap.push_back(location);
                                firstResponderQuick.push_back(location);
                            }
                        }

                        cout << "Sorting with Heap sort..." << endl;


                        theClock = clock();
                        HeapSort(firstResponderHeap, (int)firstResponderHeap.size());
                        theClock = clock() - theClock;
                        timeTaken = ((double)theClock) / CLOCKS_PER_SEC;

                        OutputForHeap(firstResponderHeap);
                        cout << "Heap sort took: " << timeTaken << "seconds!" << endl; // Show total time taken for Heap sort
                        cout << "--------------------\n" << endl;

                        cout << "Sorting with Quick sort..." << endl;

                        theClock = clock();
                        QuickSort(firstResponderQuick, 0, (int) (firstResponderQuick.size() - 1));
                        theClock = clock() - theClock;
                        timeTaken = ((double)theClock) / CLOCKS_PER_SEC;

                        OutputForQuick(firstResponderQuick);
                        cout << "Quick sort took: " << timeTaken << "seconds!" << endl; // Show total time taken for Quick sort
                        break;

                    default:
                        cout <<"Invalid Input!" << endl;
                        break;
                }
                break;

            case 2: // User chooses New York
                numOfTimes++;
                CalculateDistances(NewYorkMap.theLocations, longitude, latitude);
                cout << "What type of amenity are you looking for close to you?" << endl;
                cout << "1. Fast food" << endl;
                cout << "2. Restaurants" << endl;
                cout << "3. Places of Worship" << endl;
                cout << "4. Schools" << endl;
                cout << "5. First responder offices" << endl;

                cin >> userInput2;

                switch(userInput2){
                    case 1: // Fast food in New York
                        fastFoodQuick.clear();
                        fastFoodHeap.clear();
                        for(const auto& location : NewYorkMap.theLocations)
                        {
                            if(location.getAmenity() == "fast_food" && location.getName() != "NULL")
                            {
                                fastFoodHeap.push_back(location);
                                fastFoodQuick.push_back(location);
                            }
                        }
                        cout << "Sorting with Heap sort..." << endl;


                        theClock = clock();
                        HeapSort(fastFoodHeap, (int)fastFoodHeap.size());
                        theClock = clock() - theClock;
                        timeTaken = ((double)theClock) / CLOCKS_PER_SEC;

                        OutputForHeap(fastFoodHeap);

                        cout << "Heap sort took: " << timeTaken << "seconds!" << endl; // Show total time taken for Heap sort
                        cout << "--------------------\n" << endl;

                        cout << "Sorting with Quick sort..." << endl;

                        theClock = clock();
                        QuickSort(fastFoodQuick, 0, (int) (fastFoodQuick.size() - 1));
                        theClock = clock() - theClock;
                        timeTaken = ((double)theClock) / CLOCKS_PER_SEC;

                        OutputForQuick(fastFoodQuick);
                        cout << "Quick sort took: " << timeTaken << "seconds!" << endl; // Show total time taken for Quick sort

                        break;

                    case 2: // Resturants in New york
                        restaurantsHeap.clear();
                        restaurantsQuick.clear();
                        for(const auto& location : NewYorkMap.theLocations)
                        {
                            if(location.getAmenity() == "restaurant" && location.getName() != "NULL")
                            {
                                restaurantsHeap.push_back(location);
                                restaurantsQuick.push_back(location);
                            }
                        }

                        cout << "Sorting with Heap sort..." << endl;


                        theClock = clock();
                        HeapSort(restaurantsHeap, (int)restaurantsHeap.size());
                        theClock = clock() - theClock;
                        timeTaken = ((double)theClock) / CLOCKS_PER_SEC;

                        OutputForHeap(restaurantsHeap);
                        cout << "Heap sort took: " << timeTaken << "seconds!" << endl; // Show total time taken for Heap sort
                        cout << "--------------------\n" << endl;

                        cout << "Sorting with Quick sort..." << endl;

                        theClock = clock();
                        QuickSort(restaurantsQuick, 0, (int) (restaurantsQuick.size() - 1));
                        theClock = clock() - theClock;
                        timeTaken = ((double)theClock) / CLOCKS_PER_SEC;

                        OutputForQuick(restaurantsQuick);
                        cout << "Quick sort took: " << timeTaken << "seconds!" << endl; // Show total time taken for Quick sort

                        break;

                    case 3: // Places of worship in New york
                        placesOfWorshipQuick.clear();
                        placesOfWorshipHeap.clear();
                        for(const auto& location : NewYorkMap.theLocations)
                        {
                            if(location.getAmenity() == "place_of_worship" && location.getName() != "NULL")
                            {
                                placesOfWorshipHeap.push_back(location);
                                placesOfWorshipQuick.push_back(location);
                            }
                        }

                        cout << "Sorting with Heap sort..." << endl;


                        theClock = clock();
                        HeapSort(placesOfWorshipHeap, (int)placesOfWorshipHeap.size());
                        theClock = clock() - theClock;
                        timeTaken = ((double)theClock) / CLOCKS_PER_SEC;

                        OutputForHeap(placesOfWorshipHeap);
                        cout << "Heap sort took: " << timeTaken << "seconds!" << endl; // Show total time taken for Heap sort
                        cout << "--------------------\n" << endl;

                        cout << "Sorting with Quick sort..." << endl;

                        theClock = clock();
                        QuickSort(placesOfWorshipQuick, 0, (int) (placesOfWorshipQuick.size() - 1));
                        theClock = clock() - theClock;
                        timeTaken = ((double)theClock) / CLOCKS_PER_SEC;

                        OutputForQuick(placesOfWorshipQuick);
                        cout << "Quick sort took: " << timeTaken << "seconds!" << endl; // Show total time taken for Quick sort
                        break;

                    case 4: // Schools in New york
                        schoolsQuick.clear();
                        schoolsHeap.clear();
                        for(const auto& location : NewYorkMap.theLocations)
                        {
                            if(location.getAmenity() == "school" && location.getName() != "NULL")
                            {
                                schoolsHeap.push_back(location);
                                schoolsQuick.push_back(location);
                            }
                        }

                        cout << "Sorting with Heap sort..." << endl;


                        theClock = clock();
                        HeapSort(schoolsHeap, (int)schoolsHeap.size());
                        theClock = clock() - theClock;
                        timeTaken = ((double)theClock) / CLOCKS_PER_SEC;

                        OutputForHeap(schoolsHeap);
                        cout << "Heap sort took: " << timeTaken << "seconds!" << endl; // Show total time taken for Heap sort
                        cout << "--------------------" << endl;

                        cout << "Sorting with Quick sort..." << endl;

                        theClock = clock();
                        QuickSort(schoolsQuick, 0, (int) (schoolsQuick.size() - 1));
                        theClock = clock() - theClock;
                        timeTaken = ((double)theClock) / CLOCKS_PER_SEC;

                        OutputForQuick(schoolsQuick);
                        cout << "Quick sort took: " << timeTaken << "seconds!" << endl; // Show total time taken for Quick sort
                        break;

                    case 5: // First Responders in New York
                        firstResponderQuick.clear();
                        firstResponderHeap.clear();
                        for(const auto& location : NewYorkMap.theLocations)
                        {
                            if(location.getAmenity() == "fire_station" || location.getAmenity() == "police" && location.getName() != "NULL")
                            {
                                firstResponderHeap.push_back(location);
                                firstResponderQuick.push_back(location);
                            }
                        }

                        cout << "Sorting with Heap sort..." << endl;


                        theClock = clock();
                        HeapSort(firstResponderHeap, (int)firstResponderHeap.size());
                        theClock = clock() - theClock;
                        timeTaken = ((double)theClock) / CLOCKS_PER_SEC;

                        OutputForHeap(firstResponderHeap);
                        cout << "Heap sort took: " << timeTaken << "seconds!" << endl; // Show total time taken for Heap sort
                        cout << "--------------------\n" << endl;

                        cout << "Sorting with Quick sort..." << endl;

                        theClock = clock();
                        QuickSort(firstResponderQuick, 0, (int) (firstResponderQuick.size() - 1));
                        theClock = clock() - theClock;
                        timeTaken = ((double)theClock) / CLOCKS_PER_SEC;

                        OutputForQuick(firstResponderQuick);
                        cout << "Quick sort took: " << timeTaken << "seconds!" << endl; // Show total time taken for Quick sort
                        break;

                    default:
                        cout <<"Invalid Input!" << endl;
                        break;
                }
                break;

            case 3:
                runProgram = false;
                break;

            default:
                numOfTimes++;
                cout << "Invalid Input!" << endl;
                break;
        }

    }

    
}



