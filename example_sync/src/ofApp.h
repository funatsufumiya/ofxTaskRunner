#pragma once

#include "ofMain.h"
#include "ofxTaskRunner.h"

class ofApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		ofxTaskRunner<ofApp> taskRunner;

		std::vector<int> task_ids;
		static const int NUM_TASKS = 5;

		struct TaskParams {
			float x;
			float y;
			float size;
			float hue;
			bool visible;
		};
		
		std::vector<TaskParams> taskParams;
};
