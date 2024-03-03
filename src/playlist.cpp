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
    if (m_songPaths.isEmpty()) {
        return;
    }

    QMediaPlayer* m_player = audioControl->getMediaPlayer();
    QStringList& songPaths = (shuffled) ? m_shuffledSongPaths : m_songPaths;

    QString currentSongPath = getCurrentSongPath(m_player);
    qint64 index = songPaths.indexOf(currentSongPath);
    QString filePath;

    if (index != -1) {
        filePath = songPaths[(index + 1) % songPaths.size()];
    }
    else if (shuffled) {
        filePath = m_shuffledSongPaths.first();
    }

    m_player->setSource(QUrl::fromLocalFile(filePath));

    if (m_player->mediaStatus() != QMediaPlayer::NoMedia) {
        m_player->play();
    }
    else {
        qDebug() << "Error setting media source: " << m_player->errorString();
    }
}

void Playlist::toPreviousSong(AudioControl* audioControl, bool shuffled)
{
    if (m_songPaths.isEmpty()) {
        return;
    }

    QMediaPlayer* m_player = audioControl->getMediaPlayer();
    QStringList& songPaths = (shuffled) ? m_shuffledSongPaths : m_songPaths;

    QString currentSongPath = getCurrentSongPath(m_player);
    qint64 index = songPaths.indexOf(currentSongPath);
    QString filePath;

    if (index != -1) {
        filePath = songPaths[(index > 0) ? index - 1 : 0];
    }
    else if (shuffled) {
        filePath = m_shuffledSongPaths.first();
    }

    m_player->setSource(QUrl::fromLocalFile(filePath));

    if (m_player->mediaStatus() != QMediaPlayer::NoMedia) {
        m_player->play();
    }
    else {
        qDebug() << "Error setting media source: " << m_player->errorString();
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
        QString baseName = fileInfo.baseName();
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
    qDebug() << "Initial: " << m_shuffledSongPaths;
    qDebug() << "Fisher Yates Shuffle: ";
	int n = m_shuffledSongPaths.size();
    for (int i = 0; i < n; ++i) {
        std::uniform_int_distribution<int> dist(i, n - 1);
        int j = dist(gen);
        m_shuffledSongPaths.swapItemsAt(i, j);
    }
    
    qDebug() << "Fisher Yates Shuffled: ";
    for (auto& song : m_shuffledSongPaths) {
        qDebug() << song;
    }
}

void Playlist::shuffleTwo()
{
    m_shuffledSongPaths = m_songPaths;
}

void Playlist::shuffleCTime()
{
    m_shuffledSongPaths = m_songPaths;
    qDebug() << "Initial: " << m_shuffledSongPaths;
    qDebug() << "Ctime Shuffle:";

    std::mt19937 gen(std::time(nullptr));

    for (int i = 0; i < m_shuffledSongPaths.size(); ++i) {
        int randomIndex = std::uniform_int_distribution<int>(0, m_shuffledSongPaths.size() - 1)(gen);
        QString randomSong = m_shuffledSongPaths[randomIndex];
        m_shuffledSongPaths[i] = randomSong;
        qDebug() << m_shuffledSongPaths[i];
    }

    qDebug() << "CTime Shuffled: ";
    for (auto& song : m_shuffledSongPaths) {
        qDebug() << song;
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