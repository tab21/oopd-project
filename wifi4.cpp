#include <iostream>
#include <vector>
#include <queue>
#include <random>
#include <chrono>
#include <thread>
#include <numeric>
#include <iomanip>
#include <algorithm> // For max_element

using namespace std;

// Constants
const int PACKET_SIZE = 1024;   // 1 KB packet size in bytes
const int BANDWIDTH = 20;       // 20 MHz
const double MODULATION_RATE = 256.0; // 256-QAM
const double CODING_RATE = 5.0 / 6.0; // Coding rate
const int MAX_BACKOFF_TIME = 50; // Increased backoff time in ms

// Utility function for random backoff time
int random_backoff(int max_backoff) {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<> dis(1, max_backoff);
    return dis(gen);
}

// Access Point class
class AccessPoint {
public:
    int id;
    bool is_channel_free;

    AccessPoint(int id) : id(id), is_channel_free(true) {}

    bool checkChannel() {
        return is_channel_free;
    }

    void occupyChannel() {
        is_channel_free = false;
    }

    void releaseChannel() {
        is_channel_free = true;
    }
};

// Packet class
class Packet {
public:
    int id;
    int size;
    double start_time;
    double end_time;

    Packet(int id, int size) : id(id), size(size), start_time(0), end_time(0) {}
};

// User class
class User {
public:
    int id;
    queue<Packet> packets;

    User(int id, int num_packets) : id(id) {
        for (int i = 0; i < num_packets; ++i) {
            packets.push(Packet(i, PACKET_SIZE));
        }
    }

    bool hasPackets() {
        return !packets.empty();
    }
};

// Simulator class
class Wifi4Simulator {
private:
    AccessPoint ap;
    vector<User> users;
    int max_backoff;
    double total_time;
    vector<double> latencies;

public:
    Wifi4Simulator(int num_users, int max_backoff_time, int num_packets_per_user)
        : ap(0), max_backoff(max_backoff_time), total_time(0) {
        for (int i = 0; i < num_users; ++i) {
            users.emplace_back(i, num_packets_per_user);
        }
    }

    void simulate() {
        // cout << "Starting WiFi 4 simulation...\n";

        auto start_simulation = chrono::high_resolution_clock::now();

        while (true) {
            bool all_users_done = true;

            for (auto& user : users) {
                if (user.hasPackets()) {
                    all_users_done = false;
                    if (ap.checkChannel()) {
                        // Channel is free, transmit packet
                        Packet& packet = user.packets.front();
                        packet.start_time = getCurrentTime();
                        ap.occupyChannel();

                        // Simulate packet transmission time
                        double transmission_time = ((PACKET_SIZE * 8) / (BANDWIDTH * 1e6 * MODULATION_RATE * CODING_RATE)) * 2.0; // Simulate 2x overhead
                        this_thread::sleep_for(chrono::milliseconds(static_cast<int>(transmission_time * 1000)));

                        // Simulate network overhead
                        this_thread::sleep_for(chrono::milliseconds(1)); // Simulate protocol delays

                        packet.end_time = getCurrentTime();
                        latencies.push_back(packet.end_time - packet.start_time);

                        user.packets.pop();
                        ap.releaseChannel();
                    } else {
                        // Channel is busy, backoff
                        int backoff_time = random_backoff(max_backoff);
                        this_thread::sleep_for(chrono::milliseconds(backoff_time));
                    }
                }
            }

            if (all_users_done) {
                break;
            }
        }

        auto end_simulation = chrono::high_resolution_clock::now();
        total_time = chrono::duration<double>(end_simulation - start_simulation).count();
        // cout << "Simulation complete.\n";
    }

    void calculateMetrics() {
        if (latencies.empty()) {
            cout << "No packets transmitted, skipping metric calculation.\n";
            return;
        }

        double throughput = (latencies.size() * PACKET_SIZE * 8) / total_time; // Use actual packets sent
        double avg_latency = accumulate(latencies.begin(), latencies.end(), 0.0) / latencies.size();
        double max_latency = *max_element(latencies.begin(), latencies.end());

        cout << fixed << setprecision(2);
        cout << "Throughput: " << throughput / 1e6 << " Mbps\n";
        cout << "Average Latency: " << avg_latency << " ms\n";
        cout << "Maximum Latency: " << max_latency << " ms\n";
    }

    static double getCurrentTime() {
        auto now = chrono::high_resolution_clock::now();
        return chrono::duration<double, milli>(now.time_since_epoch()).count();
    }
};

int main() {
    const int scenarios[] = {1, 10, 100}; // Number of users in each scenario
    
    int num_packets_per_user;
    cout << "Number of packets per user : " ;
    cin >> num_packets_per_user ;

    for (int num_users : scenarios) {
        cout << "\n--- Case: " << num_users << " Users ---\n";
        Wifi4Simulator simulator(num_users, MAX_BACKOFF_TIME, num_packets_per_user);
        simulator.simulate();
        simulator.calculateMetrics();
    }

    return 0;
}
