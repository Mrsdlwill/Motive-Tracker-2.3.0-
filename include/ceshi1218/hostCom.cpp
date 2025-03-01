#pragma once
#include "hostCom.h"

/************************************************************************************************************
 *                                                                                                          *
 *                                             HostCom -- 主界面UI                                          *
 *                                                                                                          *
 ***********************************************************************************************************/

 // 构造函数
HostCom::HostCom(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	// 日志系统初始化
	LogHandler::Get().installMessageHandler();                                          // [[1]] 安装消息处理函数
	qInfo() << "*******************************************************************";   // [[2]] 输出日志头信息
	qInfo() << "系统启动时间 : " << QTime::currentTime().toString("hh:mm:ss");

	// 整体窗口初始化
	this->initialWindow();

	// 状态栏初始化
	this->initialStatusBar();

	// DockLog初始化
	this->initialDockLog();

	// DockSet初始化
	this->initialDockSet();

	// DockOpe初始化
	this->initialDockOpe();

	// 菜单栏初始化
	this->initialMenuBar();

	// 中心窗口初始化
	this->initialCentralWidget();

	// 通信服务端、客户端初始化
	this->initialTcpClient();

	// 线程初始化
	this->initialThread();

	// 系统状态变量初始化
	this->initialSysParam();

	qInfo() << "系统UI初始化完成";
}

// 析构函数
HostCom::~HostCom() {
	/*
		********************析构需知*************************
		1.为避免内存泄露，我们需要对构造的内容进行释放
		当一个类比较简单时，不写析构函数，使用系统默认生成的默认析构函数也是可以的
		当一个类比较复杂时，或者类中存在以下情况时，往往需要手动释放：打开文件、动态分配内存、连接数据库、复杂子类构造、线程分配
		简单判断就是当类中使用new进行内存分配时需要注意内存泄露问题
		2.Qt内存自动释放,Qt内存自动释放有两个前提条件：
			必须是QObject的派生类
			必须指定了parent对象
	*/
	disconnectIcon = nullptr;   // 指针置空
	delete disconnectIcon;      // 删除指针
	connectIcon = nullptr;
	delete connectIcon;
	timeIcon = nullptr;
	delete timeIcon;
	cpuIcon = nullptr;
	delete cpuIcon;

	// 日志系统关闭
	qInfo() << "系统资源释放完毕";
	qInfo() << "系统关闭时间 : " << QTime::currentTime().toString("hh:mm:ss");          // [[3]] 程序结束时释放 LogHandler 的资源，例如刷新并关闭日志文件
	LogHandler::Get().uninstallMessageHandler();
}

/************************************************************************************************************
 *                                                                                                          *
 *                                                   初始化函数                                             *
 *                                                                                                          *
 ***********************************************************************************************************/

 // 整体窗口初始化
void HostCom::initialWindow() {
	this->setWindowTitle("测试平台上位机系统2.0");                   // 设置窗口标题
	this->resize(1200, 800);                                         // 划定初始大小
	this->setWindowIcon(QIcon(":/HostCom/res/flap.png"));            // 设置窗口图标
}

// 中心窗口初始化
void HostCom::initialCentralWidget() {
	tabWidget = new QTabWidget();
	setCentralWidget(tabWidget);

	// 各选项卡页面初始化
	tabParam = new QWidget(this);
	tabDastr = new QWidget(this);
	tabChart = new QWidget(this);
	tabOptit = new QWidget(this);

	// tabWidget设置
	tabWidget->setTabPosition(QTabWidget::North);               // 设置选项卡的方位，默认在上方
	tabWidget->setTabShape(QTabWidget::Triangular);             // 设置选项卡的形状
	tabWidget->setIconSize(QSize(30, 30));                      // 设置图标的大小(选项卡的大小也会改变)
	tabWidget->setMovable(true);                                // 允许选项卡移动 
	//tabWidget->setTabsClosable(true);                         // 在选项卡上添加关闭按钮，选项卡较多时应适当增加关闭选项

	this->initialTabParam();            // 参数选项卡初始化
	this->initialTabDastr();            // 数据选项卡初始化
	this->initialTabChart();            // 图表选项卡初始化
	this->initialTabOptit();            // 动捕选项卡初始化

	tabWidget->addTab(tabParam, QIcon(":/HostCom/res/iconParam.png"), tr("性能参数"));               // 在后面添加选项卡
	tabWidget->addTab(tabDastr, QIcon(":/HostCom/res/iconDastr.png"), tr("数据流"));
	tabWidget->addTab(tabChart, QIcon(":/HostCom/res/iconChart.png"), tr("图表"));
	tabWidget->addTab(tabOptit, QIcon(":/HostCom/res/iconOptit.png"), tr("动捕系统"));

	tabWidget->setTabToolTip(0, tr("测试过程中各个传感器传回的数据"));  // 鼠标悬停弹出提示
	tabWidget->setTabToolTip(1, tr("记录测试过程的大数据流"));
	tabWidget->setTabToolTip(2, tr("测试过程中重要参数指标的图表"));
	tabWidget->setTabToolTip(3, tr("动捕系统展示交互页面"));
}

// 状态栏初始化
void HostCom::initialStatusBar() {
	// 全局设置
	ui.statusBar->setMaximumHeight(30);
	ui.statusBar->setSizeGripEnabled(false);    // 去除右下小三角
	ui.statusBar->setStyleSheet(QString("QStatusBar::item{border: 0px}")); // 不显示边框

	// 变量初始化
	statustimer = new QTimer(this);             // 指定了父对象,程序结束时会随着父对象删除而删除
	labelTemInfo = new QLabel(this);            // 创建时不指定父对象，后序可以手动指定 label->setParent(this);
	labeltimeIcon = new QLabel(this);
	labeltimeFont = new QLabel(this);
	labelConnectIcon = new QLabel(this);
	labelConnectFont = new QLabel(this);
	labelCpuIcon = new QLabel(this);
	cpuScript = new QLabel(this);
	labelCpuFont = new CpuMemoryLabel(this);
	labelServerIcon = new QLabel(this);
	labelServerFont = new QLabel(this);
	disconnectIcon = new QPixmap(":/HostCom/res/DisConnect.png");
	connectIcon = new QPixmap(":/HostCom/res/Connect.png");
	timeIcon = new QPixmap(":/HostCom/res/time.png");
	cpuIcon = new QPixmap(":/HostCom/res/iconCpu.png");
	serverCloseIcon = new QPixmap(":/HostCom/res/serverNotWork.png");
	serverOpenIcon = new QPixmap(":/HostCom/res/serverWork.png");

	// 临时信息显示
	labelTemInfo->setText(QString("This is Temporary information!"));
	ui.statusBar->addWidget(labelTemInfo);

	// 系统Cpu显示
	labelCpuIcon->setMaximumSize(30, 30);               // 图片
	labelCpuIcon->setScaledContents(true);
	labelCpuIcon->setPixmap(*cpuIcon);
	ui.statusBar->addPermanentWidget(labelCpuIcon);
	cpuScript->setMinimumWidth(200);
	ui.statusBar->addPermanentWidget(cpuScript);
	connect(labelCpuFont, SIGNAL(textChanged(QString)), cpuScript, SLOT(setText(QString)));
	labelCpuFont->lower();
	labelCpuFont->start(1000);

	// 服务器状态显示
	labelServerIcon->setMaximumSize(30, 30);           // 图片
	labelServerIcon->setScaledContents(true);
	labelServerIcon->setPixmap(*serverCloseIcon);
	ui.statusBar->addPermanentWidget(labelServerIcon);
	labelServerFont->setText(QString("服务器关闭"));    // 文字
	labelServerFont->setMinimumWidth(100);
	ui.statusBar->addPermanentWidget(labelServerFont);

	// 连接状态显示
	labelConnectIcon->setMaximumSize(30, 30);                // 图片
	labelConnectIcon->setScaledContents(true);
	labelConnectIcon->setPixmap(*disconnectIcon);
	ui.statusBar->addPermanentWidget(labelConnectIcon);
	labelConnectFont->setText(QString("未连接"));            // 文字
	labelConnectFont->setMinimumWidth(50);
	ui.statusBar->addPermanentWidget(labelConnectFont);

	// 时间显示
	labeltimeIcon->setMaximumSize(30, 30);              // 图片
	labeltimeIcon->setScaledContents(true);
	labeltimeIcon->setPixmap(*timeIcon);
	ui.statusBar->addPermanentWidget(labeltimeIcon);
	ui.statusBar->addPermanentWidget(labeltimeFont);    // 文字
	connect(statustimer, &QTimer::timeout, this, &HostCom::showTime);   // 信号连接 每一秒执行一次槽函数
	statustimer->start(1000);
}

// 日志输出DockWidget初始化
void HostCom::initialDockLog() {
	// 变量初始化
	dockLog = new QDockWidget(tr("日志"), this);
	logPlainText = new QPlainTextEdit(this);

	// dock设置
	dockLog->setFeatures(QDockWidget::DockWidgetClosable);                                  // 设置dock属性
	dockLog->setAllowedAreas(Qt::BottomDockWidgetArea);                                     // 设置可允许区域
	this->addDockWidget(Qt::BottomDockWidgetArea, dockLog, Qt::Orientation::Vertical);      // 初始化显示在窗口底部

	// 文本框设置
	logPlainText->setPlaceholderText("日志信息");                                           // 提示占位文本
	connect(logPlainText, &QPlainTextEdit::textChanged, [=]() {
		logPlainText->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);                // 设置光标到文本末尾   
		});

	dockLog->setWidget(logPlainText);                                                       // 向dock中添加文本控件
	if (!logPlainText->isReadOnly())                                                        // 设置当前文本控件只读
		logPlainText->setReadOnly(true);
}

// 设置面板DockWidget初始化
void HostCom::initialDockSet() {
	dockSet = new QDockWidget("设置", this);
	dockSetWidget = new QWidget(dockSet);               // 添加多个控件时需要一个widget作为载体，否在控件不显示
	formDockSet = new QFormLayout(dockSetWidget);       // 表单布局控件
	dockSetWidget->setMaximumWidth(230);

	// 控件初始化
	ipEdit = new QLineEdit(this);
	ipEdit->setText(this->getLocalIpAddr());
	portEdit = new QLineEdit("8080", this);
	serAddrEdit = new QLineEdit("192.168.217.1", this);
	serPortEdit = new QLineEdit("49151", this);
	periodEdit = new QLineEdit("1000", this);
	natIpEdit = new QLineEdit("127.0.0.1", this);
	minValEdit = new QLineEdit("0", this);
	maxValEdit = new QLineEdit("0", this);
	intervalEdit = new QLineEdit("1000", this);

	// dock设置
	dockSet->setFeatures(QDockWidget::DockWidgetClosable);                                  // 设置dock属性
	dockSet->setAllowedAreas(Qt::LeftDockWidgetArea);                                       // 设置可允许区域
	this->addDockWidget(Qt::LeftDockWidgetArea, dockSet, Qt::Orientation::Vertical);        // 初始化显示在窗口左侧

	// 标题分隔栏
	IconTitleDivider* serDivider = new IconTitleDivider(this, "* 服务器 *");
	formDockSet->addRow(serDivider);

	// 表单布局控件初始化
	formDockSet->setRowWrapPolicy(QFormLayout::DontWrapRows);                               // 设置标签始终在控件左侧
	formDockSet->addRow("&本机地址    :", ipEdit);
	formDockSet->addRow("&本地端口    :", portEdit);
	formDockSet->addRow("&服务器地址 : ", serAddrEdit);
	formDockSet->addRow("&服务器端口 : ", serPortEdit);
	formDockSet->addRow("&发送周期    :", periodEdit);
	formDockSet->addRow("&动捕地址    :", natIpEdit);
	formDockSet->setSpacing(10);

	// 服务器按钮
	QPushButton* refreshAddr = new QPushButton("刷新");
	connect(refreshAddr, &QPushButton::clicked, [=]() {
		this->ipEdit->setText(this->getLocalIpAddr());
		});

	connectToServer = new QPushButton("启动服务器");
	connect(connectToServer, &QPushButton::clicked, this, &HostCom::conSerButtonClicked);
	QHBoxLayout* hlayServer = new QHBoxLayout();
	hlayServer->addWidget(refreshAddr);
	hlayServer->addWidget(connectToServer);
	formDockSet->insertRow(3, hlayServer);

	// 标题分隔栏
	IconTitleDivider* cliDivider = new IconTitleDivider(this, "* 客户端 *");
	formDockSet->insertRow(4, cliDivider);

	// ATI按钮
	atiSend = new QPushButton("发送");
	connect(atiSend, &QPushButton::clicked, this, &HostCom::atiSendSlots);
	biasBtn = new QPushButton("初始化");
	connect(biasBtn, &QPushButton::clicked, this, &HostCom::initOrangePi);
	connectToATI = new QPushButton("连接ATI");
	connect(connectToATI, &QPushButton::clicked, this, &HostCom::conCliButtonClicked);
	QHBoxLayout* hlayATI = new QHBoxLayout();
	hlayATI->addWidget(atiSend);
	hlayATI->addWidget(biasBtn);
	hlayATI->addWidget(connectToATI);
	formDockSet->insertRow(8, hlayATI);

	// 标题分隔栏
	IconTitleDivider* optDivider = new IconTitleDivider(this, "* 动捕系统 *");
	formDockSet->insertRow(9, optDivider);

	// 动捕按钮
	connectToNatnet = new QPushButton("连接");
	connect(connectToNatnet, &QPushButton::clicked, this, &HostCom::connectToNatnetBtnSlots);
	parseBtn = new QPushButton("解析");
	connect(parseBtn, &QPushButton::clicked, this, &HostCom::parseBtnSlots);
	QHBoxLayout* hlayFZ = new QHBoxLayout();
	hlayFZ->addWidget(connectToNatnet);
	hlayFZ->addWidget(parseBtn);
	formDockSet->insertRow(11, hlayFZ);

	// 标题分隔栏
	IconTitleDivider* speDivider = new IconTitleDivider(this, "* 变速设置 *");
	formDockSet->addRow(speDivider);

	// 变速功能
	QHBoxLayout* hlaySpeed = new QHBoxLayout();
	QLabel* labelSpeed = new QLabel("速度变化    :");
	QLabel* labelBar = new QLabel("--");
	hlaySpeed->addWidget(labelSpeed);
	hlaySpeed->addWidget(minValEdit);
	hlaySpeed->addWidget(labelBar);
	hlaySpeed->addWidget(maxValEdit);
	formDockSet->addRow(hlaySpeed);                         // 变速范围控件
	formDockSet->addRow("&变速间隔    :", intervalEdit);   // 变速间隔控件
	speedChange = new QPushButton("变速");                  // 变速按钮控件
	connect(speedChange, &QPushButton::clicked, [&]() {
		if (isunlockFlag) {
			int minVal = minValEdit->text().toInt();
			int maxVal = maxValEdit->text().toInt();
			if (minVal < 0 || maxVal > 100 || maxVal <= minVal)
				logPlainText->appendPlainText("[Speed Info]\t变速范围设置不合理");
			else {
				int timeInterval = intervalEdit->text().toInt();            // 获取时间间隔
				if (timeInterval <= 0)
					logPlainText->appendPlainText("[Speed Info]\t变速间隔设置不合理");
				else {
					speedChange->setEnabled(false);
					logPlainText->appendPlainText("[Speed Info]\t上位机数据控制指令发送中……");
					emit sendParam(minVal, maxVal, timeInterval);         // 初始化变速线程任务类
					QThreadPool::globalInstance()->start(speedChangeTask);  // 将变速线程任务扔入线程池中自动运行
				}
			}
		}
		else
			logPlainText->appendPlainText("[Speed Info]\t未解锁，无法变速");
		});
	formDockSet->addRow("", speedChange);
	dockSet->setWidget(dockSetWidget);
}

// 操作面板DockWidget初始化
void HostCom::initialDockOpe() {
	dockOpe = new QDockWidget("操作", this);

	// dockOpe设置
	dockOpe->setFeatures(QDockWidget::DockWidgetClosable);                                  // 设置dock属性
	dockOpe->setAllowedAreas(Qt::RightDockWidgetArea);                                      // 设置可允许区域
	this->addDockWidget(Qt::RightDockWidgetArea, dockOpe, Qt::Orientation::Vertical);       // 初始化显示在窗口左侧

	QWidget* dockOpeWidget = new QWidget(dockOpe);
	QVBoxLayout* vlayOpe = new QVBoxLayout(dockOpeWidget);                                  // 新建垂直布局

	throControl = new SpinBoxAndSlider(this, "油门控制");
	connect(throControl, &SpinBoxAndSlider::sendValueChange, [&](int val) {
		if (isunlockFlag) {                                                                 // 已解锁
			qDebug() << "扑翼油门已更新 : " << val;
			logPlainText->appendPlainText("[Speed Info]\t油门已更新");
			QByteArray data = QByteArray::fromHex(QByteArray::number(val, 16));            // 返回副本
			char throVal[] = { '\x2C' , '\xE4' , '\x04' , '\x03' , '\x00' , '\x00' };
			QByteArray throSendArr(throVal, sizeof(throVal));
			throSendArr.append(data);
			throSendArr.append('\x38');
			throSendArr.append('\xFC');
			emit serverSendData(throSendArr);
		}
		else {
			logPlainText->appendPlainText("[Speed Info]\t电调未解锁，油门更新失败!");
			qWarning() << "油门未解锁，油门更新失败";
		}
		});
	vlayOpe->addWidget(throControl);

	spinSpeed = new SpinBoxAndSlider(this, "转速控制");
	connect(spinSpeed, &SpinBoxAndSlider::sendValueChange, [&](int val) {
		if (isunlockFlag) {                                                                 // 已解锁
			qDebug() << "转台转速已更新 : " << val;
			logPlainText->appendPlainText("[Speed Info]\t转速已更新");
			QByteArray data = QByteArray::fromHex(QByteArray::number(val, 16));
			char speedVal[] = { '\x2C' , '\xE3' , '\x04' , '\x04' , '\x00' , '\x00' };
			QByteArray speedSendArr(speedVal, sizeof(speedVal));
			speedSendArr.append(data);
			speedSendArr.append('\x38');
			speedSendArr.append('\xFC');
			emit serverSendData(speedSendArr);
		}
		else {
			logPlainText->appendPlainText("[Speed Info]\t电调未解锁，转速更新失败!");
			qWarning() << "电调未解锁，转速更新失败";
		}
		});
	vlayOpe->addWidget(spinSpeed);

	QHBoxLayout* hlayIcon = new QHBoxLayout(this);

	m_initial = new QLabel(this);
	m_initial->installEventFilter(this);
	m_initial->setMaximumSize(30, 30);                                                      // 设置图片
	m_initial->setScaledContents(true);
	m_initial->setPixmap(QPixmap(":/HostCom/res/initial.png"));

	lock = new QLabel(this);
	lock->installEventFilter(this);                                                         // 为图标安装鼠标点击事件过滤器
	lock->setMaximumSize(30, 30);                                                           // 设置图片
	lock->setScaledContents(true);
	lock->setPixmap(QPixmap(":/HostCom/res/lock.png"));

	hlayIcon->addWidget(m_initial);
	hlayIcon->addWidget(lock);

	vlayOpe->addLayout(hlayIcon);

	dockOpe->setWidget(dockOpeWidget);
}

// 菜单栏初始化
void HostCom::initialMenuBar() {
	pFilemenu = ui.menuBar->addMenu("File");
	pViewmenu = ui.menuBar->addMenu("View");
	pEditmenu = ui.menuBar->addMenu("Edit");
	pHelpmenu = ui.menuBar->addMenu("Help");

	// View菜单项
	pViewmenu->addAction(dockLog->toggleViewAction());
	pViewmenu->addAction(dockSet->toggleViewAction());
	pViewmenu->addAction(dockOpe->toggleViewAction());

	// Help菜单项
	actioncontact = pHelpmenu->addAction("Contact us");
	connect(actioncontact, &QAction::triggered, this, [=]() {
		QMessageBox msgbox;
		msgbox.setIconPixmap(QPixmap(":/HostCom/res/QRcode.png"));
		msgbox.setWindowTitle("Contact Us");
		msgbox.setStandardButtons(QMessageBox::Ok);
		msgbox.exec();
		});
}

// 参数选项卡初始化
void HostCom::initialTabParam() {
	// 参数选项卡初始化
	QGridLayout* gridLayout = new QGridLayout(tabParam);        // 初始化对象，并指定父对象为tabParam
	gridLayout->setSpacing(10);                                 // 设置水平和竖直方向间距,可根据需要分别设置
	gridLayout->setOriginCorner(Qt::TopLeftCorner);             // 设置坐标的左上角为坐标原点,计数从0开始
	// 第一列
	QLabel* labelVol = new QLabel("电压(V)");                   // 依次向网格中添加控件
	gridLayout->addWidget(labelVol, 0, 0);
	volEdit = new QLineEdit("0", this);
	gridLayout->addWidget(volEdit, 0, 1);

	QLabel* labelCur = new QLabel("电流(A)");
	gridLayout->addWidget(labelCur, 1, 0);
	curEdit = new QLineEdit("0", this);
	gridLayout->addWidget(curEdit, 1, 1);

	QLabel* labelPow = new QLabel("功耗(W)");
	gridLayout->addWidget(labelPow, 2, 0);
	powEdit = new QLineEdit("0", this);
	gridLayout->addWidget(powEdit, 2, 1);

	QLabel* labelFre = new QLabel("扑频(Hz)");
	gridLayout->addWidget(labelFre, 3, 0);
	freEdit = new QLineEdit("0", this);
	gridLayout->addWidget(freEdit, 3, 1);

	// 第二列
	QLabel* labelNoi = new QLabel("噪声(dB)");
	gridLayout->addWidget(labelNoi, 0, 2);
	noiEdit = new QLineEdit("0", this);
	gridLayout->addWidget(noiEdit, 0, 3);

	QLabel* labelGea = new QLabel("风速");
	gridLayout->addWidget(labelGea, 1, 2);
	geaEdit = new QLineEdit("0", this);
	gridLayout->addWidget(geaEdit, 1, 3);

	QLabel* labelTem = new QLabel("温度(℃)");
	gridLayout->addWidget(labelTem, 2, 2);
	temEdit = new QLineEdit("25", this);
	gridLayout->addWidget(temEdit, 2, 3);

	QLabel* labelAng = new QLabel("攻角(°)");
	gridLayout->addWidget(labelAng, 3, 2);
	angEdit = new QLineEdit("0", this);
	gridLayout->addWidget(angEdit, 3, 3);

	//第三列
	QLabel* labelAtiX = new QLabel("X轴(N)");
	gridLayout->addWidget(labelAtiX, 0, 4);
	atiXEdit = new QLineEdit("0", this);
	gridLayout->addWidget(atiXEdit, 0, 5);

	QLabel* labelAtiY = new QLabel("Y轴(N)");
	gridLayout->addWidget(labelAtiY, 1, 4);
	atiYEdit = new QLineEdit("0", this);
	gridLayout->addWidget(atiYEdit, 1, 5);

	QLabel* labelAtiZ = new QLabel("Z轴(N)");
	gridLayout->addWidget(labelAtiZ, 2, 4);
	atiZEdit = new QLineEdit("0", this);
	gridLayout->addWidget(atiZEdit, 2, 5);

	QLabel* labelWin = new QLabel("风速(m/s)");
	gridLayout->addWidget(labelWin, 3, 4);
	winEdit = new QLineEdit("0", this);
	gridLayout->addWidget(winEdit, 3, 5);

	//第四列
	QLabel* labelMX = new QLabel("X轴力矩(N*m)");
	gridLayout->addWidget(labelMX, 0, 6);
	mXEdit = new QLineEdit("0", this);
	gridLayout->addWidget(mXEdit, 0, 7);

	QLabel* labelMY = new QLabel("Y轴力矩(N*m)");
	gridLayout->addWidget(labelMY, 1, 6);
	mYEdit = new QLineEdit("0", this);
	gridLayout->addWidget(mYEdit, 1, 7);

	QLabel* labelMZ = new QLabel("Z轴力矩(N*m)");
	gridLayout->addWidget(labelMZ, 2, 6);
	mZEdit = new QLineEdit("0", this);
	gridLayout->addWidget(mZEdit, 2, 7);

	QLabel* labelOff = new QLabel("偏移角(°)");
	gridLayout->addWidget(labelOff, 3, 6);
	offEdit = new QLineEdit("0", this);
	gridLayout->addWidget(offEdit, 3, 7);

	QLabel* labelStroke = new QLabel("扑动行程(mm)");
	gridLayout->addWidget(labelStroke, 4, 0);
	strEdit = new QLineEdit("0", this);
	gridLayout->addWidget(strEdit, 4, 1);

	QLabel* labelEdge = new QLabel("前沿杆长(mm)");
	gridLayout->addWidget(labelEdge, 4, 2);
	edgEdit = new QLineEdit("0", this);
	gridLayout->addWidget(edgEdit, 4, 3);

	QLabel* labelFlex = new QLabel("柔性形变(mm)");
	gridLayout->addWidget(labelFlex, 4, 4);
	fleEdit = new QLineEdit("0", this);
	gridLayout->addWidget(fleEdit, 4, 5);

	// 重置按钮
	QPushButton* resetAllLine = new QPushButton("重置");
	gridLayout->addWidget(resetAllLine, 4, 6);
	connect(resetAllLine, &QPushButton::clicked, [=]() {
		volEdit->setText("0");
		curEdit->setText("0");
		powEdit->setText("0");
		freEdit->setText("0");
		noiEdit->setText("0");
		geaEdit->setText("0");
		temEdit->setText("0");
		angEdit->setText("0");
		atiXEdit->setText("0");
		atiYEdit->setText("0");
		atiZEdit->setText("0");
		winEdit->setText("0");
		mXEdit->setText("0");
		mYEdit->setText("0");
		mZEdit->setText("0");
		offEdit->setText("0");
		});
}

// 数据选项卡初始化
void HostCom::initialTabDastr() {
	m_rootDir = "";                                             // 根目录初始化
	QVBoxLayout* vlayData = new QVBoxLayout(tabDastr);          // 新建垂直布局
	dataEdit = new QTextEdit();                                 // 实例化TextEdit控件，并指定父对象为tabDastr页
	dataEdit->setAlignment(Qt::AlignJustify);                   // 设置段落两端对齐,通常情况下TextEdit中每一换行符代表一个段落
	dataEdit->setStyleSheet("QTextEdit {font-family:\"Times New Roman ,  SimHei\" ; font-size:18px ; font-weight:bold ; }");      // 基于qss设置中文黑体，英文新罗马，加粗，大小18
	connect(dataEdit, &QTextEdit::textChanged, [&]() {         // 设置文本滚动，防止接收数据大小超过文本框时看不到
		dataEdit->moveCursor(QTextCursor::End);                 // 将光标移动到末端
		});

	vlayData->addWidget(dataEdit);                              // 将文本框控件放入布局中     

	QHBoxLayout* hlayData = new QHBoxLayout();                  // 新建水平布局，添加数据交互功能
	QLineEdit* filePathEdit = new QLineEdit("Select Your path to save data File");
	QPushButton* findPath = new QPushButton("浏览");            // 添加浏览按钮
	connect(findPath, &QPushButton::clicked, [=]() {           // 选择文本保存路径,lamda函数中不能设置引用，会修改文本框指针
		QString dirName = QFileDialog::getExistingDirectory(this, "设置根目录",
			"/home",
			QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);  // 仅显示文件夹，同时不在文件对话框中解析符号链接 

		if (!dirName.isEmpty()) {
			filePathEdit->setText(dirName);                        // 更新文本框显示
			logPlainText->appendPlainText(QString("[File Info]\t系统根目录已更新为: %1").arg(dirName));
			m_rootDir = dirName;                                   // 设置根目录
			qInfo() << "系统根目录已成功设置" << dirName;
		}
		else {
			qWarning() << "未选择正确路径，系统根目录设置失败";
			logPlainText->appendPlainText("[File Warning]\t根目录为空，设置失败!");
		}
		});

	saveDataButton = new QPushButton("开始导出");               // 添加保存按钮
	connect(saveDataButton, &QPushButton::clicked, [&]() {
		if (m_rootDir.isEmpty()) {                                // 判断根目录是否存在，选择导出的csv文件保存路径
			logPlainText->appendPlainText("[File Error]\t根目录不存在导出失败");
			qWarning() << "系统根目录未正确设置";
		}
		else {
			logPlainText->appendPlainText("[File Info]\t数据导出中……");
			saveDataButton->setEnabled(false);                  // 数据保存按钮关闭
			QString dataInfo = dataEdit->toPlainText();         // 获取数据流文本
			emit sendRawData(dataInfo, m_rootDir);             // 发送初始化信号
			QThreadPool::globalInstance()->start(saveDataTask); // 将保存数据子线程扔入线程池中自动执行
		}
		});

	QPushButton* clearData = new QPushButton("清空");           // 添加清空按钮
	connect(clearData, &QPushButton::clicked, [=]() {
		dataEdit->clear();                                      // 清空文本框
		dataEdit->moveCursor(QTextCursor::Start);               // 光标移动到开头
		qInfo() << "历史数据已清除";
		});

	hlayData->addWidget(filePathEdit);                          // 依次向水平控件中添加元素
	hlayData->addWidget(findPath);
	hlayData->addWidget(saveDataButton);
	hlayData->addWidget(clearData);

	vlayData->addLayout(hlayData);
}

// 图表选项卡初始化
void HostCom::initialTabChart() {
	QVBoxLayout* vlayChart = new QVBoxLayout(tabChart);          // 新建垂直布局

	m_Chart = new QChart();                                      // 图表容器对象实例化
	m_chartView = new QChartView();
	m_axisX = new QValueAxis();                                  // 实例化XY坐标轴
	m_axisY = new QValueAxis();
	this->initialLineSeries();                                   // 图表数据对象初始化
	this->initialAxis();                                         // 图表坐标轴设置

	// 图表设置
	m_Chart->setTitleFont(QFont("Times New Roman , SimHei", 20, QFont::Bold, true));    // 设置字体
	m_Chart->setTitle("ATI数据曲线");                            // 设置标题
	m_Chart->addAxis(m_axisY, Qt::AlignLeft);                    // 将Y轴添加到图表上
	m_Chart->addAxis(m_axisX, Qt::AlignBottom);                  // 将X轴添加到图表上
	//m_Chart->legend()->hide();                                 // 隐藏图例
	m_Chart->setAnimationOptions(QChart::SeriesAnimations);      // 动画：能使曲线绘制显示的更平滑，过渡效果更好看

	for (qint8 i = 0; i < 6; i++) {
		m_lineSeries[i]->attachAxis(m_axisX);                    // 曲线对象关联上X轴，此步骤必须在m_chart->addSeries之后
		m_lineSeries[i]->attachAxis(m_axisY);                    // 曲线对象关联上Y轴，此步骤必须在m_chart->addSeries之后
	}

	m_chartView->setChart(m_Chart);                              // 显示控件添加图表对象
	m_chartView->setRenderHint(QPainter::Antialiasing);          // 引擎应尽可能对基元的边缘进行抗锯齿处理
	vlayChart->addWidget(m_chartView);                           // 将ChartView添加进布局中进行显示

	QHBoxLayout* hlayChart = new QHBoxLayout();                  // 新建水平布局，添加数据交互功能
	QPushButton* clearChart = new QPushButton("清空曲线");
	connect(clearChart, &QPushButton::clicked, this, &HostCom::clearChartView);  // 清空图表显示

	//QTimer* chartTimer = new QTimer(this);                       // 新建定时器用于控制图表更新
	//chartTimer->setInterval(50);                                 // 设置间隔，可添加lineEdit交互调整
	//connect(chartTimer , &QTimer::timeout, this , &HostCom::updateChartData); // 数据更新槽函数
	QPushButton* chartButton = new QPushButton("开启图表更新");
	//connect(chartButton, &QPushButton::clicked, [=]() {          // 开关定时器以及翻转按钮状态
	//    if (!chartTimer->isActive()) {
	//        chartTimer->start();
	//        chartButton->setText("停止图表更新");
	//        clearChart->setEnabled(false);
	//    }
	//    else {
	//        chartTimer->stop();
	//        chartButton->setText("开启图表更新");
	//        clearChart->setEnabled(true);
	//    }
	//});

	QPushButton* saveChart = new QPushButton("保存图像");
	connect(saveChart, &QPushButton::clicked, this, &HostCom::saveChartView);

	hlayChart->addStretch(2);                                    // 按钮控件布局
	hlayChart->addWidget(chartButton);
	hlayChart->addStretch(1);
	hlayChart->addWidget(clearChart);
	hlayChart->addStretch(1);
	hlayChart->addWidget(saveChart);
	hlayChart->addStretch(2);

	vlayChart->addLayout(hlayChart);                             // 放入选项卡布局中

	this->initialCheckBox(vlayChart);                            // 初始化复选框
}

// 动捕选项卡初始化
void HostCom::initialTabOptit() {
	QVBoxLayout* vlayNatnet = new QVBoxLayout(tabOptit);         // 新建垂直布局
	this->initGraph3D();                                         // 初始化散点图
	vlayNatnet->addWidget(graphContainer);                       // 向布局中添加图表容器

	QHBoxLayout* hlayNatnet = new QHBoxLayout();                 // 新建水平布局，添加交互功能
	QLabel* labelThemeFont = new QLabel("主题 : ", this);        // 文字标签
	labelThemeFont->setMaximumHeight(30);

	QComboBox* themeBox = new QComboBox(this);                   // 下拉单选框
	QStringList strList;
	strList << "Qt" << "PrimaryColors" << "Digia" << "StoneMoss" << "ArmyBlue" << "Retro" << "Ebony" << "Isabelle";
	themeBox->addItems(strList);
	connect(themeBox, &QComboBox::currentIndexChanged, [=](int index) {
		Q3DTheme* currentTheme = graph3D->activeTheme();         // 设置主题
		currentTheme->setType(Q3DTheme::Theme(index));           // 根据index设置图表主题，详细区别可以打开帮助看有什么主题
		});
	hlayNatnet->addStretch(3);
	hlayNatnet->addWidget(labelThemeFont);                      // 放入布局中
	hlayNatnet->addWidget(themeBox);
	hlayNatnet->addStretch(1);

	QCheckBox* cbBackground = new QCheckBox(this);               // 显示背景
	cbBackground->setText("显示背景");                           // 设置显示文本
	cbBackground->setCheckState(Qt::Unchecked);                  // 设置选中状态
	hlayNatnet->addWidget(cbBackground);                         // 放入布局中
	hlayNatnet->addStretch(1);
	connect(cbBackground, &QCheckBox::stateChanged, [=](bool checked) {
		graph3D->activeTheme()->setBackgroundEnabled(checked);
		});

	QCheckBox* cbGrid = new QCheckBox(this);                     // 显示网格
	cbGrid->setText("显示网格");                                 // 设置显示文本
	cbGrid->setCheckState(Qt::Checked);                          // 设置选中状态
	hlayNatnet->addWidget(cbGrid);                               // 放入布局中
	hlayNatnet->addStretch(3);
	connect(cbGrid, &QCheckBox::stateChanged, [=](bool checked) {
		graph3D->activeTheme()->setGridEnabled(checked);
		});

	vlayNatnet->addLayout(hlayNatnet);
}

// 图表数据对象初始化
void HostCom::initialLineSeries() {
	for (qint8 i = 0; i < 6; i++) {                              // 初始化各个数据对象
		m_lineSeries[i] = new QLineSeries();
	}

	for (int j = 0; j < 6; j++) {                                // 为各个数据对象赋初值
		for (int i = 0; i < AXIS_MAX_X; i++) {
			m_lineSeries[j]->append(i, 0);
		}

		switch (j)                                               // 设置曲线颜色、状态、图例 
		{
		case 0: {
			m_lineSeries[j]->setPen(QPen(Qt::black, 2, Qt::SolidLine));
			m_lineSeries[j]->setName("Fx");
			break;
		}
		case 1: {
			m_lineSeries[j]->setPen(QPen(Qt::red, 2, Qt::SolidLine));
			m_lineSeries[j]->setName("Fy");
			break;
		}
		case 2: {
			m_lineSeries[j]->setPen(QPen(Qt::green, 2, Qt::SolidLine));
			m_lineSeries[j]->setName("Fz");
			break;
		}
		case 3: {
			m_lineSeries[j]->setPen(QPen(Qt::blue, 2, Qt::SolidLine));
			m_lineSeries[j]->setName("Jx");
			break;
		}
		case 4: {
			m_lineSeries[j]->setPen(QPen(Qt::cyan, 2, Qt::SolidLine));
			m_lineSeries[j]->setName("Jy");
			break;
		}
		case 5: {
			m_lineSeries[j]->setPen(QPen(Qt::magenta, 2, Qt::SolidLine));
			m_lineSeries[j]->setName("Jz");
			break;
		}
		}

		m_lineSeries[j]->setPointsVisible(true);                 // 设置数据点可见
		m_Chart->addSeries(m_lineSeries[j]);                     // 将数据对象添加到图表中
	}
	this->pointCount = AXIS_MAX_X;                               // 初始化数据点数
}

// 图表坐标轴初始化
void HostCom::initialAxis() {
	m_axisX->setRange(AXIS_MIN_X, AXIS_MAX_X);                   // 设置坐标轴区间
	m_axisY->setRange(AXIS_MIN_Y, AXIS_MAX_Y);
	m_axisX->setTickCount(6);                                    // 设置坐标轴格数
	m_axisY->setTickCount(5);
	m_axisX->setLabelFormat("%d");                               // 设置坐标轴下标数据格式
	m_axisY->setLabelFormat("%.2f");
	m_axisX->setTitleFont(QFont("Times New Roman", 18, QFont::Normal, true));   // 设置字体
	m_axisY->setTitleFont(QFont("Times New Roman", 18, QFont::Normal, true));
	m_axisX->setTitleText("时间(s)");                            // 设置坐标轴标题
	m_axisY->setTitleText("力(N)/力矩(N·m)");
	m_axisX->setGridLineVisible(false);                          // 设置网格线
	m_axisY->setGridLineVisible(false);
}

// 图表复选框初始化
void HostCom::initialCheckBox(QVBoxLayout* vlay) {
	QHBoxLayout* hlayView = new QHBoxLayout();                   // 新建水平布局，添加数据显示/关闭功能
	hlayView->addStretch(3);                                     // 添加弹簧控件，调整页面布局
	QCheckBox* cbFx = new QCheckBox(this);                       // 创建QCheckBox对象
	cbFx->setText("Fx");                                         // 设置显示文本
	cbFx->setCheckState(Qt::Checked);                            // 设置选中状态
	hlayView->addWidget(cbFx);                                   // 放入布局中
	hlayView->addStretch(1);
	connect(cbFx, &QCheckBox::stateChanged, [=]() {
		if (cbFx->isChecked()) {
			m_Chart->addSeries(m_lineSeries[0]);
			m_lineSeries[0]->attachAxis(m_axisX);
			m_lineSeries[0]->attachAxis(m_axisY);
		}
		else
			m_Chart->removeSeries(m_lineSeries[0]);
		});

	QCheckBox* cbFy = new QCheckBox(this);                       // Fy复选框
	cbFy->setText("Fy");
	cbFy->setCheckState(Qt::Checked);
	hlayView->addWidget(cbFy);
	hlayView->addStretch(1);
	connect(cbFy, &QCheckBox::stateChanged, [=]() {
		if (cbFy->isChecked()) {
			m_Chart->addSeries(m_lineSeries[1]);
			m_lineSeries[1]->attachAxis(m_axisX);
			m_lineSeries[1]->attachAxis(m_axisY);
		}
		else
			m_Chart->removeSeries(m_lineSeries[1]);
		});

	QCheckBox* cbFz = new QCheckBox(this);                       // Fz复选框
	cbFz->setText("Fz");
	cbFz->setCheckState(Qt::Checked);
	hlayView->addWidget(cbFz);
	hlayView->addStretch(1);
	connect(cbFz, &QCheckBox::stateChanged, [=]() {
		if (cbFz->isChecked()) {
			m_Chart->addSeries(m_lineSeries[2]);
			m_lineSeries[2]->attachAxis(m_axisX);
			m_lineSeries[2]->attachAxis(m_axisY);
		}
		else
			m_Chart->removeSeries(m_lineSeries[2]);
		});

	QCheckBox* cbJx = new QCheckBox(this);                       // Jx复选框
	cbJx->setText("Jx");
	cbJx->setCheckState(Qt::Checked);
	hlayView->addWidget(cbJx);
	hlayView->addStretch(1);
	connect(cbJx, &QCheckBox::stateChanged, [=]() {
		if (cbJx->isChecked()) {
			m_Chart->addSeries(m_lineSeries[3]);
			m_lineSeries[3]->attachAxis(m_axisX);
			m_lineSeries[3]->attachAxis(m_axisY);
		}
		else
			m_Chart->removeSeries(m_lineSeries[3]);
		});

	QCheckBox* cbJy = new QCheckBox(this);                       // Jy复选框
	cbJy->setText("Jy");
	cbJy->setCheckState(Qt::Checked);
	hlayView->addWidget(cbJy);
	hlayView->addStretch(1);
	connect(cbJy, &QCheckBox::stateChanged, [=]() {
		if (cbJy->isChecked()) {
			m_Chart->addSeries(m_lineSeries[4]);
			m_lineSeries[4]->attachAxis(m_axisX);
			m_lineSeries[4]->attachAxis(m_axisY);
		}
		else
			m_Chart->removeSeries(m_lineSeries[4]);
		});

	QCheckBox* cbJz = new QCheckBox(this);                       // Jz复选框
	cbJz->setText("Jz");
	cbJz->setCheckState(Qt::Checked);
	hlayView->addWidget(cbJz);
	hlayView->addStretch(3);
	connect(cbJz, &QCheckBox::stateChanged, [=]() {
		if (cbJz->isChecked()) {
			m_Chart->addSeries(m_lineSeries[5]);
			m_lineSeries[5]->attachAxis(m_axisX);
			m_lineSeries[5]->attachAxis(m_axisY);
		}
		else
			m_Chart->removeSeries(m_lineSeries[5]);
		});

	vlay->addLayout(hlayView);                                  // 放入选项卡布局中
}

// 3D散点图初始化
void HostCom::initGraph3D() {
	graph3D = new Q3DScatter();                                  // 创建图表
	graphContainer = QWidget::createWindowContainer(graph3D);    // 放入图表容器中，以便嵌入子窗口中，让子窗口接管3D图像的所有权  

	graph3D->axisX()->setTitle("X轴");                           // 创建坐标轴
	graph3D->axisX()->setTitleVisible(true);
	graph3D->axisX()->setRange(-10.0f, 10.0f);

	graph3D->axisY()->setTitle("Y轴");
	graph3D->axisY()->setTitleVisible(true);
	graph3D->axisY()->setRange(-10.0f, 10.0f);

	graph3D->axisZ()->setTitle("Z轴");
	graph3D->axisZ()->setTitleVisible(true);
	graph3D->axisZ()->setRange(-10.0f, 10.0f);

	graph3D->activeTheme()->setLabelBackgroundEnabled(false);    // 设置全透明背景
	graph3D->setShadowQuality(QAbstract3DGraph::ShadowQualityNone); // 取消阴影

	// 创建数据代理和系列
	proxy = new QScatterDataProxy();
	series = new QScatter3DSeries(proxy);                        // 绑定数据代理
	series->setItemSize(0.05f);
	graph3D->addSeries(series);                                  // 将系列添加到散点图中
}

// 线程初始化
void HostCom::initialThread() {
	/*
	特别注意：
	   子线程不能直接操作上位机ui，会导致内存泄漏问题，
	   因此在接受子线程传回信号时，不建议使用lamda做槽函数，应当开辟单独槽函数由主程进行调用
	*/

	// 线程池初始化，设置最大线程池数
	QThreadPool::globalInstance()->setMaxThreadCount(15);

	speedChangeTask = new SpeedChangeThread(this);
	connect(this, &HostCom::sendParam, speedChangeTask, &SpeedChangeThread::recvParm);
	connect(speedChangeTask, &SpeedChangeThread::finish, this, &HostCom::speedChangeFinish);

	saveDataTask = new SaveDataThread(this);
	connect(this, &HostCom::sendRawData, saveDataTask, &SaveDataThread::recvRawData);
	connect(saveDataTask, &SaveDataThread::finish, this, &HostCom::saveDataFinish);
	qInfo() << "主线程初始化完成 : " << QThread::currentThread();
}

// 初始化系统变量
void HostCom::initialSysParam() {
	m_stateEnable = true;      // true -- 初始化 ， false -- 使能
	serverFlag = true;         // true -- 启动服务器 ， false -- 关闭服务器
	loggingFlag = true;        // true -- 数据导出 ， false -- 导出结束
	isunlockFlag = false;      // true -- 解锁 ， false -- 未解锁
	m_clientList.clear();
	m_clientCnt = 0;           // 已连接客户端数量归零
	isBias = false;            // 不偏置
	m_state = true;            // 按钮翻转标志位
	minZ = std::numeric_limits<float>::max();// 初始化扑幅上下限
	maxZ = std::numeric_limits<float>::min();

	isNatNetConnect = true;
	// 环形缓冲区初始化
	buffer = new RingBuffer(100);
}

/************************************************************************************************************
 *                                                                                                          *
 *                                                  复写系统函数                                            *
 *                                                                                                          *
 ***********************************************************************************************************/

 // 为解锁标签添加鼠标点击事件处理逻辑
bool HostCom::eventFilter(QObject* obj, QEvent* event) {
	if (obj == lock) {                                                       // 指定为解锁/闭锁Label
		if (event->type() == QEvent::MouseButtonPress) {                     // 仅提取鼠标点击事件
			qInfo() << "lock/unlock Label Clicked";
			QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);     // 事件转换
			if (mouseEvent->button() == Qt::LeftButton) {                    // 鼠标点击了
				if (m_clientCnt == 0)
					logPlainText->appendPlainText("[Server Info]\t没有客户端连接，解锁失败");
				else {
					if (!isunlockFlag) {                                        // 解锁
						lock->setPixmap(QPixmap(":/HostCom/res/unlock.png"));   // 图标更新
						throControl->setSliderValue(0);
						spinSpeed->setSliderValue(0);
						logPlainText->appendPlainText("[Speed Info]\t电调已解锁");
						QMessageBox::information(this, "提示信息", "电调已解锁请注意油门归零");
					}
					else {                                                      // 闭锁
						lock->setPixmap(QPixmap(":/HostCom/res/lock.png"));     // 图标更新
						throControl->setSliderValue(0);
						spinSpeed->setSliderValue(0);
						logPlainText->appendPlainText("[Speed Info]\t电调已闭锁");
					}
					isunlockFlag = !isunlockFlag;
				}
				return true;
			}
			else
				return false;
		}
		else
			return false;
	}
	else if (obj == m_initial) {                                            // 指定为初始化图标
		if (event->type() == QEvent::MouseButtonPress) {                    // 仅提取鼠标点击事件
			qInfo() << "ESC enbale Label Clicked";
			QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);     // 事件转换
			if (mouseEvent->button() == Qt::LeftButton) {                   // 鼠标点击了
				if (m_clientCnt == 0)
					logPlainText->appendPlainText("[Server Info]\t没有客户端连接，解锁失败");
				else {
					if (m_stateEnable) {
						const char initialData[] = { '\x2C' , '\xE4' , '\x04' , '\x01' , '\x00' , '\x0A' , '\x0A' , '\x38' , '\xFC' };    // 电调初始化指令
						QByteArray escInitial(initialData, sizeof(initialData));
						serverSendData(escInitial);                             // 电调初始化
						m_initial->setPixmap(QPixmap(":/HostCom/res/enable.png"));
						logPlainText->appendPlainText("[Speed Info]\t电调初始化完成");
					}
					else {
						const char enableData[] = { '\x2C' , '\xE4' , '\x04' , '\x02' , '\x00' , '\x0A' , '\x0A' , '\x38' , '\xFC' };     // 电调使能指令
						QByteArray escEnable(enableData, sizeof(enableData));
						serverSendData(escEnable);                              // 电调使能
						m_initial->setPixmap(QPixmap(":/HostCom/res/initial.png"));
						logPlainText->appendPlainText("[Speed Info]\t电调使能完成");
					}
					m_stateEnable = !m_stateEnable;
				}
				return true;
			}
			else
				return false;
		}
		else
			return false;
	}
	else
		return QWidget::eventFilter(obj, event);                           // 将事件交回给父类
}

// 复写关闭函数
void HostCom::closeEvent(QCloseEvent* e) {
	//窗口关闭时弹出的提示窗口
	QMessageBox::StandardButton ret = QMessageBox::question(this, "确认", "确定要退出本系统吗？", QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Yes);

	if (ret == QMessageBox::Yes)
		e->accept();                        // 若用户点击确认，则接收这个事件,当前窗口会关闭
	else
		e->ignore();                        // 若用户点击取消，则忽略这个事件，当前窗口不会关闭
}

// 覆写鼠标滚轮事件
void HostCom::wheelEvent(QWheelEvent* e) {
	QPoint numDegrees = e->angleDelta();        // 捕获鼠标滚轮滚动角度
	if (numDegrees.y() > 0) {                   // 当滚轮远离使用者时
		this->m_Chart->zoomIn();                // 进行放大
	}
	else {                                      // 当滚轮向使用者方向旋转时
		this->m_Chart->zoomOut();               // 进行缩小
	}

	QWidget::wheelEvent(e);
}

/************************************************************************************************************
 *                                                                                                          *
 *                                                  服务器端函数                                            *
 *                                                                                                          *
 ***********************************************************************************************************/

 // 连接服务器按钮槽函数
void HostCom::conSerButtonClicked() {
	if (serverFlag) {
		logPlainText->appendPlainText("[Server Info]\t服务器监听中");

		lhAddr.setAddress(ipEdit->text());              // 获取ip地址
		lhPort = portEdit->text().toInt();              // 获取端口号
		qInfo() << "服务器启动监听";
		if (!slotTryCreateTcpServer()) {
			qWarning() << "建立服务器失败! 请确认网络状态和端口";
			logPlainText->appendPlainText("[Server Error]\t尝试建立服务器失败! 请确认网络状态和端口");
			return;
		}
		logPlainText->appendPlainText("[Server Info]\t建立TCP服务器成功");
		qInfo() << "服务器建立成功 : " << lhAddr << ":" << lhPort;
		labelServerIcon->setPixmap(*serverOpenIcon);
		labelServerFont->setText("已连接客户端:0");
		connectToServer->setText("断开服务器");         // 更新标签
	}
	else {
		qInfo() << "服务器已关闭";
		logPlainText->appendPlainText("[Server Info]\t服务器已关闭");
		labelServerIcon->setPixmap(*serverCloseIcon);
		labelServerFont->setText("服务器关闭");
		m_TcpServer->disconnect();                      // 断开连接
		m_TcpServer->close();                           // 关闭服务器
		delete  m_TcpServer;                            // 释放资源
		connectToServer->setText("启动服务器");         // 更新标签
	}
	serverFlag = !serverFlag;                           // 切换状态
}

// 启动服务器并建立通信
bool HostCom::slotTryCreateTcpServer() {
	m_TcpServer = new MyTcpServer(this);

	if (!m_TcpServer->listen(lhAddr, lhPort))       // 建立监听失败
		return false;

	// 建立信号与槽通信
	connect(m_TcpServer, &MyTcpServer::updateTcpServer, this, &HostCom::tcpServerDataReceived);        // 接收数据
	connect(m_TcpServer, &MyTcpServer::addClientLink, this, &HostCom::addClientLink);                  // 添加连接
	connect(m_TcpServer, &MyTcpServer::removeClientLink, this, &HostCom::removeClientLink);            // 移除连接
	connect(this, &HostCom::serverSendData, m_TcpServer, &MyTcpServer::sendDataToClient);              // 发送指令
	connect(speedChangeTask, &SpeedChangeThread::sendSpeed, m_TcpServer, &MyTcpServer::sendDataToClient);

	return true;
}

// 读取客户端数据并更新UI显示
void HostCom::tcpServerDataReceived(QVector<float> data, uint8_t dataType) {
	if (dataType == 187) {                                                           // aa bb 01 00 - 2864382208
		freEdit->setText(QString("%1").arg(data[0]));                                       // 更新显示
		noiEdit->setText(QString("%1").arg(data[1] / 10.0));
		angEdit->setText(QString("%1").arg(data[2]));
		offEdit->setText(QString("%1").arg(data[3]));
		winEdit->setText(QString("%1").arg(data[4] * 2.2 * 3.14 / 180));                    // 风速/转速 -- (转速 * Π / 180) * 旋转半径
		dataEdit->append(QString("[FlaData] Fre:%1\tNoi:%2\tAng:%3\tOff:%4\tWin:%5\tTim:%6").arg(data[0]).arg(data[1] / 10.0).arg(data[2]).arg(data[3]).arg(data[4] * 2.2 * 3.14 / 180).arg(getTimeStr()));
	}
	else if (dataType == 1) {                                                      // aa 01 10 bb -- 2852196539
		volEdit->setText(QString("%1").arg(data[0]));
		curEdit->setText(QString("%1").arg(data[1]));
		powEdit->setText(QString("%1").arg(data[2]));
		geaEdit->setText(QString("%1").arg(data[3]));

		dataEdit->append(QString("[SpiData] Vol:%1\tCur:%2\tPow:%3\tFre:%4\tFac:%5\tTim:%6").arg(data[0]).arg(data[1]).arg(data[2]).arg(data[3]).arg(data[4]).arg(getTimeStr()));
	}
	else
		logPlainText->appendPlainText(QString("[Server Warning]\tNo Board Match , CurHeadNum : %1").arg(dataType));
}

// 添加客户端连接
void HostCom::addClientLink(QString clientAddrPort, int socketDescriptor) {
	qInfo() << "客户端连接 : " << clientAddrPort;
	logPlainText->appendPlainText(QString("[Server Info]\t客户端连接成功 : %1").arg(clientAddrPort));
	m_clientCnt++;
	m_clientList.append(socketDescriptor);                      // 添加客户端描述符
	labelServerFont->setText(QString("已连接客户端:%1").arg(m_clientCnt));
}

// 移除连接
void HostCom::removeClientLink(QString clientAddrPort, int socketDescriptor) {
	qInfo() << "客户端断开连接 : " << clientAddrPort;
	logPlainText->appendPlainText(QString("[Server Info]\t客户端已断开连接 : %1").arg(clientAddrPort));
	m_clientCnt--;                                              // 客户端数量更新
	m_clientList.removeOne(socketDescriptor);                   // 移除客户端描述符
	labelServerFont->setText(QString("已连接客户端:%1").arg(m_clientCnt));
}

// 获取本地Ip地址
QString HostCom::getLocalIpAddr() {
	QString ipAddr;
	QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
	for (int i = 0; i < ipAddressesList.size(); i++) {
		if (ipAddressesList.at(i) != QHostAddress::LocalHost && ipAddressesList.at(i).toIPv4Address()) {
			ipAddr = ipAddressesList.at(i).toString();
		}
	}
	if (ipAddr.isEmpty())
		ipAddr = QHostAddress(QHostAddress::LocalHost).toString();
	return ipAddr;
}

// ipv4地址格式标准化
QString HostCom::Ipv4Addr(QHostAddress addr) {
	quint32  addr_origin = addr.toIPv4Address();
	QHostAddress addr_host = QHostAddress(addr_origin);
	QString  addr_str = addr_host.toString();
	return addr_str;
}

/************************************************************************************************************
 *                                                                                                          *
 *                                                   客户端函数                                             *
 *                                                                                                          *
 ***********************************************************************************************************/

 // 通信客户端初始化
void HostCom::initialTcpClient() {
	atiTimer = new QTimer(this);
	connect(atiTimer, &QTimer::timeout, this, [=]() {
		QByteArray atiCommand(19, '\x00');
		if (this->isBias)
			atiCommand.append('\x01');
		else
			atiCommand.append('\x00');
		emit startSendData(atiCommand);
		});
}

// 连接服务器按钮槽函数
void HostCom::conCliButtonClicked() {
	if (m_state) {                 // 尝试连接服务器
		qInfo() << "尝试与ATI服务器建立连接";
		logPlainText->appendPlainText("[Client Info]\t连接ATI服务器中……");
		connectToATI->setText("连接中……");                      // 更新标签

		// 创建线程对象
		t = new QThread;

		// 创建任务对象
		worker = new MyATISocket;

		// 任务对象移动到线程中
		worker->moveToThread(t);

		// 注册主程向线程发送的信号与槽
		connect(this, &HostCom::startConnect, worker, &MyATISocket::connectServer);
		connect(this, &HostCom::cutConnect, worker, &MyATISocket::releaseResource);
		connect(this, &HostCom::startSendData, worker, &MyATISocket::sendCommand);

		// 启动子线程
		t->start();

		// 线程传回信号
		connect(worker, &MyATISocket::connectOK, this, [=]() {
			qInfo() << "与ATI服务器连接成功";
			logPlainText->appendPlainText("[Client Info]\t与服务器连接成功");
			connectToATI->setText("断开连接");                      // 更新标签
			labelConnectIcon->setPixmap(*connectIcon);
			labelConnectFont->setText("已连接ATI");
			m_state = false;    // 标志位置0

			// 发送获取校准信息指令
			QByteArray command(1, '\x01');
			QByteArray sendData(19, '\x00');                       // 读取校准信息命令
			command.append(sendData);
			emit startSendData(command);
			});
		connect(worker, &MyATISocket::connectCut, this, [=]() {
			qInfo() << "与ATI服务器断开连接";
			qInfo() << "子线程退出";
			// 资源释放
			t->quit();
			t->wait();
			worker->deleteLater();
			worker->deleteLater();

			// 状态复位
			m_state = true;
			logPlainText->appendPlainText("[Client Info]\t与服务器断开连接，请检查网络端口ATI服务器中");
			connectToATI->setText("连接ATI");
			labelConnectIcon->setPixmap(*disconnectIcon);
			labelConnectFont->setText("未连接");

			// 按钮复位
			atiSend->setText("发送");
			atiTimer->stop();
			biasBtn->setText("偏置");
			isBias = false;
			});
		connect(worker, &MyATISocket::updateServerData, this, &HostCom::updateATIData);

		emit startConnect(serAddrEdit->text(), serPortEdit->text().toUShort());
	}
	else {                                              // 断开服务器
		connectToATI->setText("连接ATI");               // 更新标签
		emit cutConnect();
		labelConnectIcon->setPixmap(*disconnectIcon);
		labelConnectFont->setText("未连接");
		m_state = true;

		// 按钮复位
		atiSend->setText("发送");
		atiTimer->stop();
		biasBtn->setText("偏置");
		isBias = false;
	}
}

// 发送按钮槽函数
void HostCom::atiSendSlots() {
	if (t->isRunning()) {           // 线程正在执行任务，可以发送数据
		if (!atiTimer->isActive()) {
			atiSend->setText("停止发送");
			qInfo() << "获取ATI数据中……";
			logPlainText->appendPlainText("[Client Info]\t上位机向ATI服务器发送数据中……");
			int timeInterval = periodEdit->text().toInt();
			atiTimer->start(timeInterval);
		}
		else {
			qInfo() << "停止ATI数据流接收";
			atiSend->setText("发送");
			logPlainText->appendPlainText("[Client Info]\t上位机停止向ATI服务器发送数据");
			atiTimer->stop();
		}
	}
	else {
		qWarning() << "未连接ATI服务器，力测试数据获取失败";
		logPlainText->appendPlainText("[Client Warning]\t上位机未连接ATI服务器，发送失败");
	}
}

// 更新主程ATI数据
void HostCom::updateATIData(QVector<double> ftData) {
	atiXEdit->setText(QString("%1").arg(ftData[0]));
	atiYEdit->setText(QString("%1").arg(ftData[1]));
	atiZEdit->setText(QString("%1").arg(ftData[2]));
	mXEdit->setText(QString("%1").arg(ftData[3]));
	mYEdit->setText(QString("%1").arg(ftData[4]));
	mZEdit->setText(QString("%1").arg(ftData[5]));

	dataEdit->append(QString("[ATIData] Fx:%1\tFy:%2\tFz:%3\tTx:%4\tTy:%5\tTz:%6\tTim:%7")
		.arg(ftData[0]).arg(ftData[1]).arg(ftData[2]).arg(ftData[3]).arg(ftData[4]).arg(ftData[5])
		.arg(getTimeStr()));
	//this->updateChartData(ftData);
}

/************************************************************************************************************
 *                                                                                                          *
 *                                                     槽函数                                               *
 *                                                                                                          *
 ***********************************************************************************************************/

 // 变速线程结束槽函数
void HostCom::speedChangeFinish() {
	speedChange->setEnabled(true);
	logPlainText->appendPlainText("[Speed Info]\t变速任务结束");
}

// 数据导出结束槽函数
void HostCom::saveDataFinish(QString infoStr) {
	saveDataButton->setEnabled(true);               // 数据保存按钮打开
	logPlainText->appendPlainText(infoStr);
}

// 连接动捕按钮槽函数
void HostCom::connectToNatnetBtnSlots() {
	if (isNatNetConnect) {  // 连接
		logPlainText->appendPlainText("[NatNet Info]\t与NatNet连接中");
		connectToNatnet->setText("连接中……");
		connectToNatnet->setEnabled(false);  // 禁用按钮

		// 创建并分配任务
		taskConnect = new NatNet(natIpEdit->text());

		// 子线程传回信号
		connect(taskConnect, &NatNet::connectFailed, this, [=]() {
			logPlainText->appendPlainText("[NatNet Info]\t与NatNet连接失败");
			qDebug().noquote() << QString("[NatNet Info]\t与NatNet连接失败");
			connectToNatnet->setText("连接");
			connectToNatnet->setEnabled(true);  // 连接失败，启用按钮
			isNatNetConnect = true;
			});

		connect(taskConnect, &NatNet::connectSuccess, this, [=]() {
			logPlainText->appendPlainText("[NatNet Info]\t与NatNet连接成功");
			connectToNatnet->setText("断开");
			connectToNatnet->setEnabled(true);
			isNatNetConnect = false;  // 标志位置0
			});
		QThreadPool::globalInstance()->start(taskConnect);  // 将任务放入线程池中进行处理
		connect(taskConnect, &NatNet::sendMarkerData, this, &HostCom::getNatNet);     // 解析动捕SDK数据槽函数
	}
	else {  // 断开连接
		logPlainText->appendPlainText("[NatNet Info]\t与NatNet断开连接");
		qDebug().noquote() << QString("[NatNet Info]\t与NatNet断开连接");
		connectToNatnet->setText("连接");
		connectToNatnet->setEnabled(true);
		// 确保任务正确停止并释放资源
		if (taskConnect) {
			taskConnect->disConnectClient();  // 停止任务
			taskConnect = nullptr;
			delete taskConnect;
		}
		buffer->clearRingBuffer();
		isNatNetConnect = true;  // 重新启用连接状态
	}
}

// 解析动捕按钮槽函数
void HostCom::parseBtnSlots() {
	// 从环形缓冲区中取一帧数据
	std::vector<sFrameOfMocapData> Frame3DMarker = buffer->extractFrame();

	//检查数组是否包含有效的数据
	if (Frame3DMarker.empty() == 0) {
		logPlainText->appendPlainText("Buffer is Empty or No Markers!");
		return;
	}

	// 建立任务线程，并放入线程池中
	/*DataParsing* task1 = new DataParsing(0, Frame3DMarker);
	DataParsing* task2 = new DataParsing(1, Frame3DMarker);
	QThreadPool::globalInstance()->start(task1);
	QThreadPool::globalInstance()->start(task2);*/
}

// 获取NatNetSDK传来的数据
void HostCom::getNatNet(sFrameOfMocapData* MocapData) {
	qDebug().noquote() << "[NatNet Data]\t接收数据中";
	dataEdit->append(QString("[NatNet Data]\t接收数据中"));

	if (MocapData == nullptr) {
		qDebug().noquote() << "[NatNet Data]\t数据为空";
		return;
	}

	int markerCount = MocapData->nLabeledMarkers;
	// 校验标记数量合法性
	if (markerCount < 0 || markerCount > MAX_LABELED_MARKERS) {
		qDebug() << "[NatNet Data]\tInvalid marker count:" << markerCount;
		return;
	}

	// 检查LabeledMarkers指针有效性
	if (markerCount > 0 && MocapData->LabeledMarkers == nullptr) {
		qDebug() << "[NatNet Data]\tLabeledMarkers数组为空但数量非零";
		return;
	}

	std::lock_guard<std::mutex> lock(dataMutex);  // 加锁
	std::vector<sMarker> markers;
	for (int i = 0; i < markerCount; ++i) {
		if (i >= MAX_LABELED_MARKERS) break; // 冗余保护

		sMarker marker = MocapData->LabeledMarkers[i];
		markers.push_back(marker);
		// 调试输出每个Marker的地址
		qDebug() << "Marker" << i << "address:" << (quintptr)&MocapData->LabeledMarkers[i];
		dataEdit->append(QString("Marker [ID=%1, x=%2, y=%3, z=%4]")
			.arg(marker.ID)
			.arg(marker.x)
			.arg(marker.y)
			.arg(marker.z));
	}

	if (markerCount == 0) return;
	sFrameOfMocapData frame;
	frame.iFrame = MocapData->iFrame;
	frame.nLabeledMarkers = markerCount;
	if (markerCount > 0) {
		memcpy(frame.LabeledMarkers, MocapData->LabeledMarkers, markerCount * sizeof(sMarker));
	}

	buffer->addFrame(frame);

	// 散点图数据代理
	QScatterDataArray* dataArray = new QScatterDataArray();  // 创建数据数组
	dataArray->resize(markerCount);                          // 设置数据大小
	QScatterDataItem* ptrToDataArray = &dataArray->first();  // 指向数据数组的第一个元素

	for (int i = 0; i < markerCount; ++i) {
		float posX = MocapData->LabeledMarkers[i].x;
		float posY = MocapData->LabeledMarkers[i].y;
		float posZ = MocapData->LabeledMarkers[i].z;

		// 图表数据更新
		ptrToDataArray->setPosition(QVector3D(posX / 1000, posY / 1000, posZ / 1000));  // 设置点的三维坐标
		ptrToDataArray++;

		// 坐标系是Y-up
		minZ = std::min(minZ, posY);
		maxZ = std::max(maxZ, posY);

		strEdit->setText(QString("%1").arg(maxZ - minZ));   // 扑动行程
	}
	proxy->resetArray(dataArray);  // 更新数据代理

}

// 更新图表数据槽函数
void HostCom::updateChartData(QVector<double> newData) {
	if (this->pointCount > AXIS_MAX_X) {
		for (qint8 j = 0; j < 6; j++) {
			/* 数据量庞大时，replace更新数据的效率远高于append */
			QVector<QPointF> Points = m_lineSeries[j]->points();
			QVector<QPointF> Points2 = m_lineSeries[j]->points();
			qint8 i;
			for (i = 0; i < Points.size() - 1; i++) {                                   //  移除首元素
				Points[i] = Points2[i + 1];
			}
			if (newData[i] >= AXIS_MAX_Y || newData[i] <= AXIS_MIN_Y)
				Points[i] = QPointF(pointCount, 0);
			else
				Points[i] = QPointF(pointCount, (int)newData[i]);                      // 加入新点
			m_lineSeries[j]->replace(Points);                                           // 利用replace函数更新数据
		}
		m_Chart->axes(Qt::Horizontal).back()->setRange(pointCount - AXIS_MAX_X, pointCount);
		//m_Chart->axisX()->setMin(pointCount - AXIS_MAX_X);
		//m_Chart->axisX()->setMax(pointCount);                     // 更新X轴范围
	}
	pointCount++;
}

// 清空图表显示
void HostCom::clearChartView() {
	for (qint8 i = 0; i < 6; i++) {
		m_lineSeries[i]->clear();                            // 清空各个数据对象
		for (qint8 j = 0; j < AXIS_MAX_X; j++) {             // 重新赋初值
			m_lineSeries[i]->append(j, 0);
		}
	}
	m_Chart->axes(Qt::Horizontal).back()->setRange(AXIS_MIN_X, AXIS_MAX_X);      // 重新设置XY轴范围
	m_Chart->axes(Qt::Vertical).back()->setRange(AXIS_MIN_Y, AXIS_MAX_Y);
	logPlainText->appendPlainText("[Image Info]\t图像恢复初始化");
}

// 保存曲线
void HostCom::saveChartView() {
	if (m_rootDir.isEmpty())
		logPlainText->appendPlainText("[Image Info]\t根目录不存在，图像保存失败");
	else {
		QPixmap p = m_chartView->grab();                         // 基于QWidget的grab方法捕获图片,该方法Qt5.0后引入
		QImage image = p.toImage();                              // 转换图片格式

		QString filePath = m_rootDir + QString("/atiChart-%1.png").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss")); // 设置文件路径
		filePath = QDir::toNativeSeparators(filePath);           // 正斜杠'/'转成反斜杠'\\'

		image.save(filePath, "PNG", 100);                      // 保存图像
		logPlainText->appendPlainText("[Image Info]\t图像保存成功!");
		qInfo() << "图像保存成功";
	}
}

// 状态栏更新时间
void HostCom::showTime()  // 显示时间槽函数
{
	QString string;
	ui.statusBar->setStyleSheet("color:rgb(0,0,0)");     // 设置显示颜色
	QDateTime Timedata = QDateTime::currentDateTime();   // 获取当前时间
	string = Timedata.toString("yyyy-MM-dd hh:mm:ss");   // 设置显示格式
	labeltimeFont->setText(string);                      // 设置标签内容
	labeltimeFont->setMinimumWidth(150);                 // 设置宽度
}

// 获取时间戳
QString HostCom::getTimeStr() {
	QDateTime time = QDateTime::currentDateTime();
	QString timeStr = time.toString("yyyy-MM-dd-hh-mm-ss-zzz");
	return timeStr;
}

// 初始化香橙派
void HostCom::initOrangePi() {
	// 调用系统函数，通过cmd终端启动OrangePi程序
	// put your code here 
	logPlainText->appendPlainText("[ATI Info]\t香橙派启动成功!");

}