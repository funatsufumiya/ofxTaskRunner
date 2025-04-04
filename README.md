# ofxTaskRunner

A task runner addon for openFrameworks that allows you to create time-based sequences of actions. (kinda like [UniTask](https://github.com/Cysharp/UniTask) or [bevy_flurx](https://github.com/not-elm/bevy_flurx))

## Features

- Create sequential tasks with timing control
- Chain actions using a fluent interface
- Synchronize multiple tasks

## Note: about threads

This addon is not using other threads. All actions are done on the main thread.

## Dependencies

- openFrameworks 0.11.0 or later
- C++14 or higher

## Installation

1. Download or clone this repository into your openFrameworks/addons folder
2. Include the addon in your project using the Project Generator or by adding it to your build configuration

## Usage

### Basic Example

```cpp
// In ofApp.h
#pragma once

#include "ofMain.h"
#include "ofxTaskRunner.h"

class ofApp : public ofBaseApp{
    public:
        void setup();
        void update();
        void draw();
        
        // ... other methods
        
        ofxTaskRunner<ofApp> taskRunner;
        ofColor backgroundColor;
};
```

```cpp
// In ofApp.cpp
void ofApp::setup(){
    // Initialize the task runner with a reference to the app
    taskRunner.setup(*this);
    
    // Create a task queue that changes background color over time
    taskRunner.createTaskQueue()
        .wait_sec(1.0)
        .then([](ofApp& self){
            self.backgroundColor = ofColor(255, 0, 0); // Red
        })
        .wait_sec(1.0)
        .then([](ofApp& self){
            self.backgroundColor = ofColor(0, 255, 0); // Green
        })
        .wait_sec(1.0)
        .then([](ofApp& self){
            self.backgroundColor = ofColor(0, 0, 255); // Blue
        });
}

void ofApp::update(){
    taskRunner.update(); // Update the task runner
}

void ofApp::draw(){
    ofBackground(backgroundColor);
    taskRunner.draw(); // Optional: draw any task-related visuals
}
```

### Synchronized Tasks Example

You can create multiple tasks that run in sync with each other:

```cpp
for(int i = 0; i < NUM_TASKS; i++) {
    int taskId = i + 1;

    // Register task ID first (needed for synchronization)
    taskRunner.registerTaskId(taskId);
}

// Create multiple synchronized tasks
for(int i = 0; i < NUM_TASKS; i++) {
    int taskId = i + 1;
    
    taskRunner.createTaskQueue(taskId, "sync_task") // Group tasks by task name
        .wait_sec(1.0, true) // The 'true' parameter enables synchronization
        .then([](ofApp& self){
            // First action
        })
        .wait_sec(1.0, true)
        .then([](ofApp& self){
            // Second action
        });
}
```

## API Reference

### ofxTaskRunner<AppType>

- `void setup(AppType& app)` - Initialize the task runner with a reference to your app
- `TaskQueue<AppType> createTaskQueue(int id = 0, std::string group = "")` - Create a new task queue
- `void update()` - Update all tasks (call this in your app's update method)
- `void draw()` - Draw any task-related visuals (call this in your app's draw method)

### TaskQueue<AppType>

- `TaskQueue<AppType>& wait_sec(float seconds, bool sync = false)` - Wait for the specified number of seconds
- `TaskQueue<AppType>& then(std::function<void(AppType&)> callback)` - Execute a callback function
- `TaskQueue<AppType>& then_on_update(std::function<void(AppType&)> callback)` - Execute a callback during update
- `TaskQueue<AppType>& then_on_draw(std::function<void(AppType&)> callback)` - Execute a callback during draw

## Examples

The addon includes several examples:

1. **example** - A simple example showing background color changes over time
2. **example_sync** - Demonstrates synchronized tasks with multiple animations

## License

MIT License
