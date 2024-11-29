#include <iostream>
#include <vector>
#include <queue>
#include <chrono>
#include <thread>
#include <numeric>
#include <algorithm>
#include <iomanip>

using namespace std;

const int CSI_PACKET_SIZE = 200;        // Channel state information packet size in bytes
const int DATA_PACKET_SIZE = 1024;     // Data packet size in bytes
const int BANDWIDTH = 20;              // Total bandwidth in MHz
const double MODULATION_RATE = 256.0;  // 256-QAM modulation
const double CODING_RATE = 5.0 / 6.0;  // Coding rate
const int COMMUNICATION_WINDOW = 15;    // Parallel communication window in ms

double getCurrentTime() {
    auto now = chrono::high_resolution_clock::now();
    return chrono::duration<double, milli>(now.time_since_epoch()).count();
}

class Packet {
public:
    int id;
    double start_time;
    double end_time;

    Packet(int id) : id(id), start_time(0), end_time(0) {}
};

class AccessPoint {
public:
    void broadcast() {
        this_thread::sleep_for(chrono::milliseconds(2)); // Simulate broadcast delay
    }
};

class User {
public:
    int id;
    queue<Packet> packets;

    User(int id, int num_packets) : id(id) {
        for (int i = 0; i < num_packets; ++i) {
            packets.push(Packet(i));  // Generate packets
        }
    }

    bool hasPackets() {
        return !packets.empty();
    }

    Packet& getNextPacket() {
        return packets.front(); // Access the next packet to be transmitted
    }

    void popPacket() {
        packets.pop(); // Remove the transmitted packet
    }

    void sendCSI() {
        this_thread::sleep_for(chrono::milliseconds(1)); // Simulate CSI transmission
    }
    
    void transmitData(double window_start, vector<double>& latencies) {
        if (hasPackets()) {
            Packet& packet = getNextPacket();
            packet.start_time = getCurrentTime();

            // Simulate packet transmission
            double transmission_time = (DATA_PACKET_SIZE * 8.0) / (BANDWIDTH * 1e6 * MODULATION_RATE * CODING_RATE);
            transmission_time = max(transmission_time, 0.001); // Ensure minimum delay
            this_thread::sleep_for(chrono::milliseconds(static_cast<int>(transmission_time * 1000)));

            packet.end_time = getCurrentTime();
            latencies.push_back(packet.end_time - packet.start_time);
            popPacket();  // Remove the transmitted packet
        }
    }
};

class Wifi5Simulator {
private:
    AccessPoint ap;
    vector<User> users;
    double total_time;
    vector<double> latencies;

public:
    Wifi5Simulator(int num_users, int num_packets_per_user)
        : total_time(0) {
        for (int i = 0; i < num_users; ++i) {
            users.emplace_back(i, num_packets_per_user);  // Create users with packets
        }
    }

    void simulate() {
        // cout << "Starting WiFi 5 simulation...\n";
        auto start_simulation = chrono::high_resolution_clock::now();

        while (true) {
            bool all_users_done = true;

            // Step 1: Broadcast phase
            ap.broadcast();

            // Step 2: CSI transmission by each user sequentially
            for (auto& user : users) {
                if (user.hasPackets()) {
                    all_users_done = false;
                    user.sendCSI(); // Each user sends their CSI
                }
            }

            // Step 3: Parallel communication for 15 ms
            double window_start = getCurrentTime();
            while (getCurrentTime() - window_start <= COMMUNICATION_WINDOW) {
                for (auto& user : users) {
                    if (user.hasPackets()) {
                        user.transmitData(window_start, latencies);
                    }
                }
            }

            if (all_users_done) {
                break;  // Exit the loop if all users are done
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

        double throughput = (latencies.size() * DATA_PACKET_SIZE * 8) / total_time; // in bits per second
        double avg_latency = accumulate(latencies.begin(), latencies.end(), 0.0) / latencies.size();
        double max_latency = *max_element(latencies.begin(), latencies.end());

        cout << fixed << setprecision(2);
        cout << "Throughput: " << throughput / 1e6 << " Mbps\n";  // Convert to Mbps
        cout << "Average Latency: " << avg_latency << " ms\n";
        cout << "Maximum Latency: " << max_latency << " ms\n";
    }
};


int main() {
    const int scenarios[] = {1, 10, 100}; // Number of users in each scenario
    
    int num_packets_per_user;
    cout << "Number of packets per user : " ;
    cin >> num_packets_per_user ;
    

    for (int num_users : scenarios) {
        cout << "\n--- Case: " << num_users << " Users ---\n";
        Wifi5Simulator simulator(num_users, num_packets_per_user);
        simulator.simulate();
        simulator.calculateMetrics();
    }

    return 0;
}
