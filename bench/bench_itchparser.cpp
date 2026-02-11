#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <benchmark/benchmark.h>
#include "itch_parser/ItchParser.hpp"
#include "message_handler/MessageHandler.hpp"

class NullHandler : MessageHandler {
    public:
        explicit NullHandler() : count{0} {};
        ~NullHandler() = default;

        void handle_message(ItchMessage& msg) {
            count++;
            benchmark::DoNotOptimize(msg);
        }

        size_t count;
};

class BenchItchParser : ItchParser<NullHandler> {
    public:
        BenchItchParser() : ItchParser<NullHandler>{std::make_unique<NullHandler>()} {}

        std::vector<char> load_itch_file(const std::string& path) {
            std::ifstream file(path, std::ios::binary | std::ios::ate);
            if (!file) { std::cout << "Could not open file!" << std::endl; return {}; }

            auto size = file.tellg();
            file.seekg(0);

            std::vector<char> buffer(static_cast<size_t>(size));
            file.read(buffer.data(), size);
            return buffer;
        }

        void parse_file(const std::vector<char>& buffer) {
            std::uint16_t message_size;
            size_t offset = 0;

            while (offset + 2 <= buffer.size()) {
                std::memcpy(&message_size, buffer.data() + offset, 2);
                message_size = ntohs(message_size);
                offset += 2;
                if (offset + message_size > buffer.size()) break;

                char message_type = buffer[offset];
                offset += 1;

                char message_buffer[message_size];
                message_buffer[0] = message_type;

                std::memcpy(message_buffer + 1, buffer.data() + offset, message_size - 1);

                ItchMessage msg = parse_message(message_buffer, message_size, message_type);
                message_handler_->handle_message(msg);

                offset += message_size - 1;
            }
        }
};



static void BM_ItchParser_MemoryFileParse(benchmark::State& state) {
    std::string filename = "../data/S071321-v50.bin";
    BenchItchParser parser{};

    auto buffer = parser.load_itch_file(filename);

    for (auto _ : state) {
        parser.parse_file(buffer);
    }

    state.SetBytesProcessed(int64_t(state.iterations()) * int64_t(buffer.size()));
}

static void BM_ItchParser_FileStream(benchmark::State& state) {
    ItchParser<NullHandler> parser{std::make_unique<NullHandler>()};
    
    for (auto _ : state) {
        parser.parse_file();
        benchmark::DoNotOptimize(parser);
    }
    
    state.counters["msgs"] = parser.get_handler().count;
}

BENCHMARK(BM_ItchParser_FileStream)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_ItchParser_MemoryFileParse)->Unit(benchmark::kMillisecond);