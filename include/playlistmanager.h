//playlistmanager.h

#ifndef PLAYLISTMANAGER_H
#define PLAYLISTMANAGER_H

#include <QObject>
#include <QInputDialog>
#include <QFileInfoList>
#include <QDir>
#include <QListWidgetItem>
#include "audiocontrol.h"
#include "playlist.h"

class PlaylistManager : public QObject
{
	Q_OBJECT
public:
    explicit PlaylistManager(QObject* parent = nullptr);
    ~PlaylistManager();

    void updateDefaultPlaylist();
    void addPlaylist(QString name);
    Playlist* getCurrentPlaylist() const;
    Playlist* getDefaultPlaylist() const;

signals:
    void clearSongList();

private:
    Playlist* m_defaultPlaylist;
    QList<Playlist*>* m_playlists;
    Playlist* m_currentPlaylist;
};

#endif // PLAYLISTMANAGER_H
