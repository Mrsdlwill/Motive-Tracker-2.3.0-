#pragma once

#include <QtWidgets/QMainWindow>
#include <QThreadPool>
#include <QScatterDataArray>
#include <QPlainTextEdit>
#include <QMutex>
#include <QTimer>
#include <algorithm>
#include <QFileDialog>

#include "ui_ceshi1218.h"
#include "Natnet.h"
#include "ringBuffer.h"
#include "SaveDataThread.h"



class ceshi1218 : public QMainWindow
{
	Q_OBJECT

public:
	ceshi1218(QWidget* parent = nullptr);
	~ceshi1218();

	void on_connectBtn_clicked();

	void parseBtnSlots();

	//void getNatNet(sFrameOfMocapData* MocapData);
	void getNatNet(const QVector<sMarker>& markers);
	void initialTabDastr();


private:
	Ui::ceshi1218Class ui;
	bool m_state;

	NatNet* taskconnect;

	// 环形缓冲区
	RingBuffer* buffer;

	// 仿生翅翼结构参数
	float minZ, maxZ;

	QTimer* m_updateTimer;

private:
	QString m_rootDir;                          // 数据保存根目录

	SaveDataThread* saveDataTask ;

// 自定义信号
signals:
	void sendRawData(QString data, QString rootDir);

};
