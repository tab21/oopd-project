#include <iostream>
#include <vector>
#include <queue>
#include <chrono>
#include <thread>
#include <numeric>
#include <algorithm>
#include <iomanip>

using namespace std;

// Constants
const int DATA_PACKET_SIZE = 1024;      // 1 KB data packet size
const int TOTAL_BANDWIDTH = 20;         // 20 MHz total bandwidth
const double MODULATION_RATE = 256.0;   // 256-QAM
const double CODING_RATE = 5.0 / 6.0;   // Coding rate
const vector<int> SUB_CHANNELS = {2, 4, 10}; // Sub-channel sizes in MHz

// Utility function for current time in milliseconds
double getCurrentTime() {
    auto now = chrono::high_resolution_clock::now();
    return chrono::duration<double, milli>(now.time_since_epoch()).count();
}

// Packet class
class Packet {
public:
    int id;
    double start_time;
    double end_time;

    Packet(int id) : id(id), start_time(0), end_time(0) {}
};

// User class
class User {
public:
    int id;
    queue<Packet> packets;

    User(int id, int num_packets) : id(id) {
        for (int i = 0; i < num_packets; ++i) {
            packets.push(Packet(i));
        }
    }

    bool hasPackets() {
        return !packets.empty();
    }
};

// Simulator class for WiFi 6
class Wifi6Simulator {
private:
    int total_bandwidth;
    vector<User> users;
    double total_time;
    vector<double> latencies;
    int sub_channel_size;

public:
    Wifi6Simulator(int num_users, int num_packets_per_user, int sub_channel_size)
        : total_bandwidth(TOTAL_BANDWIDTH), total_time(0), sub_channel_size(sub_channel_size) {
        for (int i = 0; i < num_users; ++i) {
            users.emplace_back(i, num_packets_per_user);
        }
    }

    void simulate() {
        // cout << "Starting WiFi 6 simulation with " << sub_channel_size << " MHz sub-channels...\n";

        auto start_simulation = chrono::high_resolution_clock::now();
        int num_sub_channels = total_bandwidth / sub_channel_size;

        while (true) {
            bool all_users_done = true;

            // Allocate sub-channels to users in a round-robin manner
            for (int i = 0; i < num_sub_channels; ++i) {
                int user_id = i % users.size();
                User& user = users[user_id];

                if (user.hasPackets()) {
                    all_users_done = false;

                    Packet& packet = user.packets.front();
                    packet.start_time = getCurrentTime();

                    // Simulate packet transmission over the sub-channel
                    double transmission_time = (DATA_PACKET_SIZE * 8.0) / (sub_channel_size * 1e6 * MODULATION_RATE * CODING_RATE);
                    transmission_time = max(transmission_time, 0.005); // Minimum delay of 5 ms
                    this_thread::sleep_for(chrono::milliseconds(static_cast<int>(transmission_time * 1000)));

                    packet.end_time = getCurrentTime();
                    latencies.push_back(packet.end_time - packet.start_time);

                    // cout << "User " << user.id << " Packet " << packet.id
                    //           << " Start: " << packet.start_time
                    //           << " End: " << packet.end_time
                    //           << " Latency: " << (packet.end_time - packet.start_time) << " ms\n";

                    user.packets.pop();
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
            cout << "No packets transmitted. Metrics unavailable.\n";
            return;
        }

        // Calculate throughput, average and maximum latency
        double throughput = (latencies.size() * DATA_PACKET_SIZE * 8) / total_time; // in bits per second
        double avg_latency = accumulate(latencies.begin(), latencies.end(), 0.0) / latencies.size();
        double max_latency = *max_element(latencies.begin(), latencies.end());

        cout << fixed << setprecision(2);
        cout << "Throughput: " << throughput / 1e6 << " Mbps\n"; // Convert to Mbps
        cout << "Average Latency: " << avg_latency << " ms\n";
        cout << "Maximum Latency: " << max_latency << " ms\n";
    }
};

int main() {
    const int scenarios[] = {1, 10, 100}; // Number of users in each scenario
    
    int num_packets_per_user;
    cout << "Number of packets per user : " ;
    cin >> num_packets_per_user ;

    // Loop through different sub-channel sizes
    for (int sub_channel_size : SUB_CHANNELS) {
        cout << "\n*** Simulation for Sub-Channel Size: " << sub_channel_size << " MHz ***\n";

        // Loop through different user scenarios
        for (int num_users : scenarios) {
            cout << "\n--- Case : " << num_users << " Users ---\n";
            Wifi6Simulator simulator(num_users, num_packets_per_user, sub_channel_size);
            simulator.simulate();
            simulator.calculateMetrics();
        }
    }

    return 0;
}
