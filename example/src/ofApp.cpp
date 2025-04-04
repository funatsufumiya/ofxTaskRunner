#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofLogToConsole();
	ofSetVerticalSync(true);
	ofSetFrameRate(60);

	// Initialize background color
	backgroundColor = ofColor(0, 0, 0);

	taskRunner.setup(*this);

	// Create a simple task queue that changes background color in 5 stages
	taskRunner.createTaskQueue()
		.wait_sec(1.0)
		.then([this](ofApp& self){
			// Stage 1: Red
			backgroundColor = ofColor(255, 0, 0);
		})
		.wait_sec(1.0)
		.then([this](ofApp& self){
			// Stage 2: Green
			backgroundColor = ofColor(0, 255, 0);
		})
		.wait_sec(1.0)
		.then([this](ofApp& self){
			// Stage 3: Blue
			backgroundColor = ofColor(0, 0, 255);
		})
		.wait_sec(1.0)
		.then([this](ofApp& self){
			// Stage 4: Yellow
			backgroundColor = ofColor(255, 255, 0);
		})
		.wait_sec(1.0)
		.then([this](ofApp& self){
			// Stage 5: Purple
			backgroundColor = ofColor(255, 0, 255);
		})
		.wait_sec(1.0)
		.then([this](ofApp& self){
			// Return to black (end of cycle)
			backgroundColor = ofColor(0, 0, 0);
		});
}

//--------------------------------------------------------------
void ofApp::update(){
	taskRunner.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	// Set background color based on the current task state
	ofBackground(backgroundColor);
	
	// Draw task runner (if needed)
	taskRunner.draw();
	
	// Display explanatory text
	ofSetColor(255);
	ofDrawBitmapString("Simple Task Example", 20, 20);
	ofDrawBitmapString("Background color changes every second", 20, 40);
	ofDrawBitmapString("Press SPACE to restart animation", 20, 60);
	
	// Display current color values
	ofDrawBitmapString("Current color: R:" + ofToString(backgroundColor.r) + 
					  " G:" + ofToString(backgroundColor.g) + 
					  " B:" + ofToString(backgroundColor.b), 20, 80);
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
