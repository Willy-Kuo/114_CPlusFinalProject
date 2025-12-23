#include <iostream>
#include <deque>
#include <cmath>
#include <memory>
#include <cstdlib>
using namespace std;

template <typename T>
class Array {
private:
    deque<T> dq;
public:
    T pop(int idx) {
        if (idx < 0 || idx >= dq.size()) throw out_of_range("Index out of range");
        T val = dq[idx];
        dq.erase(dq.begin() + idx);
        return val;
    }
    void push(T value) { dq.push_back(value); }
    int size() { return dq.size(); }
    T& operator[](int idx) { return dq[idx]; }
};

class Person {
private:
    int &unitTime;
    int arrivedTime = 0;
    int wantStayTime = 0;
public:
    int floor = 1;
    int wantFloor = 2;

    Person(int &unitTime) : unitTime(unitTime) {
        arrivedTime = unitTime;
    }

    int _get_wantFloor() {
        if (rand() % 10 < 2 && floor != 1) {
            wantFloor = 1;
        } else {
            do {
                wantFloor = 2 + rand() % 11;
            } while (wantFloor == floor); 
        }
        wantStayTime = 20 + rand() % 81;
        return wantFloor;
    }

    void arrive_floor() {
        arrivedTime = unitTime;
        floor = wantFloor;
        wantFloor = 0;
    }

    bool is_waiting() {
        if (wantFloor == 0) _get_wantFloor();
        return unitTime >= arrivedTime + wantStayTime || wantFloor != 0;
    }
};

class Elevator {
private:
    Array<shared_ptr<Person>>* floors;
    int &unitTime;
    Array<int> stayFloors;
public:
    Array<shared_ptr<Person>> persons;
    int floor = 1;
    bool state = true;  // up down
    bool is_static = true;

    Elevator(Array<shared_ptr<Person>> floorsArr[], int &unitTime)
        : floors(floorsArr), unitTime(unitTime) {}

    void take(shared_ptr<Person> person) {
        persons.push(person);
        stayFloors.push(person->wantFloor);
    }

    void _check_and_take_leave() {
        // leave
        for (int i = stayFloors.size() - 1; i >= 0; --i) {
            if (stayFloors[i] == floor) {
                persons[i]->arrive_floor();
                floors[floor - 1].push(persons[i]);
                persons.pop(i);
                stayFloors.pop(i);
            }
        }

        // take
        if (floor < 1 || floor > 12) return;

        Array<shared_ptr<Person>>& floorPersons = floors[floor - 1];
        for (int i = floorPersons.size() - 1; i >= 0; --i) {
            if (persons.size() >= 10) break;
            shared_ptr<Person> person = floorPersons[i];
            if (person->is_waiting() && ((person->wantFloor > floor) == state)) {
                take(person);
                floorPersons.pop(i);
            }
        }
    }

    void move(int toFloor) {
        floor = toFloor;
        _check_and_take_leave();
        while (stayFloors.size() > 0) {
            if (state) floor++; else floor--;
            
            if (floor > 12) { floor = 12; state = false; }
            if (floor < 1)  { floor = 1;  state = true; }

            unitTime += 2;
            _check_and_take_leave();
        }
        is_static = true;
    }
};

int find_closest_waitingFloor(Elevator &elevator, Array<shared_ptr<Person>> floors[]) {
    for (int i = 0; i < 12; ++i) {
        // upward
        int upFloor = elevator.floor + i;
        if (upFloor >= 1 && upFloor <= 12) {
            for (int n = 0; n < floors[upFloor - 1].size(); ++n) {
                auto person = floors[upFloor - 1][n];
                if (person->is_waiting()) {
                    elevator.state = (person->wantFloor > person->floor);
                    return person->floor;
                }
            }
        }
        // downward
        int downFloor = elevator.floor - i;
        if (downFloor >= 1 && downFloor <= 12) {
            for (int n = 0; n < floors[downFloor - 1].size(); ++n) {
                auto person = floors[downFloor - 1][n];
                if (person->is_waiting()) {
                    elevator.state = (person->wantFloor > person->floor);
                    return person->floor;
                }
            }
        }
    }
    return 1;
}

int main() {
    srand(48);
    int unitTime = 0;
    int total_person = 0;
    int time = 0;

    Array<shared_ptr<Person>> floors[12];
    Array<Elevator> elevators;

    for (int i = 0; i < 3; i++) {
        elevators.push(Elevator(floors, unitTime));
    }

    while (total_person <= 150) {
        for (int i = 0; i < (unitTime - time) / 10; i++) {
            time += 10;
            int newPeople = rand() % 3;
            for (int n = 0; n < newPeople; n++) {
                floors[0].push(make_shared<Person>(unitTime));
            }
        }

        for (int i = 0; i < 3; i++) {
            if (elevators[i].is_static) {
                Elevator &elevator = elevators[i];
                elevator.move(find_closest_waitingFloor(elevator, floors));
            }
        }

        total_person = 0;
        for (int i = 0; i < 12; i++) total_person += floors[i].size();
        
        unitTime++;
        if (unitTime > 10000) break; 
    }

    for (int i = 0; i < 12; i++) {
        cout << i + 1 << "F:";
        for (int n = 0; n < floors[i].size(); n++) {
            cout << " " << floors[i][n]->wantFloor;
        }
        cout << "\n";
    }

    for (int i = 0; i < 3; i++) {
        cout << "Elevator " << i << ": " << elevators[i].persons.size() << " persons\n";
    }
}