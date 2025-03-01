/************************************************************************************************************
 *                                                                                                          *
 *                                                    环形缓冲区                                            *
 *                                                                                                          *
 ***********************************************************************************************************/

#pragma once

#include <QObject>
#include <QVector>
#include <QVector3D>
#include <QMutex>
#include <QMutexLocker>
#include <QString>

#include "NatNet.h"
#include <NatNetTypes.h>



class RingBuffer {
public:
	RingBuffer(size_t maxS) : maxSize(maxS), head(0), currentSize(0)
	{
		ringBuffer.resize(maxS);	 // 根据最大容量初始化缓冲区
	}

	void addFrame(const sFrameOfMocapData& data) {
		QMutexLocker locker(&mutex);     // 加锁
		ringBuffer[head] = data;		//  存储新帧数据

		// 更新头部索引和缓冲区大小
		head = (head + 1) % maxSize;
		if (currentSize < maxSize)
			currentSize++;
	}

	//返回当前环形缓冲区中的所有数据
	QVector<sFrameOfMocapData> getBuffer() {
		QMutexLocker locker(&mutex); // 加锁
		return QVector<sFrameOfMocapData>(ringBuffer.begin(), ringBuffer.end());
	}

	//提取并返回最近添加的一帧数据。如果缓冲区为空，则输出调试信息并返回一个空的向量。
	std::vector<sFrameOfMocapData> extractFrame() {
		QMutexLocker locker(&mutex);  // 加锁
		if (currentSize == 0) {
			qDebug() << "Buffer is Empty!";
			return std::vector<sFrameOfMocapData>(); // 返回默认构造的 实例
		}

		int lastIndex = (head - 1 + maxSize) % maxSize;		// 计算最后写入数据的索引
		std::vector<sFrameOfMocapData> frame(1);
		frame[0] = ringBuffer[lastIndex]; // 返回最后一帧数据
		return frame;
	}

	void clearRingBuffer() {
		//清空缓冲区并重置所有状态
		QMutexLocker locker(&mutex);  // 加锁
		ringBuffer.clear();
		ringBuffer.resize(maxSize);
		head = 0;
		currentSize = 0;
	}
private:
	std::vector<sFrameOfMocapData> ringBuffer;		// 使用 QVector 存储帧数据	 环形缓冲区   
	size_t maxSize;									            // 缓冲区的最大容量
	size_t head;									            // 当前写入数据的位置
	size_t currentSize;								            // 缓冲区中有效数据的数量
	QMutex mutex;												// 互斥锁
};