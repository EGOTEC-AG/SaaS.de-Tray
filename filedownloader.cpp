#include "filedownloader.h"

FileDownloader::FileDownloader(QUrl fileUrl, QObject *parent) :
    QObject(parent)
{
    connect(&m_WebCtrl, SIGNAL(finished(QNetworkReply*)),
            SLOT(fileDownloaded(QNetworkReply*)));

    QNetworkRequest request(fileUrl);
    m_WebCtrl.get(request);
}

FileDownloader::~FileDownloader()
{

}

void FileDownloader::fileDownloaded(QNetworkReply *pReply)
{
    QByteArray fullReply = pReply->readAll();
    if (fullReply != "") {
        m_DownloadedData = fullReply;
        pReply->deleteLater();
        emit downloaded();
    } else {
        emit connectionError();
    }
}

QByteArray FileDownloader::downloadedData() const
{
    return m_DownloadedData;
}
