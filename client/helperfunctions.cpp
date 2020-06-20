#include "helperfunctions.h"
#include <jsoncpp/json/json.h>

Json::Value makeJSON(std::string x) {
    Json::CharReaderBuilder builder;
    Json::CharReader* reader = builder.newCharReader();

    Json::Value root;
    std::string errors;

    bool parsingSuccessful = reader->parse(
        x.c_str(),
        x.c_str() + x.size(),
        &root,
        &errors
    );
    delete reader;
    if (!parsingSuccessful) {
    	return Json::Value(); //TODO error cos it didnt parse
    }
    return root;
}
