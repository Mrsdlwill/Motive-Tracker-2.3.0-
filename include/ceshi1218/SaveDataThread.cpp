#include "SaveDataThread.h"

SaveDataThread::SaveDataThread(QObject* parent)
	: QObject(parent), QRunnable()
{
	setAutoDelete(false); // 执行完毕后自动析构
	atiFile = nullptr;
	powFile = nullptr;
	freFile = nullptr;
	MocapFile = nullptr;
}

SaveDataThread::~SaveDataThread()
{
	if (atiFile) {
		atiFile->close();
		delete atiFile;
	}
	if (powFile) {
		powFile->close();
		delete powFile;
	}
	if (freFile) {
		freFile->close();
		delete freFile;
	}
	if (MocapFile) {
		MocapFile->close();
		delete MocapFile;
	}
}

void SaveDataThread::run()
{
	qDebug() << "数据导出线程启动:" << QThread::currentThread();
	
	
	
	// 生成时间戳
	QDateTime currentTime = QDateTime::currentDateTime();
	QString timeStr = currentTime.toString("yyyy-MM-dd-hh-mm-ss");

	// 创建文件对象
	atiFile = createFile("ATI", timeStr);
	powFile = createFile("POW", timeStr);
	freFile = createFile("FRE", timeStr);
	MocapFile = createFile("Mocap", timeStr);

	// 创建数据流对象
	QTextStream atiIn(atiFile);
	QTextStream powIn(powFile);
	QTextStream freIn(freFile);
	QTextStream MocapIn(MocapFile);

	// 写表头
	atiIn << "Fx,Fy,Fz,Tx,Ty,Tz,Time\n";
	powIn << "Vol,Cur,Pow,Fre,Fac,Time\n";
	freIn << "Fre,Noi,Ang,Off,Win,Time\n";
	MocapIn << "marker.ID,marker.x,marker.y,marker.z,Time\n";

	// 分割数据行
	QStringList lines = m_rawData.split("\n");

	for (const QString& line : lines) {
		if (QThread::currentThread()->isInterruptionRequested()) {
			break;
		}

		// MocapData 解析逻辑
	// 修改 SaveDataThread.cpp 中的解析逻辑
		if (line.startsWith("[MocapData]")) {
			QStringList parts = line.split(',');
			if (parts.size() >= 5) { // 检查字段数量
				QString id = parts.at(1);  // 50044
				QString x = parts.at(2);   // 0.458771
				QString y = parts.at(3);   // 0.0471706
				QString z = parts.at(4);   // -1.59213
				QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
				MocapIn << id << "," << x << "," << y << "," << z << "," << timestamp << "\n";
			}
			continue;
		
		}

		// 其他数据解析
		QString lineHead = line.left(9);
		QVector<QString> dataFrame = splitDataFunc(line); // 现在传递 const 引用是合法的

		if (lineHead == "[FlaData]") {
			writeData(freIn, dataFrame);
		}
		else if (lineHead == "[SpiData]") {
			writeData(powIn, dataFrame);
		}
		else if (lineHead == "[ATIData]") {
			writeData(atiIn, dataFrame);
		}

		QThread::msleep(1); // 避免 CPU 忙等待
	}

	// 关闭文件
	this->closeFile();

	emit finish(QString("[File Info] 数据导出成功"));
	qDebug() << "数据导出任务结束，子线程退出";
}

void SaveDataThread::recvRawData(QString data, QString rootDir)
{
	m_rawData = data;
	m_rootDir = rootDir;
}

QFile* SaveDataThread::createFile(QString fileType, QString& timeStr)
{
	
	QString csvFileName = m_rootDir + QString("/%1_Data_Export_%2.csv").arg(fileType).arg(timeStr);
	csvFileName = QDir::toNativeSeparators(csvFileName);

	QFile* file = new QFile(csvFileName);
	if (!file->open(QIODevice::WriteOnly | QIODevice::Text)) {
		emit finish(QString("[File Info] 文件打开失败 \t%1").arg(timeStr));
		return nullptr;
	}

	return file;
}

void SaveDataThread::closeFile()
{
	if (atiFile) atiFile->close();
	if (powFile) powFile->close();
	if (freFile) freFile->close();
	if (MocapFile) MocapFile->close();
}

QVector<QString> SaveDataThread::splitDataFunc(const QString& line)
{
	QStringList parts = line.split(','); // split 不会修改 line
	QVector<QString> dataFrame;
	int n = line.size();
	int i = 13, j = i + 1;
	while (i < n && j < n) {
		if (line[j] == ':') {
			dataFrame.emplace_back(line.mid(i + 1, j - i - 4));
			i = j;
			j++;
		}
	}
	dataFrame.emplace_back(line.mid(i + 1));
	return dataFrame;
}

void SaveDataThread::writeData(QTextStream& in, QVector<QString>& dataFrame)
{
	for (QString& dataItem : dataFrame) {
		in << dataItem << ',';
	}
	in << "\n";
}