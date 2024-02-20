// playlistmanager.cpp

#include "playlistmanager.h"

PlaylistManager::PlaylistManager(QObject* parent) 
	: QObject(parent),
    m_defaultPlaylist(new Playlist(this)),
    m_currentPlaylist(m_defaultPlaylist)
{
    m_defaultPlaylist->setName("All Tracks");
    updateDefaultPlaylist();
}

PlaylistManager::~PlaylistManager()
{

}

void PlaylistManager::updateDefaultPlaylist()
{
    emit clearSongList();
	m_defaultPlaylist->addAllSongs();
}

void PlaylistManager::addPlaylist()
{
}

Playlist* PlaylistManager::getCurrentPlaylist() const
{
    return m_currentPlaylist;
}

Playlist* PlaylistManager::getDefaultPlaylist() const
{
    return m_defaultPlaylist;
}
