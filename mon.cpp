#include <iostream>
#include <mutex>
#include <vector>
#include <condition_variable>
#include <thread>

using namespace std;

class DiningPhilosophers {
private:
    enum State {
        THINKING,
        HUNGRY,
        EATING
    };

    mutex monitor_mutex;
    vector<State> state;
    vector<condition_variable> self;

    int count = 0;

    void CanEat(int i) {
        if (state[(i + state.size() - 1) % state.size()] != EATING &&
            state[i] == HUNGRY && 
            state[(i + 1) % state.size()] != EATING) {
            state[i] = EATING;
            self[i].notify_one();
            DispState();
        }
    };

    void DispState() {
        for (int i = 0; i < state.size(); i++) {
            string phil_state;
            if (state[i] == THINKING) {
                phil_state = "THINKING";
            } else if (state[i] == HUNGRY) {
                phil_state = "HUNGRY";
            } else {
                phil_state = "EATING";
            }
            cout << "P" << i << " " << phil_state << "\t";
        }
        cout << endl;
        count++;
        if (count == state.size()) {
            cout << endl;
            count = 0;
        }
    };
public:
    DiningPhilosophers(int phils_num) : self(phils_num)  {
        for (int i = 0; i < phils_num; i++) {
            state.push_back(THINKING);
        }
        DispState();
    };

    void GET(int i) {
        unique_lock<mutex> lock(monitor_mutex);
        state[i] = HUNGRY;
        CanEat(i);
        while (state[i] != EATING) {
            self[i].wait(lock);
        }
        DispState();
    };

    void PUT(int i) {
        unique_lock<mutex> lock(monitor_mutex);
        state[i] = THINKING;
        CanEat((i + state.size() - 1) % state.size());
        CanEat((i + 1) % state.size());
        DispState();
    }
};

void philosopher(DiningPhilosophers& table, int id) {
    while (true) {
        table.GET(id);
        this_thread::sleep_for(chrono::seconds(1));
        table.PUT(id);
    }
}

int main() {
    const int phil_num = 5;

    DiningPhilosophers table = DiningPhilosophers(phil_num);
    vector<thread> philosophers;
    for (int i = 0; i < phil_num; i++) {
        philosophers.push_back(thread(philosopher, ref(table), i));
    }

    for (int i = 0; i < phil_num; i++) {
        philosophers[i].join();
    }
    
    return 0;
}
