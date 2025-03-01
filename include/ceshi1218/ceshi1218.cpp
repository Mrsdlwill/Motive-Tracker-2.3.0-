#include "ceshi1218.h"

ceshi1218::ceshi1218(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	m_state = false;
	connect(ui.connectBtn, &QPushButton::clicked, this, &ceshi1218::on_connectBtn_clicked);
	saveDataTask = new SaveDataThread(this);
	connect(this, &ceshi1218::sendRawData, saveDataTask, &SaveDataThread::recvRawData);
	this->initialTabDastr();
}

ceshi1218::~ceshi1218()
{}

// 连接动捕按钮槽函数
void ceshi1218::on_connectBtn_clicked()
{
	if (!m_state) {
		taskconnect = new NatNet(ui.lineEdit->text());

		connect(taskconnect, &NatNet::connectSuccess, [=]() {
			ui.textEdit->append("连接成功");
			ui.connectBtn->setText("断开");
			m_state = true;
			});
		connect(taskconnect, &NatNet::connectFailed, [=]() {
			ui.textEdit->append("连接失败");
			ui.connectBtn->setText("连接");
			m_state = false;
			});
		QThreadPool::globalInstance()->start(taskconnect);  // 将任务放入线程池中进行处理
		connect(taskconnect, &NatNet::sendMarkerData, this, &ceshi1218::getNatNet);     // 解析动捕SDK数据槽函数
	}
	else {
		ui.textEdit->append("断开");
		ui.connectBtn->setText("连接");
		m_state = false;
		taskconnect->disConnectClient();
		delete taskconnect;
		taskconnect = nullptr;
	}
}

// 解析动捕按钮槽函数
void ceshi1218::parseBtnSlots() {
	// 从环形缓冲区中取一帧数据
	std::vector<sFrameOfMocapData> Frame3DMarker = buffer->extractFrame();

	//检查数组是否包含有效的数据
	if (Frame3DMarker.empty() == 0) {
		//dataEdit->append("Buffer is Empty or No Markers!");
		return;
	}

	// 建立任务线程，并放入线程池中
	/*DataParsing* task1 = new DataParsing(0, Frame3DMarker);
	DataParsing* task2 = new DataParsing(1, Frame3DMarker);
	QThreadPool::globalInstance()->start(task1);
	QThreadPool::globalInstance()->start(task2);*/
}

// 获取NatNetSDK传来的数据
void ceshi1218::getNatNet(const QVector<sMarker>& markers) {

	if (markers.isEmpty()) {
		qDebug().noquote() << "[NatNet Data]\t数据为空";
		ui.dataEdit->append("数据为空");
		return;
	}
	static QElapsedTimer updateTimer;
	if (updateTimer.elapsed() < 50) return; // 限制每秒最多更新20次
	updateTimer.restart();
	QStringList lines;
	for (const sMarker& marker : markers)
	{
		// 生成逗号分隔的字段
		lines.append(QString("[MocapData],%1,%2,%3,%4")
			.arg(marker.ID)
			.arg(marker.x)
			.arg(marker.y)
			.arg(marker.z));
	}
	ui.dataEdit->append(lines.join("\n"));

	// 更新高度范围（示例）
	for (const sMarker& marker : markers) {
		float posY = marker.y;
		minZ = std::min(minZ, posY);
		maxZ = std::max(maxZ, posY);
	}
}


// 数据选项卡初始化
void ceshi1218::initialTabDastr() {
	m_rootDir = "";                                             // 根目录初始化
	ui.dataEdit->setAlignment(Qt::AlignJustify);                   // 设置段落两端对齐,通常情况下TextEdit中每一换行符代表一个段落
	ui.dataEdit->setStyleSheet("QTextEdit {font-family:\"Times New Roman ,  SimHei\" ; font-size:18px ; font-weight:bold ; }");      // 基于qss设置中文黑体，英文新罗马，加粗，大小18
	connect(ui.dataEdit, &QTextEdit::textChanged, [&]() {         // 设置文本滚动，防止接收数据大小超过文本框时看不到
		ui.dataEdit->moveCursor(QTextCursor::End);                 // 将光标移动到末端
		});

	connect(ui.findPath, &QPushButton::clicked, [=]() {           // 选择文本保存路径,lamda函数中不能设置引用，会修改文本框指针
		QString dirName = QFileDialog::getExistingDirectory(this, "设置根目录",
			"/home",
			QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);  // 仅显示文件夹，同时不在文件对话框中解析符号链接 

		if (!dirName.isEmpty()) {
			ui.filePathEdit->setText(dirName);                        // 更新文本框显示
			ui.textEdit->append(QString("[File Info]\t系统根目录已更新为: %1").arg(dirName));
			m_rootDir = dirName;                                   // 设置根目录
			qInfo() << "系统根目录已成功设置" << dirName;
		}
		else {
			qWarning() << "未选择正确路径，系统根目录设置失败";
			ui.textEdit->append("[File Warning]\t根目录为空，设置失败!");
		}
		});
	connect(ui.saveDataButton, &QPushButton::clicked, [&]() {
		if (m_rootDir.isEmpty()) {                                // 判断根目录是否存在，选择导出的csv文件保存路径
			ui.textEdit->append("[File Error]\t根目录不存在导出失败");		
						
			qWarning() << "系统根目录未正确设置";
		}
		else {
			ui.textEdit->append("[File Info]\t数据导出中……");
			ui.saveDataButton->setEnabled(false);                  // 数据保存按钮关闭
			QString dataInfo = ui.dataEdit->toPlainText();         // 获取数据流文本
			if (dataInfo.isEmpty()) {
				qWarning() << "数据为空，导出失败";
				return;
			}
			emit sendRawData(dataInfo, m_rootDir);             // 发送初始化信号

			QThreadPool::globalInstance()->start(saveDataTask); // 将保存数据子线程扔入线程池中自动执行
			}
		});

	connect(ui.clearData, &QPushButton::clicked, [=]() {
		ui.dataEdit->clear();                                      // 清空文本框
		ui.dataEdit->moveCursor(QTextCursor::Start);               // 光标移动到开头
		qInfo() << "历史数据已清除";
		});

}
