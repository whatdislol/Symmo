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
#include <algorithm>
#include "audiocontrol.h"
#include "selectsongdialog.h"
#include "filepath.h"

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
    void skipOnSongEnd(AudioControl* audioControl, bool shuffled);
    void addSong(const QString& songPath);
    void addMultipleSongs(QStringList& selectedSongPaths);
    void removeSong(const QString& songPath);
    QString getName() const;
    void setName(const QString& name);
    QList<QString> getSongPaths() const;
    QStringList getSongNames() const;
    void shuffleFisherYates();
    void shuffleRandom();
    QString getNextSongName(AudioControl* audioControl, bool shuffled) const;

signals:
    void songAdded(QListWidgetItem* song);
    void songSelected(Playlist* playlist);

private:
    QString getCurrentSongPath(QMediaPlayer* m_player) const;
    QString m_musicLibraryPath;
    QList<QString> m_songPaths;
    QList<QString> m_shuffledSongPaths;
    qint64 m_currentShuffledSongIndex;
    QString m_name;
};

#endif // PLAYLIST_H