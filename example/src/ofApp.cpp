#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofLogToConsole();
	ofSetVerticalSync(true);
	ofSetFrameRate(60);

	taskRunner.setup(*this);

	taskRunner.createTaskQueue()
		.wait_sec(1.0)
		.then_on_draw([](ofApp& self){
			ofBackground(255, 0, 0);
		})
		.wait_sec(2.0)
		.then_on_draw([](ofApp& self){
			ofBackground(0, 255, 0);
		});
}

//--------------------------------------------------------------
void ofApp::update(){
	taskRunner.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	taskRunner.draw();
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
