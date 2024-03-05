//playlistmanager.h

#ifndef PLAYLISTMANAGER_H
#define PLAYLISTMANAGER_H

#include <QObject>
#include <QInputDialog>
#include <QFileInfoList>
#include <QDir>
#include <QListWidgetItem>
#include <QFileSystemWatcher>
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
    Playlist* getSelectedPlaylist() const;
    Playlist* getDefaultPlaylist() const;
    void selectPlaylist(QListWidgetItem* playlist);
    void displaySongSelectionDialog();
    void onSelectSong(QListWidgetItem* song, AudioControl* audioControl);
    void onToNextSong(AudioControl* audioControl);
    void onToPreviousSong(AudioControl* audioControl);
    void onSkipOnSongEnd(AudioControl* audioControl, QMediaPlayer::MediaStatus status);
    void onAddMultipleSongs();
    void setSelectedPlaylist(Playlist* playlist);
    void setActivePlaylist(Playlist* playlist);
    void changePlaylistDisplayOnRemove(const int& index);
    QString getMusicLibraryPath() const;
    void setPlaylists(QList<Playlist*> playlists);
    QList<Playlist*> getPlaylists();
    void onMusicLibraryChanged(const QString& path);
    void setShuffleMode(const int& mode);
    void shufflePlaylist();
    void toggleShuffleStatus();
    void onShuffleFisherYates();
    void onShuffleRandom();

signals:
    void songsDisplayCleared();
    void playlistDisplayUpdated();
    void defaultPlaylistDisplayUpdated();
    void songImportButtonHidden();
    void songImportButtonVisible();
    void searchBarCleared();
    void playlistAdded(QListWidgetItem* playlist);
    void playlistRemoved(const int& index);
    void updateShuffleStatus(bool shuffled);

private:
    Playlist* m_defaultPlaylist;
    QList<Playlist*> m_playlists;
    Playlist* m_selectedPlaylist;
    Playlist* m_activePlaylist;
    SelectSongDialog* m_songSelectionDialog;
    QFileSystemWatcher m_watcher;
    bool m_shuffled;
    int m_shuffleMode;
    QStringList getMusicLibraryAbsolutePaths(const QString& path);
};

#endif // PLAYLISTMANAGER_H