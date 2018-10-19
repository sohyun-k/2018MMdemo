#pragma once

#include <ofArduino.h>
#include "ofxXmlSettings.h"
//#include "SceneManager.hpp"

#define PAN_PIN 9
#define TILT_PIN 11
#define PAN_DEFAULT 90 //���� 70
#define TILT_DEFAULT 127 //���� 69
#define ORIGINAL_VERSION
//#define MINI_VERSION

// ���� : pan = 34, tilt = -4
// ���� �Ʒ� : pan = 46 tilt = -12
// ������ �ٱ��� : pan = -12 tilt = -2
// ���� �� ����: pan = 2 tilt = -2
// ���� ���� : pan = 64 tilt = 0
// ���� �ٱ��� : pan = 90 tilt = -4
// å�� : pan = 24 tilt = -28

class PanTiltSystem
{
public:
	void setup()
	{
		//pan_pulse = PAN_PULSE_ORIGIN;
		//tilt_pulse = TILT_PULSE_ORIGIN;
		// load xml configuration file
		load();
		COM_PORT = "COM3";
		arduino.connect(COM_PORT, 9600 /*57600*/);
		arduino.sendDigitalPinMode(PAN_PIN, ARD_PWM);
		arduino.sendDigitalPinMode(TILT_PIN, ARD_PWM);

		ofLogNotice("ofArduino") << "device connected";


		this->panAngle = PAN_DEFAULT;
		this->tiltAngle = TILT_DEFAULT;
		this->prePan = PAN_DEFAULT;
		this->preTilt = TILT_DEFAULT;
		cout << "setup PAN = " << this->panAngle << "     setup Tilt = " << this->tiltAngle << endl;
		bSceneChange = false;
		/*this->panAngle = panSwitchPulseAngle(PAN_PULSE_ORIGIN);
		cout << "PanAngle = " << this->panAngle << endl;
		cout << "PAN_PULSE_ORIGIN = " << PAN_PULSE_ORIGIN << endl;
		this->tiltAngle = tiltSwitchPulseAngle(TILT_PULSE_ORIGIN);*/
	}

	void update(int panAngle, int tiltAngle)
	{

		/*cout << "PanAngle: " << panAngle << endl;
		cout << "TiltnAngle: " << tiltAngle << endl;*/
		panAngle = min(max(panAngle, Pan_min), Pan_max);
		tiltAngle = min(max(tiltAngle, Tilt_min), Tilt_max);

		this->panAngle = panAngle;
		this->tiltAngle = tiltAngle;
		arduino.update();
		arduino.sendDigitalPinMode(PAN_PIN, ARD_PWM);
		arduino.sendDigitalPinMode(TILT_PIN, ARD_PWM);
		arduino.sendPwm(PAN_PIN, this->panAngle, true);
		arduino.sendPwm(TILT_PIN, this->tiltAngle, true);

		bSceneChange = false;

	}

	void exit()
	{
		// ���ϸ� ����?
		//initializePose();
	}

	void initializePose()
	{
		this->panAngle = PAN_DEFAULT;
		this->tiltAngle = TILT_DEFAULT;

		update(this->panAngle, this->tiltAngle);
	}

public:
	string	COM_PORT;

	ofArduino	arduino;

	int	panAngle;
	int	tiltAngle;

	int prePan;		// ���� �� ����
	int preTilt;	// ���� ƿƮ ����

	bool bSceneChange;

	int Pan_min, Pan_max;
	int Tilt_min, Tilt_max;

	/************************************************************************/
	/*	Loads/Saves scene information from/to the configuration file		*/
	/************************************************************************/
	bool load(string fileName = "pan_tilt.xml")
	{
		ofxXmlSettings settings;


		if (!settings.loadFile(fileName)) {
			ofLogError("PanTiltSystem: Pan-Tilt configuration file is missing.");
			return false;
		}

		settings.pushTag("pan_tilt_system");
		COM_PORT = settings.getValue("port", "COM3"); //��Ʈ ��ȣ ���� ��
		settings.popTag();

#ifdef MINI_VERSION
		settings.pushTag("PtMiniSystem");
#elif defined ORIGINAL_VERSION
		settings.pushTag("PtOriginalSystem", 0);
#endif

		Pan_min = settings.getValue("Pan_min", 0);
		Pan_max = settings.getValue("Pan_max", 0);
		Tilt_min = settings.getValue("Tilt_min", 0);
		Tilt_max = settings.getValue("Tilt_max", 0);
		settings.popTag();

		return true;
	}
};