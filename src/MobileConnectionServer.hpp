#include "ofxNetwork.h"
#define WAIT_TIME 10

class MobileConnectionServer : public ofThread
{
public:
	MobileConnectionServer() {}
	~MobileConnectionServer() {}

	void threadedFunction();
	void setup(ofxTCPServer tcpServer, int port, bool blocking);
	void receiveImage(ofxTCPServer tcpServer, ofImage &image, int numBytes);
	void sendFile(ofxTCPServer tcpServer, ofFile file, int fileBytesToSend);

private:
	ofxTCPServer tcpServer;
};

void MobileConnectionServer::setup(ofxTCPServer tcpServer, int port, bool blocking)
{
	tcpServer.setup(port, blocking);
}

void MobileConnectionServer::receiveImage(ofxTCPServer tcpServer, ofImage &image, int numBytes)
{
	unsigned char* buffer = new unsigned char[numBytes];
	int recd = numBytes;
	int totalReceived = 0;

	while (totalReceived != numBytes)
	{
		int ret = tcpServer.receiveRawBytes(tcpServer.getLastID() - 1, (char*)&buffer[totalReceived], recd);
		if (ret < 0)
		{
			continue;
		}
		recd -= ret;
		totalReceived += ret;
		cout << totalReceived << endl;
	}
	image.setFromPixels(buffer, 500, 500, OF_IMAGE_COLOR);

	delete[] buffer;
}

void MobileConnectionServer::sendFile(ofxTCPServer tcpServer, ofFile file, int fileBytesToSend)
{
	char* fileSize;
	int totalBytesSent = 0;
	tcpServer.sendRawBytes(tcpServer.getLastID() - 1, (char*)&file.getFileBuffer()[totalBytesSent], fileBytesToSend);
	//tcpServer.sendRawBytes(tcpServer.getLastID() - 1, (char*)&file.readToBuffer()[totalBytesSent], fileBytesToSend);

	//atoi(tcpServer.receive(tcpServer.getLastID() - 1));
}

/*void MobileConnectionServer::threadedFunction()
{
	while (isThreadRunning())
	{
		if (tcpServer.isConnected())
		{

		}
	}
}*/