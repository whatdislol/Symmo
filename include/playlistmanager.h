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
    void removePlaylist(const int& index);
    Playlist* getCurrentPlaylist() const;
    Playlist* getDefaultPlaylist() const;
    void selectPlaylist(QListWidgetItem* playlist);
    void displaySongSelectionDialog();
    void onSelectSong(QListWidgetItem* song, AudioControl* audioControl);
    void onToNextSong(AudioControl* audioControl);
    void onToPreviousSong(AudioControl* audioControl);
    void onSkipOnSongEnd(AudioControl* audioControl, QMediaPlayer::MediaStatus status);
    void onAddMultipleSongs();

signals:
    void songsDisplayCleared();
    void playlistDisplayUpdated();
    void defaultPlaylistDisplayUpdated();
    void songImportButtonHidden();
    void songImportButtonVisible();
    void playlistRemoved(const int& index);

private:
    Playlist* m_defaultPlaylist;
    QList<Playlist*>* m_playlists;
    Playlist* m_currentPlaylist;
    SelectSongDialog* m_songSelectionDialog;
};

#endif // PLAYLISTMANAGER_H