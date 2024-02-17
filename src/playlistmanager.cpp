// playlistmanager.cpp

#include "playlistmanager.h"

PlaylistManager::PlaylistManager(QObject* parent) 
	: QObject(parent) 
{

}

PlaylistManager::~PlaylistManager()
{

}

void PlaylistManager::addPlaylist()
{
}

void PlaylistManager::viewAllSongs()
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

void PlaylistManager::selectSong(QListWidgetItem* song, AudioControl* audioControl)
{
    QString filePath = m_musicLibraryPath + song->text() + ".mp3";
    QMediaPlayer* m_player = audioControl->getMediaPlayer();
    m_player->setSource(QUrl::fromLocalFile(filePath));

    if (m_player->mediaStatus() != QMediaPlayer::NoMedia) {
        m_player->play();
    }
    else {
        qDebug() << "Error setting media source: " << m_player->errorString();
    }

    QString fileName = song->text();
    fileName = fileName.left(fileName.lastIndexOf('.'));
    emit onSongStart(fileName);
}

QString PlaylistManager::getTrackQuantity() const
{
    QDir folderDir(m_musicLibraryPath);
    QFileInfoList fileInfoList = folderDir.entryInfoList(QDir::Files);

    return QString::number(fileInfoList.size());
}

void PlaylistManager::toNextSong()
{
}

void PlaylistManager::toPreviousSong()
{
}

void PlaylistManager::skipOnSongEnd()
{
}