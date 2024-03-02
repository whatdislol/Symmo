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
    m_songPaths.clear();
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
}

void Playlist::selectSong(QListWidgetItem* song, AudioControl* audioControl)
{
    QString filePath = m_musicLibraryPath + song->text() + ".mp3";
    QMediaPlayer* m_player = audioControl->getMediaPlayer();
    m_player->setSource(QUrl::fromLocalFile(filePath));

    if (m_player->mediaStatus() != QMediaPlayer::NoMedia) {
        m_player->play();
        emit songSelected(this);
    }
    else {
        qDebug() << "Error setting media source: " << m_player->errorString();
    }
}

QString Playlist::getTrackQuantity() const
{
    return QString::number(m_songPaths.size());
}

void Playlist::toNextSong(AudioControl* audioControl, bool shuffled)
{
    if (m_songPaths.size() == 0) {
		return;
	}
    QMediaPlayer* m_player = audioControl->getMediaPlayer();
    int index;
    if (!shuffled) {
        index = m_songPaths.indexOf(getCurrentSongPath(m_player));
    }
    else {
        index = m_shuffledSongPaths.indexOf(getCurrentSongPath(m_player));
    }
    if (index != -1) {
        QString filePath = (index + 1 < m_songPaths.size()) ? m_songPaths[index + 1] : m_songPaths[0];
        m_player->setSource(QUrl::fromLocalFile(filePath));

        if (m_player->mediaStatus() != QMediaPlayer::NoMedia) {
            m_player->play();
        }
        else {
            qDebug() << "Error setting media source: " << m_player->errorString();
        }
    }
}

void Playlist::toPreviousSong(AudioControl* audioControl, bool shuffled)
{
    if (m_songPaths.size() == 0) {
        return;
    }
    QMediaPlayer* m_player = audioControl->getMediaPlayer();
    int index;
    if (!shuffled) {
        index = m_songPaths.indexOf(getCurrentSongPath(m_player));
    }
    else {
        index = m_shuffledSongPaths.indexOf(getCurrentSongPath(m_player));
    }
    if (index != -1) {
        QString filePath = (index > 0) ? m_songPaths[index - 1] : m_songPaths[0];
        m_player->setSource(QUrl::fromLocalFile(filePath));

        if (m_player->mediaStatus() != QMediaPlayer::NoMedia) {
            m_player->play();
        }
        else {
            qDebug() << "Error setting media source: " << m_player->errorString();
        }
    }

    if (shuffled) {
		qDebug() << "Shuffled songs: " << m_shuffledSongPaths;
	}
}

void Playlist::skipOnSongEnd(AudioControl* audioControl, QMediaPlayer::MediaStatus status, bool shuffled)
{
    if (status == QMediaPlayer::EndOfMedia) {
        toNextSong(audioControl, shuffled);
    }
}

void Playlist::addSong(const QString& songPath)
{
    if (!m_songPaths.contains(songPath)) {
        m_songPaths.append(songPath);
        QFileInfo fileInfo(songPath);
        QString baseName = fileInfo.baseName(); // Get the file name without extension
        QListWidgetItem* musicItem = new QListWidgetItem(baseName);
        emit songAdded(musicItem);
    }
}

void Playlist::addMultipleSongs(QStringList& selectedSongPaths)
{
    for (const QString& songPath : selectedSongPaths) {
        addSong(songPath);
    }
}

void Playlist::removeSong(const QString& songPath)
{
    if (songPath.size() > 0) {
        m_songPaths.removeAll(songPath);
    }
}

QString Playlist::getName() const
{
    return m_name;
}

void Playlist::setName(const QString& name)
{
    m_name = name;
}

QList<QString> Playlist::getSongPaths() const
{
    return m_songPaths;
}

QStringList Playlist::getAllSongNames() const
{
    QStringList allSongs;
    QDir musicDir(m_musicLibraryPath);
    QStringList musicFilters;
    musicFilters << "*.mp3";
    QFileInfoList musicFiles = musicDir.entryInfoList(musicFilters, QDir::Files);
    for (const QFileInfo& fileInfo : musicFiles) {
        allSongs.append(fileInfo.fileName());
    }
    return allSongs;
}

QString Playlist::getMusicLibraryPath() const
{
    return m_musicLibraryPath;
}

void Playlist::shuffleFisherYates()
{
    std::mt19937 gen(time(0));
    m_shuffledSongPaths = m_songPaths;
	int n = m_shuffledSongPaths.size();
    for (int i = n - 1; i > 0; --i) {
        std::uniform_int_distribution<int> dist(0, i);
        int j = dist(gen);
        m_shuffledSongPaths.swapItemsAt(i, j);
    }
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