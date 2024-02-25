#include "playlistmanager.h"

PlaylistManager::PlaylistManager(QObject* parent)
    : QObject(parent),
    m_defaultPlaylist(new Playlist(this)),
    m_currentPlaylist(m_defaultPlaylist),
    m_playlists(new QList<Playlist*>())
{
    m_defaultPlaylist->setName("All Tracks");
}

PlaylistManager::~PlaylistManager()
{

}

void PlaylistManager::updateDefaultPlaylist()
{
    emit songsDisplayCleared();
    m_defaultPlaylist->addAllSongs();
    m_currentPlaylist = m_defaultPlaylist;
    emit defaultPlaylistDisplayUpdated();
    emit songImportButtonHidden();
}

void PlaylistManager::addPlaylist(QString name)
{
    Playlist* playlist = new Playlist(this);
    playlist->setName(name);
    m_playlists->append(playlist);
}

Playlist* PlaylistManager::getCurrentPlaylist() const
{
    return m_currentPlaylist;
}

Playlist* PlaylistManager::getDefaultPlaylist() const
{
    return m_defaultPlaylist;
}

void PlaylistManager::selectPlaylist(QListWidgetItem* playlist)
{
    emit songsDisplayCleared();
    QString playlistName = playlist->text();
    for (Playlist* pl : *m_playlists) {
        if (pl->getName() == playlistName) {
            m_currentPlaylist = pl; // Set current playlist 
            break;
        }
    }
    qDebug() << m_currentPlaylist->getName() << "'s song paths:" << m_currentPlaylist->getSongPaths();
    emit playlistDisplayUpdated();
    emit songImportButtonVisible();
}

void PlaylistManager::onAddMultipleSongs()
{
    m_currentPlaylist->addMultipleSongs();
    emit playlistDisplayUpdated();
}

void PlaylistManager::onSelectSong(QListWidgetItem* song, AudioControl* audioControl)
{
    m_currentPlaylist->selectSong(song, audioControl);
}

void PlaylistManager::onToNextSong(AudioControl* audioControl)
{
    m_currentPlaylist->toNextSong(audioControl);
}

void PlaylistManager::onToPreviousSong(AudioControl* audioControl)
{
    m_currentPlaylist->toPreviousSong(audioControl);
}

void PlaylistManager::onSkipOnSongEnd(AudioControl* audioControl, QMediaPlayer::MediaStatus status)
{
    m_currentPlaylist->skipOnSongEnd(audioControl, status);
}