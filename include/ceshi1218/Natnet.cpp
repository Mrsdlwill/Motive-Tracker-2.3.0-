#include "NatNet.h"

// 声明回调函数 , C回调函数，不适合做类的成员函数
void NATNET_CALLCONV DataHandler(sFrameOfMocapData* data, void* pUserData);    // receives data from the server
// Frame Queue
typedef struct MocapFrameWrapper
{
	std::shared_ptr<sFrameOfMocapData> data;
	double transitLatencyMillisec;
	double clientLatencyMillisec;
} MocapFrameWrapper;
std::timed_mutex gNetworkQueueMutex;
std::deque<MocapFrameWrapper> gNetworkQueue;
const int kMaxQueueSize = 500;
NatNet::NatNet(QString ip) : m_ip(ip), g_pClient(nullptr) {
	qDebug() << QString("NatNet 构造函数");
	setAutoDelete(false);
}

NatNet::~NatNet() {
	qDebug() << "NatNet析构函数";

	// 安全释放资源
	if (g_pClient) {
		g_pClient->Disconnect();
		delete g_pClient;
		g_pClient = nullptr; // 置空指针
	}
}

void NatNet::run() {
	qDebug() << "连接natnet线程启动:" << QThread::currentThread();
	this->connectFunc();

	// 持续处理数据，直到线程被中断
	while (!QThread::currentThread()->isInterruptionRequested()) {
		this->datarec();
		QThread::msleep(10);
	}

	// 注意：资源释放由析构函数处理，此处不再重复释放
}

void NatNet::connectFunc() {
	// 1. 实例化NatNet客户端   Create NatNet client    
	qDebug() << QString("NatNet client Established");
	g_pClient = new NatNetClient();

	// 2.set the frame callback handler
	g_pClient->SetFrameReceivedCallback(DataHandler, g_pClient);	// this function will receive data from the server

	// 3.配置连接参数 Automatic Motive server discovery
	this->AutomaticSearchServer();//自动搜索配置

	 // 4.Connect to Motive
	int iResult = ConnectClient();
	if (iResult != ErrorCode_OK) {
		qDebug() << QString("Error initializing client. See log for details. Exiting.");
		emit connectFailed();
		return;
	}
	else {
		qDebug() << QString("Client initialized and ready.");
		emit connectSuccess();
	}
}

int NatNet::ConnectClient()
{	// Disconnect from any previous server (if connected)
	g_pClient->Disconnect();

	// Connect to NatNet server (e.g. Motive)
	int retCode = g_pClient->Connect(g_connectParams);
	if (retCode != ErrorCode_OK)
	{
		// Connection failed - print connection error code
		qDebug() << QString("[NatNet Client] Unable to connect to server.  Error code: %1. Exiting.\n").arg(retCode);
		return ErrorCode_Internal;
	}
	return ErrorCode_OK;   // Connection successful and required information retrieved
}

void NatNet::AutomaticSearchServer()
{
	const unsigned int kDiscoveryWaitTimeMillisec = 1 * 1000;   // 延迟1秒响应.
	const int kMaxDescriptions = 10;                            // 最多响应10个服务端 
	sNatNetDiscoveredServer servers[kMaxDescriptions];
	int actualNumDescriptions = kMaxDescriptions;
	int findServerResult;
	findServerResult = NatNet_BroadcastServerDiscovery(servers, &actualNumDescriptions, kDiscoveryWaitTimeMillisec);

	//3. 设置连接参数
	if (servers[0].serverDescription.bConnectionInfoValid) {
		g_connectParams.connectionType = servers[0].serverDescription.ConnectionMulticast ? ConnectionType_Multicast : ConnectionType_Unicast;
		g_connectParams.serverCommandPort = servers[0].serverCommandPort;
		g_connectParams.serverDataPort = servers[0].serverDescription.ConnectionDataPort;
		g_connectParams.serverAddress = servers[0].serverAddress;
		g_connectParams.localAddress = servers[0].localAddress;
		g_connectParams.multicastAddress = nullptr;
	}
}

void NatNet::disConnectClient() {
	if (!g_pClient) return;

	void* response;
	int nBytes;
	int iResult = g_pClient->SendMessageAndWait("Disconnect", &response, &nBytes);

	if (iResult == ErrorCode_OK) {
		qDebug() << "[NatNet Client] Disconnected";
		g_pClient->Disconnect(); // 确保断开连接
		delete g_pClient;
		g_pClient = nullptr;     // 置空指针
	}
	else {
		qDebug() << "[NatNet Client] Error Disconnecting";
	}
}

void NatNet::datarec() {
	std::deque<MocapFrameWrapper> displayQueue;

	// 加锁并交换队列数据
	{
		std::lock_guard<std::timed_mutex> lock(gNetworkQueueMutex);
		displayQueue.swap(gNetworkQueue);
	}

	// 处理数据
	for (MocapFrameWrapper& f : displayQueue) {
		sFrameOfMocapData* data = f.data.get();

		// 深拷贝标记数据
		QVector<sMarker> markers;
		for (int i = 0; i < data->nLabeledMarkers; ++i) {
			markers.push_back(data->LabeledMarkers[i]);
		}
		emit sendMarkerData(markers);

		// 输出调试信息
		qDebug() << "\n=== New Packet Arrived ===";
		qDebug() << "FrameID:" << data->iFrame;
		qDebug() << "Timestamp:" << data->fTimestamp;
		qDebug() << "Params: Markers [Count=" << data->nLabeledMarkers << "]";

		// 输出每个Marker的坐标
		for (int i = 0; i < data->nLabeledMarkers; ++i) {
			sMarker marker = data->LabeledMarkers[i];
			qDebug() << QString("Marker [pos:x=%1, y=%2, z=%3]")
				.arg(marker.x).arg(marker.y).arg(marker.z);
		}
	}

	displayQueue.clear();
}

void NATNET_CALLCONV DataHandler(sFrameOfMocapData* data, void* pUserData) {
	NatNetClient* pClient = (NatNetClient*)pUserData;
	if (!pClient)
		return;

		// 深拷贝数据帧
		auto pDataCopy = std::make_shared<sFrameOfMocapData>();
		*pDataCopy = *data;

		MocapFrameWrapper frameWrapper;
		frameWrapper.data = pDataCopy;

		// 加锁并操作队列
		std::unique_lock<std::timed_mutex> lock(gNetworkQueueMutex, std::try_to_lock);
		if (lock.owns_lock()) {
			gNetworkQueue.push_back(frameWrapper);

			// 控制队列大小
			while (gNetworkQueue.size() > kMaxQueueSize) {
				gNetworkQueue.pop_front();
			}
		}
		else {
			qDebug() << "\nFrame dropped (Frame:" << data->iFrame << ")";
		}
	}