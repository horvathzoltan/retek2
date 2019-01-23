#include "zdownloader.h"
#include "zlog.h"
#include <QEventLoop>


zDownloader::zDownloader(QObject *parent) : QObject(parent)
{
}

QByteArray zDownloader::download(const QString& urlstring)
{
    manager = new QNetworkAccessManager(this);
    auto ikey = zLog::openInfo(QStringLiteral("Beolvasás: %1").arg(urlstring));

    //auto url = QUrl(QStringLiteral("https://docs.google.com/document/export?format=html&id=1qqYuhCY5iTAfzBiwQGfzRdU7C1jm1pNnNajVrSrWAfU"));
    auto url = QUrl(urlstring);
    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("User-Agent", "zDownloader 1.0");

      QNetworkReply* reply = manager->get(request);

      QEventLoop loop;
      connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
      loop.exec();
      auto e = reply->readAll();

      zLog::appendInfo(ikey, "ok");
      zLog::closeInfo(ikey);
      return e;
}

void zDownloader::downloadAsync(const QString& urlstring)
{
    manager = new QNetworkAccessManager(this);

    //auto url = QUrl(QStringLiteral("https://docs.google.com/document/export?format=html&id=1qqYuhCY5iTAfzBiwQGfzRdU7C1jm1pNnNajVrSrWAfU"));
     auto url = QUrl(urlstring);
    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("User-Agent", "zDownloader 1.0");

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));

    QNetworkReply* reply = manager->get(request);

    return;
}


void zDownloader::replyFinished (QNetworkReply *reply)
{
    if(reply->error())
    {
        qDebug() << "ERROR!";
        qDebug() << reply->errorString();
    }
    else
    {
        auto e = reply->readAll();

         //qDebug()<<e;
//        qDebug() << reply->header(QNetworkRequest::ContentTypeHeader).toString();
//        qDebug() << reply->header(QNetworkRequest::LastModifiedHeader).toDateTime().toString();
//        qDebug() << reply->header(QNetworkRequest::ContentLengthHeader).toULongLong();
//        qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
//        qDebug() << reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();

//        QFile *file = new QFile("C:/Qt/Dummy/downloaded.txt");
//        if(file->open(QFile::Append))
//        {
//            auto e = reply->readAll();
//            file->write(e);
//            file->flush();
//            file->close();
//        }
//        delete file;
    }

    reply->deleteLater();
}
