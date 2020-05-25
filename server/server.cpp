#include <string>
#include <chrono>
#include <thread>
#include <iostream>
#include <stdio.h>
#include <jsoncpp/json/json.h>
#include <fstream>
#include <asio.hpp>

//return codes
// -1: malformed JSON
// -2: invalid request

using asio::ip::tcp;

class Logger {
public:
    Logger() {}
    void warn(std::string msg) {
        std::cout << msg << "\n";
    }
};

std::mutex m;
Json::Value generation_config;
Logger logger;

int rndInt(int min, int max) {
    return rand() % (max - min) + min;
}

double rndDouble(double fMin, double fMax) {
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

class pixel {
public:
    int r = 0;
    int g = 0;
    int b = 0;
    pixel() {}
    
    pixel(int r, int g, int b) {
        this->r = r;
        this->g = g;
        this->b = b;
    }
    
    void rand() {
        r = rndInt(0, 255);
        g = rndInt(0, 255);
        b = rndInt(0, 255);
    }
    
    void rand(int min, int max) {
        r = rndInt(min, max);
        g = rndInt(min, max);
        b = rndInt(min, max);
    }
    
    int asInt() {
        return (r << 16) | (g << 8) | b;
    }
};

class Planet {
public:
    double mass;
    int radius;
    int numColours;
    std::vector<double> generationChances;
    std::vector<pixel> generationColours;
    std::vector<int> generationZValues;
    std::vector<double> generationNoise;
    pixel baseColour;
    int posFromStar;
    double theta;
    double angularVelocity;

    Planet() {}
    Planet(int posFromStar) {
        this->theta = ((rand() % 360) / 180.0) * 3.14159265358979323;
        this->posFromStar = posFromStar;
        this->radius = rndInt(8, 28);
        this->numColours = (this->radius - 5) / 23.0 * 3;

        this->generationChances = std::vector<double>(this->numColours);
        this->generationColours = std::vector<pixel>(this->numColours);
        this->generationZValues = std::vector<int>(this->numColours);
        this->generationNoise   = std::vector<double>(this->numColours);

        for (int i = 0; i < this->numColours; i++) {
            this->generationChances[i] = rndDouble(0.4, 0.7);
            this->generationColours[i].rand();
            this->generationZValues[i] = rand() % 1000;
            this->generationNoise[i]   = (rand() % 900 + 100) / 1000.0;
        }
        this->baseColour.rand(80, 255);
        this->angularVelocity = 1.0 / (posFromStar * posFromStar) * 6000.0;
    }
    
    Json::Value asJson() {
        Json::Value res;
        res["mass"] = mass;
        res["radius"] = radius;
        res["numColours"] = numColours;
        res["baseColour"] = baseColour.asInt();
        res["posFromStar"] = posFromStar;
        res["theta"] = theta;
        res["angularVelocity"] = angularVelocity;
        
        for (int i = 0; i < numColours; i++) {
            res["generationColours"].append(generationColours[i].asInt());
            res["generationChances"].append(generationChances[i]);
            res["generationZValues"].append(generationZValues[i]);
            res["generationNoise"].append(generationNoise[i]);
        }
        
        return res;
    }
};

class Star {
public:
    double radius;
    pixel colour;
    std::vector<Planet> planets;
    float x = 0;
    float y = 0;
    int num = 0;

    Star() {}

    Star(float x, float y) {
        this->x = x;
        this->y = y;

        this->num = rand() % 8;
        this->radius = rand() % 7 + 5;
        this->colour = pixel(rand() % 100 + 155, rand() % 100 + 155, rand() % 100 + 155);

        this->planets = std::vector<Planet>(this->num);
        for (int i = 0; i < this->num; i++) {
            this->planets[i] = Planet(rand() % 200 + this->radius * 6 + 20);
        }
    }
    
    Json::Value asJson() {
        Json::Value res;
        res["x"] = x;
        res["y"] = y;
        res["num"] = num;
        res["colour"] = colour.asInt();
        res["radius"] = radius;
        for (int i = 0; i < (signed)planets.size(); i++) {
            res["planets"].append(planets[i].asJson());
        }
        return res;
    }
    
};

class Sector {
public:
    int x, y, r, numStars;
    bool generated = false;
    std::vector<Star> stars;
    Sector() {}
    Sector(int sx, int sy, int sr) {
        this->x = sx;
        this->y = sy;
        this->r = sr;
    }

    Star * getStarAt(int x, int y) {
        for (int i = 0; i < numStars; i++) {
            int dx = x - stars[i].x;
            int dy = y - stars[i].y;
            if (dx * dx + dy * dy < stars[i].radius * stars[i].radius) {
                return &this->stars[i];
            }
        }
        return nullptr;
    }

    void generate() {
        this->numStars = rand() % 8 + 2;
        this->stars = std::vector<Star>(numStars);
        for (int i = 0; i < this->numStars; i++) {
            this->stars[i] = Star(rand() % this->r, rand() % this->r);
        }
        generated = true;
    }

    Json::Value asJson() {
        Json::Value res;
        for (int i = 0; i < numStars; i++) {
            res["stars"].append(stars[i].asJson());
        }
        res["numStars"] = numStars;
        res["x"] = x;
        res["y"] = y;
        res["r"] = r;
        return res;
    }
};

class SectorMap {
public:
    std::vector<std::vector<Sector>> cache;

    SectorMap() {
        this->cache = std::vector<std::vector<Sector>>(0);
    }

    Sector * getSectorAt(int x, int y) {
        std::lock_guard<std::mutex> lock(m);
        if (x < 0 || y < 0) {
            return &this->cache[0][0];
        }
        bool needToGenerate = false;
        if ((signed)cache.size() <= y) {
            cache.resize(y + 1);
            cache[y] = std::vector<Sector>();
            needToGenerate = true;
        }
        if ((signed)cache[y].size() <= x) {
            cache[y].resize(x + 1);
            needToGenerate = true;
        }
        if (needToGenerate || !this->cache[y][x].generated) {
            Sector a(x, y, 256);
            a.generate();

            cache[y][x] = a;
        }
        return &this->cache[y][x];
    }
};

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
        
        std::string req = root.get("request", "NULL").asString();
        
        if (req == "getSector") {
            int x = root.get("x", 0).asInt();
            int y = root.get("y", 0).asInt();
            Sector * sector = map.getSectorAt(x, y);
            Json::Value sec = sector->asJson();
            asio::error_code err;
            
            Json::Value result;
            result["status"] = 0;
            result["result"] = sec;
            
            Json::StreamWriterBuilder builder;
            builder["indentation"] = "";
            const std::string output = Json::writeString(builder, result);
            asio::write(sock, asio::buffer(output + "\n"), err);
        } else {
            logger.warn("Client sent invalid request: " + root.get("request", "NULL").asString());
            asio::error_code err;
            asio::write(sock, asio::buffer("{\"status\": -2}\n"), err);
        }
    }
}

int main() {
    std::ifstream generation_config_file("generation.json", std::ifstream::binary);
    generation_config_file >> generation_config;
    
    asio::io_context io_context;
    tcp::acceptor a(io_context, tcp::endpoint(tcp::v4(), 5555));
    while (true) {
        std::thread(handleClient, a.accept()).detach();
    }
    return 0;
}