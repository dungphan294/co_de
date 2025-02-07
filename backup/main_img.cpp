#include <iostream>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <string>
#include <array>
#include <stdexcept>

struct Pixel {
    std::array<uint8_t, 4> rgba;

    bool operator==(const Pixel& other) const {
        return rgba == other.rgba;
    }
};

class ImageLZW {
private:
    static const int MAX_DICT_SIZE = 4096;

    std::vector<int> compressChannel(const std::vector<uint8_t>& data) const {
        std::unordered_map<std::string, int> dictionary;
        std::vector<int> compressed;

        for (int i = 0; i < 256; i++) {
            dictionary[std::string(1, static_cast<char>(i))] = i;
        }

        std::string current;
        int nextCode = 256;

        for (uint8_t byte : data) {
            std::string next = current + static_cast<char>(byte);

            if (dictionary.find(next) != dictionary.end()) {
                current = next;
            } else {
                compressed.push_back(dictionary[current]);
                if (dictionary.size() < MAX_DICT_SIZE) {
                    dictionary[next] = nextCode++;
                }
                current = std::string(1, static_cast<char>(byte));
            }
        }

        if (!current.empty()) {
            compressed.push_back(dictionary[current]);
        }

        return compressed;
    }

    std::vector<uint8_t> decompressChannel(const std::vector<int>& compressed) const {
        std::vector<std::string> dictionary;
        std::vector<uint8_t> decompressed;

        for (int i = 0; i < 256; i++) {
            dictionary.push_back(std::string(1, static_cast<char>(i)));
        }

        if (compressed.empty()) {
            return decompressed;
        }

        std::string current = std::string(1, static_cast<char>(compressed[0]));
        decompressed.insert(decompressed.end(), current.begin(), current.end());

        for (size_t i = 1; i < compressed.size(); i++) {
            int code = compressed[i];
            std::string entry;

            if (code < int(dictionary.size())) {
                entry = dictionary[code];
            } else if (code == int(dictionary.size())) {
                entry = current + current[0];
            }

            decompressed.insert(decompressed.end(), entry.begin(), entry.end());

            if (dictionary.size() < MAX_DICT_SIZE) {
                dictionary.push_back(current + entry[0]);
            }

            current = entry;
        }

        return decompressed;
    }

public:
    std::vector<std::vector<int>> compressImage(const std::vector<Pixel>& pixels) const {
        std::vector<uint8_t> rChannel, gChannel, bChannel, aChannel;

        for (const auto& pixel : pixels) {
            rChannel.push_back(pixel.rgba[0]);
            gChannel.push_back(pixel.rgba[1]);
            bChannel.push_back(pixel.rgba[2]);
            aChannel.push_back(pixel.rgba[3]);
        }

        return {
            compressChannel(rChannel),
            compressChannel(gChannel),
            compressChannel(bChannel),
            compressChannel(aChannel)
        };
    }

    std::vector<Pixel> decompressImage(const std::vector<std::vector<int>>& compressed) const {
        if (compressed.size() != 4) {
            throw std::runtime_error("Invalid compressed data: Expected 4 channels.");
        }

        std::vector<uint8_t> rChannel = decompressChannel(compressed[0]);
        std::vector<uint8_t> gChannel = decompressChannel(compressed[1]);
        std::vector<uint8_t> bChannel = decompressChannel(compressed[2]);
        std::vector<uint8_t> aChannel = decompressChannel(compressed[3]);

        if (rChannel.size() != gChannel.size() || rChannel.size() != bChannel.size() || rChannel.size() != aChannel.size()) {
            throw std::runtime_error("Decompressed channel sizes do not match!");
        }

        std::vector<Pixel> pixels;
        pixels.reserve(rChannel.size());
        for (size_t i = 0; i < rChannel.size(); i++) {
            pixels.push_back({ {rChannel[i], gChannel[i], bChannel[i], aChannel[i]} });
        }

        return pixels;
    }
};

int main() {
    std::vector<Pixel> pixels1 = {
        {{255, 0, 0, 255}},
        {{0, 255, 0, 255}},
        {{0, 0, 255, 255}},
        {{255, 255, 255, 255}},
        {{100, 50, 150, 255}},
        {{255, 0, 0, 255}}, // Duplicate to test compression
        {{0, 255, 0, 255}}  // Duplicate to test compression
    };

    ImageLZW lzw;
    try {
        auto compressed1 = lzw.compressImage(pixels1);
        auto decompressed1 = lzw.decompressImage(compressed1);

        if (pixels1 == decompressed1) {
            std::cout << "Compression and decompression successful!" << std::endl;
            std::cout << "Original size: " << pixels1.size() * sizeof(Pixel) << " bytes" << std::endl;

            size_t compressedSize = 0;
            for (const auto& channel : compressed1) {
                compressedSize += channel.size() * sizeof(int); // Approximate compressed size
            }
            std::cout << "Compressed size: " << compressedSize << " bytes" << std::endl;

        } else {
            std::cout << "Error: Pixel data mismatch after decompression." << std::endl;
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}