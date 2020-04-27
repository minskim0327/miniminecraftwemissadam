#pragma once
#include <QRunnable>
#include <QMutex>
using namespace std;


class Worker: public QRunnable
{
private:
    QMutex *mutex;
public:
    Worker(QMutex *mutex);
    void run() override;
};


