// playlistmanager.cpp

#include "playlistmanager.h"

PlaylistManager::PlaylistManager(QObject* parent) 
	: QObject(parent),
    m_defaultPlaylist(new Playlist(this))
{
    m_defaultPlaylist->setPlaylistName("All Tracks");
    m_defaultPlaylist->loadAllSongs();
}

PlaylistManager::~PlaylistManager()
{

}

void PlaylistManager::addPlaylist()
{
}

Playlist* PlaylistManager::getCurrentPlaylist() const
{
    return nullptr;
}

Playlist* PlaylistManager::getDefaultPlaylist() const
{
    return m_defaultPlaylist;
}
