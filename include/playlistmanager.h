//playlistmanager.h

#ifndef PLAYLISTMANAGER_H
#define PLAYLISTMANAGER_H

#include <QObject>
#include <QInputDialog>
#include <QFileInfoList>
#include <QDir>
#include <QListWidgetItem>
#include <QFileSystemWatcher>
#include <QMessageBox>
#include "audiocontrol.h"
#include "playlist.h"
#include "filepath.h"

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
    Playlist* getActivePlaylist() const;
    Playlist* findPlaylistByName(QString& name) const;
    void selectPlaylist(QListWidgetItem* playlist);
    void displaySongSelectionDialog();
    void onSelectSong(QListWidgetItem* song, AudioControl* audioControl);
    void onSelectSong(const QString& songPath, AudioControl* audioControl);
    void onToNextSong(AudioControl* audioControl);
    void onToPreviousSong(AudioControl* audioControl);
    void onSkipOnSongEnd(AudioControl* audioControl);
    QString onGetNextSongName(AudioControl* audioControl) const;
    void onAddMultipleSongs();
    void setSelectedPlaylist(Playlist* playlist);
    void setActivePlaylist(Playlist* playlist);
    void onRemovePlaylist(const int& index);
    void setPlaylists(QList<Playlist*> playlists);
    QList<Playlist*> getPlaylists();
    void renamePlaylist(const int& index, const QString& name);
    void onMusicLibraryChanged(const QString& path);
    void setShuffleMode(const int& mode);
    int getShuffleMode() const;
    void shufflePlaylist();
    void toggleShuffleStatus();
    void onShuffleFisherYates();
    void onShuffleRandom();
    void loop(AudioControl* audioControl) const;
    void toggleLoopStatus();

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
    void updateLoopStatus(bool looped);
    void playlistRenamed(const int& index, const QString& name);

private:
    Playlist* m_defaultPlaylist;
    QList<Playlist*> m_playlists;
    Playlist* m_selectedPlaylist;
    Playlist* m_activePlaylist;
    SelectSongDialog* m_songSelectionDialog;
    QFileSystemWatcher m_watcher;
    bool m_shuffled;
    bool m_looped;
    int m_shuffleMode;
    QStringList getMusicLibraryAbsolutePaths(const QString& path);
};

#endif // PLAYLISTMANAGER_H