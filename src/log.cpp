//版权声明：本文为博主原创文章，遵循 CC 4.0 BY-SA 版权协议，转载请附上原文出处链接和本声明。
//原文链接：https://blog.csdn.net/xiaoyink/article/details/79721481
#include "log.h"

#include <QString>
#include <QLoggingCategory>
#include <QIODevice>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QDateTime>
#include <QMutex>
#include <QApplication>
#include <QRegExp>

static QString logDirectory;
static QString logFileName;
static QMutex mutex;
static QFile file;
static QTextStream textStream;


void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString text;
    switch(type)
    {
    case QtDebugMsg:
        text = QString("[Debug]");
        break;
    case QtInfoMsg:
        text = QString("[Info]");
        break;
    case QtWarningMsg:
        text = QString("[Warning]");
        break;
    case QtCriticalMsg:
        text = QString("[Critical]");
        break;
    case QtFatalMsg:
        text = QString("[Fatal]");
    }
    text.append(QString(" [%1] ").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
    text.append(QString(" [%1: Line: %2] ").arg(QString(context.file)).arg(context.line));
    text.append(QString(" [Function: %1] ").arg(QString(context.function)));

    mutex.lock();
    file.setFileName(logFileName);
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    textStream.setDevice(&file);
    textStream << text << endl << QString("Message: %1").arg(msg) << endl;
    file.close();
    mutex.unlock();
}

void logInit()
{
    logDirectory  = QApplication::applicationDirPath() + "/Log/";
    QDir dir(logDirectory);
    if(!dir.exists())
        dir.mkdir(logDirectory);
    logFileName = logDirectory + (QApplication::applicationName()+".log");

    /*以下这段代码的含义是初始化时检查日志文件是否存在一个月之前的日志，如果存在删除之*/
    {
        QMutexLocker locker(&mutex);
        //mutex.lock();
        QFile file(logFileName);
        file.open(QIODevice::ReadOnly);
        QTextStream textStream(&file);
        QString temp;
        QStringList tempList;
        QRegExp regExp(".*(20\\d\\d-\\d\\d-\\d\\d).*");
        while ((temp = textStream.readLine()).isEmpty() == false)
        {
            if(temp.indexOf(regExp) >= 0)
            {
                QDate date = QDate::fromString(regExp.cap(1), "yyyy-MM-dd");
                QDate currentDate = QDate::currentDate();
                /*判断当前行所记载的日期和现今的日期天数之差是否大于记录该条日志时的那个月的天数*/
                if(date.daysTo(currentDate) < date.day())
                {
                    tempList << temp;
                    tempList << textStream.readLine();
                }
            }
        }
        file.close();
        file.open(QIODevice::Truncate | QIODevice::WriteOnly);
        textStream.setDevice(&file);
        for(auto iterator = tempList.begin(); iterator != tempList.end(); iterator++)
        {
            textStream << *iterator << endl;
        }
        file.close();
        //mutex.unlock();
    }

    /*安装上述自定义函数*/
    qInstallMessageHandler(outputMessage);
}
