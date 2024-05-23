#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <memory>
#include <algorithm>

using namespace std;

class Passenger;
class Stop;

class Stop {
private:
    string name;
    mutable vector<Passenger*> passengers;

public:
    Stop(const string& n) : name(n) {}

    string getName() const {
        return name;
    }

    const vector<Passenger*>& getPassengers() const {
        return passengers;
    }

    void addPassenger(Passenger* passenger) const{
        passengers.push_back(passenger);
    }

    void removePassenger(Passenger* passenger) const {
        auto new_end = std::remove(passengers.begin(), passengers.end(), passenger);
        passengers.erase(new_end, passengers.end());
    }
};

class Passenger {
private:
    string name;
    const Stop* current_stop;         // ??????????? const Stop* ?????
    const Stop* destination_stop;     // ? ?????
    bool picked_by_taxi;

public:
    Passenger(const string& n, const Stop* current, const Stop* destination)
        : name(n), current_stop(current), destination_stop(destination), picked_by_taxi(false) {
        // ????????? ????????? ?? ??????? ?????????
        current->addPassenger(this);
    }

    string getName() const {
        return name;
    }

    const Stop* getCurrentStop() const {
        return current_stop;
    }

    const Stop* getDestinationStop() const {
        return destination_stop;
    }

    bool isPickedByTaxi() const {
        return picked_by_taxi;
    }

    void setPickedByTaxi(bool picked) {
        picked_by_taxi = picked;
    }
};

class Taxi {
private:
    string number;
    queue<const Stop*> route;
    const Stop* current_stop;
    int capacity;
    //vector<unique_ptr<Passenger>> passengers;
    vector<Passenger*> passengers;

public:
    Taxi(const string& num, const Stop* start, int cap) : number(num), current_stop(start), capacity(cap) {}

    ~Taxi() {
        while (!route.empty()) {
            route.pop();
        }
    }

    void addStop(const Stop* stop) {
        route.push(stop);
    }

    void ride() {
        if (!route.empty()) {
            current_stop = route.front();
            route.pop();

            // ??????? ?????????? ? ?????????? ?? ??????? ????????? ??? ???????? ?????
            cout << "Passengers at stop " << current_stop->getName() << ":" << endl;
            const vector<Passenger*>& stopPassengers = current_stop->getPassengers();
            vector<Passenger*> mutableStopPassengers;
            mutableStopPassengers.assign(stopPassengers.begin(), stopPassengers.end());

            if (mutableStopPassengers.empty()) {
                cout << "No passengers waiting at this stop." << endl;
            } else {
                for (const auto& passenger : mutableStopPassengers) {
                    cout << "  - " << passenger->getName() << endl;
                }
            }

            cout << "Free seats in taxi: " << (capacity - passengers.size()) << endl;


            for (auto it = mutableStopPassengers.begin(); it != mutableStopPassengers.end(); ++it) {
                if (passengers.size() < capacity) {
                    cout << "Passenger " << (*it)->getName() << " boarded the taxi." << endl;
                    passengers.push_back(*it);
                    it = mutableStopPassengers.erase(it);
                    // ????????? ????????, ??? ??? erase() ?????? ??????? ? ??????? ?????????
                    --it;
                } else {
                    break; // ??????? ?? ?????, ???? ? ????? ?????? ??? ????
                }
            }
        }
    }


    void addPassenger(unique_ptr<Passenger> passenger) {
        if (passengers.size() < capacity) {
            if (passenger->getCurrentStop() == current_stop) {
                cout << "Adding passenger " << passenger->getName() << " to the taxi." << endl;
                passengers.push_back(passenger.release());
                // release() ???????? ???????? ?????????? ??? ???????????? ??????
                passenger->setPickedByTaxi(true);
            } else {
                cout << "Passenger " << passenger->getName() << " is not at the current stop of the taxi." << endl;
            }
        } else {
            cout << "Taxi is already full. Cannot add passenger " << passenger->getName() << "." << endl;
        }
    }


    void dropOffPassengers() {
        auto new_end = std::remove_if(passengers.begin(), passengers.end(), [&](const auto& passenger) {
            if (passenger->getDestinationStop() == current_stop) {
                cout << "Passenger " << passenger->getName() << " has been dropped off." << endl;
                return true;
            }
            return false;
        });

        passengers.erase(new_end, passengers.end());
    }

    string getNumber() const {
        return number;
    }

    const Stop* getCurrentStop() const {
        return current_stop;
    }

    int getCapacity() const {
        return capacity;
    }

    void setCurrentStop(const Stop* stop) {
        current_stop = stop;
    }

    const vector<Passenger*>& getPassengers() const {
        return passengers;
    }

    bool hasPassengers() const {
        return !passengers.empty();
    }

    queue<const Stop*> getRoute() const {
        return route;
    }

    void pickupPassengers() {
        if (passengers.size() < capacity) {
            auto& stopPassengers = current_stop->getPassengers();
            cout << "Passengers at stopp " << current_stop->getName() << ":" << endl;
            cout << "Checking conditions: stopPassengers not empty: " << !stopPassengers.empty() << ", capacity available: " << (capacity - passengers.size()) << endl;
            for (const auto& passenger : stopPassengers) {
                cout << "  - " << passenger->getName() << endl;
            }
            while (!stopPassengers.empty() && passengers.size() < capacity) {
                Passenger* passenger = stopPassengers.front();
                current_stop->removePassenger(passenger);
                passenger->setPickedByTaxi(true);
                passengers.push_back(passenger);
                cout << "Passenger " << passenger->getName() << " boarded the taxi." << endl;
            }
        }
    }



};

int main() {
    vector<unique_ptr<Stop>> stops;
    vector<unique_ptr<Taxi>> taxis;
    vector<unique_ptr<Passenger>> passengers;

    while (true) {
        cout << "1) Print list of taxis" << endl;
        cout << "2) Print list of passengers" << endl;
        cout << "3) Print list of stops" << endl;
        cout << "4) Add a taxi" << endl;
        cout << "5) Add a passenger" << endl;
        cout << "6) Add a stop" << endl;
        cout << "7) Update taxi route" << endl;
        cout << "8) Move taxi to next stop" << endl;
        cout << "9) Exit" << endl;
        cout << "Enter your choice: ";

        int choice;
        cin >> choice;

        switch (choice) {
        case 1:
            cout << "List of Taxis:" << endl;
            for (const auto& taxi : taxis) {
                cout << "Taxi " << taxi->getNumber() << ":" << endl;
                cout << "  Passengers inside:" << endl;
                if (!taxi->hasPassengers()) {
                    cout << "    None" << endl;
                } else {
                    for (const auto& passenger : taxi->getPassengers()) {
                        cout << "    - " << passenger->getName() << endl;
                    }
                }
                cout << "  Current stop: " << taxi->getCurrentStop()->getName() << endl;
                cout << "  Route: ";
                if (taxi->getRoute().empty()) {
                    cout << "Empty" << endl;
                } else {
                    queue<const Stop*> tempRoute = taxi->getRoute();
                    while (!tempRoute.empty()) {
                        cout << tempRoute.front()->getName() << " ";
                        tempRoute.pop();
                    }
                    cout << endl;
                }
            }
            break;
        case 2:
            cout << "List of Passengers:" << endl;
            for (const auto& passenger : passengers) {
                cout << "Passenger " << passenger->getName() << ":" << endl;
                cout << "  Current stop: " << passenger->getCurrentStop()->getName() << endl;
                cout << "  Destination stop: " << passenger->getDestinationStop()->getName() << endl;
            }
            break;
        case 3:
            cout << "List of Stops:" << endl;
            for (const auto& stop : stops) {
                cout << stop->getName() << endl;
            }
            break;
        case 4:
        {
            string num;
            int cap;
            cout << "Enter taxi number: ";
            cin >> num;
            cout << "Enter taxi capacity: ";
            cin >> cap;

            cout << "Enter current stop name for the taxi: ";
            string curr_stop_name;
            cin >> curr_stop_name;
            const Stop* current_stop = nullptr;
            for (const auto& stop : stops) {
                if (stop->getName() == curr_stop_name) {
                    current_stop = stop.get();
                    break;
                }
            }
            if (current_stop == nullptr) {
                auto currentStopPtr = make_unique<Stop>(curr_stop_name);
                current_stop = currentStopPtr.get();
                stops.push_back(std::move(currentStopPtr));
            }

            auto newTaxi = make_unique<Taxi>(num, current_stop, cap);
            taxis.push_back(std::move(newTaxi));
        }
        break;
        case 5:
        {
            string name;
            string current_stop_name;
            string destination_stop_name;
            cout << "Enter passenger name: ";
            cin >> name;
            cout << "Enter current stop: ";
            cin >> current_stop_name;
            cout << "Enter destination stop: ";
            cin >> destination_stop_name;

            const Stop* current_stop = nullptr;
            const Stop* destination_stop = nullptr;
            for (const auto& stop : stops) {
                if (stop->getName() == current_stop_name)
                    current_stop = stop.get();
                if (stop->getName() == destination_stop_name)
                    destination_stop = stop.get();
            }
            if (current_stop != nullptr && destination_stop != nullptr) {
                // ??????????? const Stop* ?????? Stop* ??? ??????? ? ??????? ?????????
                auto newPassenger = make_unique<Passenger>(name, current_stop, destination_stop);
                if (current_stop == destination_stop) {
                    cout << "Passenger removed from the taxi as current and destination stops match." << endl;
                } else {
                    passengers.push_back(std::move(newPassenger));
                }
            } else {
                cout << "One or both of the stops do not exist!" << endl;
            }

        }
        break;
        case 6:
        {
            string name;
            cout << "Enter stop name: ";
            cin >> name;
            stops.push_back(make_unique<Stop>(name));
        }
        break;
        case 7:
        {
            string taxi_num;
            string stop_name;
            cout << "Enter taxi number: ";
            cin >> taxi_num;
            auto selectedTaxi = find_if(taxis.begin(), taxis.end(), [&](const auto& taxi) {
                return taxi->getNumber() == taxi_num;
            });
            if (selectedTaxi != taxis.end()) {
                cout << "Enter route (enter 'end' to finish): ";
                while (true) {
                    cin >> stop_name;
                    if (stop_name == "end")
                        break;
                    const Stop* stop = nullptr;
                    for (const auto& s : stops) {
                        if (s->getName() == stop_name) {
                            stop = s.get();
                            break;
                        }
                    }
                    if (stop != nullptr)
                        (*selectedTaxi)->addStop(stop);
                    else
                        cout << "Stop does not exist!" << endl;
                }
            } else {
                cout << "Taxi not found!" << endl;
            }
        }
        break;
        case 8:
        {
            string taxi_num;
            cout << "Enter taxi number: ";
            cin >> taxi_num;
            auto selectedTaxi = find_if(taxis.begin(), taxis.end(), [&](const auto& taxi) {
                return taxi->getNumber() == taxi_num;
            });
            if (selectedTaxi != taxis.end()) {
                (*selectedTaxi)->ride();
                (*selectedTaxi)->dropOffPassengers();
                cout << "Taxi " << (*selectedTaxi)->getNumber() << " moved to " << (*selectedTaxi)->getCurrentStop()->getName() << endl;
            } else {
                cout << "Taxi not found!" << endl;
            }
        }
        break;
        case 9:
            stops.clear();
            taxis.clear();
            passengers.clear();
            return 0;
        default:
            cout << "Invalid choice!" << endl;
            break;
        }
    }

    return 0;
}
