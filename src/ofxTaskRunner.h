#pragma once 

#include "ofMain.h"

#include <random>
#include <sstream>
#include "boost/optional.hpp"
#include <functional>

// ===============================================

namespace taskrunner {

namespace optional {

    template <class T>
    using optional = boost::optional<T>;

    static boost::none_t none = boost::none;

    template <class T>
    using optional_ref = optional<std::reference_wrapper<T>>;

} // namespace optional

namespace uuid {

    static std::random_device              rd;
    static std::mt19937                    gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static std::uniform_int_distribution<> dis2(8, 11);

    inline std::string generate_uuid_v4() {
        std::stringstream ss;
        int i;
        ss << std::hex;
        for (i = 0; i < 8; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 4; i++) {
            ss << dis(gen);
        }
        ss << "-4";
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        ss << dis2(gen);
        for (i = 0; i < 3; i++) {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 12; i++) {
            ss << dis(gen);
        };
        return ss.str();
    }

} // namespace uuid

namespace utils {

    [[noreturn]] inline void __unreachable(const char* file, int line)
    {
        ofLogError() << "unreachable! (" << file << ":" << line << ")";
        // Uses compiler specific extensions if possible.
        // Even if no extension is used, undefined behavior is still raised by
        // an empty function body and the noreturn attribute.
    #if defined(_MSC_VER) && !defined(__clang__) // MSVC
        __assume(false);
    #else // GCC, Clang
        __builtin_unreachable();
    #endif
    }

    // to show original file and line number
    #define unreachable() __unreachable(__FILE__, __LINE__)

    [[noreturn]] inline void __unimplemented(const char* file, int line)
    {
        ofLogError() << "unimplemented! (" << file << ":" << line << ")";
        // Uses compiler specific extensions if possible.
        // Even if no extension is used, undefined behavior is still raised by
        // an empty function body and the noreturn attribute.
    #if defined(_MSC_VER) && !defined(__clang__) // MSVC
        __assume(false);
    #else // GCC, Clang
        __builtin_unreachable();
    #endif
    }

    // to show original file and line number
    #define unimplemented() __unimplemented(__FILE__, __LINE__)

    [[noreturn]] inline void __panic(const char* file, int line)
    {
        ofLogError() << "panic! (" << file << ":" << line << ")";
        assert(false);
    }

    // to show original file and line number
    #define panic() __panic(__FILE__, __LINE__)

} // namespace utils

namespace container {

    template<class T>
    class queue : public std::queue<T>
    {
    public:
        void clear()
        {
            std::queue<T> empty;
            std::swap(empty, *this);
        }
    };

} // namespace container

} // namespace taskrunner

// ===============================================

enum class TaskType {
    WAIT,
    DRAW,
    UPDATE,
    CREATE_TASK_QUEUE,
};

class TaskId {
private:
    size_t uuid;
    static size_t uuid_counter;
    void setUuid() {
        // check max
        if (uuid_counter >= std::numeric_limits<size_t>::max() - 1) {
            uuid_counter = 0;
        }
        this->uuid = uuid_counter++;
    }

public:
    TaskId() {
        this->setUuid();
    }
    size_t id() {
        return this->uuid;
    }
    bool operator==(const TaskId& other) const {
        return this->uuid == other.uuid;
    }
    bool operator!=(const TaskId& other) const {
        return this->uuid != other.uuid;
    }
};

class Task {
private:
    TaskId uuid;
public:
    Task() : uuid() {
    }
    virtual TaskType getTaskType() const = 0;
    TaskId id() {
        return this->uuid;
    }
};

class WaitTask : public Task {
private:
    float wait_time_sec;
    float wait_started_timef;
    bool need_sync;
    int task_id;
    std::string task_queue_name;

    static map<std::string, float> wait_started_timef_map_for_names;
    static map<std::pair<int, std::string>, bool> done_map_for_name_and_task_id;
    static vector<int> registered_task_ids;
    static std::mutex sync_mutex;

public:
    WaitTask(float wait_time_sec, bool need_sync, int task_id, std::string task_queue_name) {
        this->wait_time_sec = wait_time_sec;
        this->wait_started_timef = -9999.9f;
        this->need_sync = need_sync;
        this->task_id = task_id;
        this->task_queue_name = task_queue_name;
    }

    TaskType getTaskType() const override {
        return TaskType::WAIT;
    }

    /// @brief register task id at setup (need to detect all tasks are done for sync)
    /// @param task_id 
    static void registerTaskId(int task_id) {
        if (std::find(registered_task_ids.begin(), registered_task_ids.end(), task_id) == registered_task_ids.end()) {
            registered_task_ids.push_back(task_id);
        }
    }

    void start() {
        if (this->need_sync) {
            lock_guard<mutex> lock(sync_mutex);
            if (wait_started_timef_map_for_names.count(this->task_queue_name) > 0) {
                this->wait_started_timef = wait_started_timef_map_for_names.at(this->task_queue_name);
            }else{
                this->wait_started_timef = ofGetElapsedTimef();
            }
        }else{
            this->wait_started_timef = ofGetElapsedTimef();
        }
    }

    bool isStarted() const {
        return this->wait_started_timef > 0;
    }

    bool isDone() const {
        bool bDone = ofGetElapsedTimef() - this->wait_started_timef >= this->wait_time_sec;

        if (this->need_sync) {
            if (bDone) {
                lock_guard<mutex> lock(sync_mutex);
                auto key = std::make_pair(this->task_id, this->task_queue_name);
                done_map_for_name_and_task_id[key] = true;
                
                // if all tasks/screens are done, clean them (with task queue name)
                bool all_done = true;
                for (auto& task_id : registered_task_ids) {
                    auto _key = std::make_pair(task_id, this->task_queue_name);
                    if (done_map_for_name_and_task_id.count(_key) == 0) {
                        all_done = false;
                        break;
                    }
                }

                if (all_done) {
                    // delete map item with task queue name
                    for (auto& task_id : registered_task_ids) {
                        auto _key = std::make_pair(task_id, this->task_queue_name);
                        if (done_map_for_name_and_task_id.count(_key) > 0) {
                            done_map_for_name_and_task_id.erase(_key);
                        }
                    }
                }
            }
        }

        return bDone;
    }
};

template <typename App>
class DrawTask : public Task {
public:
    std::function<void(App&)> draw_task;

    DrawTask(std::function<void(App&)> draw_task) {
        this->draw_task = draw_task;
    }

    TaskType getTaskType() const override {
        return TaskType::DRAW;
    }
};

template <typename App>
class UpdateTask : public Task {
public:
    std::function<void(App&)> update_task;

    UpdateTask(std::function<void(App&)> update_task) {
        this->update_task = update_task;
    }
    TaskType getTaskType() const override {
        return TaskType::UPDATE;
    }
};

template <typename App>
class TaskQueue;

template <typename App>
class CreateTaskQueueTask : public Task {
public:
    std::function<void(TaskQueue<App>&)> func_for_new_task_queue;
    int task_id;
    std::string task_queue_name;
    CreateTaskQueueTask(int task_id, std::string task_queue_name, std::function<void(TaskQueue<App>&)> func_for_new_task_queue) {
        this->task_id = task_id;
        this->task_queue_name = task_queue_name;
        this->func_for_new_task_queue = func_for_new_task_queue;
    }
    TaskType getTaskType() const override {
        return TaskType::CREATE_TASK_QUEUE;
    }
};

// ================================================

template <typename App>
class TaskQueue {
private:
    taskrunner::container::queue<unique_ptr<Task>> tasks;

public:
    int task_id;
    std::string task_queue_name;

    TaskQueue(int task_id, std::string task_queue_name) {
        this->task_id = task_id;
        this->task_queue_name = task_queue_name;
    }

    TaskQueue(TaskQueue&&) = default;
    TaskQueue& operator=(TaskQueue&&) = default;

    // to prevent copying unique_ptr
    TaskQueue(const TaskQueue&) = delete;
    TaskQueue& operator=(const TaskQueue&) = delete;

    size_t size() const {
        return tasks.size();
    }

    bool hasTasks() const {
        return !tasks.empty();
    }

    template <typename T>
    T& front() {
        return static_cast<T&>(*tasks.front());
    }

    void pop_front() {
        if (tasks.empty()) {
            return;
        }
        tasks.pop();
    }

    taskrunner::optional::optional<TaskType> getFirstTaskType() const {
        if (tasks.empty()) {
            return taskrunner::optional::none;
        }
        return tasks.front()->getTaskType();
    }

    /// add wait task (in seconds)
    TaskQueue<App>& wait_sec(float wait_time_sec, bool need_sync = false) {
        bool is_first_task = tasks.empty();
        tasks.push(std::move(make_unique<WaitTask>(wait_time_sec, need_sync, task_id, task_queue_name)));
        if (is_first_task) {
            WaitTask& wait_task = static_cast<WaitTask&>(*tasks.front());
            wait_task.start();
        }
        return *this;
    }

    /// add wait task (in milliseconds)
    TaskQueue<App>& wait_ms(float wait_time_millis, bool need_sync = false) {
        return wait_sec(wait_time_millis / 1000.0f, need_sync);
    }

    /// add draw task
    TaskQueue<App>& then_on_draw(std::function<void(App&)> draw_task) {
        tasks.push(std::move(make_unique<DrawTask<App>>(draw_task)));
        return *this;
    }

    /// add update task
    TaskQueue<App>& then_on_update(std::function<void(App&)> update_task) {
        tasks.push(std::move(make_unique<UpdateTask<App>>(update_task)));
        return *this;
    }
    
    /// add update task (alias)
    TaskQueue<App>& then(std::function<void(App&)> update_task) {
        return this->then_on_update(update_task);
    }

    /// add task which create new task queue
    TaskQueue<App>& then_create_task_queue(std::string task_queue_name, std::function<void(TaskQueue<App>&)> func_for_new_task_queue) {
        tasks.push(std::move(make_unique<CreateTaskQueueTask<App>>(task_id, task_queue_name, func_for_new_task_queue)));
        return *this;
    }
};

// ================================================
// ================================================
// ================================================

template <typename App>
class ofxTaskRunner {
private:
public:
    void setup(App& app) {
        this->_should_end = false;

        clearTaskQueues();
        update_tasks.clear();
        draw_tasks.clear();
        create_task_queue_tasks.clear();

        this->app = app;
    }

    /// @brief register task id at setup (need to detect all tasks are done for sync)
    /// @param task_id 
    void registerTaskId(int task_id) {
        WaitTask::registerTaskId(task_id);
    }

    void clearTaskQueues() {
        task_queues.clear();
    }

    void clear(){
        clearTaskQueues();
        update_tasks.clear();
        draw_tasks.clear();
        create_task_queue_tasks.clear();
    }

    void processTaskQueue(TaskQueue<App>& task_queue) {
        size_t task_count = task_queue.size();

        while (task_queue.hasTasks()) {
            if (task_queue.getFirstTaskType() == TaskType::WAIT) {
                WaitTask& wait_task = task_queue.template front<WaitTask>();
                if (!wait_task.isStarted()) {
                    wait_task.start();
                    break;
                }else if (wait_task.isDone()) {
                    task_queue.pop_front();
                }else {
                    break;
                }
            } else if (task_queue.getFirstTaskType() == TaskType::DRAW) {
                DrawTask<App>& draw_task = task_queue.template front<DrawTask<App>>();
                draw_tasks.push(draw_task.draw_task);
                task_queue.pop_front();
            } else if (task_queue.getFirstTaskType() == TaskType::UPDATE) {
                UpdateTask<App>& update_task = task_queue.template front<UpdateTask<App>>();
                update_tasks.push(update_task.update_task);
                task_queue.pop_front();
            } else if (task_queue.getFirstTaskType() == TaskType::CREATE_TASK_QUEUE) {
                CreateTaskQueueTask<App>& create_task_queue_task = task_queue.template front<CreateTaskQueueTask<App>>();
                create_task_queue_tasks.push(create_task_queue_task);
                task_queue.pop_front();
            }
        }

        bool going_to_next_task = task_queue.size() < task_count;

        if (going_to_next_task && task_queue.hasTasks()) {
            // check next task is wait task, if so, start it
            if (task_queue.getFirstTaskType() == TaskType::WAIT) {
                WaitTask& wait_task = task_queue.template front<WaitTask>();
                if (!wait_task.isStarted()) {
                    wait_task.start();
                }
            }
        }
    }

    void processTaskQueues(){
        for (auto& task_queue : task_queues) {
            processTaskQueue(task_queue);
        }

        std::remove_if(task_queues.begin(), task_queues.end(), [](const TaskQueue<App>& task_queue) {
            return !task_queue.hasTasks();
        });
    }

    void update() {
        if (!app) {
            ofLogError("ofxTaskRunner") << "setup() must be called before update()";
            taskrunner::utils::panic();
        }

        processTaskQueues();

        while (!update_tasks.empty()) {
            auto task = update_tasks.front();
            update_tasks.pop();
            task(*app);
        }

        while (!create_task_queue_tasks.empty()) {
            auto& t = create_task_queue_tasks.front();
            create_task_queue_tasks.pop();

            auto&& new_task_queue = createTaskQueue(t.task_id, t.task_queue_name);
            t.func_for_new_task_queue(new_task_queue);
        }
    }

    void draw() const {
        if (!app) {
            ofLogError("ofxTaskRunner") << "setup() must be called before draw()";
            taskrunner::utils::panic();
        }
        
        const auto& app_const = *app;
        auto* self = const_cast<ofxTaskRunner*>(this);

        while (!draw_tasks.empty()) {
            auto task = self->draw_tasks.front();
            self->draw_tasks.pop();
            task(app_const);
        }
    }

    TaskQueue<App>& createTaskQueue(int task_id, std::string name) {
        task_queues.push_back(TaskQueue<App>(task_id, name));
        return task_queues.back();
    }

    TaskQueue<App>& createTaskQueue(std::string name) {
        task_queues.push_back(TaskQueue<App>(0, name));
        return task_queues.back();
    }

    TaskQueue<App>& createTaskQueue() {
        std::string task_uuid = taskrunner::uuid::generate_uuid_v4();
        std::string name = "task_queue_" + task_uuid;
        task_queues.push_back(TaskQueue<App>(0, name));
        return task_queues.back();
    }

    void stop() {
        _should_end = true;
        clear();
    }

    bool shouldEnd() const {
        return _should_end;
    }

protected:
    /// This should be set on setup
    taskrunner::optional::optional_ref<App> app;

    taskrunner::container::queue<std::function<void(App&)>> update_tasks;
    taskrunner::container::queue<std::function<void(App&)>> draw_tasks;
    /// tasks which create new task queue
    taskrunner::container::queue<CreateTaskQueueTask<App>> create_task_queue_tasks;
    bool _should_end = false;
    vector<TaskQueue<App>> task_queues;
};
