#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofLogToConsole();
	ofSetVerticalSync(true);
	ofSetFrameRate(60);

	taskRunner.setup(*this);

	// Initialize task IDs
	task_ids.resize(NUM_TASKS);
	
	// Initialize task parameters
	taskParams.resize(NUM_TASKS);
	
	for(int i = 0; i < NUM_TASKS; i++) {
		task_ids[i] = i + 1; // Start task IDs from 1
		
		// Set initial parameters
		taskParams[i].x = (ofGetWidth() / (NUM_TASKS + 1)) * (i + 1);
		taskParams[i].y = ofGetHeight() / 2;
		taskParams[i].size = 0; // Initial size is 0
		taskParams[i].hue = (255.0f / NUM_TASKS) * i;
		taskParams[i].visible = false; // Initially hidden
	}

	for(int i = 0; i < NUM_TASKS; i++) {
		// Register task ID first (needed for synchronization)
		taskRunner.registerTaskId(task_ids[i]);
	}
	
	// Create each task
	for(int i = 0; i < NUM_TASKS; i++) {
		int taskIndex = i; // Variable for capture

		
		taskRunner.createTaskQueue(task_ids[i], "sync_task")
			.wait_sec(1.0, true) // Wait 1 second (synchronized)
			.then([taskIndex](ofApp& self){
				// Start displaying with small size
				self.taskParams[taskIndex].visible = true;
				self.taskParams[taskIndex].size = 50;
			})
			.wait_sec(1.0, true) // Wait 1 second (synchronized)
			.then([taskIndex](ofApp& self){
				// Set to medium size
				self.taskParams[taskIndex].size = 100;
			})
			.wait_sec(1.0, true) // Wait 1 second (synchronized)
			.then([taskIndex](ofApp& self){
				// Set to large size
				self.taskParams[taskIndex].size = 150;
			})
			.wait_sec(1.0, true) // Wait 1 second (synchronized)
			.then([taskIndex](ofApp& self){
				// Return to small size
				self.taskParams[taskIndex].size = 50;
			})
			.wait_sec(1.0, true) // Wait 1 second (synchronized)
			.then([taskIndex](ofApp& self){
				// Hide (end of animation)
				self.taskParams[taskIndex].visible = false;
			});
	}
}

//--------------------------------------------------------------
void ofApp::update(){
	taskRunner.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	// Clear background
	ofBackground(0);
	
	// Draw based on each task's parameters
	for(int i = 0; i < NUM_TASKS; i++) {
		if(taskParams[i].visible) {
			ofPushStyle();
			
			ofColor color;
			color.setHsb(taskParams[i].hue, 200, 255);
			ofSetColor(color);
			
			float halfSize = taskParams[i].size / 2;
			ofDrawRectangle(
				taskParams[i].x - halfSize, 
				taskParams[i].y - halfSize, 
				taskParams[i].size, 
				taskParams[i].size
			);
			
			ofPopStyle();
		}
	}
	
	// Draw task runner (if needed)
	taskRunner.draw();
	
	// Display explanatory text
	ofSetColor(255);
	ofDrawBitmapString("Synchronized Tasks Example", 20, 20);
	ofDrawBitmapString("All " + ofToString(NUM_TASKS) + " tasks are synchronized", 20, 40);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
