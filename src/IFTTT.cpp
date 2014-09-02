#include <Logger/Logger.hpp>
#include <curl/curl.h>
#include "json/jsoncpp.cpp" //More like a header, so I can include it
#include "Event.hpp"
#include "IFTTT.hpp"

Logger iftttLogger;

std::string DownloadedResponse;

std::string IFTTT::getEvents(std::string blinkID){
    std::string URL = "http://api.thingm.com/blink1/events/" + blinkID;
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers=NULL; // init to NULL is important
    std::ostringstream oss;
    curl_slist_append(headers, "Accept: application/json");
    curl_slist_append(headers, "Content-Type: application/json");
    curl_slist_append(headers, "charsets: utf-8");
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
        res = curl_easy_perform(curl);

        if (CURLE_OK == res) {
            char *ct;
            res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
            if((CURLE_OK == res) && ct) return DownloadedResponse;
            else {
                fprintf(stderr, "curl failed: %s\n", curl_easy_strerror(res));
                exit(8);
            }
        } else {
            fprintf(stderr, "curl failed: %s\n", curl_easy_strerror(res));
            exit(7);
        }
    } else {
        iftttLogger.log(Logger::LogType::Error, "Could not initialize curl.");
        exit(6);
    }

    curl_easy_cleanup(curl);
}

int IFTTT::writer(char *data, size_t size, size_t nmemb, std::string *buffer_in) {
    DownloadedResponse = Logger::makeString(data);
    return DownloadedResponse.size();
}

std::vector<Event> IFTTT::processEvents(std::string eventJSON){
    std::vector<Event> output;

    Json::Value root;
    Json::Reader reader;

    if (!reader.parse(eventJSON, root)){
        iftttLogger.log(Logger::LogType::Error, "Could not parse JSON.");
        return output; //Which will be empty
    }

    const Json::Value events = root["events"];
    for (int i = 0; i < events.size(); ++i){
        std::string name = events[i].get("name", "").asString();
        std::string source = events[i].get("source", "").asString();
        std::string blink_id = events[i].get("blink1_id", "").asString();
        int date = std::stoi(events[i].get("date", "").asString());
        output.push_back(Event(blink_id, date, name, source));
    }

    return output;
}
