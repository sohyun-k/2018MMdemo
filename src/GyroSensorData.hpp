#pragma once

class GyroSensorData {

private:
	float pitch;
	float roll;
	float yaw;
	int user_index;
	int touch_order;

	// + AR Kit variables
	float cam_pos_x;
	float cam_pos_y;
	float cam_pos_z;

public:
	//void setData(float pitch, float roll, float yaw, int user_index, int touch_order) {
	//	this->pitch = pitch;
	//	this->roll = roll;
	//	this->yaw = yaw;
	//	this->user_index = user_index;
	//	this->touch_order = touch_order;
	//}

	// AR Kit version
	void setData(float pitch, float roll, float yaw, int user_index, int touch_order, float cam_pos_x, float cam_pos_y, float cam_pos_z) {
		this->pitch = pitch;
		this->roll = roll;
		this->yaw = yaw;
		this->user_index = user_index;
		this->touch_order = touch_order;
		
		this->cam_pos_x = cam_pos_x;
		this->cam_pos_y = cam_pos_y;
		this->cam_pos_z = cam_pos_z;
	}

	GyroSensorData getData() {
		GyroSensorData temp_data;
		temp_data.setData(this->pitch, this->roll, this->yaw, this->user_index, this->touch_order, this->cam_pos_x, this->cam_pos_y, this->cam_pos_z);
		return temp_data;
	}

	float getPitch() {
		return this->pitch;
	}

	float getRoll() {
		return this->roll;
	}

	float getYaw() {
		return this->yaw;
	}

	int getUserIndex() {
		return this->user_index;
	}

	int getTouchOrder() {
		return this->touch_order;
	}

	float getCamPosX() {
		return this->cam_pos_x;
	}
	
	float getCamPosY() {
		return this->cam_pos_y;
	}

	float getCamPosZ() {
		return this->cam_pos_z;
	}

	void setTouchOrder(int touch_order) {
		this->touch_order = touch_order;
	}
};