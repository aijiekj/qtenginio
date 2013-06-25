
#include "imageobject.h"

#include <QtNetwork>
#include <Enginio/enginioclient.h>
#include <Enginio/enginioreply.h>

ImageObject::ImageObject(EnginioClient *enginio, const QJsonObject &file)
    : m_enginio(enginio), m_fileObject(file)
{
    QString fileId;
    fileId = file.value("id").toString();

    QJsonObject fileObject;
    fileObject.insert("id", fileId);
    fileObject.insert("variant", QString("thumbnail"));
    EnginioReply *reply = m_enginio->downloadFile(fileObject);
    connect(reply, SIGNAL(finished(EnginioReply*)), this, SLOT(replyFinished(EnginioReply*)));
}

void ImageObject::replyFinished(EnginioReply *enginioReply)
{
    QString url = enginioReply->data().value("expiringUrl").toString();
    QNetworkRequest request(url);
    m_reply = m_enginio->networkManager()->get(request);
    m_reply->setParent(this);
    connect(m_reply, SIGNAL(finished()), this, SLOT(downloadFinished()));
}

void ImageObject::downloadFinished()
{
    QByteArray imageData = m_reply->readAll();
    m_image.loadFromData(imageData);
    emit imageChanged(m_fileObject.value("id").toString());

    qDebug() << "image: " << m_image;

    delete m_reply;
    m_reply = 0;
}

QPixmap ImageObject::thumbnail()
{
    if (m_image.isNull() || !m_thumbnail.size().isNull())
        return m_thumbnail;
    m_thumbnail = QPixmap::fromImage(m_image.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    return m_thumbnail;
}