// playlist.cpp

#include "playlist.h"

Playlist::Playlist(QObject* parent) 
	: QObject(parent)
{
}

Playlist::~Playlist()
{
}

void Playlist::loadAllSongs()
{
    emit clearSongList();

    QStringList musicFilters;
    musicFilters << "*.mp3";

    QDir musicDir(m_musicLibraryPath);
    QFileInfoList musicFiles = musicDir.entryInfoList(musicFilters, QDir::Files);

    for (const QFileInfo& fileInfo : musicFiles) {
        // Get the file name without extension
        QString fileName = fileInfo.fileName();
        fileName = fileName.left(fileName.lastIndexOf('.')); // Remove the file extension

        QListWidgetItem* musicItem = new QListWidgetItem(fileName);
        emit addSongToPlaylist(musicItem);
    }
    emit setPlaylistName("All Tracks");
    emit setTrackQuantity(getTrackQuantity()); // Set Track Quantity
}

void Playlist::selectSong(QListWidgetItem* song, AudioControl* audioControl)
{
    QString filePath = m_musicLibraryPath + song->text() + ".mp3";
    qDebug() << "Selected song: " << filePath;
    QMediaPlayer* m_player = audioControl->getMediaPlayer();
    m_player->setSource(QUrl::fromLocalFile(filePath));

    if (m_player->mediaStatus() != QMediaPlayer::NoMedia) {
        m_player->play();
    }
    else {
        qDebug() << "Error setting media source: " << m_player->errorString();
    }
}

QString Playlist::getTrackQuantity() const
{
    QDir folderDir(m_musicLibraryPath);
    QFileInfoList fileInfoList = folderDir.entryInfoList(QDir::Files);

    return QString::number(fileInfoList.size());
}

void Playlist::toNextSong()
{
}

void Playlist::toPreviousSong()
{
}

void Playlist::skipOnSongEnd()
{
}
