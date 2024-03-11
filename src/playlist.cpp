// playlist.cpp

#include "playlist.h"

Playlist::Playlist(QObject* parent)
    : QObject(parent),
    m_musicLibraryPath(FilePath::getProjectRootPath() + "/music_library/"),
    m_name("Playlist"),
    m_currentShuffledSongIndex(0)
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
    QMediaPlayer* m_ambiencePlayer = audioControl->getAmbiencePlayer();
    m_player->setSource(QUrl::fromLocalFile(filePath));

    if (m_player->mediaStatus() != QMediaPlayer::NoMedia) {
        m_player->play();
        m_ambiencePlayer->play();
        emit songSelected(this);
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
    QMediaPlayer* m_ambiencePlayer = audioControl->getAmbiencePlayer();
    QStringList songPaths;

    QString currentSongPath = getCurrentSongPath(m_player);
    qint64 index;
    QString filePath;
    if (shuffled) {
        songPaths = m_shuffledSongPaths;
        index = m_currentShuffledSongIndex;
        ++m_currentShuffledSongIndex;
        if (m_currentShuffledSongIndex >= m_shuffledSongPaths.size()) {
            m_currentShuffledSongIndex = 0;
        }
    }
    else {
        songPaths = m_songPaths;
        index = songPaths.indexOf(currentSongPath);
    }
    filePath = songPaths[(index + 1) % songPaths.size()];

    m_player->setSource(QUrl::fromLocalFile(filePath));

    if (m_player->mediaStatus() != QMediaPlayer::NoMedia) {
        m_player->play();
        m_ambiencePlayer->play();
    }
}

void Playlist::toPreviousSong(AudioControl* audioControl, bool shuffled)
{
    if (m_songPaths.isEmpty()) {
        return;
    }

    QMediaPlayer* m_player = audioControl->getMediaPlayer();
    QMediaPlayer* m_ambiencePlayer = audioControl->getAmbiencePlayer();
    QStringList songPaths;

    QString currentSongPath = getCurrentSongPath(m_player);
    qint64 index;
    QString filePath;
    if (shuffled) {
        songPaths = m_shuffledSongPaths;
        index = m_currentShuffledSongIndex;
        --m_currentShuffledSongIndex;
        if (m_currentShuffledSongIndex < 0) {
            m_currentShuffledSongIndex = 0;
        }
    }
    else {
        songPaths = m_songPaths;
        index = songPaths.indexOf(currentSongPath);
    }
    filePath = songPaths[(index > 0) ? index - 1 : 0];

    m_player->setSource(QUrl::fromLocalFile(filePath));

    if (m_player->mediaStatus() != QMediaPlayer::NoMedia) {
        m_player->play();
        m_ambiencePlayer->play();
    }
}

void Playlist::skipOnSongEnd(AudioControl* audioControl, bool shuffled)
{
    QMediaPlayer::MediaStatus status = audioControl->getMediaPlayer()->mediaStatus();
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

QStringList Playlist::getSongNames() const
{
    QStringList songNames;
    for (const QString& songPath : m_songPaths) {
        QFileInfo fileInfo(songPath);
        QString fileName = fileInfo.baseName();
        songNames.append(fileName);
    }
    return songNames;
}

void Playlist::shuffleFisherYates()
{
    m_currentShuffledSongIndex = 0;
    m_shuffledSongPaths = m_songPaths;
    std::mt19937 gen(std::time(nullptr));
    int n = m_shuffledSongPaths.size();
    for (int i = n - 1; i > 0; --i) {
        std::uniform_int_distribution<int> dist(0, i);
        int j = dist(gen);
        m_shuffledSongPaths.swapItemsAt(i, j);
    }
}

void Playlist::shuffleRandom()
{
    m_currentShuffledSongIndex = 0;
    m_shuffledSongPaths = m_songPaths;
    std::mt19937 gen(std::time(nullptr));

    for (int i = 0; i < m_shuffledSongPaths.size(); ++i) {
        QString randomSong;
        do {
            int randomIndex = std::uniform_int_distribution<int>(0, m_shuffledSongPaths.size() - 1)(gen);
            randomSong = m_songPaths[randomIndex];
        } while ((m_shuffledSongPaths[i - 1] == randomSong) && i > 0);
        m_shuffledSongPaths[i] = randomSong;
    }
}

QString Playlist::getNextSongName(AudioControl* audioControl, bool shuffled) const
{
    QString nextSongPath;
    if (shuffled) {
        nextSongPath = m_shuffledSongPaths[(m_currentShuffledSongIndex + 1) % m_shuffledSongPaths.size()];
    }
    else {
        QMediaPlayer* m_player = audioControl->getMediaPlayer();
        nextSongPath = m_songPaths[(m_songPaths.indexOf(getCurrentSongPath(m_player)) + 1) % m_songPaths.size()];
    }
    return QFileInfo(nextSongPath).baseName();
}

QString Playlist::getCurrentSongPath(QMediaPlayer* m_player) const
{
    QUrl mediaUrl = m_player->source();
    QFileInfo fileInfo(mediaUrl.toLocalFile());
    return fileInfo.absoluteFilePath();
}