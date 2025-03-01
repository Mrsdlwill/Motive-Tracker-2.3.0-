#pragma once
#include <QObject>
#include <QRunnable>
#include <QDebug>
#include <QThread>

#include <memory>  // 引入 shared_ptr 的定义
#include <deque>

#include <NatNetTypes.h>
#include <NatNetCAPI.h>
#include <NatNetClient.h>

class NatNet :public QObject, public QRunnable
{
	Q_OBJECT

public:
	NatNet(QString ip);
	~NatNet();

	void run() override;
	// NatNet 成员函数
	void connectFunc();                  // 全流程连接函数
	int ConnectClient();				// 连接至NatNet服务器
	void AutomaticSearchServer();
	void disConnectClient();				// 断开NatNet服务器

	void datarec();		//接收数据
signals:
	void connectSuccess();
	void connectFailed();
	//void sendMarkerData(sFrameOfMocapData* data);

	void sendMarkerData(const QVector<sMarker>& markers); // 使用值拷贝而非指针
private:
	QString m_ip;
	NatNetClient* g_pClient;                            //NatNet客户端指针
	sNatNetClientConnectParams g_connectParams; // 连接参数
};

