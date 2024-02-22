// playlist.cpp

#include "playlist.h"

Playlist::Playlist(QObject* parent) 
	: QObject(parent),
    m_musicLibraryPath(getProjectRootPath() + "/music_library/"),
    m_name("Playlist")
{
}

Playlist::~Playlist()
{
}

void Playlist::addAllSongs()
{
    m_songPaths.clear();
    QStringList musicFilters;
    musicFilters << "*.mp3";

    QDir musicDir(m_musicLibraryPath);
    QFileInfoList musicFiles = musicDir.entryInfoList(musicFilters, QDir::Files);

    for (const QFileInfo& fileInfo : musicFiles) {
        QString songPath = fileInfo.absoluteFilePath();
        addSong(songPath);
    }
    updatePlaylistInfo();
    qDebug() << "songs in playlist: " << m_songPaths;
}

void Playlist::selectSong(QListWidgetItem* song, AudioControl* audioControl) const
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
}

QString Playlist::getTrackQuantity() const
{
    QDir folderDir(m_musicLibraryPath);
    QFileInfoList fileInfoList = folderDir.entryInfoList(QDir::Files);

    return QString::number(fileInfoList.size());
}

void Playlist::toNextSong(AudioControl* audioControl)
{
    QMediaPlayer* m_player = audioControl->getMediaPlayer();
    int index = m_songPaths.indexOf(getCurrentSongPath(m_player));
    qDebug() << "Index: " << getCurrentSongPath(m_player);
    if (index == -1) {
		return;
	}
    QString filePath = (index < m_songPaths.size()) ? m_songPaths[index + 1] : m_songPaths[0];
    m_player->setSource(QUrl::fromLocalFile(filePath));

    if (m_player->mediaStatus() != QMediaPlayer::NoMedia) {
        m_player->play();
    }
    else {
        qDebug() << "Error setting media source: " << m_player->errorString();
    }
}

void Playlist::toPreviousSong(AudioControl* audioControl)
{
    QMediaPlayer* m_player = audioControl->getMediaPlayer();
    int index = m_songPaths.indexOf(getCurrentSongPath(m_player));
    if (index == -1) {
        return;
    }
    QString filePath = (index > 0) ? m_songPaths[index - 1] : m_songPaths[0];
    m_player->setSource(QUrl::fromLocalFile(filePath));

    if (m_player->mediaStatus() != QMediaPlayer::NoMedia) {
        m_player->play();
    }
    else {
        qDebug() << "Error setting media source: " << m_player->errorString();
    }
}

void Playlist::skipOnSongEnd(AudioControl* audioControl, QMediaPlayer::MediaStatus status)
{
	if (status == QMediaPlayer::EndOfMedia) {
		toNextSong(audioControl);
	}
}

void Playlist::addSong(const QString& songPath)
{
    m_songPaths.append(songPath);
    QFileInfo fileInfo(songPath);
    QString baseName = fileInfo.baseName(); // Get the file name without extension
    QListWidgetItem* musicItem = new QListWidgetItem(baseName);
    emit addSongToPlaylist(musicItem);
}

void Playlist::removeSong(const QString& songPath)
{
    int index = m_songPaths.indexOf(songPath);

    if (index != -1) {
        // Remove the song path from m_songPaths
        m_songPaths.removeAt(index);

        // Emit a signal to notify the UI to remove the corresponding item from the playlist
        emit songRemoved(index);
    }
}

bool Playlist::containsSong(const QString& songPath) const
{
    return false;
}

QString Playlist::getName() const
{
    return QString();
}

void Playlist::setName(const QString& name)
{
    m_name = name;
}

void Playlist::updatePlaylistInfo()
{
	emit setTrackQuantity(getTrackQuantity());
    emit setPlaylistName(m_name);
}

QString Playlist::getProjectRootPath() const {
    QString executablePath = QCoreApplication::applicationDirPath();
    QDir currentDir(executablePath);
    while (!currentDir.exists("CMakeLists.txt") && currentDir.cdUp());

    return currentDir.absolutePath();
}

QString Playlist::getCurrentSongPath(QMediaPlayer* m_player) const
{
    QUrl mediaUrl = m_player->source();
    QFileInfo fileInfo(mediaUrl.toLocalFile());
    return fileInfo.absoluteFilePath();
}
