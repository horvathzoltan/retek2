#ifndef ZDOWNLOADHELPER_H
#define ZDOWNLOADHELPER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QDateTime>
#include <QFile>
#include <QDebug>

class zDownloader : public QObject
{
    Q_OBJECT
    public:
        explicit zDownloader(QObject *parent = nullptr);

        QByteArray download(const QString&);
        void downloadAsync(const QString&);

    signals:

    public slots:
        void replyFinished (QNetworkReply *reply);

    private:
       QNetworkAccessManager *manager;
};

#endif // ZDOWNLOADHELPER_H
