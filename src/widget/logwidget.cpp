#include "logwidget.h"
#include <iostream>

#include <QtCore/QDateTime>


static LogWidget * thisInstance = nullptr;

void LogHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    if (!thisInstance)
        return;

    QString color;
    switch (type) {
    case QtDebugMsg:
      color = "blue";
      break;
    case QtInfoMsg:
      color = "green";
      break;
    case QtWarningMsg:
      color = "orange";
      break;
    case QtCriticalMsg:
      color = "red";
      break;
    case QtFatalMsg:
      color = "black";
      abort();
    }

    QDateTime now = QDateTime::currentDateTime();
    QString msgHtml = msg;
    msgHtml.replace("\n", "<br>");
    QString text = QString("<font color=\"%1\">%2 : %3</font>")
                       .arg(color, now.toString("hh:mm:ss.z"), msgHtml);
    thisInstance->appendHtml(text);
}

LogWidget::LogWidget(QWidget *parent)
: QPlainTextEdit(parent)
{
    thisInstance = this;
    qInstallMessageHandler(LogHandler);

    setReadOnly(true);
}

LogWidget::~LogWidget()
{
    qInstallMessageHandler(0);
}
