#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_hostCom.h"
#include <QMenuBar>
#include <QStatusBar>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QMessageBox>
#include <QTimer>
#include <QDateTime>
#include <QString>
#include <QCloseEvent>
#include <QDockWidget>
#include <QPlainTextEdit>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFormLayout>
#include <QLineEdit>
#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkInterface>
#include <QTabWidget>
#include <QGridLayout>
#include <QTextEdit>
#include <QFileDialog>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QSplineSeries>
#include <QValueAxis>
#include <QRandomGenerator>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QMouseEvent>
#include <QCheckBox>
#include <QtDataVisualization>
#include <QComboBox>
#include <QFile>
#include <QTextStream>
#include <QThreadPool>
#include <limits>

#include "cpumemorylabel.h"
#include "SpeedChangeThread.h"
#include "SaveDataThread.h"
#include "MyTcpServer.h"
#include "MyATISocket.h"
#include "LogHandler.h"
#include "MyQControl.h"
#include "dataParsing.h"
#include "ringBuffer.h"
#include "NatNet.h"

#define AXIS_MIN_X 0
#define AXIS_MAX_X 50
#define AXIS_MIN_Y -20
#define AXIS_MAX_Y 20

QT_BEGIN_NAMESPACE
namespace Ui { class hostCom2Class; }
class QChart;
class QChartView;
class QValueAxis;
class QLineSeries;
QT_END_NAMESPACE

class HostCom : public QMainWindow
{
	Q_OBJECT

public:
	HostCom(QWidget* parent = nullptr);
	~HostCom();

	// 初始化成员函数
	void initialWindow();                       // 整体窗口初始化
	void initialCentralWidget();                // 中心窗口初始化
	void initialMenuBar();                      // 菜单栏初始化
	void initialStatusBar();                    // 状态栏初始化
	void initialDockLog();                      // 日志输出DockWidget初始化
	void initialDockSet();                      // 设置面板DockWidget初始化
	void initialDockOpe();                      // 操作面板DockWidget初始化
	void initialTcpClient();                    // 通信客户端初始化
	void initialTabParam();                     // 参数选项卡初始化
	void initialTabDastr();                     // 数据选项卡初始化
	void initialTabChart();                     // 图表选项卡初始化
	void initialTabOptit();                     // 动捕选项卡初始化
	void initialLineSeries();                   // 图表数据对象初始化
	void initialAxis();                         // 图表坐标轴初始化
	void initialCheckBox(QVBoxLayout* vlay);    // 图表复选框初始化
	void initGraph3D();                         // 3D散点图初始化
	void initialThread();                       // 线程初始化
	void initialSysParam();                     // 初始化系统变量

	// 服务器端成员函数
	QString getLocalIpAddr();                   // 读取本地IP地址
	void conSerButtonClicked();                 // 连接服务器按钮槽函数
	QString Ipv4Addr(QHostAddress addr);        // 将ip地址格式固定为IPv4模式，然后再返回
	bool slotTryCreateTcpServer();              // 启动服务器
	void tcpServerDataReceived(QVector<float>, uint8_t);// 读取数据
	void addClientLink(QString, int);           // 添加客户端连接
	void removeClientLink(QString, int);        // 移除连接

	// 客户端成员函数
	void conCliButtonClicked();                 // 连接ATI按钮槽函数
	void atiSendSlots();                        // 发送按钮定时器
	void updateATIData(QVector<double>);        // 更新主程ATI数据

	// 覆写系统函数
	void closeEvent(QCloseEvent* e);            // 覆写closeEvent函数
	void wheelEvent(QWheelEvent* e);            // 覆写wheelEvent鼠标滚轮事件
	bool eventFilter(QObject* obj, QEvent* event);	// 添加事件过滤器声明

	// 槽函数
	void showTime();                            // 状态栏时间
	void updateChartData(QVector<double>);      // 更新图表数据槽函数
	void clearChartView();                      // 清空图表显示
	void saveChartView();                       // 保存曲线
	QString getTimeStr();                       // 获取时间戳
	void speedChangeFinish();                   // 变速线程结束槽函数
	void saveDataFinish(QString infoStr);       // 数据导出结束槽函数


	void connectToNatnetBtnSlots();             // 连接动捕槽函数
	void parseBtnSlots();                       // 解析动捕槽函数
	void getNatNet(sFrameOfMocapData* data);      // 获取LuMoSDK传来的数据

	void initOrangePi();                        // 初始化香橙派

public:
	QScatterDataArray* dataArray;               // 为实现数据共享，公共化参数
	QScatterDataItem* ptrToDataArray;

	// 自定义信号
signals:
	void sendParam(int min, int max, int time);
	void sendRawData(QString data, QString rootDir);
	void serverSendData(QByteArray command);

	void startConnect(QString, unsigned short);

	void cutConnect();
	void startSendData(QByteArray);

private:
	Ui::hostCom2Class ui;

	// 菜单栏私有成员属性
	QMenu* pFilemenu;                           // 文件菜单
	QMenu* pViewmenu;                           // 查看菜单
	QMenu* pEditmenu;                           // 编辑菜单
	QMenu* pHelpmenu;                           // 帮助菜单
	QAction* actioncontact;                     // 帮助菜单下的菜单项

	// 状态栏私有成员属性
	QTimer* statustimer;                        // 新建一个时间定时器
	QLabel* labelTemInfo;                       // 临时信息标签
	QLabel* labeltimeIcon;                      // 时间图片标签
	QLabel* labeltimeFont;                      // 新建显示时间的标签
	QLabel* labelConnectIcon;                   // 连接状态图片标签
	QLabel* labelConnectFont;                   // 连接信息文本标签
	QLabel* labelCpuIcon;                       // CPU图片标签
	QLabel* cpuScript;                          // CPU文字信息
	CpuMemoryLabel* labelCpuFont;               // CPU内存控件
	QPixmap* disconnectIcon;                    // 未连接图片信息
	QPixmap* connectIcon;                       // 已连接图片信息
	QPixmap* timeIcon;                          // 时间图片信息
	QPixmap* cpuIcon;                           // cpu图片信息
	QLabel* labelServerIcon;                    // 服务器状态图标
	QLabel* labelServerFont;                    // 服务器状态文字
	QPixmap* serverCloseIcon;                   // 图片信息
	QPixmap* serverOpenIcon;                    // 图片信息

	// 日志输出Dock私有成员属性
	QDockWidget* dockLog;                       // 日志DockWidget控件
	QPlainTextEdit* logPlainText;               // 文本输出控件

	// 设置Dock私有成员属性
	QDockWidget* dockSet;                       // 设置面板dockSet控件
	QWidget* dockSetWidget;                     // 承载多个控件的容器
	QFormLayout* formDockSet;                   // 表单布局控件
	QLineEdit* ipEdit;                          // 文本框控件
	QLineEdit* portEdit;
	QLineEdit* serAddrEdit;
	QLineEdit* serPortEdit;
	QLineEdit* periodEdit;
	QLineEdit* natIpEdit;
	QLineEdit* minValEdit;                      // 最小速度
	QLineEdit* maxValEdit;                      // 最大速度
	QPushButton* speedChange;                   // 变速按钮
	QLineEdit* intervalEdit;                    // 变速间隔

	// 操作Dock私有成员属性
	QDockWidget* dockOpe;                       // 操作DockWidget控件
	QLabel* lock;                               // 闭锁/解锁图标
	SpinBoxAndSlider* throControl;              // 扑翼油门控制
	SpinBoxAndSlider* spinSpeed;                // 转台速度控制
	QLabel* m_initial;                          // 初始化图标
	bool m_stateEnable;                         // 初始化/使能状态

	// MyTcpServer私有成员属性 
	MyTcpServer* m_TcpServer;                   // 服务器对象
	QHostAddress lhAddr;                        // 监听地址
	int lhPort;                                 // 监听端口
	QList<int> m_clientList;                    // 客户端连接记录数组
	qint8 m_clientCnt;                          // 客户端连接数量记录

	// 客户端私有成员属性
	bool m_state;                               // 按钮翻转标志位
	QTimer* atiTimer;                           // ati发送数据定时器
	MyATISocket* worker;                        // ati客户端任务函数类
	QThread* t;                                 // ati客户端子线程对象
	bool isBias;                                // ATI数据是否偏置标志位

	// 按钮对象
	QPushButton* connectToServer;               // 连接服务器按钮
	QPushButton* connectToATI;                  // 连接ATI按钮
	QPushButton* atiSend;                       // ATI发送按钮
	QPushButton* connectToNatnet;               // 连接动捕系统
	QPushButton* biasBtn;                       // ATI偏置按钮
	QPushButton* parseBtn;                      // 动捕参数解析按钮

	// 标志位
	bool serverFlag;                            // 连接服务器按钮翻转标志位
	bool loggingFlag;                           // 数据导出按钮翻转标志位
	bool isunlockFlag;                          // 电调是否解锁
	bool isNatNetConnect;                        // 连接动捕标志位

	// 中心窗口私有成员属性
	QTabWidget* tabWidget;                      // 选项卡控件
	QWidget* tabParam;                          // 参数选项卡
	QWidget* tabDastr;                          // 数据流选项卡
	QWidget* tabChart;                          // 图表选项卡
	QWidget* tabOptit;                          // 动捕选项卡

	// 参数选项卡私有成员属性
	QLineEdit* volEdit;                         // 电压文本框
	QLineEdit* curEdit;                         // 电压
	QLineEdit* powEdit;                         // 功耗
	QLineEdit* freEdit;                         // 扑频
	QLineEdit* noiEdit;                         // 噪声
	QLineEdit* geaEdit;                         // 齿轮比
	QLineEdit* temEdit;                         // 温度
	QLineEdit* angEdit;                         // 攻角
	QLineEdit* winEdit;                         // 风速
	QLineEdit* offEdit;                         // 偏移角
	QLineEdit* atiXEdit;                        // ATI三轴力
	QLineEdit* atiYEdit;
	QLineEdit* atiZEdit;
	QLineEdit* mXEdit;                          // ATI三轴扭矩
	QLineEdit* mYEdit;
	QLineEdit* mZEdit;
	QLineEdit* strEdit;                         // 扑动行程
	QLineEdit* edgEdit;                         // 前沿杆长
	QLineEdit* fleEdit;                         // 柔性形变

	// 数据流选项卡私有成员属性
	QTextEdit* dataEdit;                        // 数据流展示文本框
	QPushButton* saveDataButton;                // 数据保存按钮
	QString m_rootDir;                          // 数据保存根目录

	// 图表选项卡私有成员属性
	QChart* m_Chart;                            // 图表对象
	QChartView* m_chartView;                    // 图表展示对象，用于输出展示
	QValueAxis* m_axisX;                        // X坐标轴
	QValueAxis* m_axisY;                        // Y坐标轴
	QLineSeries* m_lineSeries[6];               // 存储每一数据对象
	qint64 pointCount;                          // 记录曲线上点的数量

	// 动捕选项卡私有成员属性
	QWidget* graphContainer;                    // 图表的容器
	Q3DScatter* graph3D;                        // 散点图
	QScatterDataProxy* proxy;                   // 数据代理
	QScatter3DSeries* series;                   // 散点序列
	NatNet* taskConnect;                         // 动捕对象
	std::mutex dataMutex;  // 保护MocapData及buffer的互斥量


	// 子线程对象
	SpeedChangeThread* speedChangeTask;         // 变速线程任务类
	SaveDataThread* saveDataTask;               // 保存数据任务类

	// 环形缓冲区
	RingBuffer* buffer;

	// 仿生翅翼结构参数
	float minZ, maxZ;
	float wingStroke;                           // 扑翼行程
	float leadingEdge;                          // 前沿杆长
	float flexDeformation;                      // 柔性形变
};
