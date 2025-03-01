#pragma once

#include <QObject>
#include <QRunnable>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QStringList>
#include <QVector>
#include <QThread>

class SaveDataThread : public QObject, public QRunnable
{
	Q_OBJECT

public:
	SaveDataThread(QObject* parent);
	~SaveDataThread();

	void run() override;
	void recvRawData(QString data, QString rootDir);
	QFile* createFile(QString fileType, QString& timeStr);
	void closeFile();
	QVector<QString> splitDataFunc(const QString& line);
	void writeData(QTextStream& in, QVector<QString>& dataFrame);


signals:
	void finish(QString infoStr);

private:
	QString m_rawData;
	QString m_rootDir;

	QFile* atiFile;
	QFile* powFile;
	QFile* freFile;
	QFile* MocapFile;

};
