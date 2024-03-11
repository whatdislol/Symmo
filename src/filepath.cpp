#include "FilePath.h"

QString FilePath::getProjectRootPath()
{
    QString executablePath = QCoreApplication::applicationDirPath();
    QDir currentDir(executablePath);
    while (!currentDir.exists("CMakeLists.txt") && currentDir.cdUp());

    return currentDir.absolutePath();
}

qint64 FilePath::getSongDuration(const QString& songPath)
{
    QMediaPlayer player;
    player.setSource(QUrl::fromLocalFile(songPath));

    QEventLoop loop;
    QObject::connect(&player, &QMediaPlayer::mediaStatusChanged, &loop, &QEventLoop::quit);
    loop.exec();

    return player.duration() / 1000;
}

QString FilePath::getSongDurationString(const qint64& duration)
{
    QTime time((duration / 3600) % 60, (duration / 60) % 60, duration % 60, (duration * 1000) % 1000);
    QString format = "mm:ss";
    if (duration > 3600) {
        format = "hh:mm:ss";
    }
    return time.toString(format);
}
