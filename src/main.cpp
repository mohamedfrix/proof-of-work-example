#include <iostream>
#include <iomanip>
#include <string>
#include <chrono>
#include <openssl/sha.h>
#include <openssl/err.h>
#include <thread>
#include <mutex>
#include <vector>


bool SOLUTION_FOUND = false;
std::mutex SOLUTION_MUTEX;
std::string SOLUTION;
std::string HASH;

// Function to calculate SHA256 hash of a string
std::string sha256(const std::string& str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, str.c_str(), str.length());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
}

bool meet_requirements(const std::string& hash, int leading_zeros) {
    // Check if the hash has the required number of leading zeros
    for (int i = 0; i < leading_zeros; ++i) {
        if (hash[i] != '0') {
            return false;
        }
    }
    return true;
}

// solve the puzzle function
// takes the original string and number of leading zeros as the solution requirement
// returns the modified string that meets the requirement
void solve_puzzle(const std::string& original, int leading_zeros, u_int64_t nonce_start = 0, u_int64_t step = 1) {
    
    std::string modified = original;
    int nonce = nonce_start;

    while (!SOLUTION_FOUND) {
        // Append nonce to the original string
        std::string input = modified + std::to_string(nonce);
        std::string hash = sha256(input);

        // Check if the hash meets the requirement
        if (meet_requirements(hash, leading_zeros)) {
            std::lock_guard<std::mutex> lock(SOLUTION_MUTEX);
            if (!SOLUTION_FOUND) {
                SOLUTION_FOUND = true;
                SOLUTION = input; // Store the solution
                HASH = hash; // Store the hash
            }
            return;
        }
        nonce += step;
    }
}

// the solve puzzle driver function
// takes the original string and number of leading zeros as the solution requirement
// invokes the solve puzzle function
// returns the modified string that meets the requirement
// calculate the time needed to solve the puzzle
// prints the modified string that meets the requirement and the time taken
// prints the hash of the modified string

void solve_puzzle_driver(const std::string& original, int leading_zeros) {    

    const unsigned int thread_count = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    std::cout << "Number of threads: " << thread_count << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    for (unsigned int i = 0; i < thread_count; ++i) {
        threads.emplace_back(solve_puzzle, original, leading_zeros, i, thread_count);
    }
    
    // Join threads
    for (auto& thread : threads) {
        thread.join();
    }


    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Original string: " << original << std::endl;
    std::cout << "Modified string: " << SOLUTION << std::endl;
    std::cout << "Time taken: " << elapsed.count() << " seconds" << std::endl;
    std::cout << "Hash: " << sha256(SOLUTION) << std::endl;
    std::cout << "Hash of the modified string: " << HASH << std::endl;
    std::cout << "Hash of the original string: " << sha256(original) << std::endl;
}



// main function
// takes the original string and number of leading zeros as the solution requirement form the args
// invokes the solve puzzle driver function

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <original_string> <number_of_leading_zeros>" << std::endl;
        return 1;
    }

    std::string original = argv[1];
    int leading_zeros;
    
    try {
        leading_zeros = std::stoi(argv[2]);
        if (leading_zeros < 0) {
            throw std::invalid_argument("Number of leading zeros must be positive");
        }
    } catch (const std::exception& e) {
        std::cout << "Error: Invalid number of leading zeros" << std::endl;
        return 1;
    }

    solve_puzzle_driver(original, leading_zeros);
    return 0;
}