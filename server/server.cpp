#include <string>
#include <chrono>
#include <thread>
#include <iostream>
#include <stdio.h>
#include <jsoncpp/json/json.h>
#include <fstream>
#include <asio.hpp>

#include "server.h"
#include "logging.h"
#include "sectormap.h"
#include "sector.h"
#include "generation.h"

//return codes
// -1: malformed JSON
// -2: invalid request

using asio::ip::tcp;

std::mutex m;
Logger logger;

SectorMap map;

void handleClient(tcp::socket sock) {
    while (true) {
        asio::error_code error;
        asio::streambuf buf;
        size_t len = asio::read_until(sock, buf, "\n", error);
        std::istream is(&buf);
        std::string request;
        std::getline(is, request);
        if (error && error != asio::error::eof) {
            throw asio::system_error(error);
        } else if (error && error == asio::error::eof) {
            return;
        }
        
        Json::CharReaderBuilder builder;
        Json::CharReader* reader = builder.newCharReader();

        Json::Value root;
        std::string errors;

        bool parsingSuccessful = reader->parse(
            request.c_str(),
            request.c_str() + request.size(),
            &root,
            &errors
        );
        delete reader;

        if (!parsingSuccessful) {
            logger.warn("Client sent malformed JSON request: " + request + ". Full error: " + errors);
            asio::error_code ign_error;
            asio::write(sock, asio::buffer("{\"status\": -1}\n"), ign_error);
            continue;
        }
        
        Json::Value totalJson;
        
        for (Json::Value requestJson: root["requests"]) {
            std::string req = requestJson.get("request", "NULL").asString();
            
            if (req == "getSector") {
                int x = requestJson.get("x", 0).asInt();
                int y = requestJson.get("y", 0).asInt();
                Sector * sector = map.getSectorAt(x, y);
                Json::Value sec = sector->asJson();
                
                Json::Value result;
                result["status"] = 0;
                result["result"] = sec;
                totalJson["results"].append(result);
            } else {
                logger.warn("Client sent invalid request: " + root.get("request", "NULL").asString());
                Json::Value result;
                result["status"] = -2;
                totalJson["results"].append(result);
            }
        }
        asio::error_code err;
        Json::StreamWriterBuilder writeBuilder;
        writeBuilder["indentation"] = "";
        const std::string output = Json::writeString(writeBuilder, totalJson);
        asio::write(sock, asio::buffer(output + "\n"), err);
    }
}

int main() {
    const unsigned int LEVEL_SEED = 12345;
    srand(LEVEL_SEED);
    loadConfig();
    
    asio::io_context io_context;
    tcp::acceptor a(io_context, tcp::endpoint(tcp::v4(), 5555));
    while (true) {
        std::thread(handleClient, a.accept()).detach();
    }
    return 0;
}