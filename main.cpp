/*
clipy-dump - Dump clipboard history from the Clipy.app on MacOS
(C) - MIT License - Charles Machalow
*/

#include <cpprealm/sdk.hpp>
#include <cpprealm/experimental/sdk.hpp>
#include <external/json/json.hpp>

using namespace realm::experimental;
using json = nlohmann::json;

const auto DefaultRealmPath = getenv("HOME") + std::string("/Library/Application Support/com.clipy-app.Clipy/default.realm");

// Use Realm Studio to see this format and name.
struct CPYClip {
    primary_key<std::string> dataHash;
    std::string dataPath;
    std::string title;
    std::string primaryType;
    int64_t updateTime;
    std::string thumbnailPath;
    bool isColorCode;
};

REALM_SCHEMA(CPYClip, dataHash, dataPath, title, primaryType, updateTime, thumbnailPath, isColorCode)

void usage(char* exe) {
    std::cerr << "Usage: " << exe << " <path to realm file>" << std::endl;
    std::cerr << "If a file is not given, will copy and use: " << DefaultRealmPath << std::endl;
}

int main(int argc, char **argv) {
    std::string realm_path;
    if (argc == 1) {
        // copy the default one since it is likely locked
        auto dest_path = std::filesystem::temp_directory_path() /= std::filesystem::path("clipy-dump-clipy.realm");
        std::filesystem::copy_file(DefaultRealmPath, dest_path, std::filesystem::copy_options::overwrite_existing);
        realm_path = dest_path.string();
    } else if (argc == 2) {
        realm_path = argv[1];
    } else {
        usage(argv[0]);
        return 1;
    }

    if (!std::filesystem::exists(realm_path)) {
        std::cerr << "File does not exist: " << realm_path << std::endl;
        usage(argv[0]);
        return 2;
    }

    auto config = realm::db_config();

    // it seems like the existing schema is 7, so migrate it to match ours via
    // giving a higher number.. so 8.
    config.set_schema_version(8);
    config.set_path(realm_path);
    auto realm = db(std::move(config));

    // most recent first.
    auto clipboard_items = realm.objects<CPYClip>().sort("updateTime", false);

    auto array = json::array();
    for (auto &i : clipboard_items) {
        json j;
        j["dataHash"] = i.dataHash;
        j["dataPath"] = i.dataPath;
        j["title"] = i.title;
        j["primaryType"] = i.primaryType;
        j["updateTime"] = std::to_string(i.updateTime);
        j["thumbnailPath"] = i.thumbnailPath;
        j["isColorCode"] = std::to_string(i.isColorCode);

        array.push_back(j);
    }

    std::cout << array.dump(4) << std::endl;
    return 0;
}