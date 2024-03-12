#ifndef FILEPATH_H
#define FILEPATH_H

#include <QString>
#include <QCoreApplication>
#include <QDir>
#include <QMediaPlayer>
#include <QFileInfo>

namespace FilePath {
    QString getProjectRootPath();
    QString getCurrentSongPath(QMediaPlayer* m_player);
    qint64 getSongDuration(const QString& songPath);
    QString getSongDurationString(const qint64& duration);
}

#endif // FILEPATH_H
