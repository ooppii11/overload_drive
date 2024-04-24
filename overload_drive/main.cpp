#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <random>
#include <filesystem>
#include <algorithm>
#include <iterator>
#include <thread>


#define GB 1000000000
#define MB 1000000

namespace fs = std::filesystem;



void deleteDriveContents(const std::string& drive) {
    for (const auto& entry : fs::directory_iterator(drive)) {
        try {
            if (fs::is_regular_file(entry) || fs::is_symlink(entry)) {
                fs::remove(entry);
            }
            else if (fs::is_directory(entry)) {
                fs::remove_all(entry);
            }
        }
        catch (const std::exception& e) {
            std::cout << "Failed to delete " << entry.path() << ". Reason: " << e.what() << std::endl;
        }
    }

    std::cout << "Drive '" << drive << "' cleared." << std::endl;
}

void createRandomFile(const std::string& fileName, long long size) {
    long long s = size;
    std::ofstream file(fileName);
    std::string niceTry = "nice try\n";
    if (size > static_cast<long long>(niceTry.size())) {
        file << "nice try\n";
        size -= niceTry.size();
    }

    const std::string rnadomChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+[]{}|;:,.<>?";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, rnadomChars.size() - 1);

    while (size > 0) {
        file.put(rnadomChars[dis(gen)]);
        size--;
        if (size % MB == 0) {
            file.put('\n');
            size--;
        }
    }

    std::cout << "File '" << fileName << "' created with " << s << " bytes of random data." << std::endl;
}

void overloadDriveOneFile(const std::string& drive) {
    std::error_code ec;
    const auto spaceInfo = fs::space(drive, ec);

    if (!ec) {
        std::cout << "Drive '" << drive << "' has " << spaceInfo.available << " bytes of free space." << std::endl;
        std::string fileName = drive + "random_file.txt";
        createRandomFile(fileName, spaceInfo.available);
    }
}

void overloadDriveMultiFiles(const std::string& drive, long long size = GB) {
    if (size < GB) {
        std::cout << "Minimum offset file size is 1 GB." << std::endl;
        size = GB;
    }

    std::error_code ec;
    const auto spaceInfo = fs::space(drive, ec);

    if (!ec) {
        long long numFiles = spaceInfo.available / size;

        if (numFiles < 1) {
            size = spaceInfo.available;
            numFiles = 1;
        }

        std::cout << "Drive '" << drive << "' has " << spaceInfo.available << " bytes of free space." << std::endl;
        std::cout << "Creating " << numFiles << " files..." << std::endl;

        std::vector<std::string> fileNames;
        for (long long n = 0; n < numFiles; ++n) {
            fileNames.push_back(drive + "random_file_" + std::to_string(n + 1) + ".txt");
        }
        /*
        std::vector<std::thread> threads;
        for (const auto& fileName : fileNames) {
            threads.emplace_back([&fileName, size]() {
                createRandomFile(fileName, size);
            });
        }
        */
        std::vector<std::thread> threads;
        for (const auto& fileName : fileNames) {
            threads.emplace_back(std::thread(createRandomFile, fileName, size));
        }
        for (auto& thread : threads) {
            thread.join();
        }

        overloadDriveOneFile(drive);
    }
}

int main() {
    try {
        std::string driveLetter;
        std::cout << "Enter the drive letter (e.g., C:): ";
        std::cin >> driveLetter;
        std::transform(driveLetter.begin(), driveLetter.end(), driveLetter.begin(), ::toupper);

        if (!fs::exists(driveLetter + ":\\")) {
            std::cout << "Invalid drive letter." << std::endl;
            return 1;
        }

        std::string multiOrOne;
        std::cout << "Overload with one file or multiple files? (O or M): ";
        std::cin >> multiOrOne;
        std::transform(multiOrOne.begin(), multiOrOne.end(), multiOrOne.begin(), ::toupper);

        std::string drivePath = driveLetter + ":";

        deleteDriveContents(drivePath);

        if (multiOrOne == "O") {
            overloadDriveOneFile(drivePath);
        }
        else if (multiOrOne == "M") {
            overloadDriveMultiFiles(drivePath);
        }
        else {
            std::cout << "Invalid choice." << std::endl;
            return 1;
        }

        std::cout << "Drive overloaded successfully." << std::endl;
        deleteDriveContents(drivePath);
    }
    catch (const std::exception& e) {
        std::cout << "An exception occurred: " << e.what() << std::endl;
    }

    return 0;
}
