// playlist.h

#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QObject>
#include <QInputDialog>
#include <QFileInfoList>
#include <QDir>
#include <QListWidgetItem>
#include <QCoreApplication>
#include <QList>
#include <random>
#include <ctime>
#include "audiocontrol.h"
#include "selectsongdialog.h"

class Playlist : public QObject
{
    Q_OBJECT
public:
    explicit Playlist(QObject* parent = nullptr);
    ~Playlist();

    void addAllSongs();
    void selectSong(QListWidgetItem* song, AudioControl* audioControl);
    QString getTrackQuantity() const;
    void toNextSong(AudioControl* audiocontrol, bool shuffled);
    void toPreviousSong(AudioControl* audioControl, bool shuffled);
    void skipOnSongEnd(AudioControl* audioControl, QMediaPlayer::MediaStatus status, bool shuffled);
    void addSong(const QString& songPath);
    void addMultipleSongs(QStringList& selectedSongPaths);
    void removeSong(const int& index);
    QString getName() const;
    void setName(const QString& name);
    QList<QString> getSongPaths() const;
    QStringList getAllSongNames() const;
    QString getMusicLibraryPath() const;
    void shuffleFisherYates();
    void shuffleTwo();
    void shuffleThree();

signals:
    void songAdded(QListWidgetItem* song);
    void songRemoved(const int& index);
    void songSelected(Playlist* playlist);

private:
    QString getProjectRootPath() const;
    QString getCurrentSongPath(QMediaPlayer* m_player) const;
    QString m_musicLibraryPath;
    QList<QString> m_songPaths;
    QList<QString> m_shuffledSongPaths;
    QString m_name;
};

#endif // PLAYLIST_H