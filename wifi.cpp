#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

// Constants
const double BANDWIDTH = 20e6;  // 20 MHz in Hz (fixed bandwidth)
const double MODULATION_RATE = 256;  // 256-QAM
const double CODING_RATE = 5.0 / 6.0;
const int PACKET_SIZE = 1024;  // 1 KB packet in bytes
const double SLOT_TIME = 0.00001; // Slot time for backoff in seconds
const int MAX_BACKOFF_TIME = 10; // Max backoff time for WiFi 4 in milliseconds

// Class: WiFi4
class WiFi4 {
public:
    WiFi4() {}

    // Throughput calculation for WiFi 4
double calculateThroughput(int totalUsers) {
    // Total throughput for WiFi 4
    double totalThroughput = BANDWIDTH * MODULATION_RATE * CODING_RATE;  // in bps (bits per second)

    // Share the throughput among all users
    double throughput = totalThroughput / totalUsers;  // in bps (bits per second)
    
    // Convert from bps to Mbps (1 Mbps = 1e6 bps)
    return throughput / 1e6;
}


    // Average latency calculation for WiFi 4
    double calculateAverageLatency(int totalUsers, double throughput) {
        double transmissionTime = (PACKET_SIZE * 8) / throughput;  // in seconds
        transmissionTime *= 1000; // Convert to milliseconds

        // Gradually increase contention delay with more users
        double contentionDelay = (totalUsers > 1) ? (log(totalUsers) * 10) : 0; // Log-based contention delay scaling
        return transmissionTime + contentionDelay;  // Total latency in ms
    }

    // Maximum latency calculation for WiFi 4
    double calculateMaxLatency(int totalUsers, double throughput) {
        return calculateAverageLatency(totalUsers, throughput) * 1.5;  // Approximation for retries
    }
};

// Function to simulate for a given number of users and WiFi standard
void simulateWiFi4(int totalUsers) {
    WiFi4 wifi4;
    
    // Calculate throughput, average latency, and max latency
    double throughput = wifi4.calculateThroughput(totalUsers);
    double avgLatency = wifi4.calculateAverageLatency(totalUsers, throughput);
    double maxLatency = wifi4.calculateMaxLatency(totalUsers, throughput);

    // Output only the throughput, average latency, and maximum latency
    cout << "Simulation for " << totalUsers << " Users:" << endl;
    cout << "WiFi 4:" << endl;
    cout << "Throughput: " << throughput << " Mbps" << endl;
    cout << "Average Latency: " << avgLatency << " ms" << endl;
    cout << "Maximum Latency: " << maxLatency << " ms" << endl;
}

int main() {
    srand(time(0));  // Random seed for backoff times

    // Simulate for 1, 10, and 100 users across WiFi 4
    simulateWiFi4(1);
    simulateWiFi4(10);
    simulateWiFi4(100);

    return 0;
}
