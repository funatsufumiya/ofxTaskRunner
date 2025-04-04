#include "ofxTaskRunner.h"

size_t TaskId::uuid_counter = 0;
map<std::string, float> WaitTask::wait_started_timef_map_for_names = {};
map<std::pair<int, std::string>, bool> WaitTask::done_map_for_name_and_screen_id = {};
vector<int> WaitTask::registered_screen_ids = {};
std::mutex WaitTask::sync_mutex = std::mutex();