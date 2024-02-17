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
    QString directory = "song_library/";

    QStringList musicFilters;
    musicFilters << "*.mp3" << "*.wav";

    QDir musicDir(directory);
    QFileInfoList musicFiles = musicDir.entryInfoList(musicFilters, QDir::Files);

    for (const QFileInfo& fileInfo : musicFiles) {
        // Get the file name without extension
        QString fileName = fileInfo.fileName();
        fileName = fileName.left(fileName.lastIndexOf('.')); // Remove the file extension

        QListWidgetItem* musicItem = new QListWidgetItem(fileName);
        emit addSongToPlaylist(musicItem);
    }
    emit setPlaylistName("All Tracks");
    emit setTrackQuantity(getTotalSongsCount()); // Set Track Quantity
}

void PlaylistManager::selectSong(QListWidgetItem* song, AudioControl* audioControl)
{
    QString filePath = "music_library/" + song->text() + ".mp3";
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
    emit onSongImport(fileName);
}

int PlaylistManager::getTotalSongsCount()
{
	return 0;
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

void PlaylistManager::setSongName(const QString& name)
{

}

