//playlistmanager.h

#ifndef PLAYLISTMANAGER_H
#define PLAYLISTMANAGER_H

#include <QObject>
#include <QInputDialog>
#include <QFileInfoList>
#include <QDir>
#include <QListWidgetItem>
#include "audiocontrol.h"

class PlaylistManager : public QObject
{
	Q_OBJECT
public:
    explicit PlaylistManager(QObject* parent = nullptr);
    ~PlaylistManager();

    void addPlaylist();
    void viewAllSongs();
    void selectSong(QListWidgetItem* song, AudioControl* audioControl);
    int getTotalSongsCount();
    void toNextSong();
    void toPreviousSong();
    void skipOnSongEnd();

signals:
    void clearSongList();
    void addSongToPlaylist(const QListWidgetItem* song);
    void setPlaylistName(const QString& name);
    void setTrackQuantity(const int& quantity);
    void onSongImport(const QString& songName);
};

#endif // PLAYLISTMANAGER_H
